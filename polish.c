#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t bool;
#define true 1
#define false 0

union entry {
	char c;
	float f;
};

struct chint_stack {
	union entry top;
	struct chint_stack *next;
};

void push(struct chint_stack **stack, union entry e) {
	struct chint_stack *new_top = malloc(sizeof(struct chint_stack));
	new_top->top = e;
	new_top->next = *stack;
	*stack = new_top;
}

union entry peek(struct chint_stack **stack) {
	return (*stack)->top;
}

union entry pop(struct chint_stack **stack) {
	union entry top = (*stack)->top;
	*stack = (*stack)->next;
	return top;
}

void free_stack(struct chint_stack **stack) {
	struct chint_stack *cur = *stack;
	struct chint_stack *next;
	while (cur != NULL) {
		next = cur->next;
		free(cur);
		cur = next;
	}
	free(stack);
}

bool is_operator(char *s) {
	if (strlen(s) < 1) {
		return false;
	}
	char first = s[0];
	return first == '+' ||
		first == '-' ||
		first == '*' ||
		first == '/';
}

float evaluate(char operator, float operand1, float operand2) {
	switch(operator) {
		case '+': return operand1 + operand2;
		case '-': return operand1 - operand2;
		case '*': return operand1 * operand2;
		case '/': return operand1 / operand2;
		default: break;
	}
	return 0;
}

int main() {
	char input[] = "- * / 15 - 7 + 1 1 3 + 2 + 1 1";

	struct chint_stack **operator_stack = malloc(sizeof(struct chint_stack *));
	struct chint_stack **operand_stack = malloc(sizeof(struct chint_stack *));
	bool pending_operand = false;

	char *next = strtok(input, " ");
	while (next != NULL) {
		if (is_operator(next)) {
			union entry e;
			e.c = next[0];
			push(operator_stack, e);
			pending_operand = false;
		} else {
			float operand1 = atoi(next);
			if (pending_operand) {
				while (*operand_stack != NULL) {
					float operand2 = pop(operand_stack).f;
					char operator = pop(operator_stack).c;
					operand1 = evaluate(operator, operand2, operand1);
				}
			}
			union entry e;
			e.f = operand1;
			push(operand_stack, e);
			pending_operand = true;
		}
		next = strtok(NULL, " ");
	}
	float result = pop(operand_stack).f;
	free_stack(operand_stack);
	free_stack(operator_stack);
	printf("%f\n", result);
}
