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

# build scanner demo
cd userland/skeleton-parser/
make

# run scanner demo
../../out/Default/mipster a.out <parser.c
