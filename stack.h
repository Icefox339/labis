struct Stack {
    void* data;
    struct Stack* nextElement;
};

struct Stack* createStack(void);
struct Stack* top(struct Stack* stack);
struct Stack* pop(struct Stack* stack);
void push(struct Stack* stack, void* data);
int isEmptyStack(struct Stack* stack);