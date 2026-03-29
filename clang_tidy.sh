#!/bin/bash

# Copyright (c) 2026 Alex313031.

export HERE=${PWD} &&

export CLANG_TIDY_FILE=${HERE}/.clang-tidy &&

# Check header files first
clang-tidy --explain-config --config-file=${CLANG_TIDY_FILE} ${HERE}/src/{*.h,*.cc} &&
clang-tidy --explain-config --config-file=${CLANG_TIDY_FILE} ${HERE}/src/logging/{*.h,*.cc} &&
clang-tidy --explain-config --config-file=${CLANG_TIDY_FILE} ${HERE}/src/stress/{*.h,*.cc} &&

# Check files in osinfo dir
clang-tidy --explain-config --config-file=${CLANG_TIDY_FILE} ${HERE}/osinfo/{*.h,*.cc} &&

exit 0
