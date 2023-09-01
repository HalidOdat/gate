#!/bin/sh

REPOSITORY='https://github.com/emscripten-core/emsdk.git'

# VERSION='latest'
VERSION='3.1.45'

if [ ! -d './emsdk' ]; then
    git clone $REPOSITORY
fi

cd emsdk
git pull
./emsdk install $VERSION
./emsdk activate $VERSION
source ./emsdk_env.sh
cd ..

