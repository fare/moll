My Own Linear Lisp
==================

Goal: low key experiment with a Linear Lisp, create a maru-style evaluator.

* Implementation language: racket? gerbil? maru? SBCL? ocaml? Coq?


Slave
-----

The slave implements a trivial low-level protocol
for a slave process to be commanded by a master process,
by default via pipes for stdin and stdout.
The master process is supposed to do all the hard work,
including compiling code to binary, and
either linking it or making it position-independent.
The master may query for protocol version
and for machine architecture data from uname, in case it's unsure
(for instance, if the slave is on a remote machine).
A vector is provided for either master- or slave- side linking.
The slave makes all dynamic libraries available for use,
which is great for practical purposes, but
can also make security analysis harder.

For static analysis, a better slave would be a specially crafted
architecture-specific statically-linked assembly program
that contains but one exit instruction,
with all the program being ptrace'd into existence —
the master would fork, PTRACE_TRACEME in the slave, and waitpid in the parent,
then exec the slave and use ptrace to modify it into something useful and minimal,
the minimal aspect making whole-program static analysis easier.
However, that's harder to work with, and cannot reuse any dynamic library
(short of reimplementing and maintaining a compatible dynamic linker,
or an entire compilation toolchain).
Therefore, at least to start with, we have this trivial protocol.
The program main may still mmap existing things out of existence,
if it wants its image to be minimal.


Bibliography
------------

Linear Lisp

* Henry Baker's paper trove
  for basic ideas about a Linear Lisp
	http://home.pipeline.com/~hbaker1/

* Alan Bawden's thesis (is it "Linear Graph Reduction: Confronting the Cost of Naming" ?)
  for a complete Scheme implementation on top of a Linear Graph Reduction engine

* Jesse A. Tov's thesis "Practical Programming with Substructural Types"
  for a practical approach to dealing with linearity in a static language
	http://www.eecs.harvard.edu/~tov/pubs/dissertation/

* ATS
  for a complete static language with dependent types and linear types
	http://www.ats-lang.org/

* Computability Logic
  For the game semantics interpretation of linear operators, and
  the universal abstraction quantifier vs the regular computational quantifiers.
	http://www.csc.villanova.edu/~japaridz/CL/clx.html#Lecture_notes


Lisp Implementation to get inspired from or embrace

* Ian Piumarta's Maru
  for its short bootstrapping loop
	http://piumarta.com/hg/maru

* Abdulaziz Ghuloum's Ikarus Scheme
  for its incremental approach to compilation
  "An Incremental Approach to Compiler Construction",
  and extended draft tutorial, "Compilers: Backend to Frontend and Back to Front Again".
	https://github.com/namin/inc

* Marc Feeley's Gambit
  for clean virtual machine design.

* PLT Racket
  for its well layered "module" system that make it a multi-language platform.

* Dimitris Vyzovitis's Gerbil
  for its attempt to cleanly bootstrap a Racket-like platform on top of Gambit.

