# Physics
Exploring physics simulation

## Compiling
```
git clone --recursive https://github.com/cucumberbolts/physics
cd physics
cmake -B build
cmake --build build
```

### Compiling for web assembly
Install [emscripten](https://emscripten.org/docs/getting_started/downloads.html) and [ninja](https://ninja-build.org/).

```
emcmake cmake -B build
cmake --build build
```

To run: `cd build && python -m http.server 8080` or `emrun build/physics.html`

See [this](https://github-wiki-see.page/m/raysan5/raylib/wiki/Working-for-Web-(HTML5)#3-build-examples-for-the-web) guide for more information on compiling raylib for the web.

Troubleshooting:

If OPENGL_INCLUDE_DIR is not found, try:
```
cmake -B build -DOPENGL_INCLUDE_DIR="/usr/include/GL/"
```

If libGL is not found, try:
```
ln -s /usr/lib64/libGL.so.Version libGL.so
```
Where "Version" is a number.

## Resources
Resources used for research in this project are listed in resources.md
