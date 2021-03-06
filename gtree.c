#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct gtree_node {
	char *name;
	void *value;
	struct gtree_node *parent;
	struct gtree_node *prev;
	struct gtree_node *next; // right brother
	struct gtree_node *first_child;
	struct gtree_node *last_child;
};

struct gtree {
	char *name;
	struct gtree_node *root;
};

struct gtree_node *tree_create_node(char *name, void *value) {
	struct gtree_node *node = calloc(1, sizeof(struct gtree_node));
	if (node == NULL) {
		return NULL;
	}
	node->name = strdup(name);
	node->value = value;
	return node;
}

struct gtree *tree_create(char *name) {
	if (name == NULL) {
		return NULL;
	}

	struct gtree *tree = malloc(sizeof(struct gtree));
	if (tree == NULL) {
		return NULL;
	}
	tree->name = strdup(name);

	char buffer[128];
	sprintf(buffer, "/%s", name);
	char *root_name = strdup(buffer);

	struct gtree_node *root = tree_create_node(root_name, NULL);
	free(root_name);
	if (root == NULL) {
		return NULL;
	}
	
	root->parent = root;
	tree->root = root;
	return tree;
}

int tree_insert_child(struct gtree_node *parent, struct gtree_node *child) {
	if (parent == NULL || child == NULL) {
		return 0;
	}
	if (parent->first_child == NULL) {	// consequently, last_child is also NULL
		parent->last_child = NULL;
	} else {
		parent->first_child->prev = child;
	}
	child->next = parent->first_child;
	parent->first_child = child;
	child->parent = parent;
	return 1;
}

void node_destroy(struct gtree_node *node, void (func)(void *)) {
	free(node->name);
	func(node->value);
	if (node->next != NULL) {
		node_destroy(node->next, func);
	}
	if (node->first_child != NULL) {
		node_destroy(node->first_child, func);
	}
	free(node);
}

void tree_destroy(struct gtree *tree, void (func)(void *)) {
	if (tree == NULL || func == NULL) {
		return;
	}
	node_destroy(tree->root, func);
	free(tree->name);
	free(tree);
}

int tree_remove_node(struct gtree_node *removed) {
	if (removed == NULL || removed->parent == removed) {	// root cannot be removed
		return 0;
	}

	if (removed->prev != NULL) {	// not first child
		removed->prev->next = removed->first_child;
	}
	removed->first_child->prev = removed->prev;
	if (removed->next != NULL) {	// not last child
		removed->next->prev = removed->last_child;
	}
	removed->last_child->next = removed->next;

	// update parent of all children
	struct gtree_node *cur_child = removed->first_child;
	cur_child->parent = removed->parent;
	while (cur_child != removed->last_child) {
		cur_child = cur_child->next;
		cur_child->parent = removed->parent;
	}
}

struct gtree_node *tree_next_node(struct gtree_node *node) {
	if (node == NULL) {
		return NULL;
	}
	if (node->first_child != NULL) {
		return node->first_child;
	}
	// otherwise, return the first ancestor with a neighbour
	// works with root edgecase
	while (node->next == NULL) {
		if (node->parent == node) {
			return NULL;
		}
		node = node->parent;
	}
	return node->next;
}

void tree_walk_node(struct gtree_node *node, void (func)(void *)) {
	func(node->value);
	if (node->next != NULL) {
		tree_walk_node(node->next, func);
	}
	if (node->first_child != NULL) {
		tree_walk_node(node->first_child, func);
	}
}

void tree_walk(struct gtree *tree, void (func)(void *)) {
	if (tree == NULL || func == NULL) {
		return;
	}
	tree_walk_node(tree->root, func);
}

/******** MAIN *********/

void print(void *value) {
	if (value != NULL) {
//		fprintf(stderr, "%s\n", (char *) value);
	}
}

void free_val(void *value) {
	free(value);
}

void test() {
	const int ENTRIES = 1000000;
	const int CHILDREN = 10;

	struct gtree *tree = tree_create("tree");
	struct gtree_node **nodes = malloc(ENTRIES * sizeof(struct gtree_node *));
	nodes[0] = tree->root;

	for (int i = 1; i < ENTRIES; i++) {
		char *buffer = malloc(10 * sizeof(char));
		sprintf(buffer, "%d", i);

		struct gtree_node *child = tree_create_node(buffer, buffer);
		nodes[i] = child;

		int parent = i / CHILDREN;
		assert(tree_insert_child(nodes[parent], child) == 1);
	}

	tree_walk(tree, print);

	struct gtree_node *next = tree->root;
	while (next != NULL) {
//		print(next->value);
		next = tree_next_node(next);
	}

	tree_destroy(tree, free_val);
	free(nodes);
}

int main() {
	clock_t t1 = clock();
	test();
	clock_t t2 = clock();
	printf("%ld microseconds\n", t2-t1);
}
