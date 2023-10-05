#ifndef MY_STACK_FUNC_H
#define MY_STACK_FUNC_H

#define CANARY_PROTECTION

#define NAME_OF_VAR(x) #x

#define EL_FORMAT "%d"

#define CAN_FORMAT "%I64X"

#define STACK_DUMP(stk) StackDump(stk, __FILE__, __func__, __LINE__, NAME_OF_VAR(stk))

#define STACK_VERIFY(stk) do {                        \
                            if (StackOk (stk) != 0) { \
                                STACK_DUMP (stk);     \
                                return FAIL;          \
                            }                         \
                          } while (0)

#ifdef CANARY_PROTECTION
    #define CANARY_ON(...) __VA_ARGS__
#else
    #define CANARY_ON(...)
#endif


typedef int Elem_t;
typedef unsigned long long Canary_t;

const unsigned long long STACK_CANARY = 0xFEE1DEAD;

//const unsigned long long DEFAULT_STACK_CAPACITY = 1;

const int INT_MAX_BYTES = 8;

const int HOW_MUCH_STACK_INCREASES = 2;
const int HOW_MUCH_STACK_DECREASES = 4;

const Elem_t POISON_NUM = 0xDEAD;

extern FILE *LOG_FILE;

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
    NULL_SIZE            = 1 << 3,
    NEGATIVE_CAPACITY    = 1 << 4
};

enum StackFuncStatus {

    OK,
    FAIL,
    NOTHING_DONE
};

void LogFileClose (void);

enum StackFuncStatus StackCtor (Stack *stk, int64_t stack_capacity);

enum StackFuncStatus StackDataCtor (Stack *stk);

enum StackFuncStatus StackDtor (Stack *stk);

enum StackFuncStatus StackDataDtor (Stack *stk);

enum StackFuncStatus StackPush (Stack *stk, Elem_t value);

enum StackFuncStatus StackPop (Stack *stk, Elem_t *ret_value);

enum StackFuncStatus StackRecalloc (Stack *stk);

unsigned int StackOk (const Stack *stk);

enum StackFuncStatus StackDump (Stack *stk_for_dump, const char *file,
                            const char *func_called, const int line_called,
                            const char *stack_name);

#endif
