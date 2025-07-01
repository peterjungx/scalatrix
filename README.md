# Scalatrix

Scalatrix is a C++ library for generating musical scales using affine transformations and lattice operations, with support for both native and WebAssembly (Wasm) builds.

## Features

Generate scales with customizable base frequencies and affine transforms.
Supports native (scalatrix.a) and Wasm (scalatrix.js) targets.
Examples for native CLI and Wasm browser visualization.

## Prerequisites
- CMake (>= 3.10)
- Eigen3 (install via brew install eigen on macOS)
- Emscripten (install via brew install emscripten on macOS, version 4.0.1 tested)
- Python (for serving Wasm examples)

## Directory Structure

```
scalatrix/
├── examples/
│   ├── native/         # Native example (CLI)
│   └── wasm/           # Wasm example (browser)
├── include/scalatrix/  # Header files
├── src/                # Source files
├── build/              # Build artifacts (native and wasm)
├── setup_cmake.sh      # Setup script
└── CMakeLists.txt      # Root CMake config
```

## Building

### Standard CMake Build (Recommended)

Create a build directory and build the project:

```bash
mkdir -p build && cd build
cmake ..
make
```

This creates:
- `build/libscalatrix.a` - Static library
- `build/scalatrix_example` - Native example executable

**Important**: All build artifacts are contained in the `build/` directory. Do not run CMake or make directly in the project root.

### Alternative Build Options

The project also supports specialized builds via the setup script:

```bash
chmod +x setup_cmake.sh
./setup_cmake.sh
make native    # Creates build/native/scalatrix.a
make wasm      # Creates build/wasm/scalatrix.js  
make build_all # Build all targets
make clean_all # Clean all builds
```

## Examples

### Native Example

The example is built automatically with the main project. Run it from the build directory:

```bash
cd build
./scalatrix_example
```

This prints scale information and demonstrates various scalatrix functionality including MOS generation, pitch sets, and retuning operations.

### Wasm Example

Build:
```bash
cd examples/wasm
emcmake cmake . -DCMAKE_TOOLCHAIN_FILE=/opt/homebrew/Cellar/emscripten/4.0.1/libexec/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_PREFIX_PATH="/opt/homebrew/Cellar/eigen/3.4.0_1"
emmake make
```

Serve:
```bash
python -m http.server
```

View:

Open http://localhost:8000/wasm_example.html in a browser.

Displays a canvas with 128 scale nodes as blue dots.

## Notes

Eigen Path: Adjust Eigen3_DIR in CMakeLists.txt if your Eigen install differs (e.g., /usr/local/Cellar/eigen/3.4.0_1).
Emscripten: Ensure emcc --version matches 4.0.1 or adjust paths accordingly.
Bindings: Wasm uses Embind (--bind)—see src/main.cpp for details.

## Contributing
Feel free to fork, tweak, and submit pull requests. Issues welcome!

## License
MIT License