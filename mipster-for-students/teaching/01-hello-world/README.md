# Hello World!

*Submission tag: assignment-1*

This assignment is about getting Mipster up and running. On the way the
following necessities need to be implemented:

* MIPS32 instructions required to execute a ["Hello World" program](http://en.wikipedia.org/wiki/%22Hello,_world!%22_program). 
  To be exact, the following instructions are missing and need to be implemented:
  * SetOnLessThanInstruction
  * OrImmediateInstruction
  * AddImmediateUnsignedWordInstruction
  * JumpInstruction
  * JumpAndLinkInstruction
  * LoadWordInstruction
* Required [syscalls] in the mipster libc to write a string (`char*`) to stdout.
  * [write](http://linux.die.net/man/2/write)
* Required kernel interface to write a string to stdout.
  * [write](http://en.wikipedia.org/wiki/Write_(system_call))
* Mipster libc functions for reading `argv[0]` (read: the program name)
  * This includes a function for computing the string length of a given [C byte string](http://en.wikipedia.org/wiki/C_string_handling).


## Creating the "Hello World" project

Before adding your own implementations, create copies from the skeleton
projects. Do *not* edit the skeleton files themselves as we may update those
throughout the course.

Creating a project from those skeletons:

```sh
cd userland/
cp -r skeleton-hello-world my-hello-world
cp -r skeleton-mipster-libc my-mipster-libc
cd my-hello-world
# EDIT Makefile to point to the right mipster libc directory (=../my-mipster-libc)
make
```

You should then see a file `a.out` in the same directory. Check that the file's
signature (use the `file` utility) is similar to the following

```sh
$ file a.out
a.out: ELF 32-bit LSB  executable, MIPS, MIPS32 rel2 version 1, statically linked, not stripped
```


## Implementing MIPS32 instructions

The semantics and encoding of all instructions can be found in the [MIPS manual]. 

The idea is to enable disassembly output in mipster (`-disassemble`) and follow implement 
one instruction after another until the small *Hello World* program completes.

### Caveats

#### Branch delay slot

In order to satisfy the requirements for MIPS32 it is necessary to implement [delay slots](http://en.wikipedia.org/wiki/Delay_slot) for branching instructions.

Example

```gas
bne $v0, $v1, offset  # branch on not equal: if $v0 != $v1 then branch to next PC + offset
lw $v2, 0(SP)         # load word in delay slot: load 0(SP) (= 1st param on stack) into $v2
```
Here, no matter wether we take the branch or not, the instruction following the branch
instruction (`lw` follows) will be executed.


When writing assembly by hand this can be simply be avoided by issuing a `nop`.

```gas
bne $v0, $v1, offset
nop
```

By implication this also means that by using a compiler that emits delay slots (`gcc` does)
the emulating machine also needs to handle them.

The class hierarchy for instructions makes sure that once an instruction is declared to
need a delay slot, this delay slot is automatically fetched and decoded. The delay slot can
be executed by calling `ExecuteDelaySlot()` from within the instruction that is declared
with a delay slot.

Note: A taken branch assigns the target address to the PC during the instruction 
time of the instruction in the branch delay slot. This means that the disassembly will
show the PC of the to-be-executed instruction when executing the delay slot.

### Testing newly implemented instructions

A good indication is to check whether you improve (read: get further) in
executing the current program. A better test is to check against corner cases
and run a test suite. Mipster makes use of a CPU torture test suite for this task.

```sh
cd userland/cpu-torture-tests
make
```

Among others you should see the following files:
* cputorture-arith
* cputorture-logic
* cputorture-loadstore
* cputorture-branch

You can execute Mipster using these test files from the base repository directory

```sh
./out/Default/mipster -torture-mode userland/cpu-torture-tests/cputorture-arith
```

The CPU tests use the following semantics.
* `break 0`: Successful execution.
* `break 1`: Modified source register(s).
* `break 2`: Encountered invalid value.
* `break 3`: Did *not* trap on overflow or any other exception.

Note: *Don't* forget to include `-torture_mode`, as this tell the emulator to
skip an instruction after receiving an exception. The `break 3`follows an
instruction that should trap, so we need to skip one instruction after an
exception to signal the we properley caught it.


## Syscalls in Mipster's libc (caller side)

We aim to stay as compatible as possible to [Linux MIPS syscall
ABI](http://www.linux-mips.org/wiki/Syscall) which for a 32 bit systems follows
the [o32 ABI](../docs/mips32abi.pdf).

The base implementation for our syscalls can be found in
`userland/skeleton-mipster-libc/syscalls.c`. The prototype for the basic system
call is is

```c
int mipster_syscall(int num, int arg_1, int arg_2, int arg_3, int arg_4);
```

The provided implementation is limited to 4 arguments. All other syscalls should
be based on this one. For example, the appropriate write syscall could be
declared as (see [write documentation](http://linux.die.net/man/2/write))

```c
int mipster_write(int fd, char* buf, int count);
```


## Syscalls in Mipster's kernel (callee side)

Upon encountering a syscall, the MIPS machine transfers control (after switching to
privileged mode) to the operating system. In the emulator this is reflected by 
calling

```c
void OS::Syscall();
```

From there on the operating system can examine the arguments (following o32 ABI)
and process the request. The usual way to do this is to inspect the first argument
(register `a0`) and delegate further processing to more specialized handlers.


## Relevant files

A list of files relevant for this assignment

```sh
emulator/machine/mips32_isa.cc
emulator/os/os.cc
emulator/os/os_syscalls.cc
userland/my-mipster-libc/*
userland/my-hello-world/*
```

[syscalls]: http://www.linux-mips.org/wiki/Syscall
[MIPS manual]: ../docs/MipsInstructionSetReference.pdf
