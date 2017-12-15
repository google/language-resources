FROM langtech/base-festival

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
RUN git reset cc8a65ce2a70db21dab148011c72e3dfeff03eff --hard

# Remove size check.
RUN sed -i '511d'  /usr/local/src/merlin/src/run_merlin.py

# Setup merlin dependencies
WORKDIR /usr/local/src/merlin
RUN pip install -r requirements.txt  && \
        cd tools && ./compile_tools.sh

# Setup merlin env
WORKDIR /usr/local/src/merlin
RUN sh src/setup_env.sh
ENV THEANO_FLAGS "cuda.root=/usr/local/8.0,floatX=float32,on_unused_input=ignore"
