#!/bin/sh
# Build the motif-profile image described by the Containerfile next to
# this script.
#
#   tools/dev/profile/mkimage.sh [IMAGE]
#
# "podman build" needs an overlay mount for its build context, which
# some rootless setups (vfs storage) do not have, so this script runs
# the Containerfile's package installation in a container and commits
# it instead.  Set PODMAN to the podman command to use, for example
#   PODMAN="podman --storage-driver=vfs --root $HOME/.cache/podman"
set -eu

here=$(cd "$(dirname "$0")" && pwd)
image=${1:-motif-profile}
podman=${PODMAN:-podman}
name=motif-profile-mk-$$

base=$(sed -n 's/^FROM //p' "$here/Containerfile")
# The RUN instruction of the Containerfile, with its continuations.
run=$(sed -n '/^RUN /,/^$/p' "$here/Containerfile" |
	sed 's/^RUN //; s/\\$//' | tr '\n' ' ')

$podman run --name "$name" --network=host "$base" \
	sh -c "export DEBIAN_FRONTEND=noninteractive LANG=C.UTF-8; $run"
$podman commit "$name" "$image"
$podman rm "$name" >/dev/null
