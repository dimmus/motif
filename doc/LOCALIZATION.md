# Localization

Motif has two independent localization mechanisms:

- **Message catalogs** translate the warnings and the default labels that
  the libraries themselves produce ("Cancel", "Help", "Filter", the
  messages of Mrm and of the UIL compiler, ...).  They are X/Open
  catalogs read with `catgets()`, enabled with `WITH_MESSAGE_CATALOG`.
- **Application resources and UIL** translate an application's own
  strings, through locale-specific resource files (`XAPPLRESDIR`,
  `XFILESEARCHPATH`) or locale-specific UID files (`UIDPATH`).  This is
  plain Xt and Mrm behaviour and needs no build option.

Text input and display in a locale need that locale to be installed
(`locale -a`) and, for core fonts, fonts covering its characters; with
Xft (`WITH_XFT`) fontconfig picks the fonts.

## Message catalogs

### Sources

| File | Contents |
|------|----------|
| `src/lib/Xm/Xm.msg`, `src/lib/Mrm/Mrm.msg`, `src/lib/Uil/Uil.msg` | The English (C) catalogs, with symbolic set and message ids |
| `localized/<lang>/msg/{Xm,Mrm,Uil}.msg` | Translations: `de`, `es`, `fr`, `it` and `ja` |

All of them are UTF-8.  The translations date from the 1990s; the
messages added since then carry the English text and a
`$ TODO: translate` comment line.

### Building and installing

With `-DWITH_MESSAGE_CATALOG=ON` the build:

1. runs `mkcatdefs` on each English catalog, which writes the header with
   the numeric ids that the library code uses (`XmMsgCatI.h`, ...) and
   the numeric catalog `<build>/localized/C/msg/<Name>.msg`;
2. does the same for each translation, in the C.UTF-8 locale, and checks
   it against the English catalog: it must define the same sets and
   message ids in the same order (`mkcatdefs` numbers them by position),
   and each message must contain the same printf conversions in the same
   order (the libraries pass the arguments of the English message).  A
   translation that fails either check fails the build;
3. when `gencat` was found, compiles every catalog and installs it as
   `<localedir>/C/LC_MESSAGES/<Name>` and
   `<localedir>/<lang>/LC_MESSAGES/<Name>` (`<localedir>` is
   `CMAKE_INSTALL_LOCALEDIR`, `share/locale` by default).

The libraries open the catalogs with `catopen("Xm", NL_CAT_LOCALE)`
(and `"Mrm"`, `"Uil"`), so the catalog follows `LC_MESSAGES`.  glibc's
default `NLSPATH` includes `<prefix>/share/locale/%l/LC_MESSAGES/%N`,
where `%l` is the language part of the locale, so a system install
(`CMAKE_INSTALL_PREFIX=/usr`) works in every `de_*` locale, and so on.
For another prefix, or on musl, which has no default path, set
`NLSPATH`, for example

```sh
export NLSPATH=/opt/motif/share/locale/%l/LC_MESSAGES/%N
```

The catalogs are UTF-8 and display correctly only in UTF-8 locales.

### Trying a translation

The translated catalogs can be tested without installing the locales
system-wide.  `localedef` can build a locale into a private directory
that `LOCPATH` points at:

```sh
cmake -S . -B _build -DWITH_MESSAGE_CATALOG=ON
cmake --build _build
DESTDIR=$PWD/_stage cmake --install _build

mkdir -p _locales
localedef -i de_DE -f UTF-8 _locales/de_DE.UTF-8

LOCPATH=$PWD/_locales LANG=de_DE.UTF-8 \
NLSPATH=$PWD/_stage/usr/local/share/locale/%l/LC_MESSAGES/%N \
    some-motif-program
```

The Cancel button of a message box then reads "Abbruch".  A single
catalog can also be selected directly, whatever the locale, with
`NLSPATH=$PWD/_build/localized/de/msg/%N.cat`.

### Changing or adding translations

- Edit the files in UTF-8 and keep the structure of the English catalog:
  the same `$set` lines and message ids, in the same order.  Keep `%s`,
  `%d` and the other conversions, in their order; do not translate
  resource names (`XmN...`), `True` and `False`.
- When a message is added to an English catalog, add it at the same
  place in every translation, with the English text quoted (the
  translations use `$quote "`) and a `$ TODO: translate` line above it.
- To add a language, copy an existing translation to
  `localized/<lang>/msg`, translate it and add `<lang>` to
  `MOTIF_CATALOG_LANGUAGES` in `localized/CMakeLists.txt`.
- Build with `-DWITH_MESSAGE_CATALOG=ON` to run the checks.

## Application localization

The example programs show the resource and UIL mechanisms:

- `src/examples/programs/hellomotifi18n` (`helloint`) has one UIL file
  per language (`english`, `french`, `hebrew`, `japanese`, `swedish`)
  and finds the UID file for `$LANG` through `UIDPATH`; its `README`
  describes the environment it expects;
- `src/examples/programs/fileview` has English, French and German UIL
  files, selected by its `xmfile` script.

Several of these examples were written for ISO-8859 and EUC locales and
core fonts, and need such a locale and fonts installed to display
correctly.  `tools/dev/scripts/localization_test.sh` runs them in several
locales (`--auto-locales` limits it to the installed ones).

Useful environment variables:

| Variable | Used for |
|----------|----------|
| `LANG`, `LC_ALL`, `LC_MESSAGES` | The locale, and the message catalog language |
| `NLSPATH` | Where `catopen()` looks for message catalogs |
| `XAPPLRESDIR`, `XFILESEARCHPATH`, `XUSERFILESEARCHPATH` | Where Xt looks for application resource files (`%L` and `%l` expand to the locale) |
| `UIDPATH` | Where Mrm looks for UID files |
| `XMODIFIERS` | The X input method (`@im=...`) |
