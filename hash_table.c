#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HASH_THRESHOLD 0.75

struct hash_bin_link {
	char *key;
	int hash;
	void *value;
	struct hash_bin_link *next;
};

struct hash_table {
	struct hash_bin_link **bins;
	int size;
	int capacity;
};

struct hash_table *hash_create(int init_size) {
	struct hash_table *table = malloc(sizeof(struct hash_table));
	if (table == NULL) {
		return NULL;
	}
	table->bins = calloc(init_size, sizeof(struct hash_bin_link *));
	table->size = 0;
	table->capacity = init_size;
	return table;
}

int hash_string(char *key) {
	int len = strlen(key);
	int hash = 0;
	for (int i = 0; i < len; i++) {
		hash += key[i];
		hash = ((long) hash * 31) % 1000000007;
	}
	return hash;
}

int hash_put(struct hash_table *table, char *key, void *value) {
	if (table->size >= table->capacity * HASH_THRESHOLD) {
		int new_capacity = 2 * table->capacity;
		struct hash_bin_link **new_bins = calloc(new_capacity, sizeof(struct hash_bin_link *));
		if (new_bins == NULL) {
			return 0;
		}
		for (int i = 0; i < table->capacity; i++) {
			struct hash_bin_link *bin = table->bins[i];
			// for each link in each bin,
			// 1) find the new bin it belongs in
			// 2) append the link to the front of the new bin
			// 3) adjust bin pointer to the next link to be processed
			while (bin != NULL) {
				int new_index = bin->hash % new_capacity;
				struct hash_bin_link *new_next = new_bins[new_index];
				struct hash_bin_link *next = bin->next;
				bin->next = new_next;
				new_bins[new_index] = bin;
				bin = next;
			}
		}	
		free(table->bins);
		table->capacity = new_capacity;
		table->bins = new_bins;
	}

	struct hash_bin_link *new_link = malloc(sizeof(struct hash_bin_link));
	if (new_link == NULL) {
		return 0;
	}
	new_link->key = key;
	new_link->hash = hash_string(key);
	new_link->value = value;

	int bin_index = new_link->hash % table->capacity;
	new_link->next = table->bins[bin_index];
	table->bins[bin_index] = new_link;
	table->size += 1;

	return 1;
}

void *hash_get(struct hash_table *table, char *key) {
	struct hash_bin_link *link = table->bins[hash_string(key) % table->capacity];
	while (link != NULL) {
		if (strcmp(key, link->key) == 0) {
			return link->value;
		}
		link = link->next;
	}
	return NULL;
}

void hash_destroy(struct hash_table *table) {
	for (int i = 0; i < table->capacity; i++) {
		struct hash_bin_link *link = table->bins[i];
		while (link != NULL) {
			struct hash_bin_link *next = link->next;
			free(link);
			link = next;
		}
	}
	free(table->bins);
	free(table);
}

void hash_walk(struct hash_table *table, void *(func)(char *key, void *value)) {
	for (int i = 0; i < table->capacity; i++) {
		struct hash_bin_link *link = table->bins[i];
		while (link != NULL) {
			func(link->key, link->value);
			link = link->next;
		}
	}
}

/********** MAIN **********/

void print(char *key, char *value) {
	fprintf(stderr, "<%s, %s, %d>\n", key, value, hash_string(key));
}

int main() {
	clock_t t1 = clock();
	const int INIT_CAPACITY = 1000000;
	struct hash_table* table = hash_create(INIT_CAPACITY);
	for (int i = 0; i < INIT_CAPACITY * 2; i++) {
		char *buffer = malloc(10 * sizeof(char));
		sprintf(buffer, "%d", i);
		hash_put(table, buffer, buffer);
	}

//	hash_walk(table, *print);

	for (int i = 0; i < INIT_CAPACITY * 2; i++) {
		char *buffer = malloc(10 * sizeof(char));
		sprintf(buffer, "%d", i);

		char *received = hash_get(table, buffer);
		if (strcmp(buffer, received) != 0) {
			fprintf(stderr, "%d expected, %d received", buffer, received);
			break;
		}
	}
	clock_t t2 = clock();
	printf("%d", t2-t1);
}
