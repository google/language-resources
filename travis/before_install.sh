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
  # sudo apt-get update -q
  echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
  curl https://bazel.build/bazel-release.pub.gpg | sudo apt-key add -
  sudo apt-get update -qq
  sudo apt-get install -qq bazel libicu-dev -y
fi

if [ -z "$ANDROID_HOME" ]; then
  export ANDROID_HOME="$HOME/Android/Sdk"
  mkdir -p "$ANDROID_HOME"
  curl "https://dl.google.com/android/repository/sdk-tools-${MY_OS}-${ANDROID_SDK}.zip" > sdk-tools.zip
  unzip -q sdk-tools.zip -d "$ANDROID_HOME"
fi

echo ANDROID_HOME="$ANDROID_HOME"
