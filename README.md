# OpenGL Game in C++

This project uses [`cmake`](https://cmake.org/download/) for project building so it must be installed.

## Quick start

```console
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

In the build directory there should be the `Game` executable.

## Compiling to Wasm

Before being able to compile to you must have the Emscripten SDK installed.

```console
$ mkdir build
$ cd build
$ emcmake cmake ..
$ cmake --build .
```

There should be a `.html`, `.js` and `.wasm` file.