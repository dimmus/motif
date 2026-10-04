#!/bin/sh
#
# Make the source release of a tagged version: the tarball, its SHA-256
# checksum and the release notes.  Run it from the top of a git checkout
# that has the tag.
#
# Usage: dist.sh TAG [OUTDIR]
#
# TAG is the version, such as 2.5.0, and must match project(VERSION) in
# the tag's CMakeLists.txt.  OUTDIR (default: _dist) receives
#
#   motif-TAG.tar.xz         git archive of the tag under motif-TAG/
#   motif-TAG.tar.xz.sha256  its checksum, in sha256sum format
#   motif-TAG-notes.md       the tag's CHANGELOG.md section, with links
#                            made absolute, for the release page
#
# The tarball depends only on the tag: git archive stamps the files with
# the commit time, and single-threaded xz output is deterministic.
#
# Environment:
#   MOTIF_REPO_URL  repository URL for the links in the notes
#                   (default: https://github.com/dimmus/motif)

set -eu

die() {
  echo "dist.sh: $*" >&2
  exit 1
}

[ $# -ge 1 ] || die "usage: dist.sh TAG [OUTDIR]"
tag=$1
out=${2:-_dist}
repo_url=${MOTIF_REPO_URL:-https://github.com/dimmus/motif}

git rev-parse -q --verify "refs/tags/$tag^{commit}" >/dev/null ||
  die "no tag $tag"

version=$(git show "$tag:CMakeLists.txt" 2>/dev/null |
  sed -n 's/^project(Motif VERSION \([^ )]*\).*/\1/p')
case $tag in
  "$version" | "$version"-*) ;;
  *) die "tag $tag does not match the version in CMakeLists.txt ($version)" ;;
esac

mkdir -p "$out"
name=motif-$tag

git archive --format=tar --prefix="$name/" "$tag" | xz -9e -T1 >"$out/$name.tar.xz"
(cd "$out" && sha256sum "$name.tar.xz" >"$name.tar.xz.sha256")
sum=$(cut -d' ' -f1 "$out/$name.tar.xz.sha256")

# The CHANGELOG section of this version, up to the next "## " heading.
# Its relative links (doc/abi-policy.md) would not resolve on the
# release page, so point them at the tag.
git show "$tag:CHANGELOG.md" |
  awk -v v="$version" '/^## /{ if (on) exit; if ($2 == v) { on = 1; next } } on' |
  sed -E "s#\]\(([A-Za-z0-9_./-]+)\)#]($repo_url/blob/$tag/\1)#g" \
    >"$out/$name-notes.body"
grep -q '[^[:space:]]' "$out/$name-notes.body" ||
  die "CHANGELOG.md at $tag has no section for $version"
{
  cat "$out/$name-notes.body"
  # shellcheck disable=SC2016 # the backquotes are Markdown code spans
  printf '\n---\n\nSource: `%s.tar.xz`, SHA-256 `%s`.\n' "$name" "$sum"
} >"$out/$name-notes.md"
rm -f "$out/$name-notes.body"

echo "dist.sh: wrote $out/$name.tar.xz ($sum)"
