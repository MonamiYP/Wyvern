# Wyvern

Wyvern is a lightweight C++ game engine that is currently in active development. The engine architecture is inspired by the **Kohi Game Engine** by Travis Vroman.

---

## Project Structure
Wyvern/  
 ├── wyvern/                # Engine source  
 │   ├── include/           # Engine headers  
 │   ├── lib/               # External libraries  
 │   ├── src/               # Engine implementation  
 │   └── CMakeLists.txt     # Engine build configuration  
 ├── testapp/               # Example game/app using the engine  
 │   ├── src/               # Test app implementation  
 │   └── CMakeLists.txt     # Test app build configuration  
 ├── CMakeLists.txt         # Build configuration  
 └── README.md  

## Build Instructions
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

## Example usage
To create a game using Wyvern, simply implement:
```
Game& createGame();
```
Wyvern will handle initialisation, window creation, input, main loop, event dispatching and so on. You only need to implement the game logic.

## Notes
Wyvern is still in early development. A lot of changes are coming in the future.