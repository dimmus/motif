%bcond check 1

Name:           motif
Version:        2.5.0
Release:        1%{?dist}
Summary:        X11 widget toolkit libraries, window manager and UIL compiler

# The libraries and programs are LGPL-2.1-or-later; the bundled XPM code
# in libXm is MIT, the bundled nanosvg parser is Zlib.
License:        LGPL-2.1-or-later AND MIT AND Zlib
URL:            https://github.com/dimmus/motif
Source0:        %{url}/releases/download/%{version}/%{name}-%{version}.tar.xz

BuildRequires:  bison
BuildRequires:  cmake
BuildRequires:  flex
BuildRequires:  gcc
BuildRequires:  libfl-devel
BuildRequires:  ninja-build
BuildRequires:  pkgconfig(fontconfig)
BuildRequires:  pkgconfig(freetype2)
BuildRequires:  pkgconfig(libjpeg)
BuildRequires:  pkgconfig(libpng)
BuildRequires:  pkgconfig(x11)
BuildRequires:  pkgconfig(xbitmaps)
BuildRequires:  pkgconfig(xext)
BuildRequires:  pkgconfig(xft)
BuildRequires:  pkgconfig(xmu)
BuildRequires:  pkgconfig(xpm)
BuildRequires:  pkgconfig(xproto)
BuildRequires:  pkgconfig(xrender)
BuildRequires:  pkgconfig(xt)
%if %{with check}
BuildRequires:  bdftopcf
BuildRequires:  mkfontscale
BuildRequires:  pkgconfig(check)
BuildRequires:  xdotool
BuildRequires:  xorg-x11-fonts-misc
BuildRequires:  xorg-x11-server-Xephyr
BuildRequires:  xorg-x11-server-Xvfb
BuildRequires:  xorg-x11-xauth
%endif

%description
Motif is the user interface toolkit of the Common Desktop Environment: a
set of widgets (buttons, menus, lists, text fields, dialogs, ...) for X11
applications, built on the X Toolkit Intrinsics.

This package contains the shared libraries libXm, libMrm and libUil, the
Motif window manager mwm, the UIL compiler uil and xmbind.  The SONAMEs
are libXm.so.5, libMrm.so.5 and libUil.so.5: programs built against
Motif 2.3 (libXm.so.4) must be rebuilt.

%package devel
Summary:        Development files for Motif
Requires:       %{name}%{?_isa} = %{version}-%{release}
Requires:       pkgconfig
Requires:       pkgconfig(fontconfig)
Requires:       pkgconfig(libjpeg)
Requires:       pkgconfig(libpng)
Requires:       pkgconfig(x11)
Requires:       pkgconfig(xext)
Requires:       pkgconfig(xft)
Requires:       pkgconfig(xmu)
Requires:       pkgconfig(xpm)
Requires:       pkgconfig(xt)

%description devel
The headers of libXm, libMrm and libUil, the pkg-config files (motif,
mrm, uil), the CMake package for find_package(Motif CONFIG), the Motif
bitmaps and the manual pages of the library functions and widgets.

%prep
%autosetup

%build
# The distribution's compiler and linker flags take the place of the
# project's own hardening flags.  The examples are not packaged.
%cmake -G Ninja \
    -DWITH_DEMOS=OFF \
    -DWITH_HARDENING=OFF \
    -DWITH_TESTS=%{?with_check:ON}%{!?with_check:OFF}
%cmake_build

%install
%cmake_install
# The documentation is packaged with %%doc below.
rm -rf %{buildroot}%{_datadir}/doc/motif %{buildroot}%{_datadir}/Xm

%if %{with check}
%check
# The X11 test suites run under xvfb-run, which CTest found at configure
# time.  --no-tests=error: a build that registers no tests fails.
%ctest --no-tests=error
%endif

%files
%license LICENSE
%doc AUTHORS CHANGELOG.md README.md SECURITY.md
%config(noreplace) %{_sysconfdir}/X11/system.mwmrc
%{_bindir}/mwm
%{_bindir}/uil
%{_bindir}/xmbind
%{_libdir}/libMrm.so.5{,.*}
%{_libdir}/libUil.so.5{,.*}
%{_libdir}/libXm.so.5{,.*}
%{_datadir}/X11/bindings/
%{_mandir}/man1/mwm.1*
%{_mandir}/man1/uil.1*
%{_mandir}/man1/xmbind.1*
%{_mandir}/man4/mwmrc.4*
%{_mandir}/man5/UIL.5*
%{_mandir}/man5/WML.5*

%files devel
%doc doc/BUILD.md doc/LOCALIZATION.md doc/LOGGING_CONFIGURATION.md
%doc doc/abi-policy.md doc/guide
%{_includedir}/Mrm/
%{_includedir}/X11/bitmaps/xm_*
%{_includedir}/Xm/
%{_includedir}/uil/
%{_libdir}/cmake/Motif/
%{_libdir}/libMrm.so
%{_libdir}/libUil.so
%{_libdir}/libXm.so
%{_libdir}/pkgconfig/motif.pc
%{_libdir}/pkgconfig/mrm.pc
%{_libdir}/pkgconfig/uil.pc
%{_mandir}/man3/*.3*
%{_mandir}/man5/Traits.5*

%changelog
* Sun Oct 04 2026 dimmus <dmitri.chudinov@gmail.com> - 2.5.0-1
- Package Motif 2.5.0.  The SONAMEs are libXm.so.5, libMrm.so.5 and
  libUil.so.5; programs built against Motif 2.3 must be rebuilt.
