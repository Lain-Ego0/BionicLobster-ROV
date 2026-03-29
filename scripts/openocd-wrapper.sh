#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/env.sh"

if [[ -n "${OPENOCD_DIR:-}" && -x "${OPENOCD_DIR}/bin/openocd" ]]; then
    exec "${OPENOCD_DIR}/bin/openocd" "$@"
fi

if command -v openocd > /dev/null 2>&1; then
    exec openocd "$@"
fi

echo "openocd not found. Run ./scripts/setup-local-toolchain.sh or install openocd." >&2
exit 1
