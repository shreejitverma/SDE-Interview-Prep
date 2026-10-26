#include <stdio.h>
#include <stdlib.h>

__attribute__((noinline))
void trigger_crash(volatile int *ptr) {
    *ptr = 42;
}

__attribute__((noinline))
void process_transaction(volatile int *ptr) {
    trigger_crash(ptr);
}

int main(int argc, char **argv) {
    (void)argv;
    volatile int *ptr = (argc > 1000) ? (volatile int *)&argc : NULL;
    process_transaction(ptr);
    return 0;
}
