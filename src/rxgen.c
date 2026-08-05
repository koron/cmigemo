// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// rxgen.c - regular expression generator
//
// Written By:  MURAOKA Taro <koron.kaoriya@gmail.com>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rxgen.h"
#include "wordbuf.h"

#if defined(_MSC_VER)
# define STRDUP _strdup
#else
# define STRDUP strdup
#endif

#define RXGEN_ENC_SJISTINY
// #define RXGEN_OP_VIM

#define RXGEN_OP_MAXLEN     8
#define RXGEN_OP_OR         "|"
#define RXGEN_OP_NEST_IN    "("
#define RXGEN_OP_NEST_OUT   ")"
#define RXGEN_OP_SELECT_IN  "["
#define RXGEN_OP_SELECT_OUT "]"
#define RXGEN_OP_NEWLINE    ""

int n_rnode_new = 0;
int n_rnode_delete = 0;

typedef struct _rnode rnode;

struct _rxgen
{
    rnode *root;

    RXGEN_PROC_CHAR2INT char2int;
    RXGEN_PROC_INT2CHAR int2char;

    unsigned char op_or[RXGEN_OP_MAXLEN];
    unsigned char op_nest_in[RXGEN_OP_MAXLEN];
    unsigned char op_nest_out[RXGEN_OP_MAXLEN];
    unsigned char op_select_in[RXGEN_OP_MAXLEN];
    unsigned char op_select_out[RXGEN_OP_MAXLEN];
    unsigned char op_newline[RXGEN_OP_MAXLEN];
};

// rnode interfaces

struct _rnode
{
    unsigned int code;
    rnode *low, *high;
    rnode *child;
};

static rnode *
rnode_new(unsigned int code)
{
    ++n_rnode_new;
    rnode *p = (rnode *)calloc(1, sizeof(rnode));
    p->code = code;
    return p;
}

static void
rnode_delete(rnode *node)
{
    while (node)
    {
        rnode *child = node->child;
        if (node->low)
            rnode_delete(node->low);
        if (node->high)
            rnode_delete(node->high);
        free(node);
        node = child;
        ++n_rnode_delete;
    }
}

static rnode *
rnode_dig(rnode **pp, unsigned int code)
{
    rnode *p = *pp;
    if (p == NULL)
    {
        *pp = rnode_new(code);
        return *pp;
    }
    while (1)
    {
        if (code == p->code)
            return p;
        if (code < p->code)
        {
            if (p->low == NULL)
            {
                p->low = rnode_new(code);
                return p->low;
            }
            p = p->low;
        }
        else
        {
            if (p->high == NULL)
            {
                p->high = rnode_new(code);
                return p->high;
            }
            p = p->high;
        }
    }
}

// rxgen interfaces

static int
default_char2int(const unsigned char *in, unsigned int *out)
{
    if (out)
        *out = *in;
    return 1;
}

