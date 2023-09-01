#!/bin/sh

source scripts/setup_emscripten_sdk.sh

emcmake cmake . -B build-web
cmake --build build-web
cp web/index.html build-web
