FROM langtech/base-language-resources

RUN apt-get update && apt-get install -y \
      automake \
      bc \
      curl \
      g++ \
      git \
      libc-dev \
      libreadline-dev \
      libtool \
      make \
      ncurses-dev \
      nvi \
      pkg-config \
      python \
      python-dev \
      python-setuptools \
      unzip \
      wavpack \
      wget \
      zip \
      zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

# Fetch and prepare Festival & friends
WORKDIR /usr/local/src/tools
ENV FESTIVAL_SUIT_PATH /usr/local/src/tools
RUN wget https://raw.githubusercontent.com/googlei18n/language-resources/master/festival_utils/setup_festival.sh && sh setup_festival.sh

# Set env variables
ENV ESTDIR /usr/local/src/tools/speech_tools
ENV FESTVOXDIR /usr/local/src/tools/festvox
ENV FESTIVALDIR /usr/local/src/tools/festival
ENV FLITEDIR /usr/local/src/tools/flite
ENV SPTKDIR /usr/local/src/tools/
ENV EXDIR /usr/local/src/tools/examples

# Add festival to path
ENV PATH="/usr/local/src/tools/festival/bin:${PATH}"
