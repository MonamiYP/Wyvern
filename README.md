# Wyvern

Wyvern is a lightweight C++ game engine that is currently in active development. The engine architecture is inspired by the **Kohi Game Engine** by Travis Vroman.

---

## Build Instructions
### Requirements
- CMake (minimum 3.15)
- C++17 compiler
- Vulkan

### Compiling Wyvern
From the project root:
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
make -C build
```

### Running
After compiling:
```
./bin/testapp    # or run from vscode debugger
```

## Notes
Wyvern is still in early development. A lot of changes are coming in the future.
