#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "my_stack_func.h"

FILE *LOG_FILE = fopen ("log.txt", "w");

enum StackFuncStatus StackCtor (Stack *stk, int64_t stack_capacity) {

    CANARY_ON ((stk -> left_canary) = STACK_CANARY);
    CANARY_ON ((stk -> right_canary) = STACK_CANARY);

    (stk -> capacity) = stack_capacity;
    (stk -> stack_size) = 1;
    StackDataCtor (stk);

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);

    return OK;
}

enum StackFuncStatus StackDataCtor (Stack *stk) {

    int64_t stack_size_bytes = CANARY_ON (2 * INT_MAX_BYTES +)
                                    (stk -> capacity) * sizeof (Elem_t);

    CANARY_ON (if (stack_size_bytes % INT_MAX_BYTES != 0)
                   stack_size_bytes += INT_MAX_BYTES - (stack_size_bytes % INT_MAX_BYTES));

    (stk -> data) = (Elem_t *) calloc ((size_t) stack_size_bytes, 1);

    CANARY_ON (*(Canary_t *) (stk -> data) = STACK_CANARY);
    CANARY_ON (*(Canary_t *) ((char *) (stk -> data) + stack_size_bytes - INT_MAX_BYTES) =
                                                                                STACK_CANARY);

    CANARY_ON ((stk -> data) = (Elem_t *) ((char *) (stk -> data) + INT_MAX_BYTES));

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);


    return OK;
}

enum StackFuncStatus StackDtor (Stack *stk) {

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);

    if (StackDataDtor (stk) == StackFuncStatus::OK) {

        free (stk);
        stk = NULL;

        return OK;
    }

    return FAIL;
}
//TODO data reset
enum StackFuncStatus StackDataDtor (Stack *stk) {

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);

    free ((char *)(stk -> data)
                CANARY_ON (- INT_MAX_BYTES));

    (stk -> data) = NULL;

    if (!(stk -> data))
        return OK;

    return FAIL;
}

enum StackFuncStatus StackPush (Stack *stk, Elem_t value) {

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);

    StackRecalloc (stk);

    (stk -> data)[(stk -> stack_size)++] = value;

    return OK;
}

enum StackFuncStatus StackPop (Stack *stk, Elem_t *ret_value) {

    assert (ret_value);

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);

    StackRecalloc (stk);

    *ret_value = (stk -> data)[--(stk -> stack_size)];
    (stk -> data)[(stk -> stack_size) + 1] = POISON_NUM;

    return OK;
}

enum StackFuncStatus StackRecalloc (Stack *stk) {

    if (StackOk (stk) != 0)
        STACK_DUMP (stk);

    int64_t new_capacity = 0;

    if ((stk -> stack_size) + 1 >= (stk -> capacity))
        new_capacity = (stk -> capacity) * 2; //TODO magic consts

    if ((stk -> stack_size) < (stk -> capacity) / 4)
        new_capacity = (stk -> capacity) / 4;

    if (new_capacity != 0) {

        Elem_t *previous_data = (stk -> data);
        (stk -> capacity) = new_capacity;

        StackDataCtor (stk);

        memcpy ((stk -> data), previous_data,
                (size_t) (sizeof (Elem_t) * ((stk -> stack_size) + 1)));

        return OK;
    }

    return NOTHING_DONE;
}

unsigned int StackOk (const Stack *stk) {

    CANARY_ON (

    int64_t stack_size_bytes = INT_MAX_BYTES + (stk -> capacity) * sizeof (Elem_t);

    if (stack_size_bytes % INT_MAX_BYTES != 0)
        stack_size_bytes += INT_MAX_BYTES - (stack_size_bytes % INT_MAX_BYTES);
    )

    unsigned int errors_in_stack = 0;

    if (!stk)
        errors_in_stack |= StackErrors::STACK_PTR_NULL;

    CANARY_ON (

    if ((stk -> right_canary) != STACK_CANARY || (stk -> left_canary) != STACK_CANARY ||
        *(Canary_t *)((char *)(stk -> data) - INT_MAX_BYTES) != STACK_CANARY ||
        *(Canary_t *)((char *)(stk -> data) + stack_size_bytes - INT_MAX_BYTES) != STACK_CANARY)

        errors_in_stack |= StackErrors::STACK_CANARY_DAMAGED;
    );

    if ((stk -> data) == NULL)
        errors_in_stack |= StackErrors::DATA_PTR_NULL;

    if ((stk -> stack_size) < 0)
        errors_in_stack |= StackErrors::NEGATIVE_SIZE;

    if ((stk -> capacity) < 0)
        errors_in_stack |= StackErrors::NEGATIVE_CAPACITY;

    if (errors_in_stack != 0) {

        fprintf (LOG_FILE, "Error %d occurred. \n", errors_in_stack);
    }

    return errors_in_stack;
}

enum StackFuncStatus StackDump (Stack *stk_for_dump, const char *file_called,
                            const char *func_called, const int line_called,
                            const char *stack_name) {

    CANARY_ON (

    int64_t stack_size_bytes = INT_MAX_BYTES + (stk_for_dump -> capacity) * sizeof (Elem_t);

    if (stack_size_bytes % INT_MAX_BYTES != 0)
        stack_size_bytes += INT_MAX_BYTES - (stack_size_bytes % INT_MAX_BYTES);
    )

    fprintf (LOG_FILE,         "Stack [0x%p] \"%s\" from %s(%d) %s() \n"
                          "    { \n"
               CANARY_ON ("    left canary = 0x" CAN_FORMAT "\n")
               CANARY_ON ("    right canary = 0x" CAN_FORMAT  "\n")
                          "    size = %I64d \n"
                          "    capacity = %I64d \n"
                          "    data [0x%p] \n"
                          "        { \n"
               CANARY_ON ("        left data canary = 0x" CAN_FORMAT "\n"),
                          stk_for_dump, stack_name,
                          file_called, line_called, func_called,
               CANARY_ON ((stk_for_dump -> left_canary), (stk_for_dump -> right_canary),)
                          (long long int) (stk_for_dump -> stack_size),
                          (long long int) (stk_for_dump -> capacity),
                          (stk_for_dump -> data)
               CANARY_ON  (, *(Canary_t *)((char *)(stk_for_dump -> data) - INT_MAX_BYTES)));

    for (int i = 0; i < (stk_for_dump -> capacity); i++) {

        if ((stk_for_dump -> data[i]) != POISON_NUM)
            fprintf (LOG_FILE, "        *[%d] = " EL_FORMAT "\n", i, (stk_for_dump -> data[i]));
        else
            fprintf (LOG_FILE, "        *[%d] = POISON_NUM \n", i);

    }

    fprintf (LOG_FILE,
               CANARY_ON ("        right data canary = 0x" CAN_FORMAT "\n")
                          "        } \n"
                          "    }     \n"
               CANARY_ON (, *(Canary_t *)((char *)(stk_for_dump -> data) +
                          stack_size_bytes - INT_MAX_BYTES)));

    return OK;
}

void LogFileClose (void) {

    fclose (LOG_FILE);
}

