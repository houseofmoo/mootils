#!/usr/bin/env bash
set -euo pipefail

# ============================================================
# Usage:
#   ./builder.sh [--debug | --release] [--clean] [--run]
# ------------------------------------------------------------
# Examples:
#   ./builder.sh --debug
#   ./builder.sh --release --clean
# ============================================================

PRESET="linux-gnu"
BUILD_DIR="build"

RELEASE="false"
CLEAN="false"
RUN="false"

# -------- parse args --------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --debug)
      RELEASE="false"
      shift
      ;;
    --release)
      RELEASE="true"
      shift
      ;;
    --clean)
      CLEAN="true"
      shift
      ;;
    --run)
      RUN="true"
      shift
      ;;
    *)
      echo "Unknown option: $1" >&2
      echo "Usage: ./build.sh [--debug|--release] [--clean] [--run] [--log]"
      exit 1
      ;;
  esac
done

# -------- optional clean --------
if [[ "$CLEAN" == "true" ]]; then
  if [[ -d "$BUILD_DIR" ]]; then
    echo
    echo "Cleaning build directory \"$BUILD_DIR\"..."
    rm -rf -- "$BUILD_DIR"
  else
    echo
    echo "Build directory \"$BUILD_DIR\" does not exist, skipping clean"
  fi
fi

if [[ "$RELEASE" == "true" ]]; then
  PRESET="$PRESET-release"
else
  PRESET="$PRESET-debug"
fi

echo
echo "Running CMake configure preset \"$PRESET\""
cmake --preset "$PRESET"

echo
echo "Building project for preset \"$PRESET\""
cmake --build --preset "$PRESET" --parallel

echo
echo "Build completed successfully for preset \"$PRESET\""

# if [[ "$RUN" == "true" ]]; then
#   echo
#   echo "Running the built executable..."
#   if [[ -x "./run.sh" ]]; then
#     ./run.sh
#   else
#     # fallback: allow run.sh without +x
#     bash ./run.sh
#   fi
# fi