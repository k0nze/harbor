#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
docker_cmd="${DOCKER:-docker}"
image_name="${HARBOR_EXAMPLES_DOCKER_IMAGE:-harbor-examples-builder:latest}"
riscv_build_volume="${HARBOR_RISCV_BUILD_VOLUME:-harbor-riscv-build}"
buildroot_source_volume="${HARBOR_BUILDROOT_SOURCE_VOLUME:-harbor-buildroot-source}"
buildroot_output_volume="${HARBOR_BUILDROOT_OUTPUT_VOLUME:-harbor-buildroot-output}"
qemu_source_dir="${HARBOR_QEMU_SOURCE_DIR:-${repo_root}/external/qemu}"

# Only delete generated paths that live inside this repository.
remove_repo_path() {
  path=$1

  case "${path}" in
    "${repo_root}" | "${repo_root}/")
      echo "Refusing to remove repository root: ${path}" >&2
      exit 1
      ;;
    "${repo_root}"/*)
      if [ -e "${path}" ]; then
        rm -rf "${path}"
        echo "Removed ${path}"
      fi
      ;;
    *)
      echo "Refusing to remove path outside repository: ${path}" >&2
      exit 1
      ;;
  esac
}

# Remove generated host build outputs and any legacy host-side Buildroot tree.
remove_repo_path "${repo_root}/build"
remove_repo_path "${repo_root}/.buildroot"

cleanup_qemu_file() {
  path=$1

  case "${path}" in
    "${repo_root}/external/qemu"/*)
      if [ -f "${path}" ]; then
        rm -f "${path}"
        echo "Removed ${path}"
      fi
      ;;
    *)
      echo "Refusing to remove QEMU path outside external/qemu: ${path}" >&2
      exit 1
      ;;
  esac
}

rewrite_file() {
  path=$1
  script=$2

  if [ ! -f "${path}" ]; then
    return
  fi

  tmp=$(mktemp)
  awk "${script}" "${path}" >"${tmp}"
  if ! cmp -s "${path}" "${tmp}"; then
    mv "${tmp}" "${path}"
    echo "Removed Harbor QEMU integration from ${path}"
  else
    rm -f "${tmp}"
  fi
}

cleanup_qemu_integration() {
  if [ ! -d "${qemu_source_dir}" ]; then
    return
  fi

  cleanup_qemu_file "${qemu_source_dir}/hw/misc/harbor_register_file.c"
  cleanup_qemu_file "${qemu_source_dir}/hw/misc/harbor_register_file.cpp"

  rewrite_file "${qemu_source_dir}/hw/misc/Kconfig" '
    /^config HARBOR_REGISTER_FILE$/ { skip = 1; next }
    skip && /^    bool$/ { next }
    skip && /^$/ { skip = 0; next }
    { print }
  '

  rewrite_file "${qemu_source_dir}/hw/misc/meson.build" '
    /^harbor_qemu_adapter = declare_dependency\($/ { skip = 1; next }
    skip && /^              if_true: \[files\('\''harbor_register_file\.c'\''\), harbor_qemu_adapter\]\)$/ {
      skip = 0
      next
    }
    skip { next }
    { print }
  '

  rewrite_file "${qemu_source_dir}/meson_options.txt" '
    /^option\('\''harbor_qemu_adapter_lib'\''/ { skip = 1; next }
    skip && /^       description: '\''C\+\+ runtime linker argument required by Harbor adapter'\''\)$/ {
      skip = 0
      next
    }
    skip { next }
    { print }
  '

  rewrite_file "${qemu_source_dir}/hw/riscv/Kconfig" '
    /^    select HARBOR_REGISTER_FILE$/ { next }
    { print }
  '

  rewrite_file "${qemu_source_dir}/hw/riscv/virt.c" '
    /^    \[VIRT_HARBOR_REGISTER_FILE\] = \{ 0x10010000, 0x40 \},$/ { next }
    /^    \/\* Harbor register-file MMIO device \*\/$/ { skip = 1; next }
    skip && /^                         s->memmap\[VIRT_HARBOR_REGISTER_FILE\]\.base, NULL\);$/ {
      skip = 0
      next
    }
    skip { next }
    { print }
  '

  rewrite_file "${qemu_source_dir}/include/hw/riscv/virt.h" '
    /^    VIRT_HARBOR_REGISTER_FILE,$/ { next }
    { print }
  '
}

cleanup_qemu_integration

# Remove Docker volumes and the prepared example builder image if Docker is available.
if command -v "${docker_cmd}" >/dev/null 2>&1; then
  if ! "${docker_cmd}" info >/dev/null 2>&1; then
    echo "Docker daemon is not reachable; skipping volume and image cleanup" >&2
    exit 0
  fi

  for volume in "${riscv_build_volume}" "${buildroot_source_volume}" "${buildroot_output_volume}"; do
    if "${docker_cmd}" volume inspect "${volume}" >/dev/null 2>&1; then
      "${docker_cmd}" volume rm "${volume}"
      echo "Removed Docker volume ${volume}"
    fi
  done

  if "${docker_cmd}" image inspect "${image_name}" >/dev/null 2>&1; then
    "${docker_cmd}" image rm "${image_name}"
    echo "Removed Docker image ${image_name}"
  fi
else
  echo "Docker executable not found: ${docker_cmd}; skipping image cleanup" >&2
fi
