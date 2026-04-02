# **music++** - Turborepo Monorepo

A Turborepo monorepo containing the **music++** C++ library for vectorial representation and manipulation of musical phenomena.

## Overview

This repository uses [Turborepo](https://turbo.build/repo) to manage a monorepo workspace containing:

- **packages/cpp-sdk**: Core C++ **header-only** library (`namespace musicpp`, snake_case headers and public API)
- Future packages: TypeScript SDK, Python SDK

The C++ library provides a unified framework for scales, chords, rhythms, and transformations using mathematical and algorithmic tools.

For a concise map of the recent C++ API and file renames, see [CPP_SDK_REFACTOR.md](CPP_SDK_REFACTOR.md).

## Prerequisites

- **Node.js** (v18 or later) and **npm** (v10 or later)
- **CMake** (v3.16 or later)
- **Doxygen** (optional) — for generating API documentation from `packages/cpp-sdk`
- **C++17** compiler (GCC, Clang, Apple Clang, or MSVC). The cpp-sdk **CMake presets** on Windows assume **MinGW Makefiles** (MinGW-w64); adjust or use your own generator if you use MSVC only.

## Quick Start

### Installation

```bash
git clone <repository-url>
cd musicplusplus
npm install
```

### Build and test

```bash
# Build all workspaces (cpp-sdk uses CMake presets via package script)
npm run build

# Run tests (cpp-sdk: configure, build, ctest)
npm run test
```

## Monorepo Structure

```
musicplusplus/
├── packages/
│   └── cpp-sdk/
│       ├── src/                 # Header-only library (*.h)
│       ├── tests/               # Catch2 test executables
│       ├── build/               # CMake output (generated; gitignored)
│       ├── CMakeLists.txt
│       ├── CMakePresets.json    # unix / windows (+ release variants)
│       ├── Doxyfile
│       ├── package.json
│       ├── DEBUGGING.md
│       └── README.md
├── CPP_SDK_REFACTOR.md          # C++ rename / design notes
├── package.json
├── turbo.json
└── README.md
```

## Working with the C++ SDK

### Build

From `packages/cpp-sdk`:

```bash
npm run build
```

This runs CMake with the **`unix`** or **`windows`** preset (see `CMakePresets.json`) and builds the **Catch2**-linked test binaries into `build/`.

Or manually:

```bash
cd packages/cpp-sdk
cmake --preset unix    # or windows on Windows
cmake --build --preset unix
```

### Run tests

```bash
cd packages/cpp-sdk
npm run test
```

Or after a build:

```bash
cd packages/cpp-sdk/build
ctest --output-on-failure
```

Test executables (e.g. `vectors_test`, `chord_test`) are produced next to the build tree; exact paths depend on the generator (e.g. `build/vectors_test.exe` on Windows with MinGW).

### Documentation (Doxygen)

```bash
cd packages/cpp-sdk
npm run docs
npm run docs:clean   # remove generated docs/
```

Output: `packages/cpp-sdk/docs/html/index.html`.

### Clean

```bash
cd packages/cpp-sdk
npm run clean
```

## Turborepo commands (root)

```bash
npm run build   # all packages
npm run test    # all packages
npm run lint
npm run dev
npm run clean
```

Scoped:

```bash
npm run build --workspace=packages/cpp-sdk
npm run test --workspace=packages/cpp-sdk
```

## C++ library features (current API)

Public types live in **`namespace musicpp`** (see headers under `packages/cpp-sdk/src/`):

- **Vectors**: `position_vector`, `interval_vector`, `binary_vector`, and **`vector_set`** (synchronized triple) in `vectors.h`; conversions **`positions_to_intervals`**, **`intervals_to_positions`**, **`positions_to_binary`**
- **Math**: `math_util.h` — `euclidean_division`, `gcd`, `lcm`
- **Meta-operators**: `selection.h` — selection and modal operations
- **Chord & scale**: `chord.h`, `scale.h`, `scale_dictionary.h`, `chord_names.h`
- **Distances**: `distances.h`
- **Matrices**: `matrix.h`, `matrix_distance.h`
- **Rhythm**: `rhythm_gen.h`, `melody.h`
- **Note names**: `note_names.h`
- **Analysis**: `measures.h`
- **Automation**: `automations.h`
- **Other**: `slonimsky.h`, `quantize_transpose.h`

Detailed layout and bibliography: [packages/cpp-sdk/README.md](packages/cpp-sdk/README.md).

## Turbo configuration

`turbo.json` defines **build**, **test**, **docs**, **dev**, **clean** with caching where appropriate.

## License

MIT. See [packages/cpp-sdk/LICENSE](packages/cpp-sdk/LICENSE).

## Contributing

1. Change headers in `packages/cpp-sdk/src/` (keep **`musicpp`** and **`MUSICPP_*_H`** conventions).
2. Extend or add **Catch2** tests under `packages/cpp-sdk/tests/`.
3. Run **`npm run test --workspace=packages/cpp-sdk`**.
4. Update Doxygen comments and run **`npm run docs --workspace=packages/cpp-sdk`** if the public API changed.
5. Update root / package READMEs when behavior or layout changes.

---

Package-specific details: [packages/cpp-sdk/README.md](packages/cpp-sdk/README.md) · Debugging: [packages/cpp-sdk/DEBUGGING.md](packages/cpp-sdk/DEBUGGING.md)
