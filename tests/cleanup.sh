#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
docker_cmd="${DOCKER:-docker}"
image_name="${HARBOR_EXAMPLES_DOCKER_IMAGE:-harbor-examples-builder}"
riscv_build_volume="${HARBOR_RISCV_BUILD_VOLUME:-harbor-riscv-build}"
buildroot_source_volume="${HARBOR_BUILDROOT_SOURCE_VOLUME:-harbor-buildroot-source}"
buildroot_output_volume="${HARBOR_BUILDROOT_OUTPUT_VOLUME:-harbor-buildroot-output}"

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
