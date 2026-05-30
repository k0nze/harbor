#!/usr/bin/env bash
set -euo pipefail

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
qemu_source_dir="${HARBOR_QEMU_SOURCE_DIR:-${repo_root}/external/qemu}"

source_file="${repo_root}/src/qemu/harbor_register_file.c"
target_file="${qemu_source_dir}/hw/misc/harbor_register_file.c"
stale_cpp_file="${qemu_source_dir}/hw/misc/harbor_register_file.cpp"

if [[ ! -x "${qemu_source_dir}/configure" ]]; then
  echo "Missing QEMU submodule at ${qemu_source_dir}" >&2
  echo "Initialize it with: git submodule update --init external/qemu" >&2
  exit 1
fi

if [[ ! -f "${source_file}" ]]; then
  echo "Missing Harbor QEMU source file: ${source_file}" >&2
  exit 1
fi

# Keep generated QEMU-side source synchronized with Harbor-owned source.
install -m 0644 "${source_file}" "${target_file}"

# Remove the stale generated C++ file from the failed direct-C++ flow.
rm -f "${stale_cpp_file}"

insert_after_once() {
  local file="$1"
  local needle="$2"
  local marker="$3"
  local block="$4"
  local tmp

  if grep -Fq "${marker}" "${file}"; then
    return 0
  fi

  tmp=$(mktemp)
  awk -v needle="${needle}" -v block="${block}" '
    { print }
    index($0, needle) { print block }
  ' "${file}" >"${tmp}"
  mv "${tmp}" "${file}"

  if ! grep -Fq "${marker}" "${file}"; then
    echo "Failed to insert Harbor integration into ${file}" >&2
    exit 1
  fi
}

insert_before_once() {
  local file="$1"
  local needle="$2"
  local marker="$3"
  local block="$4"
  local tmp

  if grep -Fq "${marker}" "${file}"; then
    return 0
  fi

  tmp=$(mktemp)
  awk -v needle="${needle}" -v block="${block}" '
    index($0, needle) { print block }
    { print }
  ' "${file}" >"${tmp}"
  mv "${tmp}" "${file}"

  if ! grep -Fq "${marker}" "${file}"; then
    echo "Failed to insert Harbor integration into ${file}" >&2
    exit 1
  fi
}

append_once() {
  local file="$1"
  local marker="$2"
  local block="$3"

  if grep -Fq "${marker}" "${file}"; then
    return 0
  fi

  printf "\n%s\n" "${block}" >>"${file}"
}

insert_before_once \
  "${qemu_source_dir}/hw/misc/Kconfig" \
  "config SIFIVE_E_PRCI" \
  "config HARBOR_REGISTER_FILE" \
  "config HARBOR_REGISTER_FILE
    bool
"

insert_after_once \
  "${qemu_source_dir}/hw/misc/meson.build" \
  "system_ss.add(when: 'CONFIG_MCHP_PFSOC_SYSREG', if_true: files('mchp_pfsoc_sysreg.c'))" \
  "harbor_qemu_adapter = declare_dependency(" \
  "harbor_qemu_adapter = declare_dependency(
  link_args: [
    get_option('harbor_qemu_adapter_lib'),
    get_option('harbor_core_lib'),
    get_option('harbor_cxx_runtime'),
  ],
)
system_ss.add(when: 'CONFIG_HARBOR_REGISTER_FILE',
              if_true: [files('harbor_register_file.c'), harbor_qemu_adapter])"

append_once \
  "${qemu_source_dir}/meson_options.txt" \
  "option('harbor_qemu_adapter_lib'" \
  "option('harbor_qemu_adapter_lib', type: 'string', value: '',
       description: 'absolute path to Harbor QEMU adapter static library')
option('harbor_core_lib', type: 'string', value: '',
       description: 'absolute path to Harbor core static library')
option('harbor_cxx_runtime', type: 'string', value: '',
       description: 'C++ runtime linker argument required by Harbor adapter')"

insert_after_once \
  "${qemu_source_dir}/hw/riscv/Kconfig" \
  "    select RISCV_IMSIC" \
  "    select HARBOR_REGISTER_FILE" \
  "    select HARBOR_REGISTER_FILE"

insert_after_once \
  "${qemu_source_dir}/hw/riscv/virt.c" \
  "    [VIRT_TEST] =         {   0x100000,        0x1000 }," \
  "    [VIRT_HARBOR_REGISTER_FILE] = { 0x10010000, 0x40 }," \
  "    [VIRT_HARBOR_REGISTER_FILE] = { 0x10010000, 0x40 },"

insert_after_once \
  "${qemu_source_dir}/hw/riscv/virt.c" \
  "    sifive_test_create(s->memmap[VIRT_TEST].base);" \
  "    sysbus_create_simple(\"harbor-register-file\"," \
  "
    /* Harbor register-file MMIO device */
    sysbus_create_simple(\"harbor-register-file\",
                         s->memmap[VIRT_HARBOR_REGISTER_FILE].base, NULL);"

insert_after_once \
  "${qemu_source_dir}/include/hw/riscv/virt.h" \
  "    VIRT_TEST," \
  "    VIRT_HARBOR_REGISTER_FILE," \
  "    VIRT_HARBOR_REGISTER_FILE,"

echo "[qemu] Installed Harbor QEMU source: ${target_file}"
echo "[qemu] Installed Harbor QEMU integration into ${qemu_source_dir}"
