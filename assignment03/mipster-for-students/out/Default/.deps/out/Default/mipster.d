cmd_out/Default/mipster := g++   -o out/Default/mipster -Wl,--start-group out/Default/obj.target/mipster/emulator/bin/main.o out/Default/obj.target/mipster/emulator/machine/memory.o out/Default/obj.target/mipster/emulator/machine/mips32.o out/Default/obj.target/mipster/emulator/machine/mips32_isa.o out/Default/obj.target/mipster/emulator/os/loader.o out/Default/obj.target/mipster/emulator/os/process.o out/Default/obj.target/mipster/emulator/os/os.o out/Default/obj.target/mipster/emulator/os/os_syscall.o -Wl,--end-group -lelf -lgflags
