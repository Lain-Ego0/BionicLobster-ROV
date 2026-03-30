#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/env.sh"

if [[ -n "${ARM_GNU_TOOLCHAIN_DIR:-}" && -x "${ARM_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc" ]]; then
    exec "${ARM_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-gcc" "$@"
fi

if command -v arm-none-eabi-gcc > /dev/null 2>&1; then
    exec arm-none-eabi-gcc "$@"
fi

echo "arm-none-eabi-gcc not found. Run ./scripts/setup-local-toolchain.sh first." >&2
exit 1
