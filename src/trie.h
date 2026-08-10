// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// trie.h -

#pragma once

typedef struct
{
    size_t total_nodes;
    size_t low_count;
    size_t high_count;
    size_t child_count;
    size_t wordtail_count;

    int max_sibling_depth;
    size_t total_sibling_depth;

    int max_node_cmp_count;
    size_t total_node_cmp_count;
} trie_stat;

void trie_stat_print(const trie_stat *stat, const char *title);
