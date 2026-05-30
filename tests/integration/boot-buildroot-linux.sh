#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
log_dir="${repo_root}/build/test-logs"
linux_boot_log="${log_dir}/buildroot-linux-boot.log"
linux_boot_timeout="${LINUX_BOOT_TIMEOUT_SECONDS:-120}"
output_dir="${BUILDROOT_OUTPUT_DIR:-${repo_root}/build/buildroot/riscv64-qemu-virt}"
kernel="${output_dir}/images/Image"
rootfs="${output_dir}/images/rootfs.cpio"
harbor_qemu="${repo_root}/build/qemu/riscv64-softmmu/qemu-system-riscv64"

if [ ! -f "${kernel}" ] || [ ! -f "${rootfs}" ]; then
  echo "Missing Buildroot artifacts under ${output_dir}/images" >&2
  echo "Build them first: examples/linux/buildroot/build.sh" >&2
  exit 1
fi

echo "[integration] Verifying mmio-test is present in Buildroot rootfs"
if ! cpio -t -F "${rootfs}" 2>/dev/null | grep -qx "usr/bin/mmio-test"; then
  echo "Buildroot rootfs does not contain usr/bin/mmio-test" >&2
  echo "Rebuild it: examples/linux/buildroot/build.sh" >&2
  exit 1
fi

echo "[integration] Verifying mmio-test boot hook is present in Buildroot rootfs"
if ! cpio -t -F "${rootfs}" 2>/dev/null | grep -qx "etc/init.d/S90mmio-test"; then
  echo "Buildroot rootfs does not contain etc/init.d/S90mmio-test" >&2
  echo "Rebuild it: examples/linux/buildroot/build.sh" >&2
  exit 1
fi

mkdir -p "${log_dir}"
rm -f "${linux_boot_log}"

if [ -z "${QEMU:-}" ] && [ -x "${harbor_qemu}" ]; then
  export QEMU="${harbor_qemu}"
fi

echo "[integration] Starting QEMU for Buildroot Linux"
"${repo_root}/examples/linux/buildroot/run.sh" >"${linux_boot_log}" 2>&1 &
qemu_pid=$!

cleanup_qemu() {
  if kill -0 "${qemu_pid}" >/dev/null 2>&1; then
    kill "${qemu_pid}" >/dev/null 2>&1 || true
    wait "${qemu_pid}" >/dev/null 2>&1 || true
  fi
}

trap cleanup_qemu EXIT INT TERM

elapsed=0
linux_init_seen=0
network_seen=0
mmio_test_seen=0
mmio_write_count=0
mmio_read_count=0
last_printed_log_line=0

print_new_mmio_output() {
  if [ ! -f "${linux_boot_log}" ]; then
    return
  fi

  line_count=$(wc -l <"${linux_boot_log}" | tr -d ' ')
  if [ "${line_count}" -le "${last_printed_log_line}" ]; then
    return
  fi

  start_line=$((last_printed_log_line + 1))
  sed -n "${start_line},${line_count}p" "${linux_boot_log}" | awk '
    /Running Harbor register-file check/ { print "[linux-mmio] " $0 }
    /register-file write pass/ { print "[linux-mmio] " $0 }
    /^write 0x100100/ { print "[linux-mmio] " $0 }
    /register-file read pass/ { print "[linux-mmio] " $0 }
    /^read 0x100100/ { print "[linux-mmio] " $0 }
    /register-file check passed/ { print "[linux-mmio] " $0 }
  '

  last_printed_log_line="${line_count}"
}

while [ "${elapsed}" -lt "${linux_boot_timeout}" ]; do
  if grep -q "Run /init as init process" "${linux_boot_log}" 2>/dev/null; then
    linux_init_seen=1
  fi

  if grep -Eq "udhcpc: lease of .* obtained" "${linux_boot_log}" 2>/dev/null; then
    network_seen=1
  fi

  mmio_write_count=$(grep -c "^write 0x100100" "${linux_boot_log}" 2>/dev/null || true)
  mmio_read_count=$(grep -c "^read 0x100100" "${linux_boot_log}" 2>/dev/null || true)
  print_new_mmio_output

  if grep -q "Running Harbor register-file check" "${linux_boot_log}" 2>/dev/null &&
    grep -q "register-file write pass" "${linux_boot_log}" 2>/dev/null &&
    grep -q "register-file read pass" "${linux_boot_log}" 2>/dev/null &&
    grep -q "write 0x1001003c width 32 <- 0xa500000f" "${linux_boot_log}" 2>/dev/null &&
    grep -q "read 0x1001003c width 32 -> 0xa500000f" "${linux_boot_log}" 2>/dev/null &&
    grep -q "register-file check passed" "${linux_boot_log}" 2>/dev/null &&
    [ "${mmio_write_count}" -eq 16 ] &&
    [ "${mmio_read_count}" -eq 16 ]; then
    mmio_test_seen=1
  fi

  if [ "${linux_init_seen}" -eq 1 ] && [ "${network_seen}" -eq 1 ] && [ "${mmio_test_seen}" -eq 1 ]; then
    cleanup_qemu
    trap - EXIT INT TERM
    echo "Linux reached init, configured eth0, and ran mmio-test. Log: ${linux_boot_log}"
    exit 0
  fi

  if ! kill -0 "${qemu_pid}" >/dev/null 2>&1; then
    echo "QEMU exited before Linux reached init. Log: ${linux_boot_log}" >&2
    wait "${qemu_pid}" || true
    exit 1
  fi

  sleep 1
  elapsed=$((elapsed + 1))

  if [ $((elapsed % 10)) -eq 0 ]; then
    echo "[integration] Waiting for Linux init, eth0 DHCP, and mmio-test (${elapsed}s/${linux_boot_timeout}s). Log: ${linux_boot_log}"
  fi
done

echo "Timed out waiting for Linux init, eth0, and mmio-test after ${linux_boot_timeout}s. Log: ${linux_boot_log}" >&2
exit 1
