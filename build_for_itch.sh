#!/bin/sh

# Exit on error
set -xe

emcmake cmake -B webbuild
cmake --build webbuild
cd webbuild
mv physics.html index.html
zip physics.zip index.html physics.wasm physics.js
