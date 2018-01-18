FROM langtech/base-merlin

WORKDIR /usr/local/src/voice/
ADD synth.sh .

WORKDIR /usr/local/src/voice/data/festvox

# Setup resources needed
RUN cp /usr/local/src/language-resources/si/festvox/lexicon.scm .
RUN cp /usr/local/src/language-resources/si/festvox/txt.done.data .
# Test only one speaker
RUN sed -i '/sin_9228*/!d' txt.done.data
RUN cp /usr/local/src/language-resources/si/festvox/ipa_phonology.json .

# Download sinhala wavs
RUN mkdir wavs && cd wavs && wget http://www.openslr.org/resources/30/si_lk.tar.gz && \
tar -xf si_lk.tar.gz && \
find . -type f -not -name 'sin_9228*' -print0 | xargs -0 rm --
