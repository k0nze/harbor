#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
output_dir="${BUILDROOT_OUTPUT_DIR:-${repo_root}/build/buildroot/riscv64-qemu-virt}"
qemu="${QEMU:-qemu-system-riscv64}"
kernel="${output_dir}/images/Image"
rootfs="${output_dir}/images/rootfs.cpio"

if ! command -v "${qemu}" >/dev/null 2>&1; then
  echo "QEMU executable not found: ${qemu}" >&2
  exit 1
fi

if [ ! -f "${kernel}" ] || [ ! -f "${rootfs}" ]; then
  echo "Missing Buildroot artifacts under ${output_dir}/images" >&2
  echo "Build them first: examples/linux/buildroot/build.sh" >&2
  exit 1
fi

exec "${qemu}" \
  -machine virt \
  -m 512M \
  -nographic \
  -bios default \
  -kernel "${kernel}" \
  -initrd "${rootfs}" \
  -append "console=ttyS0"
