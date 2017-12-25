#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DYN_ARRAY_RESIZE_FACTOR 2

struct dyn_array {
	void **contents;
	int size;
	int capacity;
};

struct dyn_array *dyn_array_create(int init_size) {
	struct dyn_array *array = malloc(sizeof(struct dyn_array));
	if (array == NULL) {
		return NULL;
	}

	void **contents = malloc(init_size * sizeof(void *));
	if (contents == NULL) {
		return NULL;
	}
	array->contents = contents;
	array->size = 0;
	array->capacity = init_size;
	return array;
}

int dyn_array_append(struct dyn_array *array, void *value) {
	if (array == NULL || array->contents == NULL) {
		return 0;
	}

	if (array->size == array->capacity) {	// resize
		array->capacity *= DYN_ARRAY_RESIZE_FACTOR;
		array->contents = realloc(array->contents, array->capacity * sizeof(void *));
		if (array->contents == NULL) {
			return 0;
		}
	}
	array->contents[array->size] = value;
	array->size++;
	return 1;
}

int dyn_array_remove(struct dyn_array *array, int index) {
	if (array == NULL || array->contents == NULL || index < 0 || index >= array->size) {
		return 0;
	}
	array->contents[index] = NULL;
	return 1;
}

struct dyn_array *dyn_array_map(struct dyn_array *array, void *(func)(void *)) {
	if (array == NULL || array->contents == NULL || func == NULL) {
		return NULL;
	}
	struct dyn_array *new_array = dyn_array_create(array->capacity);
	for (int i = 0; i < array->size; i++) {
		new_array->contents[i] = func(array->contents[i]);
	}
	new_array->size = array->size;
	return new_array;
}

int dyn_array_inplace_map(struct dyn_array *array, void *(func)(void *)) {
	if (array == NULL || array->contents == NULL || func == NULL) {
		return 0;
	}
	for (int i = 0; i < array->size; i++) {
		array->contents[i] = func(array->contents[i]);
	}
	return 1;
}

void dyn_array_destroy(struct dyn_array *array) {
	if (array == NULL) {
		return;
	}
	void **contents = array->contents;
	if (contents != NULL) {
		for (int i = 0; i < array->size; i++) {
			free(contents[i]);
		}
		free(array->contents);
	}
	free(array);
}

/******* MAIN ********/

void test() {
	// TODO: test
}

int main() {
	clock_t t1 = clock();
	test();
	clock_t t2 = clock();
	printf("Success!\n%ld microseconds\n", t2-t1);
}
