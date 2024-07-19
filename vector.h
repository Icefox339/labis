typedef struct Vector {
    void *data;
    struct Vector *next;
} Vector;

Vector *createVector(void);
void push_back(Vector *v, void *data);
int isEmptyVector(Vector *v);
int vectorSize(Vector *v);
void *vGet(Vector *v, int index);
