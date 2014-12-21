/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * rxgen.c - regular expression generator
 *
 * Written By:  MURAOKA Taro <koron@tka.att.ne.jp>
 * Last Change: 19-Sep-2009.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wordbuf.h"
#include "rxgen.h"

#if defined(_MSC_VER)
# define STRDUP _strdup
#else
# define STRDUP strdup
#endif

#define RXGEN_ENC_SJISTINY
//#define RXGEN_OP_VIM

#define RXGEN_OP_MAXLEN 8
#define RXGEN_OP_OR "|"
#define RXGEN_OP_NEST_IN "("
#define RXGEN_OP_NEST_OUT ")"
#define RXGEN_OP_SELECT_IN "["
#define RXGEN_OP_SELECT_OUT "]"
#define RXGEN_OP_NEWLINE ""

int n_rnode_new = 0;
int n_rnode_delete = 0;

typedef struct _rnode rnode;

struct _rxgen
{
    rnode *node;
    RXGEN_PROC_CHAR2INT char2int;
    RXGEN_PROC_INT2CHAR int2char;
    unsigned char op_or[RXGEN_OP_MAXLEN];
    unsigned char op_nest_in[RXGEN_OP_MAXLEN];
    unsigned char op_nest_out[RXGEN_OP_MAXLEN];
    unsigned char op_select_in[RXGEN_OP_MAXLEN];
    unsigned char op_select_out[RXGEN_OP_MAXLEN];
    unsigned char op_newline[RXGEN_OP_MAXLEN];
    unsigned char *op_regexmeta;
};

/*
 * rnode interfaces
 */

struct _rnode
{
    unsigned int code;
    rnode* child;
    rnode* next;
};

    static rnode*
rnode_new()
{
    ++n_rnode_new;
    return (rnode*)calloc(1, sizeof(rnode));
}

    static void
rnode_delete(rnode* node)
{
    while (node)
    {
	rnode* child = node->child;
	if (node->next)
	    rnode_delete(node->next);
	free(node);
	node = child;
	++n_rnode_delete;
    }
}

/*
 * rxgen interfaces
 */

    static int
default_char2int(const unsigned char* in, unsigned int* out)
{
    if (out)
	*out = *in;
    return 1;
}

    static int
default_int2char(unsigned int in, unsigned char* out)
{
    int len = 0;
    /* out�͍Œ�ł�16�o�C�g�͂���A�Ƃ��������u�� */
    switch (in)
    {
	case '\\':
	case '.': case '*': case '^': case '$': case '/':
#ifdef RXGEN_OP_VIM
	case '[': case ']': case '~':
#endif
	    if (out)
		out[len] = '\\';
	    ++len;
	default:
	    if (out)
		out[len] = (unsigned char)(in & 0xFF);
	    ++len;
	    break;
    }
    return len;
}

    static int
regexmeta_int2char(unsigned char* op_regexmeta, unsigned int in, unsigned char* out)
{
    int len = 0;
    if (strchr(op_regexmeta, in))
    {
        if (out)
            out[len] = '\\';
        ++len;
    }
    if (out)
        out[len] = (unsigned char)(in & 0xFF);
    ++len;

    return len;
}

    void
rxgen_setproc_char2int(rxgen* object, RXGEN_PROC_CHAR2INT proc)
{
    if (object)
	object->char2int = proc ? proc : default_char2int;
}

    void
rxgen_setproc_int2char(rxgen* object, RXGEN_PROC_INT2CHAR proc)
{
    if (object)
	object->int2char = proc ? proc : default_int2char;
}

    static int
rxgen_call_char2int(rxgen* object, const unsigned char* pch,
	unsigned int* code)
{
    int len = object->char2int(pch, code);
    return len ? len : default_char2int(pch, code);
}

    static int
rxgen_call_int2char(rxgen* object, unsigned int code, unsigned char* buf)
{
    int len = object->int2char(code, buf);
    return len ? len :
	(object->op_regexmeta ?
		regexmeta_int2char(object->op_regexmeta, code, buf) :
		default_int2char(code, buf));
}

    rxgen*
