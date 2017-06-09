# Setting up Sinhala TTS

## Festvox Environment

Run the following command.

```FESTIVAL_SUIT_PATH=/path_to_the_festvox_directory && ./third_party/festvox/setup.sh ```

## Voice

1. Download recordings from http://www.openslr.org/resources/30/si_lk.tar

2. Train TTS Voice 

	```./utils/build_festvox_voice.sh <path to wavs> si <path_to_build_voice>```
    
3. Synthesize one example sentence.

  ``` 
    echo 'ආයතනයක්වත්' |
    ../festival/bin/text2wave \
      -eval festvox/goog_si_unison_cg.scm \
      -eval '(voice_goog_si_unison_cg)' \
      > example.wav
  ```