#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/env.sh"

if [[ -n "${ARM_GNU_TOOLCHAIN_DIR:-}" && -x "${ARM_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-gdb" ]]; then
    exec "${ARM_GNU_TOOLCHAIN_DIR}/bin/arm-none-eabi-gdb" "$@"
fi

if command -v arm-none-eabi-gdb > /dev/null 2>&1; then
    exec arm-none-eabi-gdb "$@"
fi

echo "arm-none-eabi-gdb not found. Run ./scripts/setup-local-toolchain.sh first." >&2
exit 1
