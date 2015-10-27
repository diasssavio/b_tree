#ifndef BTREE_H_
#define B_TREE_H_

#include <stdio.h>
#include <stdlib.h>

// B-tree degree
#define t 2

typedef struct btree{
	unsigned keys_number, leaf;
	int * keys;
	struct btree ** children;
} b_tree;

b_tree* init_tree();
b_tree* make_node();
b_tree* free_tree( b_tree* );
void print_tree( b_tree*, unsigned );
b_tree* search_key( b_tree*, int );
b_tree* divide_node( b_tree*, int, b_tree* );
b_tree* partial_insert( b_tree*, int );
b_tree* insert_key( b_tree*, int );
b_tree* remove_key( b_tree*, int );
b_tree* _remove_key( b_tree*, int );

#endif /* B_TREE_H_ */