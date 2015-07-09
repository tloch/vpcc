# Virtual Memory

*Submission tag: assignment-3*

Assignment 3 is about implementing virtual memory and the connected userspace
parts on the Mipster platform.

More formally, implement:
* Virtual memory as described below.
* In both, your libc and the OS, implement the following syscalls:
  * [mmap](http://man7.org/linux/man-pages/man2/mmap.2.html)
  * [munmap](http://man7.org/linux/man-pages/man2/mmap.2.html)
* In your libc:
  * An allocator as described below.
* The user programs specified below.
* Remove the code that saves/restores the full memory in a context switch.


## Prerequisites

Basic knowledge on virtual memory (page tables, free lists, page repldacement
algorithms).

Suggested reads:
* Modern Operating Systems \[[1](../references.md)\]:
  * Chapter 3 Memory Management (3rd ed: pages 175 - 247)


## Virtual memory in Mipster

In order to provide proper isolation for processes we will implement virtual
memory using page tables, page table entries, and free lists. Neither of those
parts (concepts) and approaches are set in stone. However, as virtual memory is
usually tied to hardware support (and hardware usually stays compatible), most
virtual memory systems will look similar.

### Hardware emulation

As all hardware in Mipster is emulated, so is its memory management unit. To be
exact, it is implemented (or rather should now be implemented) in:

```c++
int32_t Instruction::AddressTranslation(int32_t virtual_address);
```

As Mipster does not provide a translation lookaside buffer (TLB, p.195), the
pseudo-code for a MMU implementation could be:

```python
AddressTranslation(int32_t virtual_address):
  page_table = OS::Get().CurrentPageTable();
  phyiscal_address, error = page_table.lookup(virtual_address);
  if (error):
    physical_address = OS::Get().PageFault(virtual_address);
  return physical_address;
```

Note that the following methods are either available as
[stub](http://en.wikipedia.org/wiki/Method_stub) or still need to be properly
declared and implemented.

```c++
int32_t    OS::PageFault(int32_t virtual_address);
PageTable& OS::CurrentPageTable();
```

### Kernel subsystem

Isolation is enforced on process level. As a result, the data structures
managing virtual memory are also tied to a process.

#### Pages and page tables

In order for the hardware emulation to properly translate a virtual address into
a physical address, the OS needs to set up a so-called *page table*. The purpose
of the page table is to look up a single virtual address and get its
corresponding physical address. Managing mappings per address (memory is byte
addressed) would require enormous page tables. A solution is to handle chunks of
virtual address space, so-called *pages*. Note that while the lookup in a page table
should work on address level, all the bookkeeping is done one page level.

Address to page translation for pages of size 4k:

| bits | 31 - 12 | 11 - 0 |
| ---: |:-------:|:------:|
| **description** | *page number*  |   *offset in page*    |

In a virtual memory system a page can either be mapped or unmapped, meaning that
it can have a corresponding physical page frame, or not. If it has a physical
page frame we can just translate the address, i.e., rewrite the upper parts
(bits 12-31) of the address by looking up the page table entry (see below). If
the page is unmapped, it needs to be assigned a physical page frame.

#### Page table entry

Each entry of the page table, called *page table entry*, keeps information of
the current mapping, i.e., at the absolute minimum the physical page frame to
map to. See below for other information that might be needed.

#### Free list of physical page frames

A way (not the only one) to manage *physical page frames* is to keep them in a
free list. Note that since this list deals with physical memory, we only have
one in the system! Also note that in actual operating systems this is more
complex as the memory to manage the list also needs to reside in the memory
subsystem.

As memory is allocated and mapped into a page table (or multiple pages tables of
multiple processes -- 1:1), the free list of page frames will eventually get
empty. Since a physical page frame is still needed, the operating system needs
to decide which physical page to evict into other storage, i.e., swap out onto a
disk (in Mipster: emulator memory). The process of deciding the page to be used
is called *page replacement algorithm*. For our purposes it suffices to
implement a FIFO policy. A good way to keep track of already allocated physical
page frames is again a list.

Eviction of a page can happen by writing its contents into some other storage in
the emulator. Note that is necessary for the emulated MMU to cause a page fault
for evicted pages. As a result, the status of whether a page is mapped in or
evicted needs to be part of the page table entry.

### Syscalls

The syscalls `mmap` and `munmap` can be used to request new memory for the
userspace programs. They should be implemented in a reasonable way, i.e., no
hints, no protection, no I/O mappings!

### Caveats

* Upon transitioning to a virtual memory system the loader needs to be adapted
  to be aware of this situation. After all the purpose of this system is to
  isolate several processes from each other.
* As the emulator is not part of the loop, any part of the kernel that needs to
  write to userspace memory must be aware of which physical page is actually
  meant, i.e., they also need to translate any address they want to write to.
* Kernel writing to userspace can (rare case, but still)) hit an evicted page.
  The kernel needs to be aware of this.
* Stack access (through a user program) might also trigger page faults. (Should
  work out of the box.)

### Implementation Hints

* The free list of pages can be kept in a `std::list`.
* The page table maps page numbers (some form of int) on to a page table entry
  (some struct/class) that holds the physical page number and a present flag.
  The data structure to use is a hash map. In standard C++ you can use 
  [`std::unorded_map`](http://en.cppreference.com/w/cpp/container/unordered_map).
* In order to implement the FIFO policy for page eviction, a list needs to be
  maintained. This can be a `std::list`. The item in the list needs to map a
  physical page frame to a virtual one (described by its page table entry).


## Userspace: An allocator

Since using mmap in user programs is often an overkill (fragmentation,
performance), after all it can only retrieve memory on page level (4k), another
system for userspace is needed: an allocator.

The main purpose of an allocator is to manage (virtual) memory from within
userspace. It provides `malloc` and `free` calls with the following signatures:

```c
typedef unsigned long size_t;

void* malloc(size_t size);
void free(void* p);
```

* `malloc` allocates memory of a given size and returns a pointer to this
  memory. It returns `NULL` if it cannot allocate any more memory.
* `free` frees memory that has been allocated using `malloc`. Passing `NULL`
  as `p` results in doing nothing.

For this assignment it suffices to implement the following allocator:

* The allocator retrieves chunks of memory, called local allocation buffers
  (LAB), from the kernel using `mmap`. The size of a LAB is 4k.
* Each LAB is appended to a global list of LABs.
* Each LAB keeps a counter of allocated objects.
* Each LAB also keeps a pointer of the next usable address, called *bump pointer*.

Upon allocation (malloc) the following algorithm is performed.

*malloc:*

1. Check the list of LABs for a LAB:
  1. If there's no LAB, retrieve one using `mmap` and append it to that list.
  2. For each LAB try:
    1. Try to allocate an object in that LAB using this LAB's bump pointer.
    2. If the allocation succeeds (i.e. the object still fits in the LAB),
       increment the bump pointer and the counter. Return that object.
  3. If the object does not fit in any LAB, retrieve a new one using `mmap`,
     append it to the list, and retry at 1.

*free:*

1. Align the object's address to 4k to find the corresponding LAB.
2. Decrement the object counter.
3. If the counter hits 0, remove the LAB from the list, and `unmap` the memory.

## User programs

Feel free to write small sample programs to check any intermediate state of
implementation you have. For the assignment implement programs that are
equivalent to the pseudo code below, i.e., while calls to functions such as
print might differ, the semantic of the program should stay the same.

**A**:
```python
objs = 0;
obj_size = 16;
test_size = (1 << 20) * 30; // 30MB
while (true):
  mem = malloc(obj_size);
  if (mem == NULL):
    print("This should not happen!\n");
    return 1;
  objs++;
  if ((objs * obj_size) > test_size):
    printf("Allocated %d bytes of memory. Good bye!\n", objs * obj_size);
    return 0;
```

Start the programs with
```bash
mipster A "" 31
```
and
```bash
mipster A "" 31 A "" 31
```

**B**:
```python
objs = 0;
obj_size = 16;
num = 10000000;
while (true):
  mem = malloc(obj_size);
  if (mem == NULL):
    print("This should not happen!\n");
    return 1;
  objs++;
  free(mem);
  if (objs == num):
    print("Gracefully exiting.\n");
    return 0;
```

Start those programs with
```bash
mipster B "" 31
```
and
```bash
mipster A "" 31 B "" 31
```
and even
```bash
mipster A "" 31 B "" 31 A "" 31 B "" 31
```

