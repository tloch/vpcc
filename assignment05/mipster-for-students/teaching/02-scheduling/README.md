# Scheduling

*Submission tag: assignment-2*

Assignment 2 is about implementing a scheduler to manage the CPU in Mipster.

More formally, implement:
* A priority-based scheduler as described below.
* In both, your libc and the OS, implement the following syscalls:
  * [sched_yield](http://man7.org/linux/man-pages/man2/sched_yield.2.html)
  * [nanosleep](http://man7.org/linux/man-pages/man2/nanosleep.2.html)
  * [fork](http://man7.org/linux/man-pages/man2/fork.2.html)
  * [getpriority/setpriority](http://man7.org/linux/man-pages/man2/getpriority.2.html)
  * [getpid/getppid](http://man7.org/linux/man-pages/man2/getpid.2.html)
* The user programs specified below.

## Prerequisites

Basic knowledge on scheduling algorithms.

Suggested reads:
* Modern Operating Systems \[[1](../references.md)\]:
  * Chapter 2.4 Scheduling (3rd ed: pages 145 - 163)
  * Chapter 3.1 No Memory Abstraction (3rd ed: pages 176 - 179)
* [Pintos documentation][pintos-mlfq] on multi-level feedback queue scheduling (without fixpoint arithmetic)
* [BSD Operating System][bsd] documentation \[[2](../references.md)\]

[pintos-mlfq]: http://cksystemsteaching.github.io/pintos-for-students/pintos_7.html#SEC131
[bsd]: http://en.wikipedia.org/wiki/Berkeley_Software_Distribution

## Booting Mipster

Unlike typical operating systems, we do not `fork` from an idle process or any
other kind of initialization process. Instead, we initially prepare the
system from a given set of binaries and priorities (see format). Still, we
implement the necessities to actually fork a given process or change its
priority.

### Command line format

We adapt the already known command line format to the following specification.

```sh
mipster [options] <binary-1> [ <argv-1> <priority-1> [ <binary-n> <argv-n> <priority-n>]]
```

* `options`: A set of options for Mipster itself, e.g., `-disassemble`. Those
  options are listed when called with `--help`.
* `binary`: File path to the binary.
* `argv`: Arguments for the previously noted binary quoted as string. Example:
  "arg1 arg2"
* `priority`: Priority for the previously noted binary as int. Example: 1

A full example calling two binaries in `/path1/out1` and `/path2/out2`.

```sh
mipster -disassemble /path1/out1 "one two three" 31 /path2/out2 "1.2 2.4" 31
```


## Priority scheduling

We implement a *dynamic priority-based* scheduling algorithm that uses 
*round-robin selection with fixed time slices* within the same priority.

### Booting and initialization

The initial binaries should be loaded in `OS::Init()`. The order in which the
are processed depends on the order specified on the command line. The previous
example would yield in `out1` being scheduled before `out2`.

### Priority specification

 Minimum | Maximum | Default |
:-------:|:-------:|:-------:|
    0    |   63    |    31   |  

### Time slices

Since Mipster emulates the machine, time is relative (even if not coded the
speed of light :)). We control the speed of execution by counting each 
timer interrupt call since the operating system booted in the variable
`ticks_`. A constant `kTicksPerSec` in `emulator/config.h` is then used
to create a time source. In the same file, the constant
`kTimeSlice` refers to the amount of time (in msec) a given process is able to
execute before getting interrupted by the scheduler.


## Caveats

### Memory

You have not implemented a proper memory abstraction so far, yet you should load
multiple binaries into the same memory location (inspect the *LOAD* directives
in the ELF binaries). One way to circumvent this problem is to also include 
physical memory in context switches ([old
machines](http://en.wikipedia.org/wiki/IBM_7090) also did that). This is
possible because Mipster only emulates a very small set of phyiscal memory 
(typically around 10MB). Context switching of memory for larger amounts would require
[swapping](http://en.wikipedia.org/wiki/Paging#Terminology) support.

### Interrupts

A timer interrupt may not fire at the exact time instant a process would like to
wake up. Consider this when implementing a mechansim for sleeping. (Hint: Corner
case with same priority.)


## User programs

Feel free to write small sample programs to check any intermediate state of implementation you have. For the assignment implement programs that are equivalent to the pseudo code below, i.e., while calls to
functions such as `print` might differ, the semantic of the program should stay the same.

**A**:
```python
sched_yield()
sleep(1)  # based on nanosleep() syscall
print("A")
```

**B**:
```python
sleep(1)
print("B")
```

Start those programs with with:

```sh
mipster A "" 31 B "" 31
```


**C**:
```python
fork()
if is_parent():
  for (i = 0; i < 10; i++):
    print("C %d %d\n", getpriority(), i)
    sched_yield()
else:
  setpriority(me, getpriority() + 1)
  for (i = 0; i < 10; i++):
    print("C' %d %d\n", getpriority(), i)
```

**D**:
```python
for (i = 0; i < 10; i++):
  print("D %d %d\n", getpriority(), i)
```

Start those programs with with:

```sh
mipster C "" 31 D "" 31
```
