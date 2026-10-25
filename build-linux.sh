#!/usr/bin/env bash
set -euo pipefail

usage() {
	cat <<'EOF'
usage: ./build-linux.sh [-Debug|-Release|-ReleaseDebInfo|-RelWithDebInfo|-MinSizeRel]
       no argument builds Debug
EOF
}

BUILD_TYPE=Debug
for arg in "$@"; do
	case "${arg,,}" in
		-debug)          BUILD_TYPE=Debug ;;
		-release)        BUILD_TYPE=Release ;;
		-releasedebinfo) BUILD_TYPE=ReleaseDebInfo ;;
		-relwithdebinfo) BUILD_TYPE=RelWithDebInfo ;;
		-minsizerel)     BUILD_TYPE=MinSizeRel ;;
		-h|--help)       usage; exit 0 ;;
		*) echo "error: unknown argument: $arg" >&2; usage >&2; exit 1 ;;
	esac
done

ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build-linux"
BIN_DIR="$ROOT_DIR/bin-linux"
JOBS="$(nproc 2>/dev/null || echo 4)"

mkdir -p "$BIN_DIR"

echo "=== configure: $BUILD_TYPE ==="
cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -G "Unix Makefiles" \
	-DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
	-DLMNS_BIN_DIR="$BIN_DIR" \
	-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

echo "=== build ==="
cmake --build "$BUILD_DIR" --parallel "$JOBS"

echo
echo "build type : $BUILD_TYPE"
echo "build dir  : $BUILD_DIR"
echo "test bins  : $BIN_DIR/tests/$BUILD_TYPE"
