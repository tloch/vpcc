# Mipster Memory Access

*Legend:*
* E ... Emulator: Machine, Operating System
* U ... Userspace program

.   | Execution | Memory              | Access to Memory in E | Access to Memory in U
--- | --------- | ------------------- | --------------------- | ---------------------
E   | Native    | Native              | Native                | Memory Abstraction
U   | E         | Memory Abstraction  | Syscalls              | Native

The difference between native and abstracted access is illustrated on an example: Store a byte `'a'` at address `0x1000`.
Note: The example is very unlikely to work in real code due to memory protections.

**Native Access**

Native is the common method of accessing memory, i.e., memory is accessed just like in a regular C or C++ program.

```c
char* c = (char*)0x1000;
*c = 'a';
```

**Access via Memory Abstraction**

Memory (read: physical memory) is encapuslated into an object. This type of memory is emulated just like CPU registers are.
Access to this memory is enabled via the API of `Memory` which can be found in `emulator/machine/memory.h`.

```c
Memory* = Mips32Machine::Get().MachineState().physical_memory;
char c = 'a';
m->WriteBuffer(0x1000, &c, 1);
```
