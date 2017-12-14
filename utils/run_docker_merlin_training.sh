# Simple script to run merlin training in docker.

set -x 

PY_MERLIN_CONF="/usr/local/src/language-resources/utils/merlin_confs.py"
BASE_VOICE_PATH="/usr/local/src/merlin//egs/locale/s1"
CONF_PATH="egs/locale/s1/conf/"

# Whether to run merlin TTS training.
TRAIN=true

while getopts ":t" opt; do
  case ${opt} in
    t ) # Option to disable training.
      TRAIN=false
      echo "Disable merlin training"
      ;;
  esac
done

# Setup merlin files.
cd /usr/local/src/voice
/usr/local/src/language-resources/utils/setup_merlin.sh \
/usr/local/src/voice/training \
/usr/local/src/merlin/ \
/usr/local/src/voice/data/festvox/wavs/ \
data \
locale \
16000

# Setup required paths.
mkdir -p ${BASE_VOICE_PATH}/models/duration_model
mkdir -p ${BASE_VOICE_PATH}/models/acoustic_model
mkdir -p ${BASE_VOICE_PATH}/models/test_synthesis


# Fix merlin conf files.
COUNT=$(cat ${BASE_VOICE_PATH}/data/file_id_list.scp | wc -l)

for conf in acoustic_dnn duration_dnn; do
  S=${CONF_PATH}/_${conf}.conf
  D=${CONF_PATH}/${conf}.conf
  mv  ${D} ${S}
  python ${PY_MERLIN_CONF} ${S} ${COUNT} > ${D}
done

# Run merlin training.
if [[ ${TRAIN} == true ]]; then
  echo "Training merlin ${LANG} voice"
  time python src/run_merlin.py egs/local/s1/conf/duration_dnn.conf
  time python src/run_merlin.py egs/local/s1/conf/acoustic_dnn.conf
fi
