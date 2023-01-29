# Logic Gate Simulator in C++

# TODO

- [ ] Draw line on click from one to another
- [ ] Draw inputs and outputs on a component
- [ ] Add a toggleable switch componenet.
- [ ] Connect toggleable component with wire 

This project uses [`cmake`](https://cmake.org/download/) for project building
so it must be installed as well a `c++17` (or greater) compliant `c++` compiler.

## Quick start

```console
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build .
```

In the build directory there should be the `gate` executable.

## Compiling to Wasm

Before being able to compile to you must have the Emscripten SDK installed.

```console
$ mkdir build
$ cd build
$ emcmake cmake ..
$ cmake --build .
```

There should be a `.html`, `.js` and `.wasm` file.

### Start a server

To be able to use the web version you need to start a server.
You can use `python`'s web server module in the build directory.

```bash
python -m http.server
```

Or upload it to a server.
