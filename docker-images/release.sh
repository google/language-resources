set -x 

ORG_NAME="langtech"
BUILD_TAG=v1_1

for IMAGE_NAME in language-resources festival merlin; do
  cd ${IMAGE_NAME}
  IMAGE_NAME="base-${IMAGE_NAME}"
  docker build --cpuset-cpus=4  --no-cache -t ${ORG_NAME}/${IMAGE_NAME}:${BUILD_TAG} .
  docker push ${ORG_NAME}/${IMAGE_NAME}
  cd ../
done
