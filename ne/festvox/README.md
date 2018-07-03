# Setting up Nepali TTS

## Festvox Environment

Run the following command (This should be executed while in the cloned language-resources folder)

```
export FESTIVAL_SUIT_PATH=<your desired path to install Festival>
mkdir ${FESTIVAL_SUIT_PATH} && ./third_party/festvox/setup.sh
```

## Preparing the data
Files from the following commands are already exported to the current directory.

- Lexicon (ne/festvox/lexicon.scm) - ```bazel build //ne/festvox:make_lexicon_scm```
- Festival prompts (ne/festvox/txt.done.data) -  ```bazel build //ne/festvox:make_txt_done_data```

## Training the Voice

1. Download recordings from https://storage.googleapis.com/voice-builder-public-data/all_example_data/data/ne_np_female/ne_np_female.tar.gz
  ```
  export WAV_FOLDER=<your desired path to store downloaded wav files>
  mkdir -p ${WAV_FOLDER} && wget https://storage.googleapis.com/voice-builder-public-data/all_example_data/data/ne_np_female/ne_np_female.tar.gz && tar xzf ne_np_female.tar.gz -C ${WAV_FOLDER}
  ```

2. The setup script from 1) would give out instruction to put export commands under bash.
Please follow them strictly or always export them by yourself before running the next step.


3. Train TTS Voice (This should be executed while in the cloned language-resources folder).
This step will take couple of hours to finish.

  ```
  export OUTPUT_VOICE_FOLDER=<your desired path to store output voice>
  ./festival_utils/build_festvox_voice.sh ${WAV_FOLDER} ne ${OUTPUT_VOICE_FOLDER}
  ```

4. Synthesize one example sentence (This always needs to run from the produced output voice directory).

  ```
    cd ${OUTPUT_VOICE_FOLDER}
    echo 'यहाँका अधिकांश जनता' |
    ${FESTIVALDIR}/bin/text2wave \
      -eval festvox/goog_ne_unison_cg.scm \
      -eval '(voice_goog_ne_unison_cg)' \
      > example.wav
  ```
