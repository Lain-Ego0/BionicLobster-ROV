#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
source "${ROOT_DIR}/scripts/env.sh"

SCRIPT_SEARCH_DIRS=()
if [[ -n "${OPENOCD_DIR:-}" && -d "${OPENOCD_DIR}/openocd/scripts" ]]; then
    SCRIPT_SEARCH_DIRS+=("-s" "${OPENOCD_DIR}/openocd/scripts")
fi

exec "${ROOT_DIR}/scripts/openocd-wrapper.sh" \
    "${SCRIPT_SEARCH_DIRS[@]}" \
    -f "${OPENOCD_CONFIG}" \
    "$@"
