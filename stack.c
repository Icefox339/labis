#include <stdlib.h>
#include <string.h>

#include "stack.h"

struct Stack* createStack(void) {
	struct Stack* newStack = (struct Stack*)malloc(sizeof(struct Stack) * 1);
	if (newStack) {
		newStack->data = NULL;
		newStack->nextElement = NULL;
	}
	return newStack;
}

struct Stack* top(struct Stack* stack) {
	while (stack->nextElement != NULL)
		stack = stack->nextElement;

	return stack;
}

struct Stack* pop(struct Stack* stack) {
	struct Stack* nextElement = stack->nextElement;

	if (nextElement == NULL) {
		struct Stack* copyStack = (struct Stack*)malloc(sizeof(struct Stack));
		if (copyStack != NULL) {
			copyStack->data = stack->data;
			stack->data = NULL;
			stack->nextElement = NULL;
			return copyStack->data;
		}
	}

	if (nextElement != NULL) {
		while (nextElement->nextElement != NULL) {
			stack = stack->nextElement;
			nextElement = stack->nextElement;
		}
	}


	stack->nextElement = NULL;
	if (nextElement != NULL) {
		return nextElement->data;
	}
	else {
		return 0;
	}
}

void push(struct Stack* stack, void* data) {
	if (stack->data == NULL) {
		stack->data = data;
		return;
	}

	if (stack->nextElement != NULL)
		stack = top(stack);

	struct Stack* newElement = createStack();
	newElement->data = data;
	stack->nextElement = newElement;
	return;
}


int isEmptyStack(struct Stack* stack) {
	if (stack->data == NULL && stack->nextElement == NULL) {
		return 1;
	}
	return 0;
}