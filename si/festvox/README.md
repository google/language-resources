# Setting up Sinhala TTS

## Festvox Environment

Run the following command.

```
export FESTIVAL_SUIT_PATH=<your desire path to install festival>
mkdir ${FESTIVAL_SUIT_PATH} && ./third_party/festvox/setup.sh
```

## Voice

1. Download recordings from http://www.openslr.org/resources/30/si_lk.tar
  ```
  export WAV_FOLDER=<your desire path to store downloaded wav files>
  mkdir -p ${WAV_FOLDER} && wget http://www.openslr.org/resources/30/si_lk.tar && tar xzf si_lk.tar -C ${WAV_FOLDER}
  ```

2. The setup script from 1) would give out instruction to put export commands under bash.
Please follow strictly or always export by yourself before running the next step.


3. Train TTS Voice (make sure you are under langauge-resources repo directory)

  ```
  export OUTPUT_VOICE_FOLDER=<your desire path to store output voice>
  ./utils/build_festvox_voice.sh ${WAV_FOLDER} si ${OUTPUT_VOICE_FOLDER}
  ```

4. Synthesize one example sentence (This always needs to run from the produced output voice directory).

  ```
    cd ${OUTPUT_VOICE_FOLDER}
    echo 'ආයතනයක්වත්' |
    ${FESTIVALDIR}/bin/text2wave \
      -eval festvox/goog_si_unison_cg.scm \
      -eval '(voice_goog_si_unison_cg)' \
      > example.wav
  ```
