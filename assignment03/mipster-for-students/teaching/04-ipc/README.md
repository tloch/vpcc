# Inter-process Communication (IPC) using Shared Memory

*Submission tag: assignment-4*

Assignment 4 is about implementing the infrastructure to enable well-defined
([race free](http://en.wikipedia.org/wiki/Race_condition#Software))
communicaton between multiple processes based on shared memory,
and different methods to ensure mutual exclusion.

More formally, implement (for details see below):
* The syscalls and infrastructure in the Mipster kernel to enable sharing memory 
  between processes.
* A userspace implementation for mutual exclusion based on [Lamport's bakery algorithm](http://en.wikipedia.org/wiki/Lamport%27s_bakery_algorithm).
* Syscalls and infrastructure for semaphores.
* The program incrementing a shared counter using the userspace locks and a
  variant using semaphores.


Note that there exist 
[many concepts](http://en.wikipedia.org/wiki/Inter-process_communication#Approaches)
of providing infrastructure for inter-process communication and this 
assignment only covers a small portion of possible approaches.

## Prerequisites

Suggested reads:
* Concepts of virtual memory (recall [assignment 3](../03-virtual-memory))
* Modern Operating Systems \[[1](../references.md)\]:
  * Chapter 2.3 Interprocess Communication (3rd ed: pages 117 - 145)
* The Art of Multiprocessor Programming \[[3](../references.md)\]:
  * Chapter 1 (Introduction) and Chapter 2 (Principles)


## Sharing memory across processes

Many different models for sharing memory accross processes exist (POSIX,
System-V, etc.). The model to be implemented follows the System-V specification
and is based on the system calls
[shmget](http://man7.org/linux/man-pages/man2/shmget.2.html),
[shmat](http://man7.org/linux/man-pages/man2/shmat.2.html), and
[shmdt](http://man7.org/linux/man-pages/man2/shmat.2.html).

The system calls do not need to implement security checks but should follow the
described API, i.e., be usable in the following way:

```c
int known_key = 1234;    // A key known to all participating processes.
size_t size = 1U << 12;  // A size for the segment that is rounded to the system's page size.

int shared_segment_id = shmget(known_key, size, IPC_CREAT);
// Error handling for shmget.

void* shared_segment_addr = shmat(shared_id, NULL, 0);
// Error handling for shmat.

shmdt(shared_segment_addr);
// Error handling for shmdt.
```

Note that size is a multiple of a system's page size. As a result the mechanism
for virtual memory (hint: page table entry) can be used to handle shared memory.

Wasting memory because shared segments are aligned to pages is not an issue!


## Synchronization in userspace

### Barriers

Applications are often divided into phases of execution. A *barrier* can be used
to block a process until all processes have reached the barrier.

Implement a barrier that complies to the following specification.
```c
void barrier_wait(int key, int process_id, int num_processes);
```

The `key` for each barrier is known by all processes and can be used to allocate
a shared memory segment. Similarly, the `process_id` and `num_processes` are
known in advance (= compile time). The `process_id` can be different from the
operating system's `pid`.

Hint: The function could be implemented by setting the state of a processes in
one slot while observing other processes slots.

### Locks (using bakery algorithm)

In order to avoid race conditions implement Lamport's bakery algorithm to
provide mutual exclusion for processes on a shared memory segment.

The API should conform to the following specification

```c

struct bakery_lock {
  int num_processes;
  int* entering;  // An array of num_processes length.
  int* number;    // An array of num_processes length.
};

bakery_lock* bakery_init(int key, int num_processes);
void bakery_lock(bakery_lock* b, int process_id);
void bakery_unlock(bakery_lock* b, int process_id);
```

The `process_id` should be used as an index into `bakery_lock.entering` and
`bakery_lock.number` and can be known in advance. In other words, processes know
their ids (with the first being 0) at compile time.

The function `bakery_init` should allocate a shared memory segment and
initialize the member `num_processes`. Since all participating processes know
this number in advance, this write can happen in a raceful fashion.

Example memory layout for a call of 
`bakery_init(136, 3 , 4)`:

```
0x04 /* = # of processes; struct bakery_lock starts here */
0x00 /* = entering[0] (start of entering) */
0x00 /* = entering[1] */
0x00 /* = entering[2] */
0x00 /* = entering[3] */
0x00 /* = number[0] (start of number) */
0x00 /* = number[1] */
0x00 /* = number[2] */
0x00 /* = number[3] */
...  /* rest is wasted */
```


## Synchronization in kernelspace

### Semaphores

Implement the following pieces needed to provide unnamed semaphores as specified
by POSIX. 

```c
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);
```

[`sem_init`](http://man7.org/linux/man-pages/man3/sem_init.3.html): The function
takes a pointer to a shared memory segment and initializes a semaphore value to
some `value`. The flag `pshared` is required to be non-zero.

[`sem_wait`](http://man7.org/linux/man-pages/man3/sem_wait.3.html): The function
decrements (locks) the semaphore pointed to by `sem`. If the semaphore's value
is greater than zero, then the decrement proceeds, and the function returns,
immediately. If the semaphore currently has the value zero, then the call blocks
until it becomes possible to perform the decrement (i.e., the semaphore value
rises above zero).

[`sem_post`](http://man7.org/linux/man-pages/man3/sem_post.3.html): The function
increments (unlocks) the semaphore pointed to by `sem`. If the semaphore's value
consequently becomes greater than zero, then another process blocked in a
`sem_wait` call will be woken up and proceed to lock the semaphore.

A more detail description can be found at
[sem_overview](http://man7.org/linux/man-pages/man7/sem_overview.7.html).

## User programs

Feel free to write small sample programs to check any intermediate state of
implementation you have. For the assignment implement programs that are
equivalent to the pseudo code below, i.e., while calls to functions such as
print might differ, the semantic of the program should stay the same.

**A**:
```python
process_id = atoi(argv[0]);
num_processes = 2;

// Ids serving as keys for shared memory.
barrier_id = ...
lock_id = ...
memory_id = ...

counter_memory = get_memory_for_counter(memory_id);
lock = bakery_init(lock_id, process_id, num_processes);
barrier_wait(barrier_id, process_id, num_processes);

local_counter = 0;
while(get_counter(counter_memory) < 100):
  bakery_lock(lock, process_id);
  increment_counter(counter_memory);
  local_counter++;

printf("local counter: %d\n", local_counter);
```

Start the programs with
```bash
mipster A "0" 31 A "1" 31
```

Additionally, wrap your semaphore calls into a proper lock interface and provide
a program **B** that substitutes the bakery lock with those semaphore based
locks.
