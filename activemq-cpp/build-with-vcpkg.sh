#!/usr/bin/env bash
set -euo pipefail

# Helper script to configure & build with vcpkg toolchain
# Usage: ./build-with-vcpkg.sh [additional cmake args]

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

if [ -z "${VCPKG_ROOT:-}" ]; then
  echo "VCPKG_ROOT is not set. Please set VCPKG_ROOT to your vcpkg root or pass -DCMAKE_TOOLCHAIN_FILE=<path> to cmake."
  echo "If you want quick setup, run:"
  echo "  git clone https://github.com/microsoft/vcpkg.git \$HOME/vcpkg"
  echo "  cd \$HOME/vcpkg && ./bootstrap-vcpkg.sh"
  exit 1
fi

TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
if [ ! -f "$TOOLCHAIN_FILE" ]; then
  echo "vcpkg toolchain file not found at $TOOLCHAIN_FILE"
  exit 1
fi

# If a vcpkg manifest exists in the project, run vcpkg install for the detected triplet
if [ -f "$ROOT_DIR/vcpkg.json" ]; then
  echo "Found vcpkg.json in project — using vcpkg manifest mode to install dependencies"
  case "$(uname -s)" in
    Linux*) TRIPLET="x64-linux" ;;
    Darwin*) TRIPLET="x64-osx" ;;
    *) TRIPLET="x64-windows" ;;
  esac
  VCPKG_EXE="$VCPKG_ROOT/vcpkg"
  if [ -f "${VCPKG_EXE}.exe" ]; then
    VCPKG_EXE="${VCPKG_EXE}.exe"
  fi
  echo "Running: $VCPKG_EXE install --triplet $TRIPLET (manifest)"
  "$VCPKG_EXE" install --triplet "$TRIPLET"
fi

mkdir -p "$BUILD_DIR"
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" "$@"
cmake --build "$BUILD_DIR" -- -j"$(nproc)"
