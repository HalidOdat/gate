# Logic Gate Simulator in C++

This project uses [`cmake`](https://cmake.org/download/) for project building
so it must be installed as well a `c++17` (or greater) compliant `C++` compiler.

## Quick start

```console
$ cmake . -B build
$ cmake --build build
$ ./build/gate
```

## Compiling to Wasm

Before being able to compile to you must have the [Emscripten SDK][emscripten] installed
and sourced (as described in the link).

```console
$ emcmake cmake . -B build_web
$ cmake --build build_web
$ cp web/index.html build_web
```

### Start a server

To be able to use the web version you need to start a server.
You can use `python`'s web server module.

```bash
python -m http.server -d build_web
```

Or upload it to a server.

[emscripten]: https://emscripten.org/docs/getting_started/downloads.html
