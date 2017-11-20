#  Setting up Merlin TTS
- [Merlin TTS](https://github.com/CSTR-Edinburgh/merlin/)

1. Build an [festival voice](https://github.com/googlei18n/language-resources/tree/master/si/festvox)

2. Setup Merlin with the following command (after setup, the voice can be found at egs/si_lk/s1/... ) -
```./utils/setup_merlin.sh <FESTIVAL_VOICE_PATH> <FULL_MERLIN_PATH> <WAV_PATH>  si si_lk 16000```

3. Add your file list here - egs/si_lk/s1/data/file_id_list.scp

4. Run Merlin training
