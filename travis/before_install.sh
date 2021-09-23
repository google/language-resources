if [ "$TRAVIS_OS_NAME" = osx -o "$(uname)" = Darwin ]; then
  MY_OS=darwin
  ANDROID_HOST_OS=mac
  echo '$ python3 --version'
  python3 --version
  echo '$ pip3 --version'
  pip3 --version
  # brew update
  brew bundle --file=travis/Brewfile
  eval $(brew shellenv)
  icu4c="${HOMEBREW_PREFIX}/opt/icu4c"
  export PATH="${icu4c}/bin:${PATH}"
  export PKG_CONFIG_PATH="${icu4c}/lib/pkgconfig:${PKG_COCNFIG_PATH}"
else
  MY_OS=linux
  ANDROID_HOST_OS=linux
  ## Optionally configure Trusty backports for a less ancient ICU.
  ## Fix this when Xenial becomes available.
  # sudo add-apt-repository ppa:suawekk/trusty-backports -y
  # sudo apt-get update -qq
  sudo apt-get --no-install-recommends install -y -q libicu-dev
fi

curl -L "https://github.com/bazelbuild/bazel/releases/download/${BAZEL}/bazel-${BAZEL}-installer-${MY_OS}-x86_64.sh" > bazel-installer.sh
bash bazel-installer.sh --user

if [ -d "$JAVA_HOME" ]; then
  echo JAVA_HOME="$JAVA_HOME"
  echo '$ $JAVA_HOME/bin/java -version'
  "$JAVA_HOME/bin/java" -version
else
  echo '$ java -version'
  java -version
fi

if [ -z "$ANDROID_HOME" ]; then
  export ANDROID_HOME="$HOME/Android/Sdk"
  mkdir -p "$ANDROID_HOME/cmdline-tools"
  curl -L "https://dl.google.com/android/repository/commandlinetools-${ANDROID_HOST_OS}-${ANDROID_SDK}_latest.zip" > cmdline-tools.zip
  unzip -q cmdline-tools.zip
  mv cmdline-tools "$ANDROID_HOME/cmdline-tools/latest"
fi

echo ANDROID_HOME="$ANDROID_HOME"
