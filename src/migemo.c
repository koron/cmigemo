/* vim:set ts=8 sts=4 sw=4 tw=0: */
/*
 * migemo.c -
 *
 * Written By:  MURAOKA Taro <koron@tka.att.ne.jp>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#include "wordbuf.h"
#include "wordlist.h"
#include "mnode.h"
#include "rxgen.h"
#include "romaji.h"
#include "filename.h"
#include "charset.h"
#include "migemo.h"

#define DICT_MIGEMO "migemo-dict"
#define DICT_ROMA2HIRA "roma2hira.dat"
#define DICT_HIRA2KATA "hira2kata.dat"
#define DICT_HAN2ZEN "han2zen.dat"
#define DICT_ZEN2HAN "zen2han.dat"
#define BUFLEN_DETECT_CHARSET 4096

#ifdef __BORLANDC__
# define EXPORTS __declspec(dllexport)
#else
# define EXPORTS
#endif

typedef int (*MIGEMO_PROC_ADDWORD)(void* data, unsigned char* word);

/* migemo�I�u�W�F�N�g */
struct _migemo
{
    int enable;
    mtree_p mtree;
    int charset;
    romaji* roma2hira;
    romaji* hira2kata;
    romaji* han2zen;
    romaji* zen2han;
    rxgen* rx;
    MIGEMO_PROC_ADDWORD addword;
    CHARSET_PROC_CHAR2INT char2int;
};

static const unsigned char VOWEL_CHARS[] = "aiueo";

    static int
my_strlen(const char* s)
{
    size_t len;

    len = strlen(s);
    return len <= INT_MAX ? (int)len : INT_MAX;
}

    static mtree_p
load_mtree_dictionary(mtree_p mtree, const char* dict_file)
{
    FILE *fp;

    if ((fp = fopen(dict_file, "rt")) == NULL)
	return NULL;			/* Can't find file */
    mtree = mnode_load(mtree, fp);
    fclose(fp);
    return mtree;
}

    static mtree_p
load_mtree_dictionary2(migemo* obj, const char* dict_file)
{
    if (obj->charset == CHARSET_NONE)
    {
	/* �����̕����Z�b�g�ɂ��킹�Đ��K�\���������̊֐���ύX���� */
	CHARSET_PROC_CHAR2INT char2int = NULL;
	CHARSET_PROC_INT2CHAR int2char = NULL;
	obj->charset = charset_detect_file(dict_file);
	charset_getproc(obj->charset, &char2int, &int2char);
	if (char2int)
	{
	    migemo_setproc_char2int(obj, (MIGEMO_PROC_CHAR2INT)char2int);
	    obj->char2int = char2int;
	}
	if (int2char)
	    migemo_setproc_int2char(obj, (MIGEMO_PROC_INT2CHAR)int2char);
    }
    return load_mtree_dictionary(obj->mtree, dict_file);
}

    static void
dircat(char* buf, const char* dir, const char* file)
{
    strcpy(buf, dir);
    strcat(buf, "/");
    strcat(buf, file);
}

/*
 * migemo interfaces
 */

/**
 * Migemo�I�u�W�F�N�g�Ɏ����A�܂��̓f�[�^�t�@�C����ǉ��ǂݍ��݂���B
 * dict_file�͓ǂݍ��ރt�@�C�������w�肷��Bdict_id�͓ǂݍ��ގ����E�f�[�^��
 * ��ނ��w�肷����̂ňȉ��̂����ǂꂩ����w�肷��:
 *
 *  <dl>
 *  <dt>MIGEMO_DICTID_MIGEMO</dt>
 *	<dd>mikgemo-dict����</dd>
 *  <dt>MIGEMO_DICTID_ROMA2HIRA</dt>
 *	<dd>���[�}�����������ϊ��\</dd>
 *  <dt>MIGEMO_DICTID_HIRA2KATA</dt>
 *	<dd>���������J�^�J�i�ϊ��\</dd>
 *  <dt>MIGEMO_DICTID_HAN2ZEN</dt>
 *	<dd>���p���S�p�ϊ��\</dd>
 *  <dt>MIGEMO_DICTID_ZEN2HAN</dt>
 *	<dd>�S�p�����p�ϊ��\</dd>
 *  </dl>
 *
 *  �߂�l�͎��ۂɓǂݍ��񂾎�ނ������A��L�̑��ɓǂݍ��݂Ɏ��s�������Ƃ�����
 *  ���̉����Ԃ邱�Ƃ�����B
 *
 *  <dl><dt>MIGEMO_DICTID_INVALID</dt></dl>
 * @param obj Migemo�I�u�W�F�N�g
 * @param dict_id �����t�@�C���̎��
 * @param dict_file �����t�@�C���̃p�X
 */
    EXPORTS int MIGEMO_CALLTYPE
