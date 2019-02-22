# Setting up Bangla TTS

## Festvox Environment

Run the following command (This should be executed while in the cloned language-resources folder)

```
export FESTIVAL_SUIT_PATH=<your desired path to install Festival>
mkdir ${FESTIVAL_SUIT_PATH} && ./third_party/festvox/setup.sh
```

- Make sure to add the environment variables to .bashrc

## Preparing the data

- Lexicon - ```bazel build //bn/festvox:make_lexicon_scm```
- Festival prompts -  ```bazel build //bn/festvox:make_festvox_prompts```

## Training the Voice

1. Download recordings from 

http://storage.googleapis.com/voice-builder-public-data/all_example_data/data/bn_bd_male/bn_bd_male_16.tar.gz

  ```
  export WAV_FOLDER=<your desired path to store downloaded wav files>
  mkdir -p ${WAV_FOLDER} && wget http://storage.googleapis.com/voice-builder-public-data/all_example_data/data/bn_bd_male/bn_bd_male_16.tar.gz && tar xzf bn_bd_male_16.tar.gz -C ${WAV_FOLDER}
  ```

2. Train TTS Voice (This should be executed while in the cloned language-resources folder).
This step will take couple of hours to finish.

  ```
  export OUTPUT_VOICE_FOLDER=<your desired path to store output voice>
  ./festival_utils/build_festvox_voice.sh ${WAV_FOLDER} bn ${OUTPUT_VOICE_FOLDER}
  ```

3. Synthesize one example sentence (This always needs to run from the produced output voice directory).

  ```
    cd ${OUTPUT_VOICE_FOLDER}
    echo 'অলিম্পিক ইন্ডাস্ট্রিজ জীবনকে সহজ করে তুলেছে' |
    ${FESTIVALDIR}/bin/text2wave \
      -eval festvox/goog_bn_unison_cg.scm \
      -eval '(voice_goog_bn_unison_cg)' \
      > example.wav
  ```
