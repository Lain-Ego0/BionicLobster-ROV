#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if ! compgen -G "${ROOT_DIR}/.tools/arm-gnu-toolchain-*/bin/arm-none-eabi-gcc" > /dev/null; then
    "${ROOT_DIR}/scripts/setup-local-toolchain.sh" --arm-only
fi

source "${ROOT_DIR}/scripts/env.sh"

cmake \
    -S "${ROOT_DIR}" \
    -B "${ROOT_DIR}/build" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_TOOLCHAIN_FILE="${ROOT_DIR}/cmake/arm-none-eabi-gcc.cmake"

cmake --build "${ROOT_DIR}/build"