migemo_load(migemo* obj, int dict_id, const char* dict_file)
{
    if (!obj && dict_file)
	return MIGEMO_DICTID_INVALID;

    if (dict_id == MIGEMO_DICTID_MIGEMO)
    {
	/* migemo�����ǂݍ��� */
	mtree_p mtree;

	if ((mtree = load_mtree_dictionary2(obj, dict_file)) == NULL)
	    return MIGEMO_DICTID_INVALID;
	obj->mtree = mtree;
	obj->enable = 1;
	return dict_id;			/* Loaded successfully */
    }
    else
    {
	romaji *dict;

	switch (dict_id)
	{
	    case MIGEMO_DICTID_ROMA2HIRA:
		/* ���[�}�������ǂݍ��� */
		dict = obj->roma2hira;
		break;
	    case MIGEMO_DICTID_HIRA2KATA:
		/* �J�^�J�i�����ǂݍ��� */
		dict = obj->hira2kata;
		break;
	    case MIGEMO_DICTID_HAN2ZEN:
		/* ���p���S�p�����ǂݍ��� */
		dict = obj->han2zen;
		break;
	    case MIGEMO_DICTID_ZEN2HAN:
		/* ���p���S�p�����ǂݍ��� */
		dict = obj->zen2han;
		break;
	    default:
		dict = NULL;
		break;
	}
	if (dict && romaji_load(dict, dict_file) == 0)
	    return dict_id;
	else
	    return MIGEMO_DICTID_INVALID;
    }
}

/**
 * Migemo�I�u�W�F�N�g���쐬����B�쐬�ɐ�������ƃI�u�W�F�N�g���߂�l�Ƃ���
 * �Ԃ�A���s�����NULL���Ԃ�Bdict�Ŏw�肵���t�@�C����migemo-dict�����Ƃ���
 * �I�u�W�F�N�g�쐬���ɓǂݍ��܂��B�����Ɠ����f�B���N�g����:
 *
 *  <dl>
 *  <dt>roma2hira.dat</dt>
 *	<dd>���[�}�����������ϊ��\ </dd>
 *  <dt>hira2kata.dat</dt>
 *	<dd>���������J�^�J�i�ϊ��\ </dd>
 *  <dt>han2zen.dat</dt>
 *	<dd>���p���S�p�ϊ��\ </dd>
 *  </dl>
 *
 * �Ƃ������O�̃t�@�C�������݂���΁A���݂������̂������ǂݍ��܂��Bdict��
 * NULL���w�肵���ꍇ�ɂ́A�������܂߂Ă����Ȃ�t�@�C�����ǂݍ��܂�Ȃ��B
 * �t�@�C���̓I�u�W�F�N�g�쐬��ɂ�migemo_load()�֐����g�p���邱�ƂŒǉ��ǂ�
 * ���݂��ł���B
 * @param dict migemo-dict�����̃p�X�BNULL�̎��͎�����ǂݍ��܂Ȃ��B
 * @returns �쐬���ꂽMigemo�I�u�W�F�N�g
 */
    EXPORTS migemo* MIGEMO_CALLTYPE
migemo_open(const char* dict)
{
    migemo *obj;

    /* migemo�I�u�W�F�N�g�Ɗe�����o���\�z */
    if (!(obj = (migemo*)calloc(1, sizeof(migemo))))
	return obj;
    obj->enable = 0;
    obj->mtree = mnode_open(NULL);
    obj->charset = CHARSET_NONE;
    obj->rx = rxgen_open();
    obj->roma2hira =	romaji_open();
    obj->hira2kata =	romaji_open();
    obj->han2zen =	romaji_open();
    obj->zen2han =	romaji_open();
    if (!obj->rx || !obj->roma2hira || !obj->hira2kata || !obj->han2zen
	    || !obj->zen2han)
    {
	migemo_close(obj);
	return obj = NULL;
    }

    /* �f�t�H���gmigemo�������w�肳��Ă����烍�[�}���ƃJ�^�J�i�������T�� */
    if (dict)
    {
#ifndef _MAX_PATH
# define _MAX_PATH 1024 /* ���������Ȑ��l */
#endif
	char dir[_MAX_PATH];
	char roma_dict[_MAX_PATH];
	char kata_dict[_MAX_PATH];
	char h2z_dict[_MAX_PATH];
	char z2h_dict[_MAX_PATH];
	const char *tmp;
	mtree_p mtree;

	filename_directory(dir, dict);
	tmp = strlen(dir) ? dir : ".";
	dircat(roma_dict, tmp, DICT_ROMA2HIRA);
	dircat(kata_dict, tmp, DICT_HIRA2KATA);
	dircat(h2z_dict,  tmp, DICT_HAN2ZEN);
	dircat(z2h_dict,  tmp, DICT_ZEN2HAN);

	mtree = load_mtree_dictionary2(obj, dict);
	if (mtree)
	{
	    obj->mtree = mtree;
	    obj->enable = 1;
	    romaji_load(obj->roma2hira, roma_dict);
	    romaji_load(obj->hira2kata, kata_dict);
	    romaji_load(obj->han2zen, h2z_dict);
	    romaji_load(obj->zen2han, z2h_dict);
	}
    }
    return obj;
}

