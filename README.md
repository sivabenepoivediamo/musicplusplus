# **music++** - Turborepo Monorepo

A Turborepo monorepo containing the **music++** C++ library for vectorial representation and manipulation of musical phenomena.

## Overview

This repository uses [Turborepo](https://turbo.build/repo) to manage a monorepo workspace containing:

- **packages/cpp-sdk**: Core C++ library for musical vector manipulation
- Future packages: TypeScript SDK, Python SDK

The C++ library provides a unified framework for working with musical structures such as scales, chords, rhythms, and transformations using mathematical and algorithmic tools.

## Prerequisites

Before getting started, ensure you have the following installed:

- **Node.js** (v18 or later) and **npm** (v10 or later)
- **CMake** (v3.10 or later) - for building C++ examples
- **Doxygen** - for generating API documentation
- **C++ Compiler** with C++17 support (e.g., GCC, Clang, MSVC)

## Quick Start

### Installation

```bash
# Clone the repository
git clone <repository-url>
cd vectors

# Install dependencies (includes Turborepo)
npm install
```

### Build All Packages

```bash
# Build all packages in the monorepo
npm run build
```

This command uses Turborepo to build all packages in the correct dependency order.

## Monorepo Structure

```
musicplusplus/
├── packages/
│   └── cpp-sdk/              # C++ library package
│       ├── src/              # Header-only library source files
│       ├── examples/         # Example programs demonstrating library features
│       ├── docs/             # Generated Doxygen documentation
│       ├── build/            # CMake build output (generated)
│       ├── CMakeLists.txt    # CMake configuration
│       ├── Doxyfile          # Doxygen configuration
│       ├── package.json      # Package configuration with build scripts
│       └── README.md         # C++ library documentation
├── node_modules/
│   └── cpp-sdk/              # Symlink to packages/cpp-sdk
├── package.json              # Root package configuration
├── turbo.json                # Turborepo configuration
└── README.md                 # This file
```

## Working with the C++ SDK

### Building Examples

The C++ SDK uses CMake to build example executables:

```bash
cd packages/cpp-sdk

# Configure and build using npm scripts
npm run build

# Or manually with CMake
cmake -B build -S .
cmake --build build
```

This creates executables for all example files in the `build/` directory.

### Running Examples

After building, you can run any example:

```bash
# On Windows
.\packages\cpp-sdk\build\Debug\vectortest.exe

# On Linux/macOS
./packages/cpp-sdk/build/vectortest
```

### Generating Documentation

Generate Doxygen HTML documentation:

```bash
cd packages/cpp-sdk

# Generate documentation
npm run docs

# Clean documentation
npm run docs:clean
```

Documentation will be available at `packages/cpp-sdk/docs/html/index.html`.

### Cleaning Build Artifacts

```bash
cd packages/cpp-sdk

# Clean CMake build files
npm run clean

# Clean documentation
npm run docs:clean
```

## Turborepo Commands

### Root Level Commands

From the root directory, you can run commands across all packages:

```bash
# Build all packages
npm run build

# Run tests in all packages
npm run test

# Run linting in all packages
npm run lint

# Start development mode in all packages
npm run dev

# Clean all packages
npm run clean
```

### Package-Specific Commands

You can also run commands in specific packages:

```bash
# Build only cpp-sdk
npm run build --workspace=packages/cpp-sdk

# Generate cpp-sdk documentation
npm run docs --workspace=packages/cpp-sdk
```

## Development Workflow

### Adding a New Example

1. Create a new `.cpp` file in `packages/cpp-sdk/examples/`
2. Include necessary headers from `src/`
3. Rebuild the project: `npm run build --workspace=packages/cpp-sdk`
4. The new executable will be created in `packages/cpp-sdk/build/`

### Modifying the Library

1. Edit or add header files in `packages/cpp-sdk/src/`
2. Update Doxygen comments for documentation
3. Rebuild examples to test changes: `npm run build --workspace=packages/cpp-sdk`
4. Regenerate documentation: `npm run docs --workspace=packages/cpp-sdk`

## C++ Library Features

The **vectors** library provides:

- **Vector Classes**: PositionVector, IntervalVector, BinaryVector, and unified Vectors container
- **Meta-Operators**: Selection and transformation operators for musical structures
- **Chord & Scale Utilities**: Generation and transformation of chords and scales
- **Distance Metrics**: Euclidean, Manhattan, Hamming, edit distance, and more
- **Matrix Utilities**: Modal matrices, transposition matrices, transformation distance
- **Rhythmic Generators**: Euclidean rhythms, Clough-Douthett, deep rhythms, tihai
- **Note Naming**: MIDI/position to human-readable note names with enharmonic handling
- **Analysis Tools**: Spectrum, symmetry, entropy, deepness, geodesic distances
- **Automation Helpers**: Voice-leading, degree automation, modal interchange, modulation

For detailed library documentation, see [packages/cpp-sdk/README.md](packages/cpp-sdk/README.md).

## Turbo Configuration

The `turbo.json` file configures task pipelines:

- **build**: Builds packages with dependency ordering
- **test**: Runs tests after building
- **docs**: Generates documentation with caching
- **dev**: Runs development servers (no caching, persistent)
- **clean**: Cleans build artifacts

Tasks automatically cache outputs for faster incremental builds.

## License

MIT License. See [LICENSE](packages/cpp-sdk/LICENSE) for details.

## Next Steps

- Explore the [C++ SDK documentation](packages/cpp-sdk/README.md)
- Browse [example programs](packages/cpp-sdk/examples/)
- View the [generated API documentation](packages/cpp-sdk/docs/html/index.html) (after running `npm run docs`)
- Read about the [Turborepo features](https://turbo.build/repo/docs)

## Contributing

When adding new features:

1. Update source files in `packages/cpp-sdk/src/`
2. Add example usage in `packages/cpp-sdk/examples/`
3. Document with Doxygen-style comments
4. Rebuild and test: `npm run build && npm run docs`
5. Update this README if adding major features

---

For package-specific information, see the README files in each package directory.



