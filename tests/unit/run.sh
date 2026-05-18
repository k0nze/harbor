#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
build_dir="${BUILD_DIR:-${repo_root}/build/unit}"

if [ -n "${CXX:-}" ] && ! command -v "${CXX}" >/dev/null 2>&1; then
  echo "[unit] Ignoring unavailable CXX=${CXX}"
  unset CXX
fi

cmake -S "${repo_root}" -B "${build_dir}" \
  -DHARBOR_BUILD_RISCV_EXAMPLES=OFF \
  -DHARBOR_BUILD_TESTS=ON

cmake --build "${build_dir}" --target harbor_register_file_test
ctest --test-dir "${build_dir}" --output-on-failure
