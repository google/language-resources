if [ "$TRAVIS_OS_NAME" = osx -o "$(uname)" = Darwin ]; then
  MY_OS=darwin
  brew update
  brew bundle --file=travis/Brewfile
  if [ -z "$VIRTUAL_ENV" ]; then
    virtualenv -p "$(which "$PY")" venv
    . venv/bin/activate
    python --version
    pip --version
  fi
else
  MY_OS=linux
  ## Optionally configure Trusty backports for a less ancient ICU.
  ## Fix this when Xenial becomes available.
  # sudo add-apt-repository ppa:suawekk/trusty-backports -y
  sudo apt-get update -q
  sudo apt-get install -y libicu-dev
  BAZEL=0.17.1
  curl -L "https://github.com/bazelbuild/bazel/releases/download/${BAZEL}/bazel-${BAZEL}-installer-${MY_OS}-x86_64.sh" > bazel-installer.sh
  bash bazel-installer.sh --user
  jdk_switcher use openjdk10
fi

if [ -z "$ANDROID_HOME" ]; then
  export ANDROID_HOME="$HOME/Android/Sdk"
  mkdir -p "$ANDROID_HOME"
  curl "https://dl.google.com/android/repository/sdk-tools-${MY_OS}-${ANDROID_SDK}.zip" > sdk-tools.zip
  unzip -q sdk-tools.zip -d "$ANDROID_HOME"
fi

echo JAVA_HOME="$JAVA_HOME"
echo ANDROID_HOME="$ANDROID_HOME"
