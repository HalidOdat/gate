# Logic Gate Simulator in C++

# TODO

- [x] Only quit with `q` on native platform.
- [ ] Better grid cell selection
- [ ] Align drawnlines better
- [ ] Draw line on click from one to another
- [ ] Draw inputs and outputs on a component
- [ ] Add a toggleable switch componenet
- [ ] Connect toggleable component with wire
- [ ] Serializable board.
- [x] Merge Board into EditorLayer
- [x] Rename Line to Wire

This project uses [`cmake`](https://cmake.org/download/) for project building
so it must be installed as well a `c++17` (or greater) compliant `c++` compiler.

## Quick start

```console
$ cmake . -B build
$ cmake --build build
$ ./build/gate
```

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
