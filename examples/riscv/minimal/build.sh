#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
if [ -n "${BUILD_DIR:-}" ]; then
  case "${BUILD_DIR}" in
    /*) host_build_dir="${BUILD_DIR}" ;;
    *) host_build_dir="${repo_root}/${BUILD_DIR}" ;;
  esac
else
  host_build_dir="${repo_root}/build/riscv"
fi
docker_cmd="${DOCKER:-docker}"
image_name="${HARBOR_EXAMPLES_DOCKER_IMAGE:-harbor-examples-builder:latest}"
build_volume="${HARBOR_RISCV_BUILD_VOLUME:-harbor-riscv-build}"

if ! command -v "${docker_cmd}" >/dev/null 2>&1; then
  echo "Docker executable not found: ${docker_cmd}" >&2
  exit 1
fi

if ! "${docker_cmd}" info >/dev/null 2>&1; then
  echo "Docker daemon is not reachable. Start Docker Desktop and try again." >&2
  exit 1
fi

if ! "${docker_cmd}" image inspect "${image_name}" >/dev/null 2>&1; then
  echo "Missing Docker image: ${image_name}" >&2
  echo "Build it first with the command documented in README.md." >&2
  exit 1
fi

mkdir -p "${host_build_dir}"

# Keep CMake's write-heavy build tree in a Docker volume, then copy the ELF
# and map file back to the host build directory for the run script.
echo "[riscv-minimal] Preparing Docker build volume"
"${docker_cmd}" run --rm \
  -v "${build_volume}:/docker-build" \
  "${image_name}" \
  sh -c "mkdir -p /docker-build/riscv && chown $(id -u):$(id -g) /docker-build /docker-build/riscv"

echo "[riscv-minimal] Building in Docker image ${image_name}"
"${docker_cmd}" run --rm \
  -u "$(id -u):$(id -g)" \
  -v "${repo_root}:/work:ro" \
  -v "${build_volume}:/docker-build" \
  -v "${host_build_dir}:/host-build" \
  -w /work \
  "${image_name}" \
  sh -lc 'cmake -S /work -B /docker-build/riscv -G Ninja && cmake --build /docker-build/riscv --target harbor_riscv_assembly && mkdir -p /host-build/examples/riscv/minimal && cp /docker-build/riscv/examples/riscv/minimal/riscv-minimal.elf /docker-build/riscv/examples/riscv/minimal/riscv-minimal.map /host-build/examples/riscv/minimal/'