/**
 * Migemo�I�u�W�F�N�g��j�����A�g�p���Ă������\�[�X���������B
 * @param obj �j������Migemo�I�u�W�F�N�g
 */
    EXPORTS void MIGEMO_CALLTYPE
migemo_close(migemo* obj)
{
    if (obj)
    {
	if (obj->zen2han)
	    romaji_close(obj->zen2han);
	if (obj->han2zen)
	    romaji_close(obj->han2zen);
	if (obj->hira2kata)
	    romaji_close(obj->hira2kata);
	if (obj->roma2hira)
	    romaji_close(obj->roma2hira);
	if (obj->rx)
	    rxgen_close(obj->rx);
	if (obj->mtree)
	    mnode_close(obj->mtree);
	free(obj);
    }
}

/*
 * query version 2
 */

/*
 * mnode�̎��P�ꃊ�X�g�𐳋K�\�������G���W���ɓ��͂���B
 */
    static void
migemo_query_proc(mnode* p, void* data)
{
    migemo *object = (migemo*)data;
    wordlist_p list = p->list;

    for (; list; list = list->next)
	object->addword(object, list->ptr);
}

/*
 * �o�b�t�@��p�ӂ���mnode�ɍċA�ŏ������܂���
 */
    static void
add_mnode_query(migemo* object, unsigned char* query)
{
    mnode *pnode;

    if ((pnode = mnode_query(object->mtree, query)) != NULL)
	mnode_traverse(pnode, migemo_query_proc, object);
}

/**
 * ���͂����[�}���牼���ɕϊ����Č����L�[�ɉ�����B
 */
    static int
add_roma(migemo* object, unsigned char* query)
{
    unsigned char *stop, *hira, *kata, *han;

    hira = romaji_convert(object->roma2hira, query, &stop);
    if (!stop)
    {
	object->addword(object, hira);
	/* �������ɂ�鎫������ */
	add_mnode_query(object, hira);
	/* �Љ���������𐶐������ɉ����� */
	kata = romaji_convert2(object->hira2kata, hira, NULL, 0);
	object->addword(object, kata);
	/* TODO: ���p�J�i�𐶐������ɉ����� */
#if 1
	han = romaji_convert2(object->zen2han, kata, NULL, 0);
	object->addword(object, han);
	/*printf("kata=%s\nhan=%s\n", kata, han);*/
	romaji_release(object->zen2han, han);
#endif
	/* �J�^�J�i�ɂ�鎫������ */
	add_mnode_query(object, kata);
	romaji_release(object->hira2kata, kata); /* �J�^�J�i��� */
    }
    romaji_release(object->roma2hira, hira); /* ��������� */

    return stop ? 1 : 0;
}

/**
 * ���[�}���̖����ɕꉹ��t�������āA�e�X�������L�[�ɉ�����B
 */
    static void
add_dubious_vowels(migemo* object, unsigned char* buf, int index)
{
    const unsigned char* ptr;
    for (ptr = VOWEL_CHARS; *ptr; ++ptr)
    {
	buf[index] = *ptr;
	add_roma(object, buf);
    }
}

/*
 * ���[�}���ϊ����s���S���������ɁA[aiueo]�����"xn"��"xtu"�����ĕϊ�����
 * �݂�B
 */
    static void
