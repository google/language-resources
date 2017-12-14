FROM java:8

RUN apt-get update && apt-get install -y \
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
RUN curl -k https://bootstrap.pypa.io/get-pip.py > get-pip.py && ls -l && python get-pip.py
RUN pip install PyICU

# Install android SDK
RUN wget https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip  \
      && mkdir -p -m 1777 /usr/local/android-sdk  \
      && unzip sdk-tools-linux-3859397.zip -d /usr/local/android-sdk  \
      && echo y | /usr/local/android-sdk/tools/bin/sdkmanager 'build-tools;26.0.3' 'platforms;android-24'

# Install bazel
RUN wget https://github.com/bazelbuild/bazel/releases/download/0.8.0/bazel-0.8.0-installer-linux-x86_64.sh \
      && bash bazel-0.8.0-installer-linux-x86_64.sh --user

# Add bazel to path.
ENV PATH=/root/bin:$PATH
