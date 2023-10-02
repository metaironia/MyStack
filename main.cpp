#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "my_stack_func.h"

int main (void) {

    Stack test_stack = {};

    StackCtor (&test_stack);
    StackPush (&test_stack, 1);
    StackPush (&test_stack, 20);

    Elem_t stack_last_elem = POISON_NUM;

    StackPop(&test_stack, &stack_last_elem);

    printf ("%d\n", stack_last_elem);

    StackPop(&test_stack, &stack_last_elem);

    printf ("%d\n", stack_last_elem);

}

