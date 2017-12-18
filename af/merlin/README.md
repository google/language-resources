#  Setting up Merlin TTS
- [Merlin TTS](https://github.com/CSTR-Edinburgh/merlin/)

1. Build an  [festival voice](https://github.com/googlei18n/language-resources/tree/master/af/festvox)

2. Setup Merlin with the following command (after setup, voice can be found at egs/af_za/s1/... ) -
```./festival_utils/setup_merlin.sh <FESTIVAL_VOICE_PATH> <FULL_MERLIN_PATH> <WAV_PATH>  af af_za 48000```

3. Add you file list here - egs/af_za/s1/data/file_id_list.scp

4. Run Merlin training
