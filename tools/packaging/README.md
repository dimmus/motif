# Distribution packaging

Packaging of this tree for Debian and Fedora, kept here so that CI can
build real packages from every commit (the "Distribution package" jobs
of `.github/workflows/build.yml`).  Distributions are welcome to start
from it.

- `debian/`: `dpkg-buildpackage` packaging (debhelper 13, `dh` with the
  CMake and Ninja build system).  It is not at the top of the tree, as
  Debian prefers upstreams not to ship `debian/`: copy it into an
  unpacked release tarball (or a checkout) and build there.
- `rpm/motif.spec`: an `rpmbuild` spec for Fedora; `Source0` is the
  release tarball that `tools/dev/env/ci/dist.sh` makes.

Both build without the examples, use the distribution's compiler and
linker flags instead of the project's `WITH_HARDENING` set, and run the
test suite under Xvfb (skip it with `DEB_BUILD_OPTIONS=nocheck` or
`rpmbuild --without check`).

| Debian package   | RPM package    | Contents |
|------------------|----------------|----------|
| `libxm5`         | `motif`        | `libXm.so.5` |
| `libmrm5`        | `motif`        | `libMrm.so.5` |
| `libuil5`        | `motif`        | `libUil.so.5` |
| `libmotif-common`| `motif`        | the virtual key bindings (`/usr/share/X11/bindings`) |
| `mwm`            | `motif`        | `mwm`, `xmbind`, `system.mwmrc` |
| `uil`            | `motif`        | the UIL compiler `uil` |
| `libmotif-dev`   | `motif-devel`  | headers, `.so` links, pkg-config and CMake files, bitmaps, manual pages of the API |

The Debian symbols files match every symbol of the version nodes
(`(symver)XM_2.5` and so on), so a new version node needs a new line
there.  The SONAME is 5 (`doc/abi-policy.md`): these packages install
next to the `libxm4` of a distribution's Motif 2.3, and replace its
`libmotif-dev`, `libmotif-common`, `mwm` and `uil` (or `motif` and
`motif-devel`).

To try it, run in a throwaway `debian:trixie` or `fedora:latest`
container, from the top of the tree:

```sh
tools/dev/env/ci/distro-package.sh _pkg
```

which builds the packages, runs lintian or rpmlint (errors fail),
installs them and builds and runs a client against them.

A container that shares the host's network namespace (`--network=host`)
also shares its abstract X sockets: the test suite's `xvfb-run -a` then
picks `:99` when the host already runs a server there, and the tests
talk to the host's server instead of their own (the visual tests fail,
as the server cannot open their font directory).  Use a network
namespace of its own, or create `/tmp/.X99-lock` and the following lock
files in the container first.

On a release, set the version in `debian/changelog` (`dch -v
X.Y.Z-1`) and in `Version:` and `%changelog` of the spec;
`distro-package.sh` fails when they do not match `project()` in
`CMakeLists.txt`.
