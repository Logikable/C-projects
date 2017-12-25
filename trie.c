#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct trie_node {
	char *word;
	struct trie_node **children;
};

struct trie_node *trie_create() {
	struct trie_node *root = calloc(1, sizeof(struct trie_node));
	if (root == NULL) {
		return NULL;
	}
	return root;
}

int trie_add(struct trie_node *root, char *word) {
	if (root == NULL || word == NULL) {
		return 0;
	}

	int len = strlen(word);
	struct trie_node *cur = root;
	for (int i = 0; i < len; i++) {
		if (cur->children == NULL) {
			struct trie_node **children = malloc(26 * sizeof(struct trie_node *));
			if (children == NULL) {
				return 0;
			}
			cur->children = children;

			for (int j = 0; j < 26; j++) {
				struct trie_node *child = calloc(1, sizeof(struct trie_node));
				if (child == NULL) {
					return 0;
				}
				cur->children[j] = child;
			}
		}
		cur = cur->children[word[i] - 'a'];
	}
	if (cur->word != NULL && strcmp(cur->word, word) == 0) {
		return 0;
	}
	cur->word = strdup(word);
	return 1;
}

int trie_remove(struct trie_node *root, char *word) {
	if (root == NULL || word == NULL) {
		return 0;
	}

	struct trie_node *cur = root;
	int len = strlen(word);
	for (int i = 0; i < len; i++) {
		if (cur == NULL) {
			return 0;
		}
		if (cur->children == NULL) {
			return 0;
		}
		cur = cur->children[word[i] - 'a'];
	}
	if (cur->word != NULL && strcmp(cur->word, word) == 0) {
		free(cur->word);
		return 1;
	}
	return 0;
}

int trie_contains(struct trie_node *root, char *word) {
	if (root == NULL || word == NULL) {
		return 0;
	}

	struct trie_node *cur = root;
	int len = strlen(word);
	for (int i = 0; i < len; i++) {
		if (cur == NULL || cur->children == NULL) {
			return 0;
		}
		cur = cur->children[word[i] - 'a'];
	}
	return cur->word != NULL && strcmp(cur->word, word) == 0;
}

void trie_destroy(struct trie_node *node) {
	if (node == NULL) {
		return;
	}
	if (node->children != NULL) {
		for (int i = 0; i < 26; i++) {
			if (node->children[i] != NULL) {
				trie_destroy(node->children[i]);
			}
		}
		free(node->children);
	}
	if (node->word != NULL) {
		free(node->word);
	}
	free(node);
}

void trie_walk_words(struct trie_node *node, void (func)(char *)) {
	if (node == NULL || func == NULL) {
		return;
	}

	if (node->word != NULL) {
		func(node->word);
	}
	if (node->children != NULL) {
		for (int i = 0; i < 26; i++) {
			trie_walk_words(node->children[i], func);
		}
	}
}

/***** MAIN ******/

char randChar() {
	return (rand() % 26) + 'a';
}

char *randWord(int len) {
	char *word = malloc((len + 1) * sizeof(char));
	for (int j = 0; j < len; j++) {
		word[j] = randChar();
	}
	word[len] = '\0';
	return word;
}

void test() {
	srand(time(NULL));
	const int NUM_WORDS = 100000;
	const int WORD_LEN = 16;

	struct trie_node *root = trie_create();
	char **all_words = malloc(NUM_WORDS * sizeof(char *));
	for (int i = 0; i < NUM_WORDS; i++) {
		char *word = randWord(WORD_LEN);
		assert(trie_add(root, word) == 1);
		all_words[i] = word;
	}

	for (int i = 0; i < NUM_WORDS; i++) {
		assert(trie_contains(root, all_words[i]));
	}

	trie_destroy(root);
	for (int i = 0; i < NUM_WORDS; i++) {
		free(all_words[i]);
	}
	free(all_words);
}

int main() {
	clock_t t1 = clock();
	test();
	clock_t t2 = clock();
	printf("Success!\n%ld microseconds\n", t2-t1);
}
