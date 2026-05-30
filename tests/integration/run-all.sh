#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)

echo "[integration] Running Harbor unit tests"
"${repo_root}/tests/unit/run.sh"

echo "[integration] Preparing shared examples Docker image"
"${repo_root}/tests/integration/ensure-examples-docker-image.sh"

echo "[integration] Running RISC-V bare-metal examples"
"${repo_root}/tests/integration/run-riscv-examples.sh"

echo "[integration] Fetching Buildroot sources"
"${repo_root}/examples/linux/buildroot/fetch.sh"

echo "[integration] Building Buildroot Linux image"
"${repo_root}/examples/linux/buildroot/build.sh"

echo "[integration] Building Harbor-enabled QEMU"
"${repo_root}/qemu-build.sh"

echo "[integration] Booting Buildroot Linux image"
"${repo_root}/tests/integration/boot-buildroot-linux.sh"

echo "[integration] All integration checks passed"