add_dubious_roma(migemo* object, rxgen* rx, unsigned char* query)
{
    int max;
    int len;
    char *buf;

    if (!(len = my_strlen(query)))
	return;
    /*
     * ���[�}���̖����̃A�����W�̂��߂̃o�b�t�@���m�ۂ���B
     *	    ����: �I���W�i���̒����ANUL�A�h��(xtu)�A�⑫�ꉹ([aieuo])
     */
    max = len + 1 + 3 + 1;
    buf = malloc(max);
    if (buf == NULL)
	return;
    memcpy(buf, query, len);
    memset(&buf[len], 0, max - len);

    if (!strchr(VOWEL_CHARS, buf[len - 1]))
    {
	add_dubious_vowels(object, buf, len);
	/* ���m��P��̒�����2�������A���m�蕶���̒��O���ꉹ�Ȃ�΁c */
	if (len < 2 || strchr(VOWEL_CHARS, buf[len - 2]))
	{
	    if (buf[len - 1] == 'n')
	    {
		/* �u��v�����Ă݂� */
		memcpy(&buf[len - 1], "xn", 2);
		add_roma(object, buf);
	    }
	    else
	    {
		/* �u��{���̎q��}{�ꉹ}�v�����Ă݂� */
		buf[len + 2] = buf[len - 1];
		memcpy(&buf[len - 1], "xtu", 3);
		add_dubious_vowels(object, buf, len + 3);
	    }
	}
    }

    free(buf);
}

/*
 * query�𕶐߂ɕ�������B���߂̐؂�ڂ͒ʏ�A���t�@�x�b�g�̑啶���B���߂���
 * �������̑啶���Ŏn�܂������߂͔�啶������؂�Ƃ���B
 */
    static wordlist_p
parse_query(migemo* object, const unsigned char* query)
{
    const unsigned char *curr = query;
    const unsigned char *start = NULL;
    wordlist_p querylist = NULL, *pp = &querylist;

    while (1)
    {
	int len, upper;
        int sum = 0;

	if (!object->char2int || (len = object->char2int(curr, NULL)) < 1)
	    len = 1;
	start = curr;
	upper = (len == 1 && isupper(*curr) && isupper(curr[1]));
	curr += len;
        sum += len;
	while (1)
	{
	    if (!object->char2int || (len = object->char2int(curr, NULL)) < 1)
		len = 1;
	    if (*curr == '\0' || (len == 1 && (isupper(*curr) != 0) != upper))
		break;
	    curr += len;
            sum += len;
	}
	/* ���߂�o�^���� */
	if (start && start < curr)
	{
	    *pp = wordlist_open_len(start, sum);
	    pp = &(*pp)->next;
	}
	if (*curr == '\0')
	    break;
    }
    return querylist;
}

/*
 * 1�̒P���migemo�ϊ��B�����̃`�F�b�N�͍s�Ȃ�Ȃ��B
 */
    static int
query_a_word(migemo* object, unsigned char* query)
{
    unsigned char* zen;
    unsigned char* han;
    unsigned char* lower;
    int len = my_strlen(query);

    /* query���M�͂��������ɉ����� */
    object->addword(object, query);
    /* query���̂��̂ł̎������� */
    lower = malloc(len + 1);
    if (!lower)
	add_mnode_query(object, query);
    else
    {
	int i = 0, step;

	// MB���l�������啶�����������ϊ�
	while (i <= len)
	{
	    if (!object->char2int
		    || (step = object->char2int(&query[i], NULL)) < 1)
		step = 1;
	    if (step == 1 && isupper(query[i]))
		lower[i] = tolower(query[i]);
	    else
		memcpy(&lower[i], &query[i], step);
	    i += step;
	}
	add_mnode_query(object, lower);
	free(lower);
    }

    /* query��S�p�ɂ��Č��ɉ����� */
    zen = romaji_convert2(object->han2zen, query, NULL, 0);
    if (zen != NULL)
    {
	object->addword(object, zen);
	romaji_release(object->han2zen, zen);
    }

    /* query�𔼊p�ɂ��Č��ɉ����� */
    han = romaji_convert2(object->zen2han, query, NULL, 0);
    if (han != NULL)
    {
	object->addword(object, han);
	romaji_release(object->zen2han, han);
    }

    /* �������A�J�^�J�i�A�y�т���ɂ�鎫�������ǉ� */
    if (add_roma(object, query))
	add_dubious_roma(object, object->rx, query);

    return 1;
}

    static int
addword_rxgen(migemo* object, unsigned char* word)
{
    /* ���K�\�������G���W���ɒǉ����ꂽ�P���\������ */
    /*printf("addword_rxgen: %s\n", word);*/
    return rxgen_add(object->rx, word);
}

