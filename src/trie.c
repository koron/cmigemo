// vim:set ts=8 sts=4 sw=4 tw=0 et:
//
// trie.c -

#include <stdio.h>

#include "trie.h"

void
trie_stat_print(const trie_stat *stat, const char *title)
{
    printf("=== %s\n", title ? title : "");
    printf("Total Nodes          : %zu\n", stat->total_nodes);
    printf("Wordtail Nodes       : %zu\n", stat->wordtail_count);
    printf("Pointer Counts       : low=%zu, high=%zu, child=%zu\n",
            stat->low_count, stat->high_count, stat->child_count);
    printf("Low/High Balance Ratio: %.2f%% / %.2f%%\n",
            stat->low_count + stat->high_count > 0
                    ? (double)stat->low_count
                              / (stat->low_count + stat->high_count) * 100.0
                    : 0.0,
            stat->low_count + stat->high_count > 0
                    ? (double)stat->high_count
                              / (stat->low_count + stat->high_count) * 100.0
                    : 0.0);
    printf("Sibling Depth        : max=%d, avg=%.2f\n", stat->max_sibling_depth,
            stat->total_nodes > 0
                    ? (double)stat->total_sibling_depth / stat->total_nodes
                    : 0.0);
    printf("Node Compare Count   : max=%d, avg=%.2f\n",
            stat->max_node_cmp_count,
            stat->total_nodes > 0
                    ? (double)stat->total_node_cmp_count / stat->total_nodes
                    : 0.0);
    printf("Word Compare Count   : max=%d, avg=%.2f\n",
            stat->max_node_cmp_count,
            stat->wordtail_count > 0
                    ? (double)stat->total_node_cmp_count / stat->wordtail_count
                    : 0.0);
    printf("===\n");
}
