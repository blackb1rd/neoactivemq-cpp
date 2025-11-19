# CMake build (vcpkg friendly)

This folder provides a minimal CMake build for `activemq-cpp` and a helper script to use the vcpkg toolchain.

Basic usage (recommended with vcpkg):

1. Install vcpkg if you don't have it:

```sh
git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg
````markdown
# CMake build (vcpkg friendly)

This folder provides a minimal CMake build for `activemq-cpp` and a helper script to use the vcpkg toolchain.

Basic usage (recommended with vcpkg):

1. Install vcpkg if you don't have it:

```sh
git clone https://github.com/microsoft/vcpkg.git $HOME/vcpkg
cd $HOME/vcpkg
./bootstrap-vcpkg.sh
```

2. Install required packages with vcpkg (example):

```sh
$HOME/vcpkg/vcpkg install openssl
```

3. Build using the helper script (sets `CMAKE_TOOLCHAIN_FILE` automatically):

```sh
# from this folder
export VCPKG_ROOT="$HOME/vcpkg"
./build-with-vcpkg.sh -DCMAKE_BUILD_TYPE=Release
```

If a `vcpkg.json` manifest is present in this folder the helper script will
automatically install the manifest dependencies for a sensible default
triplet (Linux -> `x64-linux`, macOS -> `x64-osx`, Windows -> `x64-windows`) before
configuring CMake.

Alternatively you can call `cmake` directly and pass the vcpkg toolchain file:

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=$HOME/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build -- -j4
```

Using vcpkg manifest mode manually
-------------------------------

If you prefer to control installation explicitly, from the `activemq-cpp`
folder run:

```sh
$VCPKG_ROOT/vcpkg install --triplet x64-linux
# or on Windows (PowerShell):
& "$env:VCPKG_ROOT\\vcpkg.exe" install --triplet x64-windows
```

This will read `vcpkg.json` in this folder and install the listed packages.

Notes:
- This CMake setup is intentionally minimal to get you started. It collects source files under `src/` and creates a library target `activemq-cpp`.
- It will link OpenSSL automatically if found by `find_package(OpenSSL)` (install via vcpkg as shown above).
- The CMake files do not yet replicate the full Autotools feature set (options, plugins, packaging). Use this as a starting point and I can expand targets and flags on request.

````
