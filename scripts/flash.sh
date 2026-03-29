#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
ELF_FILE="${1:-${ROOT_DIR}/build/firmware.elf}"

if [[ ! -f "${ELF_FILE}" ]]; then
    "${ROOT_DIR}/scripts/build.sh"
fi

source "${ROOT_DIR}/scripts/env.sh"

if [[ -n "${OPENOCD_DIR:-}" && -x "${OPENOCD_DIR}/bin/openocd" ]] || command -v openocd > /dev/null 2>&1; then
    SCRIPT_SEARCH_DIRS=()
    if [[ -n "${OPENOCD_DIR:-}" && -d "${OPENOCD_DIR}/openocd/scripts" ]]; then
        SCRIPT_SEARCH_DIRS+=("-s" "${OPENOCD_DIR}/openocd/scripts")
    fi

    "${ROOT_DIR}/scripts/openocd-wrapper.sh" \
        "${SCRIPT_SEARCH_DIRS[@]}" \
        -f "${OPENOCD_CONFIG}" \
        -c "program ${ELF_FILE} verify reset exit"
    exit 0
fi

echo "OpenOCD is required for DAPLink flashing. Run ./scripts/setup-local-toolchain.sh first." >&2
exit 1
