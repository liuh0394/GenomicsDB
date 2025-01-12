#!/bin/bash

# The MIT License (MIT)
# Copyright (c) 2019-2020 Omics Data Automation, Inc.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

set -e 

PREREQS_ENV=${PREREQS_ENV:-$HOME/genomicsdb_prereqs.sh}

# Install GenomicsDB Dependencies
HOMEBREW_NO_AUTO_UPDATE=1
brew list cmake &>/dev/null || brew install cmake
brew list mpich &>/dev/null || brew install mpich
brew list ossp-uuid &>/dev/null || brew install ossp-uuid
brew list libcsv &>/dev/null || brew install libcsv
brew list automake &> /dev/null || brew install automake
brew list openssl@1.1 &> /dev/null || brew install openssl@1.1
brew list zstd &> /dev/null || brew install zstd
echo "export OPENSSL_ROOT_DIR=$(brew --prefix openssl@1.1)" >> $PREREQS_ENV
