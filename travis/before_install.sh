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
  echo "deb [arch=amd64] http://storage.googleapis.com/bazel-apt stable jdk1.8" | sudo tee /etc/apt/sources.list.d/bazel.list
  curl https://bazel.build/bazel-release.pub.gpg | sudo apt-key add -
  sudo apt-get update -qq
  sudo apt-get install -qq bazel -y
fi