/**
 * query�ŗ^����ꂽ������(���[�}��)����{�ꌟ���̂��߂̐��K�\���֕ϊ�����B
 * �߂�l�͕ϊ����ꂽ���ʂ̕�����(���K�\��)�ŁA�g�p���#migemo_release()�֐�
 * �֓n�����Ƃŉ�����Ȃ���΂Ȃ�Ȃ��B
 * @param object Migemo�I�u�W�F�N�g
 * @param query �₢���킹������
 * @returns ���K�\��������B#migemo_release() �ŉ������K�v�L��B
 */
    EXPORTS unsigned char* MIGEMO_CALLTYPE
migemo_query(migemo* object, const unsigned char* query)
{
    unsigned char *retval = NULL;
    wordlist_p querylist = NULL;
    wordbuf_p outbuf = NULL;

    if (object && object->rx && query)
    {
	wordlist_p p;

	querylist = parse_query(object, query);
	if (querylist == NULL)
	    goto MIGEMO_QUERY_END; /* ��query�̂��߃G���[ */
	outbuf = wordbuf_open();
	if (outbuf == NULL)
	    goto MIGEMO_QUERY_END; /* �o�͗p�̃������̈�s���̂��߃G���[ */

	/* �P��Q��rxgen�I�u�W�F�N�g�ɓ��͂����K�\���𓾂� */
	object->addword = (MIGEMO_PROC_ADDWORD)addword_rxgen;
	rxgen_reset(object->rx);
	for (p = querylist; p; p = p->next)
	{
	    unsigned char* answer;

	    /*printf("query=%s\n", p->ptr);*/
	    query_a_word(object, p->ptr);
	    /* �����p�^�[��(���K�\��)���� */
	    answer = rxgen_generate(object->rx);
	    rxgen_reset(object->rx);
	    wordbuf_cat(outbuf, answer);
	    rxgen_release(object->rx, answer);
	}
    }

MIGEMO_QUERY_END:
    if (outbuf)
    {
	retval = outbuf->buf;
	outbuf->buf = NULL;
	wordbuf_close(outbuf);
    }
    if (querylist)
	wordlist_close(querylist);

    return retval;
}

/**
 * �g���I�����migemo_query()�֐��œ���ꂽ���K�\�����������B
 * @param p Migemo�I�u�W�F�N�g
 * @param string ���K�\��������
 */
    EXPORTS void MIGEMO_CALLTYPE
migemo_release(migemo* p, unsigned char* string)
{
    free(string);
}

/**
 * Migemo�I�u�W�F�N�g���������鐳�K�\���Ɏg�p���郁�^����(���Z�q)���w�肷
 * ��Bindex�łǂ̃��^���������w�肵�Aop�Œu��������Bindex�ɂ͈ȉ��̒l���w
 * ��\�ł���:
 *
 *  <dl>
 *  <dt>MIGEMO_OPINDEX_OR</dt>
 *	<dd>�_���a�B�f�t�H���g�� "|" �Bvim�ŗ��p����ۂ� "\|" �B</dd>
 *  <dt>MIGEMO_OPINDEX_NEST_IN</dt>
 *	<dd>�O���[�s���O�ɗp����J�����ʁB�f�t�H���g�� "(" �Bvim�ł̓��W�X�^
 *	\\1�`\\9�ɋL�������Ȃ��悤�ɂ��邽�߂� "\%(" ��p����BPerl�ł����l��
 *	���Ƃ�ژ_�ނȂ�� "(?:" ���g�p�\�B</dd>
 *  <dt>MIGEMO_OPINDEX_NEST_OUT</dt>
 *	<dd>�O���[�s���O�̏I����\�������ʁB�f�t�H���g�ł� ")" �Bvim�ł�
 *	"\)" �B</dd>
 *  <dt>MIGEMO_OPINDEX_SELECT_IN</dt>
 *	<dd>�I���̊J�n��\���J���p���ʁB�f�t�H���g�ł� "[" �B</dd>
 *  <dt>MIGEMO_OPINDEX_SELECT_OUT</dt>
 *	<dd>�I���̏I����\�����p���ʁB�f�t�H���g�ł� "]" �B</dd>
 *  <dt>MIGEMO_OPINDEX_NEWLINE</dt>
 *	<dd>�e�����̊Ԃɑ}�������u0�ȏ�̋󔒂������͉��s�Ƀ}�b�`����v
 *	�p�^�[���B�f�t�H���g�ł� "" �ł���ݒ肳��Ȃ��Bvim�ł� "\_s*" ���w
 *	�肷��B</dd>
 *  <dt>MIGEMO_OPINDEX_REGEXMETA</dt>
 *      <dd>���K�\���ɂ����郁�^�����Q�B�����Ŏw�肵��������Ɋ܂܂�镶���́A
 *      ��������鐳�K�\���ɂ����ăG�X�P�[�v�����B</dd>
 *  </dl>
 *
 * �f�t�H���g�̃��^�����͓��ɒf�肪�Ȃ�����Perl�̂���Ɠ����Ӗ��ł���B�ݒ�
 * �ɐ�������Ɩ߂�l��1(0�ȊO)�ƂȂ�A���s�����0�ɂȂ�B
 * @param object Migemo�I�u�W�F�N�g
 * @param index ���^�������ʎq
 * @param op ���^����������
 * @returns ������0�ȊO�A���s��0�B
 */
    EXPORTS int MIGEMO_CALLTYPE
