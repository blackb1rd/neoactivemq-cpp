#!/usr/bin/env bash
set -euo pipefail

# This script moves legacy Autotools/Makefile files into `legacy-autotools`.
# It uses `git mv` where possible so history is preserved. Run and commit the result.

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
LEGACY_DIR="$ROOT_DIR/legacy-autotools"
mkdir -p "$LEGACY_DIR"

files=(
  "Makefile.am"
  "Makefile.win"
  "autogen.sh"
  "build.sh"
  "build.conf"
  "configure.ac"
  "activemq-cpp.pc.in"
  "activemqcpp-config.in"
  "doxygen-include.am"
  "m4"
)

for f in "${files[@]}"; do
  src="$ROOT_DIR/$f"
  if [ -e "$src" ]; then
    if git rev-parse --git-dir > /dev/null 2>&1; then
      git mv -f "$src" "$LEGACY_DIR/" || {
        cp -a "$src" "$LEGACY_DIR/"
        rm -rf "$src"
      }
    else
      cp -a "$src" "$LEGACY_DIR/"
      rm -rf "$src"
    fi
    echo "Moved $f -> legacy-autotools/"
  fi
done

echo "Legacy Autotools files moved to: $LEGACY_DIR"
echo "Review, then commit the changes:"
echo "  git add $LEGACY_DIR && git commit -m 'Move legacy autotools files to legacy-autotools/'"
