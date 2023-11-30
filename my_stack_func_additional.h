#ifndef MY_STACK_FUNC_ADDITIONAL_H
#define MY_STACK_FUNC_ADDITIONAL_H

/// Status of hash protection (0 - off, 1 - on)
#define HASH_PROTECTION 1

/// Status of canary protection (0 - off, 1 - on)
#define CANARY_PROTECTION 1

/// Status of debug (0 - off, 1 - on)
#define DEBUG 1

/// Printing format of element of data in stack.
#define EL_FORMAT "%d"

/// Printing format of canaries in stack.
#define CAN_FORMAT "%llx"

/// Type definition of canaries in stack.
typedef unsigned long long Canary_t;

/// Function exists to close log file.
void LogFileClose (void);

#endif
