#!/bin/sh

set -x;

# configure userland/cross-common.mk
sed --silent '/BEGIN CROSS COMMON/,/END CROSS COMMON/ p' Makefile >mipster/userland/cross-common.mk

# enter mipster directory
cd mipster-for-students

# specified build workflow, copied from readme file
contrib/install_deps.sh
contrib/get_gyp.sh
build/gyp/gyp --depth=. emulator.gyp
V=1 make

# build compiler
cd userland/skeleton-parser/
make

# compile and run demo 1
../../out/Default/mipster a.out <tests/assignment05.c
../../out/Default/mipster -debug_registers -load_raw a.mipster

# compile and run demo 2
../../out/Default/mipster a.out <tests/factorial.c
../../out/Default/mipster -debug_registers -load_raw a.mipster

# compile and run demo 3
../../out/Default/mipster a.out <tests/human.c
../../out/Default/mipster -debug_registers -load_raw a.mipster
