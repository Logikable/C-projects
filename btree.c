#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct btree_node {
	void *value;
	struct btree_node *parent;
	struct btree_node *left_child;
	struct btree_node *right_child;
};


