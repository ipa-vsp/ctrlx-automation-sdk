#!/bin/bash
#
# Script to run inside a docker runtime container
# For snapcraft core* installation it follows the following description: 
# https://raw.githubusercontent.com/snapcore/snapcraft/master/docker/Dockerfile

# Fix Python3 Installation: 
unlink /snap/snapcraft/current/usr/bin/python3
ln -s /snap/snapcraft/current/usr/bin/python3.* /snap/snapcraft/current/usr/bin/python3
echo /snap/snapcraft/current/lib/python3.*/site-packages >> /snap/snapcraft/current/usr/lib/python3/dist-packages/site-packages.pth

# Create a snapcraft runner (TODO: move version detection to the core of
# snapcraft).
mkdir -p /snap/bin
echo '#!/bin/sh' > /snap/bin/snapcraft
snap_version="$(awk '/^version:/{print $2}' /snap/snapcraft/current/meta/snap.yaml)" && echo "export SNAP_VERSION=\"$snap_version\"" >> /snap/bin/snapcraft
echo 'export SNAP="/snap/snapcraft/current"' >> /snap/bin/snapcraft
echo 'export SNAP_NAME="snapcraft"' >> /snap/bin/snapcraft
echo "export SNAP_ARCH=\"arm64\"" >> /snap/bin/snapcraft
echo 'exec "$SNAP/usr/bin/python3" "$SNAP/bin/snapcraft" "$@"' >> /snap/bin/snapcraft
chmod +x /snap/bin/snapcraft

export PATH=/snap/bin:$PATH
export SNAPCRAFT_BUILD_ENVIRONMENT="host"
echo SNAPCRAFT_BUILD_ENVIRONMENT=$SNAPCRAFT_BUILD_ENVIRONMENT >> /etc/environment
export SNAPCRAFT_ENABLE_EXPERIMENTAL_TARGET_ARCH=1
echo SNAPCRAFT_ENABLE_EXPERIMENTAL_TARGET_ARCH=$SNAPCRAFT_ENABLE_EXPERIMENTAL_TARGET_ARCH >> /etc/environment
export SNAPCRAFT_BUILD_INFO=1
export PYTHONPATH=/snap/snapcraft/current/lib/python3.10/site-packages/:$PYTHONPATH