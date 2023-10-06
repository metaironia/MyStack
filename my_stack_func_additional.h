#ifndef MY_STACK_FUNC_ADDITIONAL_H
#define MY_STACK_FUNC_ADDITIONAL_H

#define HASH_PROTECTION 1
#define CANARY_PROTECTION 1
#define DEBUG 1

#define EL_FORMAT "%d"
#define CAN_FORMAT "%I64X"

typedef int Elem_t;
typedef unsigned long long Canary_t;

void LogFileClose (void);

#endif
