## Build System

This project uses **CMake** with **CMake Presets** for cross-platform builds. It works on Windows, Linux, and macOS without any configuration changes.

### Prerequisites

- **CMake 3.19+** (for preset support)
- **A C++17 compiler**: GCC, Clang, AppleClang, or MinGW-w64

## How to Run/Debug an Example

### Method 1: Using VS Code Debugger (Recommended)

1. **Open an example file** from `packages/cpp-sdk/examples/` (e.g., `autoscale.cpp`)
2. **Select the debug configuration** matching your OS from the dropdown (Linux, macOS, or Windows)
3. **Press F5** or click the "Run and Debug" button
4. CMake will automatically:
   - Build the specific example using the configured target
   - Link all necessary dependencies (including C++ standard library)
   - Place the executable in `build/`
5. The debugger will launch and stop at any breakpoints you've set

### Method 2: Build Specific Example

1. Open the example file you want to build
2. Press **Ctrl+Shift+B** (or run task: `CMake: Build active example`)
3. The executable will be created in `build/`
4. Run it from terminal:
   ```powershell
   # Windows
   .\build\autoscale.exe

   # Linux/macOS
   ./build/autoscale
   ```

### Method 3: Build All Examples

From the monorepo root:
```bash
# Build all packages (including cpp-sdk examples)
npm run build

# Build only cpp-sdk
npm run build --workspace=packages/cpp-sdk
```

Or from the `packages/cpp-sdk` directory:
```bash
# Configure and build with CMake
npm run build

# Clean build artifacts
npm run clean
```

Or manually with CMake presets:
```bash
# Configure (only needed once, or after CMakeLists.txt changes)
# Use "unix" on Linux/macOS, "windows" on Windows
cmake --preset unix

# Build all examples
cmake --build --preset unix

# Build a specific example
cmake --build build --target autoscale --config Debug
```

## What's Configured

### CMake Presets (`CMakePresets.json`)

Platform-conditional presets are available — only the ones matching your OS are visible:

- **unix** / **unix-release** — Linux/macOS (Unix Makefiles)
- **windows** / **windows-release** — Windows (MinGW Makefiles)

The `npm run build` script auto-detects your platform and picks the right preset.

### VS Code Tasks (`.vscode/tasks.json`)

Three build tasks are available:

1. **CMake: Build active example** (Default: Ctrl+Shift+B)
   - Builds only the currently open example file
   - Fast incremental builds

2. **CMake: Configure**
   - Configures the CMake build system using presets
   - Run this if you modify `CMakeLists.txt`

3. **CMake: Build all examples**
   - Builds all 17 example executables

### Debug Configurations (`.vscode/launch.json`)

Three platform-specific configurations:

- **Debug Example (Linux)** — Uses GDB
- **Debug Example (macOS)** — Uses LLDB
- **Debug Example (Windows)** — Uses GDB with `.exe` extension

Select the one matching your OS from the debug dropdown. VS Code remembers your selection.

## Available Examples

The `examples/` directory contains 17 example files demonstrating various features:

### Core Functionality
- **vectortest.cpp** - Comprehensive demonstration of Vectors class
- **classtest.cpp** - Extensive class testing

### Musical Features
- **automations.cpp** - Automation helpers (voice leading, degree automation, modulation)
- **automationsSeq.cpp** - Sequential automation patterns
- **autoscale.cpp** - Automatic scale selection based on note input
- **chordClass.cpp** - Chord class usage and examples
- **chordNames.cpp** - Chord naming utilities
- **chordTest.cpp** - Chord helper tests
- **noteNames.cpp** - Note naming system examples
- **scale.cpp** - Scale class demonstrations
- **scaleDictionary.cpp** - Scale dictionary usage
- **selection.cpp** - Selection meta-operators demo

### Rhythmic and Mathematical
- **rhythmGen.cpp** - Rhythmic generators (Euclidean, Clough-Douthett, deep rhythms)
- **matrix.cpp** - Matrix generation and utilities
- **matrixDistances.cpp** - Matrix-distance examples
- **distances.cpp** - Distance metrics and transformation examples
- **measures.cpp** - Analysis and measurement helpers

## Quick Start

Try running `autoscale.cpp` as your first example:

1. Open `packages/cpp-sdk/examples/autoscale.cpp`
2. Press **F5**
3. Watch the output in the Debug Console

## Architecture Notes

### Header-Only Library

This library is **header-only**, meaning:
- All implementation is in header files (`.h`)
- No separate compilation of library code
- Examples compile by including headers from `src/`
- CMake configures the library as an `INTERFACE` target

### CMakeLists.txt Structure

```cmake
# Header-only library
add_library(musicplusplus INTERFACE)
target_include_directories(musicplusplus INTERFACE src)

# C++17 standard
set(CMAKE_CXX_STANDARD 17)

# Build each example
foreach(SOURCE_FILE ${EXAMPLE_SOURCES})
    get_filename_component(EXE_NAME ${SOURCE_FILE} NAME_WE)
    add_executable(${EXE_NAME} ${SOURCE_FILE})
    target_link_libraries(${EXE_NAME} musicplusplus)
endforeach()
```

## Troubleshooting

### "CMake not found"

Install CMake 3.19+:
- **Windows**: `winget install Kitware.CMake`
- **macOS**: `brew install cmake`
- **Linux**: `sudo apt install cmake` (or your distro's package manager)
- Or download from: https://cmake.org/download/

### "No compiler found"

Install a C++17 compiler:
- **Windows**: Install MinGW-w64 (e.g. via MSYS2: `pacman -S mingw-w64-ucrt-x86_64-toolchain`)
- **macOS**: `xcode-select --install` (installs AppleClang)
- **Linux**: `sudo apt install g++` (or your distro's package manager)

### "Target not found" when building

CMake hasn't been configured yet:
1. Run task: `CMake: Configure`
2. Then build with: `CMake: Build active example`

### Breakpoints not working

- CMake automatically includes `-g` flag in Debug builds
- Make sure you're building in Debug configuration (default)
- Check that you're debugging (F5) not just running the program

### Build directory issues

```bash
# Clean and rebuild
npm run clean
npm run build

# Or manually (use "windows" preset on Windows)
rm -rf build
cmake --preset unix
cmake --build --preset unix
```

## Advanced Usage

### Release Build

```bash
# Use "windows-release" on Windows
cmake --preset unix-release
cmake --build --preset unix-release
```

### Verbose Build Output

```bash
cmake --build build --verbose
```
