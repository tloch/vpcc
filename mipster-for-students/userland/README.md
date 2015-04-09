#Mipster userland

All Makefiles are configured to work with `mipsel-unknown-linux-uclibc`.

All following commands assume that the current working directory is `userland/`. 


## Requirements

A cross compilation toolchain set up in `$HOME/x-tools/`.


## Building an existing project

    cd <project>
    make
    # use binaries
    # clean up object files and executables
    make clean 


## Creating a new project

Mipster's userland support the following project types.

Description | build_identifier
------------|-----------------
Bare metal (all assembly) | as
Mipster's libc | cc-mipster
uClibc | cc-uclibc

To create a project simply create a new folder to contain your program

    mkdir <new-project>
    cd <new-project>

Then you need to create a Makefile in this directory that contains at least 2
statements:

    include ../cross-common.mk
    OBJECTS += <list your object files here>
    include ../cross-<build_identifier>

For example, if you want to create a project that uses uClibc and provides all
code in one file (main.c) you would create the following Makefile:

    include ../cross-common.mk
    OBJECTS += main.o
    include ../cross-cc-uclibc

For building against Mipster's libc (which has nothing but a syscall), see the
skeleton projects.


## Additional information

You can make use of the script `env.sh` to add the cross compilation toolchain
to your path (temporarily).

    . ./env.sh
