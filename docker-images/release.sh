set -x 

USERNAME="langtech"
BUILD_TAG=v1

for IMAGE_NAME in language-resources festival merlin; do
  cd ${IMAGE_NAME}
  IMAGE_NAME="base-${IMAGE_NAME}"
  docker build --cpuset-cpus=4  --no-cache -t ${USERNAME}/${IMAGE_NAME}:${BUILD_TAG} .
  docker push ${USERNAME}/${IMAGE_NAME}
  cd ../
done
