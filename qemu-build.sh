#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
qemu_source_dir="${repo_root}/external/qemu"
qemu_build_dir="${HARBOR_QEMU_BUILD_DIR:-${repo_root}/build/qemu/riscv64-softmmu}"
qemu_prefix="${HARBOR_QEMU_INSTALL_PREFIX:-${repo_root}/build/qemu/install}"
harbor_build_dir="${HARBOR_BUILD_DIR:-${repo_root}/build/harbor}"
qemu_install_sources_script="${repo_root}/scripts/qemu-install-harbor-sources.sh"
python="${HARBOR_PYTHON:-${repo_root}/.venv/bin/python}"
qemu_mode="systemc"

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

echo "[qemu] Building Harbor QEMU adapter"
harbor_qemu_target="harbor_qemu_adapter"
harbor_qemu_adapter_lib="${harbor_build_dir}/libharbor_qemu_adapter.a"
harbor_systemc_lib="${harbor_build_dir}/libharbor_systemc.a"
harbor_systemc_link_args="$(pkg-config --libs systemc)"
qemu_extra_cflags="-I${repo_root}/include"

echo "[qemu] Enabling Harbor SystemC-backed MMIO adapter"

cmake -S "${repo_root}" -B "${harbor_build_dir}" \
  -DHARBOR_BUILD_RISCV_EXAMPLES=OFF \
  -DHARBOR_BUILD_TESTS=OFF \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DHARBOR_ENABLE_SYSTEMC=ON
cmake --build "${harbor_build_dir}" --target "${harbor_qemu_target}"

harbor_core_lib="${harbor_build_dir}/libharbor.a"

if [ ! -f "${harbor_core_lib}" ] || [ ! -f "${harbor_qemu_adapter_lib}" ]; then
  echo "Missing Harbor QEMU adapter libraries in ${harbor_build_dir}" >&2
  exit 1
fi

if [ -n "${harbor_systemc_lib}" ] && [ ! -f "${harbor_systemc_lib}" ]; then
  echo "Missing Harbor SystemC library: ${harbor_systemc_lib}" >&2
  exit 1
fi

case "$(uname -s)" in
  Darwin)
    harbor_cxx_runtime="-lc++"
    ;;
  *)
    harbor_cxx_runtime="-lstdc++"
    ;;
esac

if [ -x "${qemu_install_sources_script}" ]; then
  "${qemu_install_sources_script}"
fi

integration_signature() {
  find "${repo_root}/scripts" "${repo_root}/src/qemu" -type f \
    \( -name 'qemu-install-harbor-sources.sh' -o -name 'harbor_register_file.c' \) \
    -exec cksum {} \; |
    sort |
    cksum |
    awk '{ print $1 ":" $2 }'
}

mkdir -p "${qemu_build_dir}"
current_integration_signature=$(integration_signature)
integration_stamp="${qemu_build_dir}/.harbor-qemu-integration.stamp"
config_signature_stamp="${qemu_build_dir}/.harbor-qemu-config.stamp"
current_config_signature=$(printf "%s\n%s\n%s\n%s\n%s\n%s\n" \
  "${qemu_mode}" \
  "${harbor_qemu_adapter_lib}" \
  "${harbor_systemc_lib}" \
  "${harbor_systemc_link_args}" \
  "${harbor_core_lib}" \
  "${qemu_extra_cflags}")

cd "${qemu_build_dir}"

if [ ! -f "${integration_stamp}" ] || [ "$(cat "${integration_stamp}")" != "${current_integration_signature}" ]; then
  echo "[qemu] QEMU integration changed; reconfiguring"
  rm -f config-host.mak
  printf "%s\n" "${current_integration_signature}" >"${integration_stamp}"
fi

if [ ! -f "${config_signature_stamp}" ] || [ "$(cat "${config_signature_stamp}")" != "${current_config_signature}" ]; then
  echo "[qemu] Harbor QEMU configuration changed; reconfiguring"
  rm -f config-host.mak
  printf "%s" "${current_config_signature}" >"${config_signature_stamp}"
fi

if [ -f config.status ] && ! grep -F -- "--python=${python}" config.status >/dev/null 2>&1; then
  echo "[qemu] Existing configuration does not use ${python}; reconfiguring"
  rm -f config-host.mak
fi

if [ -f config.status ] && ! grep -F -- "${harbor_qemu_adapter_lib}" config.status >/dev/null 2>&1; then
  echo "[qemu] Existing configuration does not link the Harbor QEMU adapter; reconfiguring"
  rm -f config-host.mak
fi

if [ -f config.status ] && ! grep -F -- "${qemu_extra_cflags}" config.status >/dev/null 2>&1; then
  echo "[qemu] Existing configuration does not use the current Harbor QEMU C flags; reconfiguring"
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
    --disable-werror \
    --extra-cflags="${qemu_extra_cflags}" \
    -Dharbor_qemu_adapter_lib="${harbor_qemu_adapter_lib}" \
    -Dharbor_qemu_support_lib="" \
    -Dharbor_systemc_lib="${harbor_systemc_lib}" \
    -Dharbor_systemc_link_args="${harbor_systemc_link_args}" \
    -Dharbor_core_lib="${harbor_core_lib}" \
    -Dharbor_cxx_runtime="${harbor_cxx_runtime}"
else
  echo "[qemu] Reusing existing QEMU configuration in ${qemu_build_dir}"
fi

echo "[qemu] Building qemu-system-riscv64"
ninja -C "${qemu_build_dir}" qemu-system-riscv64

echo "QEMU binary: ${qemu_build_dir}/qemu-system-riscv64"
