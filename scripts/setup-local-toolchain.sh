#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TOOLS_DIR="${ROOT_DIR}/.tools"

ARM_GNU_VERSION="${ARM_GNU_VERSION:-15.2.rel1}"
ARM_GNU_ARCHIVE="arm-gnu-toolchain-${ARM_GNU_VERSION}-x86_64-arm-none-eabi.tar.xz"
ARM_GNU_URL="https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_GNU_VERSION}/binrel/${ARM_GNU_ARCHIVE}"

OPENOCD_VERSION="${OPENOCD_VERSION:-0.12.0-7}"
OPENOCD_ARCHIVE="xpack-openocd-${OPENOCD_VERSION}-linux-x64.tar.gz"
OPENOCD_URL="https://github.com/xpack-dev-tools/openocd-xpack/releases/download/v${OPENOCD_VERSION}/${OPENOCD_ARCHIVE}"

INSTALL_OPENOCD=1

for arg in "$@"; do
    case "${arg}" in
        --arm-only)
            INSTALL_OPENOCD=0
            ;;
        *)
            echo "Unknown option: ${arg}" >&2
            exit 1
            ;;
    esac
done

mkdir -p "${TOOLS_DIR}"

download_and_extract() {
    local archive_name="$1"
    local url="$2"
    local expected_dir="$3"

    if [[ -d "${TOOLS_DIR}/${expected_dir}" ]]; then
        echo "Using existing ${expected_dir}"
        return 0
    fi

    local archive_path="${TOOLS_DIR}/${archive_name}"
    echo "Downloading ${archive_name}"
    if ! curl -L --fail --retry 3 --output "${archive_path}" "${url}"; then
        echo "curl failed, retrying with wget"
        rm -f "${archive_path}"
        wget -O "${archive_path}" "${url}"
    fi
    echo "Extracting ${archive_name}"
    tar -xf "${archive_path}" -C "${TOOLS_DIR}"
}

download_and_extract "${ARM_GNU_ARCHIVE}" "${ARM_GNU_URL}" "arm-gnu-toolchain-${ARM_GNU_VERSION}-x86_64-arm-none-eabi"

if [[ "${INSTALL_OPENOCD}" -eq 1 ]]; then
    download_and_extract "${OPENOCD_ARCHIVE}" "${OPENOCD_URL}" "xpack-openocd-${OPENOCD_VERSION}"
fi

echo
echo "Local toolchain is ready in ${TOOLS_DIR}"
echo "To use it in the current shell:"
echo "  source ./scripts/env.sh"
