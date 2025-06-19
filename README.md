# Physics
Exploring physics simulation

## Compiling
```
git clone --recursive https://github.com/cucumberbolts/physics
cd physics
mkdir build
cmake -B build
cmake --build build
```

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
Resources used for reasearch in this project are listed in resources.md