rxgen_open()
{
    rxgen* object = (rxgen*)calloc(1, sizeof(rxgen));
    if (object)
    {
	rxgen_setproc_char2int(object, NULL);
	rxgen_setproc_int2char(object, NULL);
	strcpy(object->op_or,		RXGEN_OP_OR);
	strcpy(object->op_nest_in,	RXGEN_OP_NEST_IN);
	strcpy(object->op_nest_out,	RXGEN_OP_NEST_OUT);
	strcpy(object->op_select_in,	RXGEN_OP_SELECT_IN);
	strcpy(object->op_select_out,	RXGEN_OP_SELECT_OUT);
	strcpy(object->op_newline,	RXGEN_OP_NEWLINE);
	object->op_regexmeta = NULL;
    }
    return object;
}

    void
rxgen_close(rxgen* object)
{
    if (object)
    {
	rnode_delete(object->node);
	free(object->op_regexmeta);
	free(object);
    }
}

    static rnode*
search_rnode(rnode* node, unsigned int code)
{
    while (node && node->code != code)
	node = node->next;
    return node;
}

    int
rxgen_add(rxgen* object, const unsigned char* word)
{
    rnode **ppnode;
    rnode *pnode;

    if (!object || !word)
	return 0;

    ppnode = &object->node;
    while (1)
    {
	unsigned int code;
	int len = rxgen_call_char2int(object, word, &code);
	/*printf("rxgen_call_char2int: code=%08x\n", code);*/

	/* ���̓p�^�[�����s������I�� */
	if (code == 0)
	{
	    /* ���̓p�^�[���������������p�^�[���͔j������ */
	    if (*ppnode)
	    {
		rnode_delete(*ppnode);
		*ppnode = NULL;
	    }
	    break;
	}
	pnode = search_rnode(*ppnode, code);
	if (pnode == NULL)
	{
	    /* code�����m�[�h�������ꍇ�A�쐬�ǉ����� */
	    pnode = rnode_new();
	    pnode->code = code;
	    pnode->next = *ppnode;
	    *ppnode = pnode;
	}
	else if (pnode->child == NULL)
	{
	    /*
	     * code�����m�[�h�͗L�邪�A���̎q���������ꍇ�A����ȍ~�̓���
	     * �p�^�[���͔j������B��:
	     *     ������ + �����邢 -> ����
	     *	   ���̂��� + ���̂��� -> ���̂�
	     */
	    break;
	}
	/* �q�m�[�h��H���Đ[�����֒����_���ړ� */
	ppnode = &pnode->child;
	word += len;
    }
    return 1;
}

    static void
rxgen_generate_stub(rxgen* object, wordbuf_t* buf, rnode* node)
{
    unsigned char ch[16];
    int chlen, nochild, haschild = 0, brother = 1;
    rnode *tmp;

    /* ���݂̊K�w�̓���(�Z��̐��A�q���̐�)���`�F�b�N���� */
    for (tmp = node; tmp; tmp = tmp->next)
    {
	if (tmp->next)
	    ++brother;
	if (tmp->child)
	    ++haschild;
    }
    nochild = brother - haschild;
#if 0 /* For debug */
    printf("node=%p code=%04X\n  nochild=%d haschild=%d brother=%d\n",
	    node, node->code, nochild, haschild, brother);
#endif
    /* �K�v�Ȃ��()�ɂ��O���[�s���O */
    if (brother > 1 && haschild > 0)
	wordbuf_cat(buf, object->op_nest_in);
#if 1
    /* �q�̖����m�[�h����[]�ɂ��O���[�s���O */
    if (nochild > 0)
    {
	if (nochild > 1)
	    wordbuf_cat(buf, object->op_select_in);
	for (tmp = node; tmp; tmp = tmp->next)
	{
	    if (tmp->child)
		continue;
	    chlen = rxgen_call_int2char(object, tmp->code, ch);
	    ch[chlen] = '\0';
	    /*printf("nochild: %s\n", ch);*/
	    wordbuf_cat(buf, ch);
	}
	if (nochild > 1)
	    wordbuf_cat(buf, object->op_select_out);
    }
#endif
#if 1
    /* �q�̂���m�[�h���o�� */
    if (haschild > 0)
    {
	/* �O���[�v���o�͍ς݂Ȃ�OR�Ōq�� */
	if (nochild > 0)
	    wordbuf_cat(buf, object->op_or);
	for (tmp = node; !tmp->child; tmp = tmp->next)
	    ;
	while (1)
	{
	    chlen = rxgen_call_int2char(object, tmp->code, ch);
	    /*printf("code=%04X len=%d\n", tmp->code, chlen);*/
	    ch[chlen] = '\0';
	    wordbuf_cat(buf, ch);
	    /* �󔒁E���s��΂��̃p�^�[����}�� */
	    if (object->op_newline[0])
		wordbuf_cat(buf, object->op_newline);
	    rxgen_generate_stub(object, buf, tmp->child);
	    for (tmp = tmp->next; tmp && !tmp->child; tmp = tmp->next)
		;
	    if (!tmp)
		break;
	    if (haschild > 1)
		wordbuf_cat(buf, object->op_or);
	}
    }
#endif
    /* �K�v�Ȃ��()�ɂ��O���[�s���O */
    if (brother > 1 && haschild > 0)
	wordbuf_cat(buf, object->op_nest_out);
}

    unsigned char*
