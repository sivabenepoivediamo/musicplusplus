## Build system

This package uses **CMake** with **CMake Presets**. It targets Windows, Linux, and macOS; on Windows the default preset uses **MinGW Makefiles**.

### Prerequisites

- **CMake 3.16+** (presets use schema version 6; 3.19+ is typical for preset workflows)
- **A C++17 compiler**: GCC, Clang, AppleClang, or MinGW-w64

There is **no** `examples/` tree here. Executables are **Catch2 tests** under `tests/`; CMake links each test to **`musicplusplus`** (header-only `INTERFACE` target) and **`Catch2::Catch2WithMain`**.

## How to run or debug a test

### Method 1: VS Code debugger (recommended)

1. Open **`packages/cpp-sdk`** as the VS Code workspace folder (or ensure the active folder is this package so `.vscode/` applies).
2. Open a test source such as `tests/vectors_test.cpp`.
3. Pick **Debug Test (Linux)**, **Debug Test (macOS)**, or **Debug Test (Windows)** from the Run and Debug dropdown (match your OS).
4. Press **F5**. The **preLaunchTask** runs **CMake: Build active test**, which builds the target whose name equals the **current file’s basename without extension** (e.g. `vectors_test.cpp` → target `vectors_test`).
5. The debugger starts the matching binary under `build/` (on Windows, `build/vectors_test.exe`).

**Requirement:** The CMake target name must match the test filename stem. That is how this repo’s `CMakeLists.txt` defines targets (`vectors_test`, `chord_test`, …).

### Method 2: Build the active test only

1. Open the test file you care about (still under `tests/`).
2. **Ctrl+Shift+B** (default build task) runs **CMake: Build active test**.
3. Run from a terminal:

   ```powershell
   # Windows
   .\build\vectors_test.exe

   # Linux / macOS
   ./build/vectors_test
   ```

### Method 3: Full configure, build, and ctest

From the monorepo root:

```bash
npm run build --workspace=packages/cpp-sdk
npm run test  --workspace=packages/cpp-sdk
```

From `packages/cpp-sdk`:

```bash
npm run build
npm run test
```

Or with presets directly (use **`windows`** / **`windows-release`** on Windows):

```bash
cmake --preset unix
cmake --build --preset unix
ctest --preset unix --output-on-failure
```

## What is configured

### CMake presets (`CMakePresets.json`)

- **unix** / **unix-release** — non-Windows hosts (Unix Makefiles); binary dir **`build`** or **`build-release`**
- **windows** / **windows-release** — Windows (MinGW Makefiles); same binary dir layout

`npm run build` / `npm run test` in this package pick the preset that matches the host.

### VS Code tasks (`packages/cpp-sdk/.vscode/tasks.json`)

1. **CMake: Build active test** (default **Ctrl+Shift+B**) — builds `--target ${fileBasenameNoExtension}` in `build/`.
2. **CMake: Configure** — runs `cmake --preset unix` or `windows` via a small Node one-liner.
3. **CMake: Build all tests** — `cmake --build build` (all test executables).

### VS Code launch (`packages/cpp-sdk/.vscode/launch.json`)

- **Debug Test (Linux)** — GDB, program `build/${fileBasenameNoExtension}`
- **Debug Test (macOS)** — LLDB, same program path
- **Debug Test (Windows)** — GDB, program `build/${fileBasenameNoExtension}.exe`

All three use **preLaunchTask**: **CMake: Build active test**.

## Test executables (`tests/`)

| Source file        | CMake target     |
|--------------------|------------------|
| `vectors_test.cpp` | `vectors_test`   |
| `chord_test.cpp`   | `chord_test`     |
| `scale_test.cpp`   | `scale_test`     |
| `automation_test.cpp` | `automation_test` |
| `matrix_test.cpp`  | `matrix_test`    |
| `selection_test.cpp` | `selection_test` |
| `analysis_test.cpp`| `analysis_test`  |
| `rhythm_test.cpp`  | `rhythm_test`    |
| `note_names_test.cpp` | `note_names_test` |
| `slonimsky_test.cpp` | `slonimsky_test` |

## Quick start

1. Run **CMake: Configure** once (or `npm run build` from this package).
2. Open `tests/vectors_test.cpp`.
3. Press **F5** with **Debug Test** for your OS selected.

## Architecture notes

### Header-only library

- Implementation lives in headers under `src/`.
- Consumers get **`musicplusplus`** as an `INTERFACE` library with `target_include_directories(... INTERFACE src)` and C++17.

### `CMakeLists.txt` (tests)

```cmake
add_library(musicplusplus INTERFACE)
target_include_directories(musicplusplus INTERFACE src)
target_compile_features(musicplusplus INTERFACE cxx_std_17)

FetchContent_MakeAvailable(Catch2)
enable_testing()

function(add_musicpp_test test_name source_file)
    add_executable(${test_name} ${source_file})
    target_link_libraries(${test_name} PRIVATE musicplusplus Catch2::Catch2WithMain)
    target_include_directories(${test_name} PRIVATE tests)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()

add_musicpp_test(vectors_test tests/vectors_test.cpp)
# ... other tests ...
```

## Troubleshooting

### CMake not found

Install CMake (e.g. Windows: `winget install Kitware.CMake`; macOS: `brew install cmake`; Linux: distro package). See https://cmake.org/download/

### No compiler found

Install a C++17 toolchain (MinGW-w64 on Windows, Xcode CLT on macOS, `g++` on Linux).

### “Target not found” when building the active test

The open editor file must be a **`tests/*_test.cpp`** whose basename matches a declared target (see table above). If you have a header or a non-test file focused, the build task passes the wrong target name.

Configure first: **CMake: Configure** or `cmake --preset …`.

### Breakpoints not hit

- Use a **Debug** configuration build (default preset uses `CMAKE_BUILD_TYPE=Debug`).
- Start debugging with **F5** so the **preLaunchTask** rebuilds the test you are editing.

### Clean rebuild

```bash
npm run clean
npm run build
```

On Windows, delete `build` manually if needed, then `cmake --preset windows` and `cmake --build --preset windows`.

## Advanced usage

### Release build + tests

```bash
cmake --preset unix-release
cmake --build --preset unix-release
ctest --preset unix-release --output-on-failure
```

### Verbose build

```bash
cmake --build build --verbose
```
