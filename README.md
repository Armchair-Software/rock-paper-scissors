# Rock Paper Scissors for IMC by Eugene Hopkinson

This is my interpretation of an interactive rock, paper, scissors game.

The fundamental architecture is a C++23 program compiled to WebAssembly with Emscripten, running in the browser, rendering graphics with WebGL2.

This document covers how to build it and get it to run, the tech I used and rationale for the design decisions I made, and documentation of the project structure.

## Building
The project is intended to be built on Linux.  It may build elsewhere there is a posix toolchain and working Emscripten configuration, but this is not tested.

Once built, it can run on any platform with a modern browser.

### Build dependencies
- [Emscripten](https://emscripten.org/docs/getting_started/downloads.html)
- CMake >= 3.10
- Boost >= 1.81
- rsync

This repo provides a simple build wrapper script to assist with building.  It is not necessary to use this script - you can call `cmake` and `emmake` directly - but the script is recommended as it helps to correctly assemble the destination directory structure.

### Build script

After installing the Emscripten SDK, and before attempting to build or test this, you must `source ./emsdk_env.sh` to add the Emscripten SDK tools to your path.

To build, run:
```sh
./build.sh
```
The build products are:
- ./build/client.wasm - this project compiled to webassembly
- ./build/client.data - embedded assets
- ./build/client.js - the javascript runtime to execute the webassembly
- ./build/client.html - the "shell" HTML page that loads the javascript
- ./build/favicon.ico - browser page icon

To build in release mode, just do:
```sh
CMAKE_BUILD_TYPE=Release ./build.sh
```

### Running
To run the program, it should be hosted on a webserver - opening the html file in a local browser will not work.  Emscripten provides the `emrun` helper to run a minimal local webserver, and there is an optional run script provided, to help executing this.  Again, the script is not necessary, but provides extra convenience.

```sh
./run.sh
```

This will fire up Emscripten's webserver and open the page in your default browser.

To run a release build, use:
```sh
CMAKE_BUILD_TYPE=Release ./run.sh
```

Once built, this can be deployed to the web and executed from browsers on any platform.

## Design decisions

As the requirement is an interactive game, based on a well-known playground game mechanic, a simple and quickly accessible graphical interface is most appropriate.

The decision to build as WebAssembly to run in-browser is also motivated by the fundamental game concept.  A game like this should be quickly accessible, it should run easily on any platform with the minimum of setup - none, in this case.  It is not particularly performance-critical, so the performance tradeoffs of WASM are acceptable in this case.

The back-end rendering is abstracted away from the main program logic.   The game logic is also abstracted, is modular and can easily be switched.  One game logic system is currently implemented - "offline" logic for local play against the computer.  Additional game logic schemes could easily be added for other types of play, for example online multiplayer play.

The game logic has switchable difficulty levels.  It has a modular library of strategies, and selects an appropriate strategy to use for a requested difficulty level - see game_logic/strategy/offline.cpp for the details.

A player using "rock", "paper" or "scissors" is encapsulated in the concept of a "move".  All strategies are agnostic as to the actual contents of a move, or the number of possible moves - therefore it is easy to add additional "moves", as long as they follow the sequential each-one-beats-the-next concept, without any need to modify any of the strategies.

A dramatic and flashy kitsch pixel-art style was chosen for the graphics - both for simplicity of development, and maximum visual impact.  Using a small number of low-resolution PNG assets means they can be embedded in the program itself rather than requiring slow downloads by the browser.  These are decoded at startup from program memory using a fork of LibPNG++.

The render canvas window is managed by GLFW3.  Fast OpenGL-accelerated GUI graphics are provided by a fork of ImGUI.  Vector maths is provided by my VectorStorm library, developed for my former game studio VoxelStorm.

In order to maintain modularity and reduce the number of hard-coded strings, move names ("rock", "paper", "scissors") and verdicts ("win", "draw", "lose") are automatically derived from their enum names by `magic_enum`.  Iteration over enums is also provided by this handy library, so the move buttons are dynamically generated from the enums - simply add another move type, and a new button will appear without manual intervention.  Longer verbal descriptions are still specified manually, but are kept to a minimum.

### Oddities
There are a few unusual details that arise as a result of using Emscripten that are worth mentioning.

#### Emscripten's loops
The browser event model requires cooperative multitasking, and conventional infinite loops are not feasible.  See the [emscripten docs on loops](https://emscripten.org/docs/porting/emscripten-runtime-environment.html#browser-main-loop) for more detail.  Instead, pseudo-loops are executed as repeated callbacks by Emscripten, which places some limits on how data can be passed around.  As a result, key game state is wrapped in a `game_state` struct which is given to the loop dispatcher, and a pointer to this is passed by emscripten back to each function executed as a loop.

#### Non-compliant main
For the same reasons, execution flow can never return after a loop has been dispatched - only new pseudo-loops can be started.  Once a loop is launched from `main`, execution never returns there, and so `main` can never return `int`, and is declared `[noreturn]` - this is not standards-compliant, but is correct for use with Emscripten.

#### No exceptions
Although there is progress with C++ exceptions in WASM, currently support is patchy, and the most common emulation method produces a significant performance impact on the code.  Therefore exception handling is disabled in this project.  In a handful of places where exceptions cannot be usefully replaced with another paradigm, in case of errors during initialisation of the graphics systems, the program will `abort()`.

#### Debugging & profiling
Debugging messages are printed to the browser console - press F12 to see them.

Profiling properly-built C++ programs running in the browser is a very pleasant experience.  Debug builds are built with debugging information that is interpreted nicely by the built-in profiling tools in Chromium and Firefox, with C++ namespaces and type names correctly demangled, and complete & accurate stack traces displayed.  Give it a try in the "Performance" tab of the browser developer console!

Release builds strip all of this and run a closure compiler on the javascript, making meaningful debugging almost impossible, but producing a noticeable speedup.

## Rock Paper Scissors game strategies
"Medium" difficulty is provided by a 100% random strategy that does not take into account past events.

"Hard" difficulty attempts to use an optimal strategy modified for detection.  The base strategy is based on the paper [Social cycling and conditional responses in the Rock-Paper-Scissors game by Wang et al, 2014](https://arxiv.org/pdf/1404.5199v1.pdf).  The core concept is quite simple, and depends on cycling choices based on the last game's success or failure, and the player's last move.  For implementation details, see `game_logic/strategy/wang_et_al_2014.h` and `.cpp`.

When you choose "Hard" difficulty, the "offline" game logic selects a modified strategy, `game_logic/strategy/wang_et_al_2014_mod1.h` / `.cpp`.  Wang et al's strategy can be taken advantage of if it is understood by the player, in its predictability.  This modified strategy attempts to detect and defeat prediction attacks - if the player defeated the Wang et al strategy the last two rounds, this strategy makes a random move instead of following the Wang et al strategy next.

Intentionally losing at Rock Paper Scissors is actually just as difficult as intentionally winning.  The "Easy" difficulty applies a simple inversion of the Wang et al strategy (detailed in `wang_et_al_2014_inverse.h` / `.cpp`), which plays the losing move instead of the winning move as dictated by the Wang strategy at every point.  It does not include a random element, and can be predicted and relatively easily defeated once the pattern is observed.

## Libraries used
- [Boost](https://www.boost.org/)
- [Emscripten](https://emscripten.org/)
- OpenGL / WebGL2 (via Emscripten & GLFW)
- [GLFW](https://github.com/glfw/glfw)
- LibPNG (via Emscripten ports)
- [PNG++](https://www.nongnu.org/pngpp/) - this project uses my fork at https://github.com/Armchair-Software/pngpp, which adds PNG metadata handling
- [Dear ImGui](https://github.com/ocornut/imgui) - this project uses my fork at https://github.com/slowriot/imgui/tree/patch-1, which eliminates certain graphics pipeline stalls in browser
- [magic_enum](https://github.com/Neargye/magic_enum)
- VectorStorm - a vector maths library I developed for my game studio [VoxelStorm](https://en.wikipedia.org/wiki/VoxelStorm) (not currently open source)

## Assets:

- Hand images - pixel art based on stock photographs
- Hand icons - derived from royalty-free stock icons
