import os.path
import setuptools
import sys

if not os.path.exists('fonbund/distinctive_features_pb2.py'):
  sys.stderr.write('''\

Generated protobuf source file not found. Most likely this means setup.py was
run directly from within the Git repo. This won't work because the *_pb2.py
files need to be generated py protoc from the corresponding *.proto files.

Instead run setup.py from within a runfiles directory that contains all
required source files and generated files. This is done via bazel-bin/setup:
```
bazel build //...
bazel-bin/setup --help
```
''')
  sys.exit(1)

setuptools.setup()