migemo_set_operator(migemo* object, int index, const unsigned char* op)
{
    if (object)
    {
	int retval = rxgen_set_operator(object->rx, index, op);
	return retval ? 0 : 1;
    }
    else
	return 0;
}

/**
 * Migemo�I�u�W�F�N�g���������鐳�K�\���Ɏg�p���Ă��郁�^����(���Z�q)���擾
 * ����Bindex�ɂ��Ă�migemo_set_operator()�֐����Q�ƁB�߂�l�ɂ�index�̎w
 * �肪��������΃��^�������i�[����������ւ̃|�C���^���A�s���ł����NULL��
 * �Ԃ�B
 * @param object Migemo�I�u�W�F�N�g
 * @param index ���^�������ʎq
 * @returns ���݂̃��^����������
 */
    EXPORTS const unsigned char* MIGEMO_CALLTYPE
migemo_get_operator(migemo* object, int index)
{
    return object ? rxgen_get_operator(object->rx, index) : NULL;
}

/**
 * Migemo�I�u�W�F�N�g�ɃR�[�h�ϊ��p�̃v���V�[�W����ݒ肷��B�v���V�[�W����
 * ���Ă̏ڍׂ́u�^���t�@�����X�v�Z�N�V������MIGEMO_PROC_CHAR2INT���Q�ƁB
 * @param object Migemo�I�u�W�F�N�g
 * @param proc �R�[�h�ϊ��p�v���V�[�W��
 */
    EXPORTS void MIGEMO_CALLTYPE
migemo_setproc_char2int(migemo* object, MIGEMO_PROC_CHAR2INT proc)
{
    if (object)
	rxgen_setproc_char2int(object->rx, (RXGEN_PROC_CHAR2INT)proc);
}

/**
 * Migemo�I�u�W�F�N�g�ɃR�[�h�ϊ��p�̃v���V�[�W����ݒ肷��B�v���V�[�W����
 * ���Ă̏ڍׂ́u�^���t�@�����X�v�Z�N�V������MIGEMO_PROC_INT2CHAR���Q�ƁB
 * @param object Migemo�I�u�W�F�N�g
 * @param proc �R�[�h�ϊ��p�v���V�[�W��
 */
    EXPORTS void MIGEMO_CALLTYPE
migemo_setproc_int2char(migemo* object, MIGEMO_PROC_INT2CHAR proc)
{
    if (object)
	rxgen_setproc_int2char(object->rx, (RXGEN_PROC_INT2CHAR)proc);
}

/**
 * Migemo�I�u�W�F�N�g��migemo_dict���ǂݍ��߂Ă��邩���`�F�b�N����B�L����
 * migemo_dict��ǂݍ��߂ē����ɕϊ��e�[�u�����\�z�ł��Ă����0�ȊO(TRUE)
 * ���A�\�z�ł��Ă��Ȃ��Ƃ��ɂ�0(FALSE)��Ԃ��B
 * @param obj Migemo�I�u�W�F�N�g
 * @returns ������0�ȊO�A���s��0�B
 */
    EXPORTS int MIGEMO_CALLTYPE
migemo_is_enable(migemo* obj)
{
    return obj ? obj->enable : 0;
}

#if 1
/*
 * ��Ƀf�o�b�O�p�̉B���֐�
 */
    EXPORTS void MIGEMO_CALLTYPE
migemo_print(migemo* object)
{
    if (object)
	mnode_print(object->mtree, NULL);
}
#endif
