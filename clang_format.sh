#!/bin/bash

# Copyright (c) 2026 Alex313031.

export HERE=${PWD} &&

export CLANG_FORMAT_FILE=${HERE}/.clang-format &&

# Check header files first
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/{*.h,*.cc} &&
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/logging/{*.h,*.cc} &&
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/stress/{*.h,*.cc} &&

# Check files in osinfo dir
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/osinfo/{*.h,*.cc} &&

exit 0
