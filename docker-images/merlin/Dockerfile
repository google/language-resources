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
# Copyright 2018 Google, Inc.

FROM langtech/base-festival:v1_1

# Required by run_merlin.py
ENV USER root

RUN apt-get update && apt-get install -y \
  autoconf \
  build-essential \
  ca-certificates \
  cmake  \
  csh \
  gawk \
  libatlas3-base \
  libncurses5-dev \
  lsb-release \
  python-numpy \
  python-scipy \
  python-tk \
  zlib1g-dev && \
  rm -rf /var/lib/apt/lists/*

# Install python pip
WORKDIR /opt
RUN curl -k https://bootstrap.pypa.io/get-pip.py > get-pip.py && ls -l && python get-pip.py

# Clone merlin
WORKDIR /usr/local/src/
RUN git clone https://github.com/CSTR-Edinburgh/merlin

# Set to known commit
WORKDIR /usr/local/src/merlin
RUN git reset 626bdb98911b181eab10f3a644346f64965bf755 --hard

# Remove size check.
RUN sed -i '551d'  /usr/local/src/merlin/src/run_merlin.py

# Setup merlin dependencies
WORKDIR /usr/local/src/merlin
RUN pip install -r requirements.txt  && \
        cd tools && ./compile_tools.sh

# Setup merlin env
WORKDIR /usr/local/src/merlin
RUN sh src/setup_env.sh
ENV THEANO_FLAGS "cuda.root=/usr/local/8.0,floatX=float32,on_unused_input=ignore"
