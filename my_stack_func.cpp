#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "my_stack_func.h"

enum StackStatus StackCtor (Stack *stk) {

    (stk -> right_canary) = STACK_CANARY;

    (stk -> capacity) = STACK_CAPACITY;
    StackDataCtor ((stk -> data), (stk -> capacity));
    (stk -> stack_size) = 0;

    (stk -> right_canary) = STACK_CANARY;

    return StackStatus::STACK_CTOR_OK;
}

enum StackStatus StackDataCtor (Elem_t *stack_data, int64_t stack_capacity) {

    int64_t stack_size_bytes = 2 * INT_MAX_BYTES + stack_capacity * sizeof (Elem_t);

    if (stack_size_bytes % INT_MAX_BYTES != 0)
        stack_size_bytes += INT_MAX_BYTES - (stack_size_bytes % INT_MAX_BYTES);

    stack_data = (Elem_t *) calloc ((size_t) stack_size_bytes, 1);

    *(Canary_t *)stack_data = STACK_CANARY;
    *(Canary_t *)((char *)stack_data + stack_size_bytes - INT_MAX_BYTES) = STACK_CANARY;

    stack_data = (Elem_t *)((char *) stack_data + INT_MAX_BYTES);

    return StackStatus::DATA_GENERATION_OK;
}

enum StackStatus StackDtor (Stack *stk) {

    if (StackDataDtor (stk) == StackStatus::DATA_DESTRUCT_OK) {

        free (stk);
        stk = NULL;

        return StackStatus::STACK_DESTRUCT_OK;
    }

    return StackStatus::STACK_DESTRUCT_FAILED;
}

enum StackStatus StackDataDtor (Stack *stk) {

    free (stk -> data);
    (stk -> data) = NULL;

    if (!(stk -> data))
        return StackStatus::DATA_DESTRUCT_OK;

    return StackStatus::DATA_DESTRUCT_FAILED;
}

enum StackStatus StackPush (Stack *stk, Elem_t value) {

    StackRecalloc (stk);

    (stk -> data)[(stk -> stack_size)++] = value;

    return StackStatus::STACK_PUSH_OK;
}

enum StackStatus StackPop (Stack *stk, Elem_t *ret_value) {

    StackRecalloc (stk);

    *ret_value = (stk -> data)[(stk -> stack_size)];
    (stk -> data)[(stk -> stack_size)--] = POISON_NUM;

    return StackStatus::STACK_POP_OK;
}

enum StackStatus StackRecalloc (Stack *stk) {

    int64_t new_capacity = 0;

    if ((stk -> stack_size) + 1 >= (stk -> capacity))
        new_capacity = (stk -> capacity) * 2;

    if ((stk -> stack_size) < (stk -> capacity) / 4)
        new_capacity = (stk -> capacity) / 4;

    if (new_capacity != 0) {

        Elem_t *previous_data = (stk -> data);
        StackDataCtor ((stk -> data), new_capacity);
        memcpy ((stk -> data), previous_data,
                (size_t) (sizeof (Elem_t) * ((stk -> stack_size) + 1)));

        return StackStatus::SUCCESS_REALLOC;
    }

    return StackStatus::NO_REALLOC_DONE;
}

unsigned int StackOk (const Stack *stk) {

    int64_t stack_size_bytes = 2 * INT_MAX_BYTES + (stk -> capacity) * sizeof (Elem_t);

    if (stack_size_bytes % INT_MAX_BYTES != 0)
        stack_size_bytes += INT_MAX_BYTES - (stack_size_bytes % INT_MAX_BYTES);

    unsigned int errors_in_stack = 0;

    if (!stk)
        errors_in_stack |= StackErrors::STACK_PTR_NULL;

    if ((stk -> right_canary) != STACK_CANARY || (stk -> left_canary) != STACK_CANARY ||
        *(Canary_t *)((char *)(stk -> data) - INT_MAX_BYTES) != STACK_CANARY ||
        *(Canary_t *)((char *)(stk -> data) + stack_size_bytes - INT_MAX_BYTES) != STACK_CANARY)

        errors_in_stack |= StackErrors::STACK_CANARY_DAMAGED;

    if ((stk -> data))
        errors_in_stack |= StackErrors::DATA_PTR_NULL;

    if ((stk -> stack_size) < 0)
        errors_in_stack |= StackErrors::NEGATIVE_SIZE;

    if ((stk -> capacity) < 0)
        errors_in_stack |= StackErrors::NEGATIVE_CAPACITY;

    return errors_in_stack;
}
