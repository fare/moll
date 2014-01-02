// A mindless slave, waiting to be taken over by another process
// like a Rykor waiting for a Kaldane.

#include <stdlib.h>
#include <sys/ptrace.h>

void (* volatile vector) (void) = NULL;

char data[1<<12]; // preallocate 4KB of scratch space, to start with.

volatile int main (int argc, char**argv)
{
        register void (*function) ();
        (void)argc;
        (void)argv;

        while(1) {
                ptrace(PTRACE_TRACEME);
                function = vector;
                if (function) {
                        (*function)();
                }
        }
}
