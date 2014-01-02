/*
A mindless slave, waiting to be taken over by another process
like a Rykor waiting for a Kaldane.

TODO: find minimal ways to
  * Have executable scratch space for bootstrapping.
    Can/should we do it all via ptrace-driven calls to mmap, shmget, etc?
    We could "simply" have a SYSCALL at a fixed location (better not rely on vsyscall).
    Or have some helper function prepackage this initialization?
  * Have dynamic loader symbol resolution.
    I don't think it's maintainably possible from the outside process,
    so we may have to use libdl helpers inside the initial slave.
  * Determine useful base addresses from outside.
    Whatever nm does, we can do on the executable to extract static linker values?
    Otherwise we can cheat and write the address to a pipe to the master.
  * We can use PTRACE_TRACEME in the parent before the fork,
    with the parent synchronizing using waitpid(slave_pid).
    Or we can use that communiation pipe to do things in userland.
  * Because it all happens in a different process, we can debug that interactively
    — assuming that for bootstrap purposes we already have a working interactive meta-environment.
  * Ideally, for each architecture, have a trivial executable, that just uses SYSCALL to exit(),
    or possibly one that forever loops in a deep sleep(). This minimizes the attack surface,
    and makes whole-program static code analysis possible.
    However, it doesn't play well with reusing dynamically linked libraries, libc, etc.,
    that defeat the whole-program static code analysis, anyway.
    A control pipe can also be easier and less system-dependent, and can be made remote,
    or go across security barriers; however, it has a mini interpreter loop at fixed address,
    which can be a big security concern for code injection, and should be mmap()ed out before
    calling unsafe libraries, or at least randomized.
    Therefore, a minimal ptrace-only slave is best in a completely self-contained world,
    but a control pipe slave is more versatile and incremental, so is the low-hanging fruit.
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
