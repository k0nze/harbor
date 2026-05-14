#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
elf="${repo_root}/build/examples/riscv/minimal/riscv-hello.elf"

if ! command -v qemu-system-riscv64 >/dev/null 2>&1; then
  echo "qemu-system-riscv64 not found. Install QEMU first." >&2
  exit 1
fi

if [ ! -f "${elf}" ]; then
  echo "Missing ELF: ${elf}" >&2
  echo "Build it first: cmake --build build --target harbor_riscv_c" >&2
  exit 1
fi

exec qemu-system-riscv64 \
  -machine virt \
  -nographic \
  -bios none \
  -kernel "${elf}"
