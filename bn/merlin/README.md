#  Setting up Merlin TTS
1. Install [Merlin TTS](https://github.com/CSTR-Edinburgh/merlin/)

2. Build an [Bangla Festival Voice](https://github.com/googlei18n/language-resources/tree/master/bn/festvox)

3. Setup Merlin with the following command (after setup, the voice can be found at ```<FULL_MERLIN_PATH>/egs/bn_bd/```)

   ```./festival_utils/setup_merlin.sh <FESTIVAL_VOICE_PATH> <FULL_MERLIN_PATH> <WAV_PATH>  bn bn_bd 16000```

4. Add your file list here - ```egs/bn_bd/s1/data/file_id_list.scp```

5. Run Merlin training
    
   - Duration - ```python src/run_merlin.py egs/bn_bd/s1/conf/duration_dnn.conf```
   - Acoustic - ```python src/run_merlin.py egs/bn_bd/s1/conf/acoustic_dnn.conf```
