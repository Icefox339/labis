#include <stdlib.h>

#include "vector.h"

Vector* createVector(void) {
    Vector* v = malloc(sizeof(Vector));
    if (v) {
        v->data = NULL;
        v->next = NULL;
        return v;
    }
    return v;
}

void push_back(Vector* v, void* data) {

    if (v->data == NULL) {
        v->data = data;
        v->next = NULL;
        return;
    }

    Vector* new = createVector();
    new->data = data;
    while (v->next != NULL) {
        v = v->next;
    }
    v->next = new;
}

int isEmptyVector(Vector* v) {
    if (v->data == NULL && v->next == NULL) {
        return 1;
    }
    return 0;
}

int vectorSize(Vector* v) {
    int size = 0;
    if (v->data != NULL) {
        size++;
    }
    while (v->next != NULL) {
        size++;
        v = v->next;
    }
    return size;
}

void* vGet(Vector* v, int index) {
    if (index < 0 || index >= vectorSize(v)) {
        return NULL;
    }
    if (index == 0) {
        return v->data;
    }
    int i = 0;
    while (i < index) {
        v = v->next;
        i++;
    }
    return v->data;
}