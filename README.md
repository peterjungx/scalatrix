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

### Setup

Run the setup script to configure native and Wasm builds:

```bash
chmod +x setup_cmake.sh
./setup_cmake.sh
```

Creates `build/native/` and `build/wasm/` with respective Makefiles.

### Native Build

```bash
make native
```

Outputs: `build/native/scalatrix.a`

### Wasm Build

```bash
make wasm
```
Outputs: `build/wasm/scalatrix.js`

### Clean

```bash
make clean_all
```

### Build All

```bash
make build_all
```

## Examples

### Native Example

Build:

```bash
cd examples/native
cmake .
make
```

Run:

```bash
./native_example
```

Prints a 5-note scale segment starting at index 58.

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