/*
 * Copyright (c) 2018-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __LISTS_H__
#define __LISTS_H__

/********* Bi-directional list operations ********/
/* Bi-directional list structure */
struct bi_list_node_t {
    struct bi_list_node_t *bprev;
    struct bi_list_node_t *bnext;
};

/* Init an empty node. */
#define BI_LIST_INIT_NODE(node) do {              \
    (node)->bnext = node;                         \
    (node)->bprev = node;                         \
} while (0)

/* Insert a new node after current node: (bnext) of current. */
#define BI_LIST_INSERT_AFTER(curr, node) do {     \
    (node)->bnext = (curr)->bnext;                \
    (node)->bprev = curr;                         \
    (curr)->bnext->bprev = node;                  \
    (curr)->bnext = node;                         \
} while (0)

/* Add one node into list as the tail: (bprev) of head. */
#define BI_LIST_INSERT_BEFORE(curr, node) do {    \
    (curr)->bprev->bnext = node;                  \
    (node)->bprev = (curr)->bprev;                \
    (curr)->bprev = node;                         \
    (node)->bnext = curr;                         \
} while (0)

/* Remove one node from the list. */
#define BI_LIST_REMOVE_NODE(node) do {            \
    (node)->bprev->bnext = (node)->bnext;         \
    (node)->bnext->bprev = (node)->bprev;         \
} while (0)

/* Is the head empty? */
#define BI_LIST_IS_EMPTY(head)      ((head)->bnext == (head))

/* The node's next node */
#define BI_LIST_NEXT_NODE(node)     ((node)->bnext)

/* Go through each node of a list */
#define BI_LIST_FOR_EACH(node, head)              \
    for (node = (head)->bnext; node != head; node = (node)->bnext)

/********* Uni-directional list operations ********/
/*
 * To use these single linked list operations, a head node must have been
 * defined already, and the "next" pointer initialized to "NULL". Like:
 * struct head_t {
 *      uint32_t data;
 *      User_Type *next;
 * } head;
 */

/* Initialize the head node */
#define UNI_LISI_INIT_HEAD(head) do {             \
    if ((head) != NULL) {                         \
        (head)->next = NULL;                      \
    }                                             \
} while (0)

/* Insert a node after current node */
#define UNI_LIST_INSERT_AFTER(curr, node) do {    \
    (node)->next = (curr)->next;                  \
    (curr)->next = node;                          \
} while (0)

/* Move a node after posi node */
#define UNI_LIST_MOVE_AFTER(posi, prev, node) do {\
    if (prev != NULL) {                           \
        (prev)->next = (node)->next;              \
        (node)->next = (posi)->next;              \
        (posi)->next = node;                      \
    }                                             \
} while (0)

/* Go through each node of a list */
#define UNI_LIST_FOR_EACH(node, head)             \
    for (node = (head)->next; node != NULL; node = (node)->next)

/* Go through each node of a list with prev node */
#define UNI_LIST_FOR_EACH_PREV(prev, node, head)  \
    for (prev = NULL, node = (head)->next;        \
                 node != NULL; prev = node, node = (prev)->next)

#endif /* __LISTS_H__ */
