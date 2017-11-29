FROM ppasindud/base-festival

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

# Install python pip.
WORKDIR /opt
RUN curl -k https://bootstrap.pypa.io/get-pip.py > get-pip.py && ls -l && python get-pip.py

WORKDIR /usr/local/src/
RUN git clone https://github.com/CSTR-Edinburgh/merlin

WORKDIR /usr/local/src/merlin
RUN pip install -r requirements.txt  && \
        cd tools && ./compile_tools.sh

WORKDIR /usr/local/src/merlin
RUN sh src/setup_env.sh


