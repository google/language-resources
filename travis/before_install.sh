if [ "$TRAVIS_OS_NAME" = osx -o "$(uname)" = Darwin ]; then
  MY_OS=darwin
  echo '$ python --version'
  python --version
  echo '$ pip --version'
  pip --version
  # brew update
  brew bundle --file=travis/Brewfile
else
  MY_OS=linux
  ## Optionally configure Trusty backports for a less ancient ICU.
  ## Fix this when Xenial becomes available.
  # sudo add-apt-repository ppa:suawekk/trusty-backports -y
  # sudo apt-get update -qq
  sudo apt-get install -q -y libicu-dev
fi

curl -L "https://github.com/bazelbuild/bazel/releases/download/${BAZEL}/bazel-${BAZEL}-installer-${MY_OS}-x86_64.sh" > bazel-installer.sh
bash bazel-installer.sh --user
export JAVA_HOME="$(bazel info java-home)"
echo '$ $JAVA_HOME/bin/java -version'
"$JAVA_HOME/bin/java" -version

if [ -z "$ANDROID_HOME" ]; then
  export ANDROID_HOME="$HOME/Android/Sdk"
  mkdir -p "$ANDROID_HOME"
  curl "https://dl.google.com/android/repository/sdk-tools-${MY_OS}-${ANDROID_SDK}.zip" > sdk-tools.zip
  unzip -q sdk-tools.zip -d "$ANDROID_HOME"
fi

echo ANDROID_HOME="$ANDROID_HOME"
