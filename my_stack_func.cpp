#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "my_stack_func.h"

FILE *LOG_FILE = fopen ("log.txt", "w");

enum StackFuncStatus StackCtor (Stack *stk, int64_t stack_capacity) {

    //CANARY_ON ((stk -> left_canary) = STACK_CANARY);
    CANARY_ON ((stk -> right_canary) = STACK_CANARY);

    (stk -> capacity) = stack_capacity;
    (stk -> stack_size) = 0;
    StackDataCtor (stk);

    STACK_VERIFY (stk);

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackDataCtor (Stack *stk) {

    int64_t stack_size_bytes = CANARY_ON (2 * MAX_CANARY_SIZE_BYTES +)
                                    (stk -> capacity) * sizeof (Elem_t);

    CANARY_ON (if (stack_size_bytes % MAX_CANARY_SIZE_BYTES != 0)
                   stack_size_bytes += MAX_CANARY_SIZE_BYTES - (stack_size_bytes % MAX_CANARY_SIZE_BYTES));

    (stk -> data) = (Elem_t *) calloc ((size_t) stack_size_bytes, 1);

    CANARY_ON (*(Canary_t *) (stk -> data) = STACK_CANARY);
    CANARY_ON (*(Canary_t *) ((char *) (stk -> data) + stack_size_bytes - MAX_CANARY_SIZE_BYTES) =
                                                                                STACK_CANARY);

    CANARY_ON ((stk -> data) = (Elem_t *) ((char *) (stk -> data) + MAX_CANARY_SIZE_BYTES));

    StackDataReset (stk);

    STACK_VERIFY (stk);

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackDtor (Stack *stk) {

    STACK_VERIFY (stk);

    if (StackDataDtor (stk) != StackFuncStatus::OK)
        return FAIL;

    free (stk);
    stk = NULL;

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackDataReset (Stack *stk) {

    STACK_VERIFY (stk);

    for (int64_t index = 0; index < (stk -> capacity); index++)
        (stk -> data)[index] = POISON_NUM;

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackDataDtor (Stack *stk) {

    STACK_VERIFY (stk);

    StackDataReset (stk);

    free ((char *)(stk -> data)
                CANARY_ON (- MAX_CANARY_SIZE_BYTES));

    (stk -> data) = NULL;

    if (stk -> data)
        return FAIL;

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackPush (Stack *stk, Elem_t value) {

    STACK_VERIFY (stk);

    StackRecalloc (stk);

    (stk -> data)[(stk -> stack_size)++] = value;

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackPop (Stack *stk, Elem_t *ret_value) {

    assert (ret_value);

    STACK_VERIFY (stk);

    if (((stk -> stack_size) - 1) < 0) {

        fprintf (LOG_FILE, "Cannot do StackPop().\n");
        STACK_DUMP (stk);
        return FAIL;
    }

    StackRecalloc (stk);

    *ret_value = (stk -> data)[(stk -> stack_size) - 1];
    (stk -> data)[--(stk -> stack_size)] = POISON_NUM;

    ON_DEBUG (STACK_DUMP (stk));

    return OK;
}

enum StackFuncStatus StackRecalloc (Stack *stk) {

    STACK_VERIFY(stk);

    int64_t new_capacity = 0;

    if ((stk -> stack_size) >= (stk -> capacity))
        new_capacity = (stk -> capacity) * INCREASE_AMOUNT;

    if ((stk -> stack_size) <= (stk -> capacity) / HOW_MUCH_STACK_DECREASES)
        new_capacity = (stk -> capacity) / DECREASE_AMOUNT;

    if (new_capacity != 0) {

        Elem_t *previous_data = (stk -> data);
        (stk -> capacity) = new_capacity;

        StackDataCtor (stk);

        memcpy ((stk -> data), previous_data,
                (size_t) (sizeof (Elem_t) * ((stk -> stack_size))));

        free (previous_data);

        ON_DEBUG (STACK_DUMP (stk));

        return OK;
    }

    ON_DEBUG (STACK_DUMP (stk));

    return NOTHING_DONE;
}

unsigned int StackOk (const Stack *stk) {

    CANARY_ON (

    int64_t stack_size_bytes = MAX_CANARY_SIZE_BYTES + (stk -> capacity) * sizeof (Elem_t);

    if (stack_size_bytes % MAX_CANARY_SIZE_BYTES != 0)
        stack_size_bytes += MAX_CANARY_SIZE_BYTES - (stack_size_bytes % MAX_CANARY_SIZE_BYTES);
    )

    unsigned int errors_in_stack = 0;

    if (!stk)
        errors_in_stack |= StackErrors::STACK_PTR_NULL;

    CANARY_ON (

    if ((stk -> right_canary) != STACK_CANARY || (stk -> left_canary) != STACK_CANARY ||
        *(Canary_t *)((char *)(stk -> data) - MAX_CANARY_SIZE_BYTES) != STACK_CANARY ||
        *(Canary_t *)((char *)(stk -> data) + stack_size_bytes - MAX_CANARY_SIZE_BYTES) != STACK_CANARY)

        errors_in_stack |= StackErrors::STACK_CANARY_DAMAGED;
    );

    if ((stk -> data) == NULL)
        errors_in_stack |= StackErrors::DATA_PTR_NULL;

    if ((stk -> stack_size) < 0)
        errors_in_stack |= StackErrors::NEGATIVE_SIZE;

    if ((stk -> capacity) < 0)
        errors_in_stack |= StackErrors::NEGATIVE_CAPACITY;

    if (errors_in_stack != 0) {

        fprintf (LOG_FILE, "Error ");

        for (int curr_err_num = STACK_ERRORS_AMOUNT - 1, curr_err_status = 0; curr_err_num >= 0; curr_err_num--) {

            curr_err_status = (((1 << curr_err_num) & errors_in_stack) == 0) ? 0 : 1;

            fprintf (LOG_FILE, "%d", curr_err_status);
        }

        fprintf (LOG_FILE, " occurred. \n");
    }

    return errors_in_stack;
}

enum StackFuncStatus StackDump (Stack *stk_for_dump, const char *file_called,
                            const char *func_called, const int line_called,
                            const char *stack_name) {

    CANARY_ON (

    int64_t stack_size_bytes = MAX_CANARY_SIZE_BYTES + (stk_for_dump -> capacity) * sizeof (Elem_t);

    if (stack_size_bytes % MAX_CANARY_SIZE_BYTES != 0)
        stack_size_bytes += MAX_CANARY_SIZE_BYTES - (stack_size_bytes % MAX_CANARY_SIZE_BYTES);
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
               CANARY_ON  (, *(Canary_t *)((char *)(stk_for_dump -> data) - MAX_CANARY_SIZE_BYTES)));

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
                          "          \n"
               CANARY_ON (, *(Canary_t *)((char *)(stk_for_dump -> data) +
                          stack_size_bytes - MAX_CANARY_SIZE_BYTES)));

    return OK;
}

void LogFileClose (void) {

    fclose (LOG_FILE);
}

