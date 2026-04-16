# Physics
Exploring physics simulation with single and double pendulums.

A demo compiled for web assembly can be viewed [here](https://cucumberbolts.itch.io/double-pendulum).

![Simple Pendulum](https://github.com/cucumberbolts/physics/blob/assets/simple_pendulum.png?raw=true)
![Double Pendulum](https://github.com/cucumberbolts/physics/blob/assets/double_pendulum.png?raw=true)

## Compiling

### Clone the repository
```
git clone https://github.com/cucumberbolts/physics
cd physics
```

### Build using CMake

The following compilation steps can be simplified with the `build.sh` script on Linux. Enter `./build.sh --help` for more information.  

```
cmake -B build
cmake --build build
```
CMake will automatically download [raylib](https://github.com/raysan5/raylib) as a dependency.

### Compiling for web assembly
Install [emscripten](https://emscripten.org/docs/getting_started/downloads.html) and [ninja](https://ninja-build.org/).

```
emcmake cmake -B build -DPLATFORM=Web
cmake --build build
```

To run: `cd build && python -m http.server 8080` or `emrun build/physics.html`

See [this](https://github-wiki-see.page/m/raysan5/raylib/wiki/Working-for-Web-(HTML5)#3-build-examples-for-the-web) guide for more information on compiling raylib for the web.

### Cross compiling for Windows on Linux using MinGW
1) Install [MinGW](https://www.mingw-w64.org/downloads/)
- Note: Need to install `mingw64-gcc-c++.x86_64` and `mingw64-gcc.x86_64`
- `x86_64-w64-mingw32-gcc` is the gcc equivalent for MinGW
2) Install [Wine](https://www.winehq.org/)
3) 
```
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw64.cmake
export WINEDEBUG=-all # Optional, it disables Wine's debug messages
wine build/physics.exe
```

### Troubleshooting

- GLFW may require installation of [dependencies](https://www.glfw.org/docs/latest/compile.html) to build.  

- If OPENGL_INCLUDE_DIR is not found, try:
```
cmake -B build -DOPENGL_INCLUDE_DIR="/usr/include/GL/"
```

- If libGL is not found, try:
```
ln -s /usr/lib64/libGL.so.Version libGL.so
```
Where "Version" is a number.

## Resources
Resources used for research in this project are listed in resources.md
