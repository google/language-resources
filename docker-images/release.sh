set -x 

USERNAME="langtech"

for IMAGE_NAME in language-resources festival merlin; do
  cd ${IMAGE_NAME}
  IMAGE_NAME="base-${IMAGE_NAME}"
  docker build --no-cache -t ${USERNAME}/${IMAGE_NAME}:latest .
  docker push ${USERNAME}/${IMAGE_NAME}
  cd ../
done
