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
# Copyright 2017 Google, Inc.

FROM java:8

# Versions of different tools installed.
ENV NODEJS_VERSION="v8.9.3"
ENV BAZEL_VERSION="0.8.0"
ENV ANDROID_TOOLS_VERSION="3859397"

RUN apt-get update && apt-get install -y \
      sox \
      curl \
      libicu-dev \
      g++ \
      git \
      python \
      python-dev \
      python-setuptools \
      unzip \
      wget \
      && rm -rf /var/lib/apt/lists/*

# Clone language resources
WORKDIR /usr/local/src
RUN git clone https://github.com/googlei18n/language-resources

# Load language resources python modules
ENV PYTHONPATH="/usr/local/src/language-resources"

# Install python pip
WORKDIR /opt
RUN curl -k https://bootstrap.pypa.io/get-pip.py > get-pip.py && ls -l && python get-pip.py && rm get-pip.py
RUN pip install PyICU

# Install android SDK
RUN wget https://dl.google.com/android/repository/sdk-tools-linux-${ANDROID_TOOLS_VERSION}.zip  \
      && mkdir -p -m 1777 /usr/local/android-sdk  \
      && unzip sdk-tools-linux-${ANDROID_TOOLS_VERSION}.zip -d /usr/local/android-sdk  \
      && echo y | /usr/local/android-sdk/tools/bin/sdkmanager 'build-tools;26.0.3' 'platforms;android-24' \
      && rm sdk-tools-linux-${ANDROID_TOOLS_VERSION}.zip

# Install bazel
RUN wget https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh \
      && bash bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh --user \
      && rm bazel-${BAZEL_VERSION}-installer-linux-x86_64.sh

# Add bazel to path
ENV PATH=/root/bin:$PATH

# Install Nodejs
WORKDIR /opt/
RUN wget http://storage.googleapis.com/gae_node_packages/node-${NODEJS_VERSION}-linux-x64.tar.gz \
      && tar -xf node-${NODEJS_VERSION}-linux-x64.tar.gz \
      && rm node-${NODEJS_VERSION}-linux-x64.tar.gz

ENV PATH $PATH:/opt/node-${NODEJS_VERSION}-linux-x64/bin

# Set common env vars
ENV NODE_ENV production
