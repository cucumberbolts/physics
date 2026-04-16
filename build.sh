#!/bin/sh

# Exit on error
set -e

platform="unix"
build_dir="build"

if [[ "$1" == "-h" ]] || [[ "$1" == "--help" ]]; then
    echo "Usage: $0 [OPTIONS] [PLATFORM=unix]"
    echo ""
    echo "Options:"
    echo "  -d, --dir                The build directory"
    echo "  -r, --run                Run the executable if the build was successful"
    echo "  --dist                   When building for web assembly, package the files for itch.io format"
    echo ""
    echo "Platforms:"
    echo "  unix      Default platform"
    echo "  windows   Cross compile using MinGW (requires wine to run)"
    echo "  web       Compile for web assembly (requires emscripten and ninja)"
    exit 0
fi

while [[ $# -gt 0 ]]; do
    case $1 in
	-d|--dir)
	    special_build_dir="$2"
	    shift
	    shift
	    ;;
	-r|--run)
	    run=true
	    shift
	    ;;
	--dist)
	    dist=true
	    shift
	    ;;
	unix)
	    ;;
	windows)
	    platform="windows"
	    build_dir="build_win"
	    shift
	    ;;
	web)
	    platform="web"
	    build_dir="build_web"
	    shift
	    ;;
	-*|--*)
	    echo "$0: ERROR: Unrecognized option $1"
	    exit 1
	    ;;
	*)
	    echo "$0: ERROR: Unsupported platform $1"
	    exit 1
	    ;;
    esac
done

if [[ $special_build_dir ]]; then
    build_dir = special_build_dir
fi

echo "$0: Build directory: $build_dir"
echo "$0: Platform: $platform"

if [[ $platform == "unix" ]]; then
    cmake -B $build_dir -DOPENGL_INCLUDE_DIR="/usr/include/GL"
    cmake --build $build_dir

    if [[ $run ]]; then
	$build_dir/physics
    fi
elif [[ $platform == "windows" ]]; then
    cmake -B build_win -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw64.cmake
    cmake --build build_win

    if [[ $run ]]; then
        export WINEDEBUG=-all # Disable pesky debug messages from wine
        wine build_win/physics.exe
    fi
elif [[ $platform == "web" ]]; then
    emcmake cmake -B $build_dir -DPLATFORM=Web
    cmake --build $build_dir

    html_dir="$build_dir/physics.html"

    if [[ $dist ]]; then
	cd $build_dir
	mv physics.html index.html
	zip physics.zip index.html physics.wasm physics.js
	cd ..

	html_dir="$build_dir/index.html"
    fi

    if [[ $run ]]; then
	emrun $html_dir
    fi
fi
