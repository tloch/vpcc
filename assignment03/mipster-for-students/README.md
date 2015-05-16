# Mipster

Mipster is a MIPS32 emulator and framework for emulating parts of operating systems.

Mipster is constrained to the following environment:
* MIPS32
* Little endian (LE) ELF binaries on a LE host system
* Linux ABI (read: sys-v MIPS o32 syscalls and calling convention)
* Statically-linked binaries

It is known to work with uClibc (disabled threads, all statically linked).

## Why (yet) another MIPS emulator?

There are already quite a number of MIPS simulator available. We know of:
* [SPIM](http://pages.cs.wisc.edu/~larus/spim.html)
* [cspim](http://zvrba.net/software/cspim.html)
* vmips
* [Nachos](http://homes.cs.washington.edu/~tom/nachos/)
* gxemaul
* qemu

*So why build a new one?*

We think the following points justify building another environment (for teaching):
* For teaching OS algorithms it is beneficial to not have the OS running on the actual machine. Breaking the self-referentiality results in an experience that we think makes it easier to understand the actual algorithms.
* Debugging: Mipster can be debugged using gdb and common Linux utilities. The barrier of entry is quite low for people that already know how to debug regular C/C++ programs.
* Unlike SPIM, Mipster can directly load ELF (mips LE) binaries. This applies for all statically linked executables, even ones linked against uClibc.
* Unlike other systems, Mipster does not rely on tailored syscalls (it is NOT compatible with SPIM), but rather tries to stay compatible with Linux. As a result it is compatible with libraries and tools people know from the Linux world (uclibc, binutils and friends, ...)


## Building the emulator on Linux

The build process is known to work on Ubuntu 14.04.1.

```sh
contrib/install_deps.sh
contrib/get_gyp.sh
build/gyp/gyp --depth=. emulator.gyp
V=1 make
```

The emulator can be found in the directory out/Default.


## Building the emulator on OSX

The build process is known to work on OSX 10.9.5.

The build requires an installation of [MacPorts](https://www.macports.org/) in `/opt/local/lib/`.

```sh
sudo port install libelf gflags
contrib/get_gyp.sh
build/gyp/gyp --depth=. emulator.gyp
```

The project is set up to work with XCode, so just open `emulator.xcodeproj`.


## Getting a cross-compile toolchain

We use crosstool-ng to create our MIPS toolchain. 

```sh
git clone https://github.com/cksystemsteaching/crosstool-ng
cd crosstool-ng
git checkout se-ws14
```

Afterwards just follow the instructions on http://crosstool-ng.org/

For the interested reader: We build a variant of `mipsel-unknown-linux-uclibc` with
threads disabled.
