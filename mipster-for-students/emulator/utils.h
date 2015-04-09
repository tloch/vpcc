// Copyright (c) 2014, the Mipster Project authors.  All rights reserved.
// Please see the AUTHORS file for details.  Use of this source code is governed
// by a BSD license that can be found in the LICENSE file.

#ifndef EMULATOR_UTILS_H_
#define EMULATOR_UTILS_H_

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

const intptr_t kKiloByte = 1024;
const intptr_t kMegaByte = 1024 * kKiloByte;


#define UNREACHABLE()                                                          \
  fprintf(stderr, "unreachable code\n");                                       \
  abort();


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// For colored output.
#define COLORIZE 1

#ifdef COLORIZE
#define RESET       "\033[0m"
#define BLACK       "\033[30m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"
#define BOLDBLACK   "\033[1m\033[30m"
#define BOLDRED     "\033[1m\033[31m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDYELLOW  "\033[1m\033[33m"
#define BOLDBLUE    "\033[1m\033[34m"
#define BOLDMAGENTA "\033[1m\033[35m"
#define BOLDCYAN    "\033[1m\033[36m"
#define BOLDWHITE   "\033[1m\033[37m"
#else
#define RESET       ""
#define BLACK       ""
#define RED         ""
#define GREEN       ""
#define YELLOW      ""
#define BLUE        ""
#define MAGENTA     ""
#define CYAN        ""
#define WHITE       ""
#define BOLDBLACK   ""
#define BOLDRED     ""
#define BOLDGREEN   ""
#define BOLDYELLOW  ""
#define BOLDBLUE    ""
#define BOLDMAGENTA ""
#define BOLDCYAN    ""
#define BOLDWHITE   ""
#endif  // COLORIZE


inline void VDebugModule(const char* module, const char* format, va_list argptr) {
    const int kLineLength = 256;
    int rest = kLineLength;
    char line[kLineLength];

    int written = snprintf(line, rest, "%s\t", module);
    assert(written >= 0);
    rest -= written;
    assert(rest > 0);
    written = vsnprintf(&line[kLineLength - rest], rest, format, argptr);
    rest -= written;
    fprintf(stdout, "%s" RESET "\n", line);
}


inline void DebugModule(const char* module, const char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    VDebugModule(module, format, argptr);
    va_end(argptr);
}


inline void ErrorOut(int exit_code,
                     const char* module,
                     const char* format, ...) {
    const int kLineLength = 256;
    int rest = kLineLength;
    char line[kLineLength];

    int written = snprintf(line, rest, "%s\t" RED "ERROR: ", module);
    assert(written >= 0);
    rest -= written;
    assert(rest > 0);
    va_list argptr;
    va_start(argptr, format);
    written = vsnprintf(&line[kLineLength - rest], rest, format, argptr);
    rest -= written;
    va_end(argptr);
    fprintf(stdout, "%s" RESET "\n", line);
    exit(exit_code);
}


#endif  // EMULATOR_UTILS_H_
