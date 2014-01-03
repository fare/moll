/*
A mindless slave, waiting to be taken over by another process
like a Rykor waiting for a Kaldane.

This slave assumes a byte-addressed machine with basic Unix runtime,
and word size 64 byte or smaller.
The master better know about the low-level details of the slave machine.
If it's remote, it can use the 'u' command to get uname details.
*/

#include <stdlib.h>
#include <stdint.h>
#include <endian.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/utsname.h>
#include <sys/mman.h>


#define PROTOCOL_VERSION 1

// Size of command packets in 8-byte words, then in bytes.
// It better be less than 512 octets, so it's portably smaller than PIPE_BUF
#define PACKET_WORDS 10
#define PACKET_BYTES (8*PACKET_WORDS)

void write_buffer (int out, void* buffer, size_t count)
{
        uint8_t* buf = buffer;
        ssize_t n;

        while(count) {
                n = write(out, buf, count);
                if (!n) {
                        exit(101);
                }
                buf += n;
                count -= n;
        }
}

void write_word (int out, uint64_t w)
{
        uint64_t le = htole64(w);
        write_buffer(out, &le, 8);
}

void write_counted_buffer (int out, void* buffer, size_t count)
{
        write_word(out, count);
        write_buffer(out, buffer, count);
}

void write_uname (int out)
{
        struct utsname u;
        int n = uname(&u);
        if (!n) {
                exit(102);
        }
        write_counted_buffer(out, &u, sizeof(u));
}


void read_buffer (int in, void* buffer, size_t count)
{
        uint8_t* buf = buffer;
        ssize_t n;
        while(count) {
                n = read(in, buf, count);
                if (!n) {
                        exit(103);
                }
                buf += n;
                count -= n;
        }
}

int main (int argc, char**argv)
{
        void* vector[] = { (void*)PROTOCOL_VERSION, &vector, &read_buffer, &write_buffer,
                           &dlopen, &dlsym, &dlerror, &dlsym, &dlclose };
        int in = 0;
        int out = 1;
        int n;
        union packet_t {
                uint8_t c[PACKET_BYTES];
                uint64_t w[PACKET_WORDS];
        } p;
        uint8_t* base;

        // Parse arguments: alternate i/o fds.
        for (n = 1; n <= argc; n++) {
                if (strcmp(argv[n],"-i")) {
                        in = atoi(argv[++n]);
                } else if (strcmp(argv[n],"-o")) {
                        out = atoi(argv[++n]);
                } else {
                        exit(104);
                }
        }

        // Command loop
        while(1) {
                read_buffer(in, &p, PACKET_BYTES);
                switch(p.c[0]) {
                case 'V': // Version
                        write_word(out, PROTOCOL_VERSION);
                        break;
                case 'u': // uname
                        write_uname(out);
                        break;
                case 'v': // dynamic linking vector
                        write_counted_buffer(out, vector, sizeof(vector));
                        break;
                case 'c': // call routine with (up to) 8 arguments, output result
                        write_word(out,
                                   ((uint64_t(*)(uint64_t,uint64_t,uint64_t,uint64_t,
                                                 uint64_t,uint64_t,uint64_t,uint64_t))(p.w[1]))
                                   (p.w[2], p.w[3], p.w[4], p.w[5], p.w[6], p.w[7], p.w[8], p.w[9]));
                        break;
                case 'r': // read (peek) data
                        write_buffer(out, (void*)p.w[1], p.w[2]);
                        break;
                case 'w': // write (poke) data
                        read_buffer(out, (void*)p.w[1], p.w[2]);
                        break;
                case 'p': // mmap program into existence, all in one swoop
                        base = mmap((void*)p.w[1], p.w[2], p.w[3], p.w[4], p.w[5], p.w[6]);
                        if (base == MAP_FAILED) {
                                exit(105);
                        }
                        read_buffer(in, base+p.w[7], p.w[8]);
                        // if given a start vector, call it, if not, output base address
                        if (p.w[9]) {
                                ((void(*)(void*,void*[]))(base+p.w[9]))(base, vector);
                        } else {
                                write_word(out, (uint64_t)base);
                        }
                        break;
                }
        }
}
