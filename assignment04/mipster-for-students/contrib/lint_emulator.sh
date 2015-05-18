#!/bin/bash

# Copyright (c) 2014, the Mipster Project Authors.  All rights reserved.
# Please see the AUTHORS file for details.  Use of this source code is governed
# by a BSD license that can be found in the LICENSE file.

./contrib/cpplint.py \
    emulator/*.cc \
    emulator/*.h \
    emulator/**/*.cc \
    emulator/**/*.h

exit $?
