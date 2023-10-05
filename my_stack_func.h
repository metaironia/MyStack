#ifndef MY_STACK_FUNC_H
#define MY_STACK_FUNC_H

#define CANARY_PROTECTION

#define NAME_OF_VAR(x) #x

#define El_format "%d"

#define Can_format "%I64X"

#define STACK_DUMP(stk) StackDump(stk, __FILE__, __func__, __LINE__, NAME_OF_VAR(stk))

#ifdef CANARY_PROTECTION
    #define CANARY_ON(...) __VA_ARGS__
#else
    #define CANARY_ON(...)
#endif

typedef int Elem_t;
typedef unsigned long long Canary_t;

const unsigned long long STACK_CANARY = 0xFEE1DEAD;

const unsigned long long STACK_CAPACITY = 1;

const int INT_MAX_BYTES = 8;

const Elem_t POISON_NUM = 0xDEAD;

struct Stack {

    Canary_t left_canary;
    Elem_t *data;
    int64_t capacity;
    int64_t stack_size;
    Canary_t right_canary;
};

enum StackErrors {

    STACK_PTR_NULL       = 1 << 0,
    STACK_CANARY_DAMAGED = 1 << 1,
    DATA_PTR_NULL        = 1 << 2,
    NEGATIVE_SIZE        = 1 << 3,
    NEGATIVE_CAPACITY    = 1 << 4
};

enum StackStatus {

    STACK_CTOR_OK,
    DATA_GENERATION_OK,
    STACK_DESTRUCT_OK,
    STACK_DESTRUCT_FAILED,
    DATA_DESTRUCT_OK,
    DATA_DESTRUCT_FAILED,
    STACK_PUSH_OK,
    STACK_POP_OK,
    SUCCESS_REALLOC,
    NO_REALLOC_DONE,
    SUCCESS_STACK_DUMP
};

enum StackStatus StackCtor (Stack *stk);

enum StackStatus StackDataCtor (Stack *stk);

enum StackStatus StackDtor (Stack *stk);

enum StackStatus StackDataDtor (Stack *stk);

enum StackStatus StackPush (Stack *stk, Elem_t value);

enum StackStatus StackPop (Stack *stk, Elem_t *ret_value);

enum StackStatus StackRecalloc (Stack *stk);

unsigned int StackOk (const Stack *stk);

enum StackStatus StackDump (Stack *stk_for_dump, const char *file,
                            const char *func_called, const int line_called,
                            const char *stack_name);

#endif
