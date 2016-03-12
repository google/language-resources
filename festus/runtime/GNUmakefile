# festus/runtime/GNUmakefile
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# Copyright 2016 Google, Inc.
# Author: mjansche@google.com (Martin Jansche)

# Path to OpenFst installation.
OPENFST=/usr/local
#OPENFST=/usr/local/stow/openfst-1.3.4
#OPENFST=/usr/local/stow/openfst-1.4.1
#OPENFST=/usr/local/stow/openfst-1.5.0
#OPENFST=/usr/local/stow/openfst-1.5.1

# C++ compiler that supports C++11.
CXX=g++-4.8
#CXX=clang++-3.5
#CXX=clang++-3.6

CXXFLAGS=-std=c++11 -O2
CPPFLAGS=-I$(OPENFST)/include
LDFLAGS=-L$(OPENFST)/lib/fst -L$(OPENFST)/lib

# Link against dynamic libfst.so at a known runtime search path.
# Use this if you don't want to deal with LD_LIBRARY_PATH or ldconfig.
LDLIBS=-Wl,-rpath=$(OPENFST)/lib/fst -lfstngram -Wl,-rpath=$(OPENFST)/lib -lfst -ldl

# Link against static libfst.a.
# Requires OpenFst to have been configured with --enable-static.
#LDLIBS=-Wl,-Bstatic -lfstngram -lfst -Wl,-Bdynamic -ldl

# Link against dynamic libraries, but do not set runtime search path.
#LDLIBS=-lfstngram -lfst -ldl

g2p-lookup:	g2p-lookup.cc

total-weight:	total-weight.cc

clean:
	$(RM) g2p-lookup total-weight
