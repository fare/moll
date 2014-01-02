/*
A mindless slave, waiting to be taken over by another process
like a Rykor waiting for a Kaldane.

TODO: find minimal ways to
  * Have executable scratch space for bootstrapping,
    or mmap space from the outside ptracing process?
  * have dynamic loader symbol resolution,
    if possible from the outside process,
    otherwise from the inside as helpers available in the slave with -ldl
  * Determine useful linked addresses from outside.
    Whatever nm does, we can do on the executable to extract static linker values?
  * Something even simpler, with the PTRACE_TRACEME in the parent before the fork,
    with the parent synchronizing using waitpid(slave_pid).
  * See PinkTrace? http://dev.exherbo.org/~alip/pinktrace/ Also articles on lwn.net
  * Have a SYSENTER a fixed location — or just use one in the vsyscall page?
    Instead of hardwiring the offset into the vsyscall page,
    we can jump into the vsyscall and find the exact instruction for sure using PTRACE_SYSEMU.
  * Because it all happens in a different process, we can debug that interactively
    — assuming that for bootstrap purposes we already have a working interactive meta-environment.
*/

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
