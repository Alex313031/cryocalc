#!/bin/bash

# Copyright (c) 2026 Alex313031.

export HERE=${PWD}

export CLANG_FORMAT_FILE=${HERE}/.clang-format

clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/{*.cc,*.h} &&
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/logging/{*.cc,*.h} &&
clang-format --verbose -i --style=file:${CLANG_FORMAT_FILE} ${HERE}/src/stress/{*.cc,*.h} &&

exit 0
