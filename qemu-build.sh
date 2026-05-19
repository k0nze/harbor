#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
qemu_source_dir="${repo_root}/external/qemu"
qemu_build_dir="${HARBOR_QEMU_BUILD_DIR:-${repo_root}/build/qemu/riscv64-softmmu}"
qemu_prefix="${HARBOR_QEMU_INSTALL_PREFIX:-${repo_root}/build/qemu/install}"
python="${HARBOR_PYTHON:-${repo_root}/.venv/bin/python}"

if [ -n "${CC:-}" ] && ! command -v "${CC}" >/dev/null 2>&1; then
  echo "[qemu] Ignoring unavailable CC=${CC}"
  unset CC
fi

if [ -n "${CXX:-}" ] && ! command -v "${CXX}" >/dev/null 2>&1; then
  echo "[qemu] Ignoring unavailable CXX=${CXX}"
  unset CXX
fi

if [ ! -x "${qemu_source_dir}/configure" ]; then
  echo "Missing QEMU submodule at ${qemu_source_dir}" >&2
  echo "Initialize it with: git submodule update --init external/qemu" >&2
  exit 1
fi

if [ ! -x "${python}" ]; then
  echo "Missing Python virtual environment: ${python}" >&2
  echo "Create it first: python3 -m venv .venv" >&2
  exit 1
fi

mkdir -p "${qemu_build_dir}"

cd "${qemu_build_dir}"

if [ -f config.status ] && ! grep -F -- "--python=${python}" config.status >/dev/null 2>&1; then
  echo "[qemu] Existing configuration does not use ${python}; reconfiguring"
  rm -f config-host.mak
fi

if [ ! -f config-host.mak ]; then
  echo "[qemu] Configuring host-native QEMU build"
  "${qemu_source_dir}/configure" \
    --python="${python}" \
    --target-list=riscv64-softmmu \
    --prefix="${qemu_prefix}" \
    --disable-docs \
    --disable-gtk \
    --disable-sdl \
    --disable-cocoa \
    --disable-werror
else
  echo "[qemu] Reusing existing QEMU configuration in ${qemu_build_dir}"
fi

echo "[qemu] Building qemu-system-riscv64"
ninja -C "${qemu_build_dir}" qemu-system-riscv64

echo "QEMU binary: ${qemu_build_dir}/qemu-system-riscv64"