rxgen_generate(rxgen* object)
{
    unsigned char* answer = NULL;
    wordbuf_t *buf;

    if (object && (buf = wordbuf_open()))
    {
	if (object->node)
	    rxgen_generate_stub(object, buf, object->node);
	answer = STRDUP(WORDBUF_GET(buf));
	wordbuf_close(buf);
    }
    return answer;
}

    void
rxgen_release(rxgen* object, unsigned char* string)
{
    free(string);
}

/*
 * rxgen_add()���Ă����p�^�[����S�ă��Z�b�g�B
 */
    void
rxgen_reset(rxgen* object)
{
    if (object)
    {
	rnode_delete(object->node);
	object->node = NULL;
    }
}

    static unsigned char*
rxgen_get_operator_stub(rxgen* object, int index)
{
    switch (index)
    {
	case RXGEN_OPINDEX_OR:
	    return object->op_or;
	case RXGEN_OPINDEX_NEST_IN:
	    return object->op_nest_in;
	case RXGEN_OPINDEX_NEST_OUT:
	    return object->op_nest_out;
	case RXGEN_OPINDEX_SELECT_IN:
	    return object->op_select_in;
	case RXGEN_OPINDEX_SELECT_OUT:
	    return object->op_select_out;
	case RXGEN_OPINDEX_NEWLINE:
	    return object->op_newline;
	case RXGEN_OPINDEX_REGEXMETA:
	    return object->op_regexmeta;
	default:
	    return NULL;
    }
}

    const unsigned char*
rxgen_get_operator(rxgen* object, int index)
{
    return (const unsigned char*)
	(object ? rxgen_get_operator_stub(object, index) : NULL);
}

    int
rxgen_set_operator(rxgen* object, int index, const unsigned char* op)
{
    unsigned char* dest;

    if (!object)
	return 1; /* Invalid object */
    if (index <= RXGEN_OPINDEX_NEWLINE && strlen(op) >= RXGEN_OP_MAXLEN)
	return 2; /* Too long operator */
    if (index == RXGEN_OPINDEX_REGEXMETA)
    {
        free(object->op_regexmeta);
        object->op_regexmeta = (unsigned char*)malloc(strlen(op) + 1);
    }
    if (!(dest = rxgen_get_operator_stub(object, index)))
	return 3; /* No such an operator */
    strcpy(dest, op);

    return 0;
}

#if 0
/*
 * main
 */
    int
main(int argc, char** argv)
{
    rxgen *prx;

    if (prx = rxgen_open())
    {
	char buf[256], *ans;

	while (gets(buf) && !feof(stdin))
	    rxgen_add(prx, buf);
	ans = rxgen_generate(prx);
	printf("rxgen=%s\n", ans);
	rxgen_release(prx, ans);
	rxgen_close(prx);
    }
    fprintf(stderr, "n_rnode_new=%d\n", n_rnode_new);
    fprintf(stderr, "n_rnode_delete=%d\n", n_rnode_delete);
}
#endif
