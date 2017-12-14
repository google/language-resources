FROM langtech/base-merlin

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

# Setup festvox for given data
WORKDIR /usr/local/src/voice
RUN ls data/festvox && mkdir training && /usr/local/src/language-resources/festival_utils/build_festvox_voice.sh -t \
/usr/local/src/voice/data/festvox/wavs/ \
data \
training

# Generate utts and initial labels
WORKDIR /usr/local/src/voice/training
RUN ./bin/do_build parallel build_prompts
RUN ./bin/do_build label
RUN ./bin/do_clustergen parallel build_utts
RUN cd lab && ls *.lab -1 | sed -e 's/\.lab$//' > ../file_id_list.scp

# Setup merlin
WORKDIR /usr/local/src/voice/training
RUN /usr/local/src/language-resources/festival_utils/run_docker_merlin_training.sh -t

