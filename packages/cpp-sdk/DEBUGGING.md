## Build System

This project uses **CMake** as the build system, which:
- Properly compiles all examples as separate executables
- Links the header-only library correctly
- Manages include paths automatically
- Ensures C++17 standard compliance
- Handles platform-specific compilation flags

## How to Run/Debug an Example

### Method 1: Using VS Code Debugger (Recommended)

1. **Open an example file** from `packages/cpp-sdk/examples/` (e.g., `autoscale.cpp`)
2. **Press F5** or click the "Run and Debug" button
3. CMake will automatically:
   - Build the specific example using the configured target
   - Link all necessary dependencies (including C++ standard library)
   - Place the executable in `build/`
4. The debugger will launch and stop at any breakpoints you've set

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

Or manually with CMake:
```powershell
# Configure (only needed once, or after CMakeLists.txt changes)
cmake -S . -B build -G "MinGW Makefiles"

# Build all examples
cmake --build build --config Debug

# Build a specific example
cmake --build build --target autoscale --config Debug
```

## What's Configured

### VS Code Tasks (`.vscode/tasks.json`)

Three build tasks are available:

1. **CMake: Build active example** (Default: Ctrl+Shift+B)
   - Builds only the currently open example file
   - Fast incremental builds

2. **CMake: Configure**
   - Configures the CMake build system
   - Run this if you modify `CMakeLists.txt`

3. **CMake: Build all examples**
   - Builds all 17 example executables
   - Useful for testing changes across the codebase

### Debug Configuration (`.vscode/launch.json`)

- **Configuration**: "Debug Example (CMake)"
- **Debugger**: GDB from MSYS2
- **Pre-launch**: Automatically builds the active example using CMake
- **Pretty Printing**: Enabled for better variable inspection (STL containers, etc.)
- **Working Directory**: Project root (`packages/cpp-sdk`)
- **Executable Path**: `build/${fileBasenameNoExtension}.exe`

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

This example demonstrates:
- Automatic scale selection based on input notes
- Complexity-based selection from candidate scales
- Multiple test cases with different musical scenarios

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
    target_link_libraries(${EXE_NAME} vectors)
endforeach()
```

## Troubleshooting

### Build Errors: "Undefined reference to..."

**Problem**: Linker errors about undefined C++ standard library symbols.

**Solution**: This was the old issue when compiling directly with `g++`. Now that we use CMake, this is automatically resolved. If you see this:
- Make sure you're using the CMake build tasks (not manual `g++` commands)
- Run `CMake: Configure` task to reconfigure
- Rebuild with `CMake: Build active example`

### "CMake not found"

**Problem**: CMake is not installed or not in PATH.

**Solution**: 
- Install CMake: `winget install Kitware.CMake` (Windows)
- Or download from: https://cmake.org/download/
- Ensure it's in your system PATH

### "MinGW Makefiles" generator not found

**Problem**: MinGW/MSYS2 is not installed or not configured.

**Solution**:
- Ensure MSYS2 is installed at `C:\msys64`
- Install development tools: `pacman -S mingw-w64-ucrt-x86_64-toolchain`
- Add to PATH: `C:\msys64\ucrt64\bin`

### "gdb not found"

**Problem**: GDB debugger not available.

**Solution**:
- Ensure MSYS2 is installed at `C:\msys64`
- Install GDB: `pacman -S mingw-w64-ucrt-x86_64-gdb`
- The debugger expects gdb at: `C:\msys64\ucrt64\bin\gdb.exe`

### "Target not found" when building

**Problem**: CMake hasn't been configured yet.

**Solution**:
1. Run task: `CMake: Configure`
2. Then build with: `CMake: Build active example`

### Breakpoints not working

**Problem**: Debugging without proper symbols.

**Solution**:
- CMake automatically includes `-g` flag in Debug builds
- Make sure you're building in Debug configuration (default)
- Check that you're debugging (F5) not just running the program

### Build directory issues

**Problem**: Stale or corrupted build cache.

**Solution**:
```bash
# Clean and rebuild
npm run clean
npm run build

# Or manually
rm -rf build
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

## Advanced Usage

### Building with Different Generators

```bash
# Ninja (faster builds)
cmake -S . -B build -G Ninja

# Visual Studio (if installed)
cmake -S . -B build -G "Visual Studio 17 2022"

# Unix Makefiles (Git Bash/WSL)
cmake -S . -B build -G "Unix Makefiles"
```

### Release Build

```bash
# Configure for Release
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build-release --config Release

# Run (faster, no debug symbols)
.\build-release\autoscale.exe
```

### Verbose Build Output

```bash
# See full compiler commands
cmake --build build --verbose
```

