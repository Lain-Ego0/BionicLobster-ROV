#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS_DIR="${ROOT_DIR}/.tools"

find_latest_dir() {
    local pattern="$1"
    local latest=""
    shopt -s nullglob
    for candidate in "${TOOLS_DIR}"/${pattern}; do
        if [[ -d "${candidate}" ]]; then
            latest="${candidate}"
        fi
    done
    shopt -u nullglob
    if [[ -n "${latest}" ]]; then
        printf '%s\n' "${latest}"
    fi
}

ARM_GNU_TOOLCHAIN_DIR="${ARM_GNU_TOOLCHAIN_DIR:-$(find_latest_dir 'arm-gnu-toolchain-*')}"
OPENOCD_DIR="${OPENOCD_DIR:-$(find_latest_dir 'xpack-openocd-*')}"
OPENOCD_CONFIG="${OPENOCD_CONFIG:-${ROOT_DIR}/openocd/daplink-stm32f103c8.cfg}"

export OPENOCD_CONFIG

if [[ -n "${ARM_GNU_TOOLCHAIN_DIR}" ]]; then
    export ARM_GNU_TOOLCHAIN_DIR
    export PATH="${ARM_GNU_TOOLCHAIN_DIR}/bin:${PATH}"
fi

if [[ -n "${OPENOCD_DIR}" ]]; then
    export OPENOCD_DIR
    export PATH="${OPENOCD_DIR}/bin:${PATH}"
fi
