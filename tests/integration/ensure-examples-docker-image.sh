#!/usr/bin/env sh
set -eu

repo_root=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
docker_cmd="${DOCKER:-docker}"
image_name="${HARBOR_EXAMPLES_DOCKER_IMAGE:-harbor-examples-builder:latest}"

if ! command -v "${docker_cmd}" >/dev/null 2>&1; then
  echo "Docker executable not found: ${docker_cmd}" >&2
  exit 1
fi

if ! "${docker_cmd}" info >/dev/null 2>&1; then
  echo "Docker daemon is not reachable. Start Docker Desktop and try again." >&2
  exit 1
fi

if "${docker_cmd}" image inspect "${image_name}" >/dev/null 2>&1; then
  echo "[integration] Docker image available: ${image_name}"
  exit 0
fi

# The integration suite owns preparing the shared cross-compilation image. The
# individual example build scripts intentionally only consume this image.
echo "[integration] Building missing Docker image: ${image_name}"
"${docker_cmd}" build \
  -t "${image_name}" \
  -f "${repo_root}/examples/Dockerfile" \
  "${repo_root}/examples"