static int
default_int2char(unsigned int in, unsigned char *out)
{
    int len = 0;
    // outは最低でも16バイトはある、という仮定を置く
    switch (in)
    {
        case '\\':
        case '.':
        case '*':
        case '^':
        case '$':
        case '/':
#ifdef RXGEN_OP_VIM
        case '[':
        case ']':
        case '~':
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

void
rxgen_setproc_char2int(rxgen *object, RXGEN_PROC_CHAR2INT proc)
{
    if (object)
        object->char2int = proc ? proc : default_char2int;
}

void
rxgen_setproc_int2char(rxgen *object, RXGEN_PROC_INT2CHAR proc)
{
    if (object)
        object->int2char = proc ? proc : default_int2char;
}

static int
rxgen_call_char2int(rxgen *object, const unsigned char *pch, unsigned int *code)
{
    int len = object->char2int(pch, code);
    return len ? len : default_char2int(pch, code);
}

static int
rxgen_call_int2char(rxgen *object, unsigned int code, unsigned char *buf)
{
    int len = object->int2char(code, buf);
    return len ? len : default_int2char(code, buf);
}

rxgen *
rxgen_open()
{
    rxgen *object = (rxgen *)calloc(1, sizeof(rxgen));
    if (object)
    {
        rxgen_setproc_char2int(object, NULL);
        rxgen_setproc_int2char(object, NULL);
        strcpy(object->op_or, RXGEN_OP_OR);
        strcpy(object->op_nest_in, RXGEN_OP_NEST_IN);
        strcpy(object->op_nest_out, RXGEN_OP_NEST_OUT);
        strcpy(object->op_select_in, RXGEN_OP_SELECT_IN);
        strcpy(object->op_select_out, RXGEN_OP_SELECT_OUT);
        strcpy(object->op_newline, RXGEN_OP_NEWLINE);
    }
    return object;
}

void
rxgen_close(rxgen *object)
{
    if (object)
    {
        rnode_delete(object->root);
        free(object);
    }
}

int
rxgen_add(rxgen *object, const unsigned char *word)
{
    if (!object || !word)
        return 0;

    rnode **ppnode = &object->root;
    while (1)
    {
        unsigned int code;
        int len = rxgen_call_char2int(object, word, &code);
        // printf("rxgen_call_char2int: code=%08x\n", code);

        // 入力パターンが尽きたら終了
        if (code == 0)
        {
            // 入力パターンよりも長い既存パターンは破棄する
            if (*ppnode)
            {
                rnode_delete(*ppnode);
                *ppnode = NULL;
            }
            break;
        }
        rnode *pnode = rnode_dig(ppnode, code);
        if (pnode != NULL && pnode->child == NULL)
        {
            // codeを持つノードは有るが、その子供が無い場合、それ以降の入力
            // パターンは破棄する。例:
            //     あかい + あかるい -> あか
            //	   たのしい + たのしみ -> たのし
            break;
        }
        // 子ノードを辿って深い方へ注視点を移動
        ppnode = &pnode->child;
        word += len;
    }
    return 1;
}

static void
rxgen_rnode_count(rnode *node, int *childrenCount, int *brotherCount)
{
    if (node->child)
        (*childrenCount)++;
    if (node->low)
    {
        (*brotherCount)++;
        rxgen_rnode_count(node->low, childrenCount, brotherCount);
    }
    if (node->high)
    {
        (*brotherCount)++;
        rxgen_rnode_count(node->high, childrenCount, brotherCount);
    }
}

static void rxgen_generate_stub(rxgen *object, wordbuf_t *buf, rnode *node);

static void
rxgen_write_node_code(rxgen* object, wordbuf_t *buf, rnode *node)
{
    unsigned char bytes[6];
    int len = rxgen_call_int2char(object, node->code, bytes);
    wordbuf_write_bytes(buf, bytes, len);
}

static void
rxgen_write_node_no_children(rxgen* object, wordbuf_t *buf, rnode *node)
{
    if (node->low)
        rxgen_write_node_no_children(object, buf, node->low);
    if (node->child == NULL)
        rxgen_write_node_code(object, buf, node);
    if (node->high)
        rxgen_write_node_no_children(object, buf, node->high);
}

static void
rxgen_write_node_has_children(rxgen* object, wordbuf_t *buf, rnode *node, bool *needOr)
{
    if (node->low)
        rxgen_write_node_has_children(object, buf, node->low, needOr);
    if (node->child != NULL)
    {
        // 必要ならばORを出力
        if (*needOr)
            wordbuf_cat(buf, object->op_or);
        rxgen_write_node_code(object, buf, node);
        // 空白・改行飛ばしのパターンを挿入
        if (object->op_newline[0])
            wordbuf_cat(buf, object->op_newline);
        rxgen_generate_stub(object, buf, node->child);
        *needOr = true;
    }
    if (node->high)
        rxgen_write_node_has_children(object, buf, node->high, needOr);
}

static void
rxgen_generate_stub(rxgen *object, wordbuf_t *buf, rnode *node)
{
    // 現在の階層の特性(兄弟の数、子供の数)をチェックする
    int childrenCount = 0;
    int brotherCount = 1;
    rxgen_rnode_count(node, &childrenCount, &brotherCount);
    int noChildrenCount = brotherCount - childrenCount;
#if 0 // For debug
    printf("node=%p code=%04X\n  noChildrenCount=%d childrenCount=%d brotherCount=%d\n",
	    node, node->code, noChildrenCount, childrenCount, brotherCount);
#endif

    bool needGroup = brotherCount > 1 && childrenCount > 0;
    bool needClass = noChildrenCount > 1;

    // 必要ならば()によるグルーピング
    if (needGroup)
        wordbuf_cat(buf, object->op_nest_in);

#if 1
    // 子の無いノードを先に[]によりグルーピング
    if (noChildrenCount > 0)
    {
        if (needClass)
        {
            wordbuf_cat(buf, object->op_select_in);
            rxgen_write_node_no_children(object, buf, node);
            wordbuf_cat(buf, object->op_select_out);
        }
        else
            rxgen_write_node_no_children(object, buf, node);
    }
#endif

#if 1
    // 子のあるノードを出力
    if (childrenCount > 0)
    {
        bool needOr = noChildrenCount > 0;
        rxgen_write_node_has_children(object, buf, node, &needOr);
    }
#endif

    // 必要ならば()によるグルーピング
    if (needGroup)
        wordbuf_cat(buf, object->op_nest_out);
}

unsigned char *
rxgen_generate(rxgen *object)
{
    unsigned char *answer = NULL;
    wordbuf_t *buf;

    if (object && (buf = wordbuf_open()))
    {
        if (object->root)
            rxgen_generate_stub(object, buf, object->root);
        answer = STRDUP(WORDBUF_GET(buf));
        wordbuf_close(buf);
    }
    return answer;
}

void
rxgen_release(rxgen *object, unsigned char *string)
{
    free(string);
}

// rxgen_add()してきたパターンを全てリセット。
void
rxgen_reset(rxgen *object)
{
    if (object)
    {
        rnode_delete(object->root);
        object->root = NULL;
    }
}

static unsigned char *
rxgen_get_operator_stub(rxgen *object, int index)
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
        default:
            return NULL;
    }
}

const unsigned char *
rxgen_get_operator(rxgen *object, int index)
{
    return (const unsigned char *)(object ? rxgen_get_operator_stub(
                                                    object, index)
                                          : NULL);
}

int
rxgen_set_operator(rxgen *object, int index, const unsigned char *op)
{
    unsigned char *dest;

    if (!object)
        return 1; // Invalid object
    if (strlen(op) >= RXGEN_OP_MAXLEN)
        return 2; // Too long operator
    if (!(dest = rxgen_get_operator_stub(object, index)))
        return 3; // No such an operator
    strcpy(dest, op);

    return 0;
}

#if 0
// main
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
