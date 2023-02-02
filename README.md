# Logic Gate Simulator in C++

This project uses [`cmake`](https://cmake.org/download/) for project building
so it must be installed as well a `c++17` (or greater) compliant `C++` compiler.

# TODO

- [ ] Board
    - [x] Better grid cell selection
    - [x] Define components/wires based on grid cell size
    - [x] Draw line on click from one to another

- [ ] Wire
    - [x] Rename Line to Wire
    - [x] Align drawn wires better
    - [x] Draw wire end points
    - [x] Add active state to wire

- [ ] Component
    - [x] Draw inputs and outputs on a component
    - [x] Add a toggleable switch componenet
    - [x] Connect toggleable component with wire

- [ ] Misc
    - [x] Merge Board into EditorLayer
    - [x] Only quit with `q` on native platform.
    - [ ] Serializable board.
    - [x] Define global editor state
    - [ ] Undo & Redo

## Graph Traversal Algorithm

This algorithm is specialization of a Breadth First Search Graph algorithm.

The components as well as wires have visited boolean flags.

## Rules

These are rules to eliminate invalid state.

1. An input pin can **only** have one connection.
2. An output pin can **many** have one connection.
3. A circuit without a input catagory component is not valid.
4. Wires inherit the type of of the pin (`Input`, `Output`).
5. Traversal starts from components that are from the `Input` catagory type.
6. Traversal is one directional.


```txt
TODO: define algorithm
procedure algorithm(root) is
    let Q be a queue
    Q.enqueue(root)
    while Q is not empty do
        v := Q.dequeue()
        label v as visited
        for all wires w connected to v do
            if w is (not visited) or (not active and visited) then
                label w as visited
                for all components c connected to w do
                    Q.enqueue(c)

Step 1. Clear all visited flags for components and wires.
Step 2. Start the traversal with the list of emitting components (i.e. SwitchComponent) then non-emitting components
Step 2. Initialize queue
Step 2. For each component
    Step 2.2. For each output pin
        Step 2.2.1. For each connected wire
```


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
