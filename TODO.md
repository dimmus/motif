# Motif maturity TODO

## Status as of this branch

Branch `motif-maturity`: `maturity/stage2` plus the `todo3/sweep` and
`todo3/legacy` streams and the final integration fixes.  The checkboxes
below were ticked by reading the code and the git log, not from the stream
reports; a *(status: ...)* note marks items that are only partly done or
done differently from the plan.  119 of 141 items are ticked.

Verification of this branch (2 CPUs, Arch Linux, GCC 16, Clang 23):
- Fresh GCC RelWithDebInfo build of everything (tests, demos, docs): no
  warnings in the libraries, mwm, uil, the build tools or the tests; 472
  distinct warnings remain, all in `src/examples` (which `WITH_WERROR`
  exempts).
- Fresh Clang Debug build with ASan+UBSan, tests, fuzzers and demos, done
  with the source tree read-only: `ctest --no-tests=error` passes 95/95
  with `UBSAN_OPTIONS=halt_on_error=1`, none skipped (X tests under
  `xvfb-run`).  The GCC build passes its 80 tests (the same suite
  without the fuzzers).
- Every fuzzer ran 120 s.  Findings fixed here: a UIL NULL operand and a
  widget-as-operand over-read, two XmString rendition leaks and an Mrm
  callback leak.  A libX11 `memcmp` report was not Motif's (see
  `src/tests/fuzz/run.sh`).
- Installed with `cmake --install --prefix`: `pkg-config motif/mrm/uil` and
  `find_package(Motif CONFIG)` hello worlds build and run; SONAMEs
  `libXm.so.5`, `libMrm.so.5`, `libUil.so.5`, versioned `XM_2.4` /
  `MRM_2.4` / `UIL_2.4`; 1737 / 219 / 51 exported symbols; no internal
  `*I.h` installed apart from `Mrm/MrmosI.h`, which Motif 2.3 installs too.
  abidiff against the master baseline: 0 changed functions or variables,
  only removals of symbols hidden by the version scripts.
- mwm and 32 demos were started under Xvfb with the ASan+UBSan build
  (with ASan `strict_memcmp=0` for the libX11 quark lookup): mwm and 29
  demos ran without sanitizer reports.  periodic_demo and
  hellomotifi18n_demo exit at once because the build does not put their
  .uid files where they look; the workspace demo's Wsm library
  (`src/examples/lib/Wsm/pack.c`) over-reads the empty reply of an mwm
  built without WSM.

What remains, by priority:
- **P0/P1, known open bugs** (reproducers in `src/tests/fuzz/crashes/`):
  - Mrm passes an argument value to the widget without checking it against
    the resource's type, so a corrupt `.uid` can hand a widget a wild
    XmString (`uid/corrupt-xmstring-segv`).
  - A corrupt `_MOTIF_CLIP_*` record still makes any Motif application
    `exit(1)` through `ClipboardError` (`clipboard/corrupt-record-exit`).
  - A crafted XPM can make the bundled libXpm allocate up to INT_MAX bytes
    (`xpm/huge-chars-per-pixel-oom`); no lower size policy yet.
  - A crafted `.wmd` database is still trusted inside its tables.
  - Known leaks left suppressed for the fuzzers: Mrm after a failed widget
    tree (`Urm__CW_InitWRef`) and a compound-text separator
    (`XmStringSeparatorCreate`).
  - The `Widgets` suite tracks FontSelector, SlideContext, GrabShell and
    Label/LabelG bugs as expected failures.
- **Tests:** a hostile-client end-to-end test against a drag source and a
  clipboard owner; secondary selection and undo; ja_JP/de_DE, RTL,
  preedit and message-catalog tests; publishing coverage reports; OSS-Fuzz.
  TSan and MSan have never been run (MSan needs instrumented X libraries).
- **CI:** none of the GitHub/GitVerse jobs has been run from here; the
  cppcheck baseline must come from the first CI run; the ABI job does not
  build upstream 2.3.8; there is no real `dpkg-buildpackage`/`rpmbuild`.
- **Hardening:** a full audit of the mutable statics with a two-context
  TSan test; the remaining ~90 type-punned out-parameters (the build uses
  `-fno-strict-aliasing` until then); constifying the rest of the public
  string API; replacing the bundled libXpm with the system one.
- **Docs:** man pages for 135 exported functions (mostly `Xme*`); real
  translations of 21 catalog messages.
- **Performance:** XmString extent cache and builder, List and Container
  data structures (blocked by installed struct layouts), render-table
  converter caching, lock-free traits, the remaining per-tick ScrollBar
  `XSync`, idle coalescing of IM spot updates; mwm benchmarks, a CI
  performance gate and profiling.
- **Warnings at `-O3`:** the zero-warning result holds for GCC at
  `-O2` (RelWithDebInfo). A CMake `Release` build (`-O3`) still reports
  optimisation-dependent warnings in library code:
  - `-Warray-bounds` in `TextStrSo.c` `ScanStart` (`XmSELECT_LINE`, about
    line 1111/1116): a 2- or 4-byte character is read through `Look()`
    from what GCC sees as a 1-byte buffer. The same code is on `master`.
    Check whether `_XmStringSourceGetChar` can return a 1-byte buffer for
    wide sources.
  - `-Wmaybe-uninitialized` for `cursorPos`/`nextPos` in `DataF.c` (about
    3392-3466).
  - `-Wstringop-truncation` in `Mrmwci.c:886`.
  Make the CI `-Werror` job build `Release` as well as `RelWithDebInfo`.

---

Audit date: 2026-10-03. Baseline: `master` @ `c4e9902f` (2.4.1, 17 commits ahead of origin).

How the audit was done:
- A full Debug build (`-Wall -Wextra`, with tests and demos) using Ninja.
  - It built cleanly with 1,688 warnings: 885 `-Wsign-compare`, 673 `-Wmissing-field-initializers`, 82 `-Wempty-body` and 42 `-Wcast-function-type`.
  - That is only because about 18 warning classes are switched off globally.
  - `ctest` reports **"No tests were found"**.
- A Clang static-analyzer run.
- Five read-only review passes: build/CI, memory safety and security, performance, tests and code health, and parsers.

Every item cites `file:line`. Items marked **[verified]** were checked again by hand.

Priority legend:
- **P0**: broken or exploitable; fix before any release.
- **P1**: needed to call the project stable.
- **P2**: maturity and speed.
- **P3**: polish.

---

## Phase 0 — Stop the bleeding (P0)

### 0.1 Repository and build hygiene
- [x] **[verified]** `src/lib/Xm/Xm.h` is tracked in git as a symlink to an absolute path on a developer machine (`/media/dimmus/dev2/x/xde-classic/build_linux/include/Xm/Xm.h`).
  - The build also re-points it into whatever build directory ran last (`src/lib/Xm/CMakeLists.txt:309-318`). As a result:
    - a fresh clone has a dangling header;
    - concurrent build directories silently share one `Xm.h`;
    - every build dirties git.
  - Fix:
    - `git rm --cached src/lib/Xm/Xm.h` and add it to `.gitignore`.
    - Delete the `xm_h_symlink` target.
    - Give consumers (`ToolTipI.h`) `${CMAKE_BINARY_DIR}/include` through `target_include_directories`.
- [x] Never write into the source tree: `src/lib/Uil/CMakeLists.txt:90-96` creates and deletes placeholder `UilSymGen.h`, `UilLexPars.[ch]`. `include/CMakeLists.txt:2-5` globs `src/lib/Xm/*.h`, which picks up that symlink. Add a CI check that the source tree is read-only during the build (`chmod -R a-w`). *(status: done; the Ubuntu CI jobs run tools/dev/env/ci/build.sh with MOTIF_CI_READONLY_SRC=1, and a full Clang build with tests, fuzzers and demos was done here with the tree chmod a-w)*
- [x] Remove the 18 prebuilt x86-64 ELF binaries committed under `src/tests/XmString/` (`StringComp`, `StringExt`, …), plus `StringExt.c.backup` and the generated `test_report.md`.
- [x] Add a `LICENSE` / `COPYING` file (LGPL-2.1). `README.md:4` links to it and `:568` claims it, but the file does not exist.
- [x] Resolve the licence contamination:
  - `src/internal/**` is Blender code, about 20 files under GPL-2.0-or-later, including the C++ `clog.cc`. It is never `add_subdirectory`'d and nothing references `MEM_`, `CLG_` or `atomic_ops`. **Delete it.**
  - These `tools/cmake` files are GPL Blender files: `motif_testing.cmake`, `motif_macros.cmake`, `motif_have_features.cmake`, `motif_packaging.cmake`. Rewrite them or replace them with minimal LGPL/MIT helpers.

### 0.2 Build-system correctness
- [x] **[verified]** `motif.pc` is broken:
  - `data/motif.pc.in` uses `@prefix@`, `@libdir@` and `@includedir@`, which are never set (`CMakeLists.txt:1589`).
  - So `pkg-config --libs motif` emits `-L -lXm`, which swallows `-lXm`.
  - Fix:
    - Use `@CMAKE_INSTALL_PREFIX@` / `@CMAKE_INSTALL_FULL_LIBDIR@` / `@CMAKE_INSTALL_FULL_INCLUDEDIR@`.
    - Build `Requires.private` from the features actually found.
    - Add `mrm.pc` and `uil.pc`.
- [x] **[verified]** The build type is ignored. `CMakeLists.txt:600-604` appends `-O2 -DNDEBUG` whenever `WITH_DEBUG=OFF`, so `CMAKE_BUILD_TYPE=Debug` gets `-O2 -DNDEBUG`. The debug preset also defines `NDEBUG=0` (`tools/cmake/config/motif_debug.cmake:46`), which still disables `assert()`.
  - Fix:
    - Drop `WITH_DEBUG`.
    - Use `CMAKE_C_FLAGS_<CONFIG>`.
    - Default to `RelWithDebInfo`.
- [x] **[verified]** Conflicting `-std` flags: `CMAKE_C_STANDARD 17` and also `-std=gnu11` (`CMakeLists.txt:1315-1326`). The README claims C23. Pick one standard (gnu17 now; C23 once the `()` prototypes are fixed) and make the README match.
- [x] Restore ABI compatibility: `ABI_CURRENT 5` (`CMakeLists.txt:218`) produces `libXm.so.5` / `libMrm.so.5`, while upstream ships `.so.4`. That breaks every existing Motif binary (CDE, nedit, xpdf, …). *(status: abidiff against upstream 2.3.8 shows a real break, so the SONAME stays 5 and the break is documented (MOTIF_SOVERSION comment, doc/abi-policy.md, CHANGELOG.md); libUil is shared; VERSION is 5.0.0)*
  - Fix:
    - Return to SOVERSION 4, unless `abidiff` against upstream 2.3.8 shows a real break; if it does, document the break.
    - Ship `libUil` shared, as upstream does (`Uil/CMakeLists.txt:114`).
    - Stop using the libtool triple as `VERSION` (`Xm/CMakeLists.txt:690`).
- [x] Fix runtime paths, which are compiled in as *relative* paths: `DATADIR="share"`, `XMBINDDIR_FALLBACK="share/X11/bindings"` and `LIBDIR="lib/X11"` (`Xm/CMakeLists.txt:730-736`, `Uil:140-143`). These break `VirtKeys.c:746` and `Xmos.c:861`. Use `CMAKE_INSTALL_FULL_*`.
- [x] **[verified]** `Xm/Xmfuncs.h:9` uses `#warn`, which is a hard preprocessor error for any user who includes it. Change it to `#warning`, or delete the header.

### 0.3 Remote memory corruption — any X client on the display can trigger these
> Drag and drop, the clipboard and selections parse window properties and ClientMessages written by *other* clients. Treat all of them as hostile input.

- [x] **[verified]** `DragICC.c:744` `_XmReadDragBuffer` has an unsigned underflow.
  - When `numCurr > buf->size`, `size = buf->size - numCurr` wraps to about 4 GB, which is then memcpy'd onto the stack.
  - `buf->size = heap_offset` comes straight from the remote `_MOTIF_DRAG_RECEIVER_INFO` (`DragICC.c:981`). `heap.size = lengthRtn - heap_offset` (`:983`) underflows too.
  - Fix:
    - In `_XmGetDragReceiverInfo`, require `format==8 && 16 <= heap_offset <= lengthRtn`.
    - Return 0 when `numCurr >= size`.
- [x] **[verified]** `DragICC.c:1154` + `Region.c:138` is a heap overflow. `XtMalloc((Cardinal)(sizeof(XmRegionBox)*size))` truncates a remote CARD32 box count, and the loop then writes attacker data. Fix:
  - Bound the count by the bytes remaining.
  - Compute the size in `size_t` with an overflow check.
- [x] `DragICC.c:1170` + `DropSMgr.c:2132-2155` is a use-after-free. `dsmInfo` is freed after `numDropSites` sites, but the remote "close" flags keep the parse going. Recursion depth is attacker-controlled. Stop at `numDropSites`, cap the depth, and free in one place only.
- [x] **[verified]** `CutPaste.c:714`: `cbProcTable[formatitem->cutByNameCBIndex]` is checked only for `>= 0`, and the result is then **called** as a function pointer. Both the index and the trigger (`_MOTIF_CLIP_MESSAGE` via `XSendEvent`) are remote-controlled. Fix: *(status: index and slot are validated; ClientMessages with send_event are deliberately still accepted, because cut-by-name between clients arrives that way)*
  - Check `idx < maxCbProcs` and `formatlength >= sizeof *formatitem`.
  - Ignore events with `send_event` set.
- [x] `CutPaste.c:833-838` `ClipboardFindItem` overflows in its chunked read. The buffer is sized from the first chunk, but `BYTELENGTH` counts 8 bytes per format-32 item on LP64 against the server's 4. Realloc per chunk, and reject a format change between chunks.
- [x] CutPaste header and lock records (`:391, :733, :1066-1100, :1403, :1493, :1923, :2439, :3287`): offsets and counts from the root property are used unchecked, and there are NULL dereferences when the property is missing. Add one `ClipboardFindItem(…, min_len, type, format)` helper and use it everywhere. *(status: corrupt records still make the application exit through ClipboardError (crashes/clipboard/corrupt-record-exit), but no longer corrupt memory)*
- [x] `DragBS.c:597-645, 798-852` `ReadAtomsTable` / `ReadTargetsTable` trust `num_atoms`, `num_target_lists` and `num_targets` (a signed `short`) without comparing them with `lengthRtn`, and never check `format`. Add a bounds-checked cursor.
- [x] `DragICC.c:172`: `messageTable[messageType]` has 9 entries, but the index comes from the wire and can be 0..127.
- [x] `ColorObj.c:504-570`: `value[length-1]` with `length==0` touches `value[-1]`. `FetchPixelData` advances by the length of its re-formatted `sprintf` output instead of the bytes it consumed, which over-reads. `colors[]` is used uninitialised. The error paths leak.
- [x] TARGETS replies are cast to `Atom*` without checking `type==XA_ATOM && format==32`: `TextFSel.c:313,1207`, `TextSel.c:325,825`, `DataF.c:5206`, `DataFSel.c:234`.
- [x] INSERT_SELECTION parameters are not validated (`DataFSel.c:286-300`, `Transfer.c:329-357`). On the error path `secondary_lock` stays 1 forever, and the synchronous `XtAppNextEvent` spin loops hang if the peer never answers.
- [x] `RowColumn.c:1863-1891` `GetRealKey` (stack `buf[1000]`) and `ClipWindow.c:225-258` (static `buf[1000]`, `assert` after the write) `strcat` one entry per keysym taken from the root `_MOTIF_BINDINGS` property, which any client can write. Use a growable buffer.

- [x] `XmRenderT.c:2589-2775` `XmRenderTableCvtFromProp` parses the `_MOTIF_RENDER_TABLE` property sent by a peer.
  - Repeated `font`/`tag` header columns overflow the stack arrays `freelater[5]` and `args[20]`, and the header loop reads `items[20]`.
  - It loops forever on `T_EOF`, on `""` and on non-numeric `strtod` input.
  - An unterminated `"` makes it skip past the NUL.
- [x] `XmString.c` byte-stream parser (`~4250-4573`, `:188`, `:5553`), fed from `_MOTIF_COMPOUND_STRING`, .uid files and the clipboard:
  - Component lengths are never checked against the end of the buffer.
  - Long-form ASN.1 lengths desynchronise the parser: it uses `_asn1_size(value)` instead of the header size actually read.
  - The 256th RENDITION_BEGIN wraps the `unsigned char` counter (`XmStringI.h:308`) to 0, and the code then writes `[-1]` (`:4349, :4426`).
  - `TxtPropCv.c:558,564` walks the stream without bounding by `nitems`.

### 0.4 Local and untrusted-file memory corruption
- [x] `ResConvert.c:1833-1864` `GetNextToken` (behind `CvtStringToAtomList` / `importTargets`) is **confirmed under ASan**. A leading delimiter (`"ab,,c"`) gives `e < s`, so `while (s != e)` runs off the heap. Empty tokens never advance `*context`, so memory grows without bound. The resource can come from `RESOURCE_MANAGER`.
- [x] **[verified]** `ResConvert.c:2143`: `char unitType[12]` is filled with `%12[`, which writes 13 bytes. `value` is used uninitialised after a 0-conversion `sscanf`.
- [x] **[verified]** `XmRenderT.c:1477` `XmRenderTableCopy` stores `_XmRTRenditions(rt)[i]` where it should use `[count++]`. A non-matching tag leaves an uninitialised pointer that is later freed.
- [x] `XmRenderT.c:1428-1468`: when the refcount wraps, the table is shared without taking a reference (use-after-free). `XmRenderT.c:2138` `CVTaddString` doubles its buffer only once (heap overflow), and `:2259` passes a stale size, which leaks heap bytes into the property.
- [x] XmString limits: `segment_count : 8` (`XmStringI.h:277`) silently truncates at 256 segments. Byte streams over 64 KiB have their length written as `unsigned short`. *(status: segment_count widened to 24 bits; streams over 64 KiB are refused safely rather than supported)*
- [x] Mrm `.uid` files: add one record/offset bounds helper and route every reader through it. Sites: *(status: byte-swapped .uid files were checked by reading only; Mrm still does not check that argument values match the widget's resource types (crashes/uid/corrupt-xmstring-segv))*
  - `MrmIentry.c:188-192`: overflow segments are copied as `segment_count × segment_size` into a buffer sized from `entry_size` (heap overflow). `MrmIentry.c:152,182` trusts `item_offs` / `entry_size`.
  - `MrmIheader.c:211,227,233`: an unterminated `db_version` is formatted with `%s` into `errmsg[300]` (stack overflow).
  - `Mrmicon.c:376-420,783-795`: icon `width*height` and colour indices are trusted.
  - `Mrmwcrw.c:609,905`: the `num_listent` short wraps, so the arglist is undersized.
  - `Mrmwcrw.c:1199,1753,1846,2667`: callback, vector and font-list counts are trusted.
  - `MrmIswap.c:156-175`: index swaps write past the record.
  - `MrmIfile.c:139`: `strcpy(dummy[300], UIDPATH)`.
  - About 36 `%s` writes into `err_msg[300]`, including `Mrmerror.c:196`.
  - `Mrmhier.c:194`: `name[strlen-4]` on short names.
  - Cyclic B-tree pointers or widget trees cause infinite loops (`MrmIindex.c:241,568`, `Mrmwcrw.c:283`).
- [x] Uil compiler: *(status: values inside a crafted .wmd database are still trusted)*
  - `UilSrcSrc.c:656-701`: the include file name is copied into `buffer[256]`.
  - `UilSemCSet.c:268`: `strcpy(uname[200], $LANG codeset)`.
  - `UilDiags.c:301-377`: `loc_buffer[132]` holds file names up to 255 characters.
  - `UilP2Out.c:164`: `result_file[256]` is filled from `-o`.
  - `UilDB.c:1236`: `free(getenv(...))`.
  - `UilDB.c:1201`: one-byte heap overflow.
- [x] Remaining `XmStackAlloc` call sites (29): audit them for the cache-reuse bug from `TextF.c:5369`.
- [x] `ResInd.c:313`: SIGFPE (division by zero) when the screen reports 0 mm.
- [x] **[verified]** `Jpeg.c`:
  - `Xmalloc(w*h*3)` overflows its 32-bit size (`:77`).
  - The RGB row stride is `w*i` where it should be `3*w*i` (`:87`), so **every colour JPEG decodes as garbage**.
  - The grayscale expansion uses stride `h` instead of `w` (`:96`), an out-of-bounds read.
  - `out_color_space` is never forced to `JCS_RGB`, so CMYK writes 4 bytes per pixel into 3-byte rows.
  - In the `setjmp` path, `data` and `rows` must be `volatile`.
- [x] `Png.c:70`: `w*h*4` overflows. `setjmp` is not re-armed after the decoder is re-created (`:55`).
- [x] Bundled libXpm (`Xpm*.c`, about 7.5k LOC) is missing upstream CVE fixes: CVE-2022-46285 (infinite loop on an unterminated comment, `Xpmdata.c:140-160`) and the CVE-2023-43788 class (`Xpmdata.c:81-92,182-188,233-237`, `XpmI.h:275`). There is also a one-byte write past the end in `XpmCrBufFrI.c:188`. **Preferred fix:** delete the copy and link the system `libXpm`, which CI installs already. *(status: CVE fixes backported into the bundled copy; it is not replaced by the system libXpm)*
- [x] `Xmos.c:1013-1016` `_XmOSAbsolutePathName`: `strcat`/`strcpy` into a `PATH_MAX` stack buffer, reachable from any long `./…` pixmap or icon name (`IconFile.c:284,462`). `IconFile.c:285/300` keeps a pointer into a dead stack frame.
- [x] `VendorS.c:1402-1456` `MotifWarningHandler`: unbounded `sprintf`/`strcat` into `buf[1024]`, and a re-indent loop that overflows `buf2[1024]`. It is reachable through long widget names, long font names, and TextF's Xft warning, whose text is 4×len (`TextF.c:5384`).
- [x] `DataF.c:5951,5967`: `char warn_str[52]` receives a 48-byte format plus the character, so any 4-byte UTF-8 character (an emoji) overflows the stack.
- [x] `XmIm.c:1744-1747` (`strcat` into `tmp[BUFSIZ]`; assert after the write), `FontS.c:1078-1129,1218` (`left_buf[i]` out of bounds; `BuildFontString` ignores its `size`), `DropSMgrI.c:203,215`.
- [x] `TextF.c:5368-5373`: `XmStackAlloc(6, stack_cache)` reuses the buffer that still holds the text being validated, so the warning text overwrites it.
- [x] `VirtKeys.c:646-688`: an empty `~/.motifbind` produces an uninitialised, unterminated binding string that is then published to the root window.
- [x] Mrm, untrusted `.uid` files: `MrmIindex.c:355-380` trusts `index_count` / `index_stg` and runs `strncmp` on unterminated data. `MrmOpenHierarchyFromBuffer` (`MrmIbuffer.c:476`) takes no length at all, so add `MrmOpenHierarchyFromBufferWithSize`.
- [x] **[verified]** `ResEncod.c:1404-1418` (`processExtendedSegments`) and `:784-803` (`…Hack`) scan for STX with no limit at `seglen`. `len = seglen - len - 1` then underflows: `XtMalloc(0)` followed by a memcpy of about 4 GB. It is reachable by **pasting** compound text (`ESC % / 1 0x80 0x80 STX`) through `TxtPropCv.c:546`, `SelectioB.c` and `XmCvtCTToXmString`. Bound the scan with `len < seglen`.
- [x] `Mrmwcrw.c:1129,1271-1281,1474-1481`: `pixargs[10]` is filled with no bound, so 11 or more icon/bitmap args overflow the stack. `Mrmwcrw.c:1782`: a wide-char count of -1 gives `XtMalloc(0)` and then an unbounded `mbstowcs`.
- [x] `ImageCache.c:1808` / `Svg.c:36-41,127`: `rasterize()` returns NULL for 0-size or huge SVGs and the caller dereferences it. The `(int)` cast of a huge float is undefined behaviour. `ImageCache.c:1212-1215` reads `image->data` after `XDestroyImage` and frees it twice (latent today).
- [x] Bundled libXpm, remaining CVEs: *(status: fixed in the bundled copy; a crafted header can still make it allocate up to INT_MAX bytes (crashes/xpm/huge-chars-per-pixel-oom))*
  - CVE-2022-44617: runaway loop on width 0 with a huge height (`Xpmdata.c:173-221`, `Xpmparse.c:415-511`).
  - CVE-2022-4883: `xpmPipeThrough` runs `execlp("gunzip"…)` through `PATH` (`XpmRdFToI.c:130,199,203`). Define `NO_ZPIPE`.
  - CVE-2023-43789: `xpmNextWord` has no NUL check.
  - CVE-2016-10164 is already fixed.
  - Again, the preferred fix is to link the system libXpm (≥ 3.5.17).

### 0.5 mwm — clients are untrusted from the window manager's point of view
- [x] **[verified]** `WmProperty.c:848-849,883-905`: the `WM_COLORMAP_WINDOWS` buffers are allocated `nitems*sizeof(Window) + 1`, which is one *byte*, not one element. Then `nitems+1` entries are written, overflowing the heap. Any client can trigger it.
- [x] `WmResParse.c:5174-5186` `GetNextLine`: a client's `_MOTIF_WM_MENU` line is copied into `static line[MAXLINE+1]` with no bound (global overflow).
- [x] `WmResParse.c:2864` / `WmMenu.c:3770`: `_MOTIF_WM_MENU` items from a client go through the full function parser, so a client (for example a remote `ssh -X` app) can install `f.exec "…"` as a menu item or key accelerator. Allow only `f.send_msg`, labels and separators from clients.
- [x] `WmXSMP.c:584`: `sprintf` of `SM_CLIENT_ID` (a client property up to 1 MB) into `resourceBuf[1024]`. `WmXSMP.c:933,1063`: newline injection into `.mwmclientdb`. `WmXSMP.c:1187,1204`: `strcpy` into `dbFileName`.
- [x] `WmProperty.c:698`: `_MOTIF_WM_HINTS` checks neither the format nor `nitems`, so a 40-byte struct is read from a 2–9 byte buffer. `WmProperty.c:571,760` do not check the format of `WM_STATE` / `_MOTIF_WM_INFO`. `WmWinInfo.c:1013` reads `WM_SAVE_HINT` when `nitems` is 0.
- [x] `WmSignal.c:261` `QuitWmSignalHandler` calls Xlib and Xt (`ConfirmAction`, `XFlush`, `Do_Quit_Mwm`) **from inside the SIGTERM/SIGHUP handler**, which is not async-signal-safe. It happens on `master` too. If a signal arrives while mwm is inside Xlib (for example during an interactive move) and the server then goes away, mwm can be left blocked or spinning on the dead connection instead of exiting. This was seen during testing: orphaned mwm processes using most of the CPU. Fix: defer the work with `XtAppAddSignal` / `XtNoticeSignal` (or a self-pipe) and do it from the event loop. Separately, document that SIGTERM shows a confirmation dialog by default (`showFeedback: kill`). *(status: the handler only calls XtNoticeSignal; the dialog or shutdown runs from the event loop; the man page documents the kill feedback)*
- [x] `WmResParse.c:7065-7964` contains `tmpnam()`, an unbounded `pchCmd[1024]` and `system("/bin/rm …")`. **It is not compiled today** (it sits inside `#ifdef WSM` / `PANELIST`). Delete it, or fix it before those flags are ever enabled.
- [x] `ColorObj.c:1016-1020` `XmeGetPixelData` returns with `_XmProcessLock` still held, which deadlocks multithreaded apps.

---

## Phase 1 — Real tests and CI (P1)

### 1.1 Make the test suite exist again
- [x] **[verified]** `src/tests/CMakeLists.txt` is a 7-line stub that tests `ENABLE_TESTS`, but the option is called `WITH_TESTS`. It contains no `add_test` at all. The suite was lost when commit `2490f84f` removed the `Makefile.am` files.
- [x] Wire `runner.c` + `suites.h` + `Xm/*.c` (libcheck, 66 cases: FontList, FontListEntry, PNG, JPEG, SVG, Log, LogConfig) into CTest: *(status: the fixtures are copied into the build tree and the tests run there, so nothing writes into src/tests)*
  - `pkg_check_modules(CHECK check)`;
  - `WORKING_DIRECTORY src/tests`, because the fixtures are opened by relative path;
  - the X-dependent suites under `xvfb-run -a`, or skip them with exit code 77 when there is no `DISPLAY`.
- [x] `ctest --no-tests=error` everywhere. `make test` currently prints "All tests passed!" after running zero tests (`tools/cmake/scripts/make_test.py`).
- [x] Delete `tools/cmake/motif_testing.cmake`, which is Blender's gtest logic with zero call sites.
- [x] Archive or delete the dead legacy test trees: `auto/` (the OSF MVS library with no test cases), `General/`, `environment/bin/*`, and the `revive_tests.sh` / `setup_graphical_tests.sh` / `run_graphical_tests.sh` scripts, which depend on a non-existent `.attic`.

### 1.2 Tests to add, in this order
1. [x] **Widget smoke test for every widget class** (about 75, generated from the WML database). Run it under Xvfb with ASan, UBSan and LSan: *(status: 69 classes, generated from the public headers; known library bugs are tracked as xfail cases)*
   - create, realize, `XtSetValues` / `XtGetValues` on every resource, destroy;
   - no leaks, no errors.
2. [x] **UIL → UID → Mrm round trip:** compile every `.uil` file in the tree with `uil`, then `MrmOpenHierarchy` + `MrmFetchWidget` each one, and assert on the resources.
3. [x] **Headless XmString tests:** create, concat, compare, the byte-stream round trip, `XmStringToXmStringTable`, ParseTable, UTF-8 and CT conversion. Port the assertions from the manual `XmString/*.c` programs, and add a regression test for each recent XmString fix (`d7c5c844`, `e41107cc`, `97547c53`, …).
4. [x] **Fuzz targets** (libFuzzer plus a corpus in the repo, and OSS-Fuzz once stable): *(status: 12 targets with seed corpora, replayed by CTest; .mwmrc has no fuzzer (covered by the mwm parse test); no OSS-Fuzz integration)*
   - PNG, JPEG, SVG (nanosvg), XPM, XBM;
   - `.uid` (`MrmOpenHierarchyFromBuffer`);
   - the XmString byte stream and CT;
   - the UIL lexer and parser;
   - `.mwmrc` and `.motifbind`;
   - **DnD receiver-info and drop-site stream**, and clipboard header records (feed them through a fake property buffer).
5. [ ] **Hostile-client tests:** a helper X client sets malformed `_MOTIF_DRAG_RECEIVER_INFO`, `_MOTIF_BINDINGS`, `_MOTIF_CLIP_*` and TARGETS replies, and the Motif app must survive under ASan. *(status: partial: the DnD, clipboard, bindings and compound-text parsers are covered by the fuzzers, and malformed window-manager properties by the mwm test; there is no end-to-end hostile client against a drag source or clipboard owner)*
6. [ ] **Text and TextField:** editing, clipboard, primary/secondary selection, undo, large documents (10 MB). *(status: partial: editing, primary selection, clipboard and a 10 MB document are tested, also with real xdotool input; secondary selection and undo are not)*
7. [ ] **i18n:** `ja_JP.UTF-8`, `de_DE.UTF-8`, RTL layout, input-method preedit (with a stub IM), message catalogs. *(status: partial: UTF-8, C locale and wide-character conversions are tested; ja_JP/de_DE (locales not installed here), RTL, preedit with a stub IM and message catalogs are not)*
8. [x] **mwm:** `.mwmrc` parse tests; a headless start under Xvfb; `f.*` functions driven through `xdotool`; malformed `WM_HINTS`, `WM_NORMAL_HINTS` and `_MOTIF_WM_HINTS` from a hostile client.
9. [x] **Visual regression:** `XGetImage` under Xvfb with the BDF fonts in `src/tests/environment/fonts` to keep output deterministic, compared against golden PNGs with a tolerance.
10. [x] A **coverage gate**: wire `WITH_COMPILER_CODE_COVERAGE` (it currently does nothing, see 1.3), publish the reports, and ratchet the target up. Start at 30% line coverage of `libXm`, then raise it. *(status: the coverage target fails below MOTIF_COVERAGE_MIN (30); measured 33.76% line coverage of libXm; reports are not published yet)*

### 1.3 Sanitizers, analysis and CI
- [x] The `WITH_COMPILER_ASAN` and `WITH_COMPILER_CODE_COVERAGE` flags are computed (`CMakeLists.txt:291-351`) and **never applied**. Use `add_compile_options` / `add_link_options`, and add `WITH_UBSAN`, `WITH_TSAN` and `WITH_MSAN`. *(status: TSan and MSan were only configured, never run (MSan needs instrumented X libraries))*
- [x] Fix the CI triggers. `.github/workflows/build.yml:6-10` uses `pull_request: types: [opened]`, so later pushes to a PR are never built. Reduce `permissions: contents: write` to `read`.
- [x] Un-comment the test steps (`build.yml:50-64`, `.gitverse/workflows/build.yaml:50-55`) and run them under `xvfb-run`.
- [x] CI matrix, at minimum: *(status: CI definitions only; the GitHub-hosted jobs (Alpine, s390x under qemu, FreeBSD VM) were not run from here)*
  - {gcc, clang} × {Debug+ASan+UBSan, Release+LTO};
  - Alpine (musl) and Ubuntu (glibc);
  - one 32-bit or big-endian job (qemu s390x), because the byte-order paths in DnD and Mrm are otherwise untested;
  - a real FreeBSD VM (`Dockerfile.freebsd` is currently Debian).
- [x] Static analysis in CI: *(status: cppcheck runs in bootstrap mode until a baseline from the first CI run is committed)*
  - `scan-build --status-bugs`;
  - `clang-tidy` with `bugprone-*`, `cert-*`, `clang-analyzer-*` and `security.insecureAPI.*`;
  - CodeQL;
  - `cppcheck`.
  - Start in baseline/ratchet mode.
- [x] An install-and-consume job: `cmake --install` into a staging directory, then compile hello-world through `pkg-config motif` **and** through `find_package(Motif)`.
- [ ] An ABI job: `abidiff` against the last tag and against upstream 2.3.8. *(status: partial: the informational job compares with the previous tag and the PR base; upstream 2.3.8 is not built in CI (it was compared once by hand))*
- [x] A reproducibility job: two builds plus `diffoscope` with `SOURCE_DATE_EPOCH` set.
- [ ] Distro packaging smoke tests: Debian `dpkg-buildpackage` and Fedora `rpmbuild`. *(status: partial: builds with the dpkg-buildflags and rpm %build_cflags flags and checks the staged install; there is no debian/ or .spec, so no real dpkg-buildpackage or rpmbuild run)*
- [x] Turn warnings into errors in CI (`CMAKE_COMPILE_WARNING_AS_ERROR`) once the curated warning list (2.3) is clean. *(status: WITH_WERROR=ON (all targets except the examples), and the blocking CI job uses it)*

---

## Phase 2 — Hardening and maturity (P1/P2)

### 2.1 Systematic safety sweep (beyond the individual bugs above)
- [x] Add a `_XmMallocArray(n, size)` / `_XmReallocArray` helper with overflow checks, and convert the **422** `XtMalloc`/`XtRealloc` calls whose size involves a multiplication. 132 of them cast the size to `Cardinal`/`int`/`unsigned`, which truncates on LP64. *(status: libXm converted; Mrm and Uil keep their own size checks, the helper is private to libXm)*
- [x] Add one `_XmGetWindowPropertyChecked(dpy, w, atom, type, format, min_items, …)` helper. Convert all 22 `XGetWindowProperty` sites; about 10 currently lack validation of type, format or nitems. *(status: every libXm reader converted except the chunked clipboard reader, which keeps its own checks)*
- [x] Replace `sprintf` / `strcat` / `strcpy` into fixed buffers with `snprintf` or a small growable string builder: *(status: GCC enforces the ban in every build; Clang only without glibc's _FORTIFY_SOURCE wrappers)*
  - Xm: 96 `sprintf` (about 74 into fixed buffers), 56 `strcat`, 32 `strcpy`.
  - Mrm: 69 `sprintf`.
  - Uil: 96 `sprintf`.
  - Ban them with `-Werror=deprecated-declarations` through a poisoning header.
- [x] Strict-aliasing: there are 37 `(XtPointer *)&typed_ptr` out-parameters (30 in `CutPaste.c`), and function pointers are stored through `XtPointer*` (`XmString.c:6206, 6428, 7260`). This is the same class as the `_XmEntrySegmentGet` miscompile (`97547c53`). Fix them, or build with `-fno-strict-aliasing` until they are fixed. *(status: the XtPointer * out-parameters are fixed; about 90 other type-punned out-parameters remain, so the build uses -fno-strict-aliasing)*
- [ ] Thread safety: about 166 file-scope and 160 function-scope mutable statics, many not protected by `_XmProcessLock` (e.g. `DataFSel.c:261`, `DropTrans.c:404`, `ResConvert.c:582,1092,1741`, `ClipWindow.c:225`). Audit them, and add a TSan test with two `XtAppContext`s. *(status: partial: the locks are now real (they compiled to nothing before) and DataField, DrawUtils, Obso1_2, TabBox and IconG statics were fixed; the full audit and a TSan test with two XtAppContexts are not done)*
- [x] Fix the logic bugs found along the way:
  - XdndProxy is never honoured (`DragICC.c:1015`; `length` should be `lengthRtn`).
  - XdndTypeList is only read when `XGetWindowProperty` itself fails (`DragICC.c:861-893`).
  - `TearOff.c:791` compares against the wrong constant.
  - `Xmos.c:1220` builds the mask name as `foo.xpm_m.xpm`.
  - `ImageCache.c:328` `SymbolicColorUsed` always returns False.
  - Leaks: `Transfer.c:1890`, `TextF.c:7205,7278`.
- [x] Hardening flags behind `WITH_HARDENING=ON` by default: `-D_FORTIFY_SOURCE=3`, `-fstack-protector-strong`, `-fstack-clash-protection`, `-fcf-protection`, `-Wl,-z,relro,-z,now`.

### 2.2 Symbol hygiene and ABI
- [x] `libXm` exports **3,224** dynamic symbols, and **2,149** of them are `_Xm*`. Among them are vendored `nsvg*`, `xpmPipeThrough`, empty `dump_fontlist` stubs, `NumLockMask`, `SetMwmStuff` and `XME_WARNING`. *(status: version scripts (XM_2.4/MRM_2.4/UIL_2.4) instead of -fvisibility=hidden: libXm exports 1737 symbols, libMrm 219, libUil 51)*
  - Fix:
    - `-fvisibility=hidden` plus `XM_EXPORT`, or a versioned linker script generated from the existing (unused) `libXm.elist`, `libMrm.elist` and `libUil.elist`.
    - Keep the roughly 10 `_Xm*` symbols that Mrm and the clients actually need.
- [x] Stop installing the roughly 95–194 internal `*I.h` headers (`Xm/CMakeLists.txt:573,803-806`), as well as Mrm's `IDB.h`. Install Uil's public `UilDef.h` / `UilAPI.h`, which are currently missing. *(status: Mrm/MrmosI.h is still installed on purpose: Motif 2.3 installs it)*
- [x] Make `MrmDecls.h` and `MrmosI.h` self-contained. Add `extern "C"` guards to the remaining headers: `DragDrop.h`, `XmAll.h`, `Xmpoll.h`, `obsolete.h`, `version.h`. Delete `Xpmrgbtab.h`, which needs the Windows `COLORREF` type.
- [x] Write an API/ABI policy document. Add symbol versioning (`.gnu.version_d`), and a `NEWS`/`CHANGELOG` that records the SONAME decision.
- [ ] Constify string parameters in the public API where doing so is ABI-neutral (`XmStringCreate(char*)`, `XmTextSetString(Widget, char*)`, …). There are only 135 `const` in all public headers. *(status: partial: XmStringCreate, XmStringCreateLocalized, XmStringLtoRCreate, XmStringCreateSimple and XmStringCreateLtoR take const char *; XmTextSetString cannot (modifyVerify hands the caller's buffer to callbacks), the rest is not audited)*

### 2.3 CMake modernisation
- [x] `project(Motif VERSION 2.4.1 LANGUAGES C)`. CXX is enabled but nothing built is C++. A C++ compiler is still *required*, and its version check fails on GCC < 11 (`CMakeLists.txt:130,160-172,1307`).
- [x] Make it target-based: replace the global `add_compile_definitions` / `include_directories` (`CMakeLists.txt:420-429,1494-1515`). Remove the absolute `-include .../motif_system.h` and `MOTIF_SYSTEM_H` (`:1498`, `Xm:725`), which defeat ccache and reproducibility. Use `Threads::Threads` and imported targets from `pkg_check_modules(... IMPORTED_TARGET)`.
- [x] Fix the feature macros: `_POSIX_C_SOURCE=200809L` conflicts with `_XOPEN_SOURCE=600`, and `_BSD_SOURCE` / `_SVID_SOURCE` are deprecated. Detect `HAVE_SYS_POLL_H` instead of forcing it (`:1487`).
- [x] Ship a `find_package(Motif)` config package:
  - one `MotifTargets` export (Xm, Mrm, Uil);
  - `configure_package_config_file` + `write_basic_package_version_file`;
  - namespaced `Motif::Xm`, `Motif::Mrm`, `Motif::Uil`.
- [x] Delete or wire the dead options:
  - `WITH_STATIC_LIBS`, `WITH_WML_TOOLS`, `WITH_COMPILER_CCACHE` (ccache is force-enabled anyway at `:116-123`);
  - `MOTIF_WERROR` (it lives in a function that is never called);
  - `WITH_UIL_DEBUG`, which is checked under the name `ENABLE_UIL_DEBUG`;
  - drop `FORCE` from the presets in `tools/cmake/config/*.cmake`.
- [x] Prune the roughly 400 lines of MSVC/clang-cl/Intel warning setup (`:883-1305`) and the Blender helpers with zero call sites (`CheckALSA`, `CheckWayland`, `CheckVulkan`, `glsl_to_c`, …). Fix the Clang minimum-version check, which never runs (`:173-187`).
- [x] Curate one warning list. Remove the "enable, then disable" pairs (`:641-703`). Re-enable these and fix what they report, in this order: `-Wimplicit-fallthrough`, `-Wstrict-prototypes`, `-Wmissing-prototypes`, `-Wdiscarded-qualifiers`, `-Wstringop-overflow`, `-Wsign-compare` (885 hits). Fix the two real hits that are already visible: `FontS.c:1325` `-Wstringop-truncation` and `mkcatdefs.c:525` `-Wcalloc-transposed-args`. *(status: the whole list is on (including -Wsign-compare) and the libraries, programs, tools and tests build without warnings with GCC and Clang; the examples still have about 470 distinct warnings)*
- [x] Fix the install layout:
  - don't install the build-time generators `makestrs`, `mkcatdefs`, `mkmsgcat`, `wml*` or `libwml.a`;
  - don't install the demo libraries `Exm`, `Xmd`, `WsmDemo`;
  - use `GNUInstallDirs` for lib64;
  - don't force `/usr` as the prefix (`:136`);
  - remove the `sudo` and `xargs rm` install/uninstall in `GNUmakefile:363-385`.
- [x] Make cross-compilation possible. Host tools run during the build (`makestrs`, `wml`, `wmluiltok`, `wmldbcreate`; the last one needs an X server). Provide `MOTIF_HOST_TOOLS` imports, or commit the WML outputs as pre-generated sources.
- [x] Make code generation robust:
  - `generate_xm_strings.sh` hides makestrs' stderr and writes fallback headers silently;
  - `DEPENDS` is missing the `.ht`/`.ct` templates and `Uil.y`;
  - `wmlparse.h` is produced by two rules;
  - the yacc and bison paths contradict each other;
  - header copies use a configure-time GLOB.
- [x] Ninja job pools are hard-coded to 4 compile and 2 link jobs and are on by default (`:398-411`). Size them from the CPU count, or default them off.

### 2.4 Legacy cleanup
- [x] Fix the 45 unprototyped `()` definitions and 19 `()` declarations, which are errors under C23. Then enable `-Wstrict-prototypes`.
- [x] Make `XTHREADS` unconditional (60 sites). Remove the `__osf__`, `sun`, `_AIX`/`AIXV3`, `SVR4`, `SYSV`, `CSRG_BASED`, `X_NOT_POSIX`, `c_plusplus` and `NeedWidePrototypes` branches. Remove `Xmos_r.h`, a 1,074-line copy of `Xos_r.h`, and `XmosP.h:92` (`vax`, `apollo`, `stellar`). Remove `char *alloca();`. *(status: NeedWidePrototypes stays in Mrm (it is part of the ABI where Xfuncproto.h sets it); Xmos_r.h and Xmpoll.h remain as installed stubs that include the X.Org headers)*
- [x] Remove the 41 `#if 0` blocks; start with `Column.c`, which has 15. *(status: the vendored Xpm sources keep theirs)*
- [x] Resolve the 21 TODO/FIXME markers, starting with `XmString.c:3234-3273, 5009, 5065` and `XmRenderT.c:577`. *(status: four notes remain, in the vendored Xpm code and two refactoring hints in DropDown.c and LabelG.c)*
- [x] Optional mechanical cleanup in a separate commit series: drop `register` (980 occurrences). Leave `externalref`/`_XmConst` (1,512 and 2,130 occurrences) for a later major version, because they appear in public headers. *(status: register removed; externalref/_XmConst left, as planned)*
- [x] Move `.clang-format` to the repository root (it currently lives only at `tools/dev/`) and adapt it to the Xm style. Add `.editorconfig`, `.clang-tidy` and a pre-commit hook. Format new and changed code only. *(status: .clang-tidy comes with the CI static analysis)*

### 2.5 Documentation, localisation and project metadata
- [x] Make the README true:
  - C23 is not used; the build uses gnu17 *and* gnu11.
  - It says GCC ≥ 13; CMake accepts 11.
  - The version badge says 2.4.0; the code says 2.4.1.
  - It claims "screen reader support"; there is no AT-SPI/ATK code.
  - It lists Solaris and FreeBSD as supported; neither is in CI.
  - It documents `make deps`; that target builds a non-existent directory.
  - The build badge is static.
- [ ] Write man pages for the 265 exported functions that have none. The largest groups are the `XmLog*` API (about 20), `XmeXpm*` (31), TabStack/TabBox (24), the DataField accessors (19), `XmI18List` (22) and DropDown. *(status: partial: the listed groups are documented (132 new pages); 135 exported functions, mostly Xme* widget-writer functions, still have no page)*
- [x] Add `CHANGELOG`/`NEWS`, `CONTRIBUTING.md`, `SECURITY.md` (the DnD and clipboard findings warrant a disclosure policy) and `AUTHORS`.
- [x] Localisation: no rule builds or installs `localized/`. All catalogs date from 1996, are ISO-8859-1 or EUC-JP, and are missing 20 of the 388 message IDs. Either convert them to UTF-8, fill the gaps, gencat them and install them, or drop them. Stop tracking the generated `localized/C/msg/*.msg`. *(status: 21 messages use the English text with a '$ TODO: translate' marker)*
- [x] Rewrite `src/tests/TEST_ENVIRONMENT_README.md` for the new CTest and Xvfb flow. *(status: the file was deleted; src/tests/README.md describes the CTest and Xvfb flow)*

---

## Phase 3 — Blazingly fast (P2)

### 3.0 Measure first
- [x] Add `src/tests/bench/xmbench`, one subcommand per case. Each case reports:
  - ns per operation;
  - mallocs per operation (LD_PRELOAD counter);
  - X requests per operation (`XNextRequest` delta);
  - **round trips per operation** (LD_PRELOAD wrapper on `_XReply`). This is the key metric, because most of the wins below remove round trips.
- [ ] Run it headless with `xvfb-run -s "-screen 0 1920x1080x24 +extension RENDER"`. Expose latency with TCP plus `tc netem delay 2ms`, or with `xtrace -c`. Pin with `taskset`, take the median of 5 runs, store JSON, and fail CI on a regression above 5%. *(status: partial: runs headless under xvfb-run with JSON output; no netem/xtrace latency setup and no CI regression gate)*
- [x] Micro-benchmarks:
  - XmString: create, concat ×10k, extent ×1M (optimized and unoptimized; core, fontset and Xft), draw;
  - `XmeTraitGet` ×10M;
  - gadget Get/SetValues ×10k;
  - render-table conversion;
  - `_XmXftDrawCreate` with 10k windows.
- [ ] Macro-benchmarks: *(status: partial: the widget, Form, Container, List, Text and menu cases exist; the mwm cases do not)*
  - 10k PushButtons and 10k gadgets in a RowColumn;
  - a Form with 1k and 5k chained children;
  - a Container with 10k IconGadgets;
  - an XmList with 100k items (add, select by value, delete, 1000 page-downs);
  - XmText: 10 MB inserts and 100k keystrokes;
  - menu post/unpost ×1k;
  - mwm: map 500 clients, 10k title updates, a scripted drag.
- [ ] Profile with `perf record --call-graph dwarf`, `callgrind`, `heaptrack` and `xtrace`. *(status: not done: perf, valgrind and heaptrack are not available here)*

### 3.1 Remove server round trips from hot paths (highest impact, small changes)
- [x] **[verified]** `XmRenderT.c:2438-2440` `_XmXftDrawString2` makes a blocking `XGetGCValues` + `XQueryColor` round trip **on every Text and TextField draw** (`TextOut.c:2112…`, `TextF.c:1412`, `DataF.c:1574`). `_XmXftDrawString` (`:2490-2511`) does the same for every Label, gadget and mwm title, because unspecified FG/BG is the default.
  - Fix:
    - Pass the pixel down.
    - Resolve the `XftColor` once per rendition/GC, through a hash keyed by (dpy, colormap, pixel).
    - Use the widget's colormap instead of `DefaultColormap`, which is also a correctness bug.
    - Replace `GetCachedXftColor` (linear search, +1 realloc growth).
- [x] **[verified]** `Xm.c:296-313` `_XmIsISO10646` makes an `XInternAtom` + `XGetAtomName` round trip for each font property, **on every segment draw** (`XmString.c:3084`, `TextF.c:1434`). Compute it once at font load and store it as a flag on the rendition. *(status: done with per-display interned atoms instead of a per-rendition flag; no round trip and no allocation per draw)*
- [ ] `XmImVaSetValues(XmNspotLocation)` runs on every cursor move (35 sites) and ends in a synchronous `XSetICValues` with ibus or fcitx. Send only when the value changes, and coalesce updates in an idle work proc. *(status: partial: duplicate spot locations are no longer sent; updates are not coalesced in an idle work proc)*
- [ ] `ScrollBar.c:2192,3187,3208` calls `XSync` per autorepeat tick; `XFlush` is enough. `RCMenu.c:655-700` calls `XSync` and `XGetWindowAttributes` per menu unpost. `Traversal.c:1094-1150` `XmGetVisibility` calls `XQueryTree` and `XGetWindowAttributes` per sibling; track map state from events instead. `XmString.c:1768` also falls back to `XGetWindowAttributes`. *(status: partial: the first autorepeat XSync is an XFlush, RCMenu unposting makes no round trip, XmGetVisibility uses Xt geometry for widget siblings and the XmString tab fallback is gone; the per-tick ScrollBar XSync stays on purpose (back-pressure) and XmGetVisibility still makes one XQueryTree)*
- [x] mwm:
  - `WmWinConf.c:3717-3775` polls `XQueryPointer` during move/resize; use `PointerMotionHintMask` and block instead.
  - `FlashOutline` (`:1560-1568`) busy-spins on `XSync` at 100% CPU; use a timer.
  - Cache the title width (`WmCDecor.c:1145`).

### 3.2 Fix algorithmic complexity
- [x] **Form** (`Form.c`):
  - `SortChildren` is O(n²) even when the input is already sorted (`:1531-1625`), and it runs on every `DeleteChild`, so destroying the Form is O(n³).
  - `CalcFormSize` re-syncs each prefix of the children, up to `MAX_LOOP=10000` times (`:498-635`).
  - The `Check*Base` functions recurse.
  - Fix:
    - a Kahn topological sort behind a dirty flag;
    - skip the sort while `XtIsBeingDestroyed(parent)`;
    - a single relaxation pass;
    - memoise the `Check*Base` results.
- [ ] **Container** (`Container.c:5490-5560`): `InsertNode` renumbers all siblings on each insert, so filling it is O(n²). Add a tail fast path and lazy `position_index`. *(status: partial: tail fast path, and whole-container destroy no longer renumbers; inserts before the tail are still O(siblings))*
- [ ] **XmList** (`List.c`): *(status: partial: one-pass selection rebuild, cheaper lookups, exact max-extent tracking on delete and XCopyArea scrolling for the scrollbar path; no geometric growth, array of structs, content hash or lazy extents (ListP.h layout is installed))*
  - Arrays are resized to the exact size, one malloc per element, and every item's extent is computed at insert (`:2464-2476,2683`).
  - `OnSelectedList` is O(N·S) (`:2854`).
  - `ItemNumber` / `ItemExists` do linear `XmStringCompare` scans (`:2823`).
  - `UpdateSelected*` rebuilds the selection and copies every selected string (`:3025-3100`).
  - Scrolling repaints every visible row; `List.c` never calls `XCopyArea`.
  - Fix:
    - geometric growth and an array of structs;
    - lazy extents and a max-width histogram;
    - a content-hash index;
    - an incremental selected list;
    - scroll by `XCopyArea` and paint only the newly exposed rows.
- [ ] **XmString:** *(status: partial: the tab-only screen lookup, the UTF-8 flag and the allocation-free UTF-8 decoding are done; the extent cache for optimized strings is not (no room in the installed structures))*
  - Extents are never cached for *optimized* strings, the common case (`CacheGet` returns NULL, `XmString.c:2208`).
  - `OptLineMetrics` calls `XmGetXmDisplay` (app lock, process lock, `XFindContext`) even when there are no tabs (`:1930,1983`).
  - There are per-call `strcmp`s for UTF-8 detection (`:1916`).
  - `_XmUtf8ToUcs2` mallocs per call (marked "TODO: very unoptimized", `:5008`).
  - Fix: a (render-table generation → extent) cache; move the screen lookup into the tab branch; an `is_utf8` flag on the rendition.
- [ ] **XmString building:** *(status: not done: the O(n^2) did not reproduce in the scale runs, and the refcount and header changes need room in installed structures)*
  - `Concat`, `Generate` and `ParseText` are O(n²) (exact-size reallocs, `ConcatAndFree` per match, `:1011-1368, 6265-6456`).
  - The 6-bit refcount (`XmStringI.h:146`) forces a full clone every 63 `XmStringCopy` calls.
  - Fix: add an internal builder, keep the last direction in the header, and widen the refcount (the struct is internal, so this does not break ABI).
- [x] `_XmXftDrawCreate` / `_XmXftDrawDestroy` (`XmRenderT.c:2377-2408`) do a linear scan over every window that has ever drawn, 2–3 times per string. Use an XContext or hash keyed by window, and skip redundant clip changes.
- [x] **XmText** (`TextStrSo.c:76,834-925`): the gap buffer grows by an additive 1024 bytes, which is O(N²/1024) at the head, and shrinks back to the minimum. Grow by ×1.5 and add hysteresis.
- [ ] **Extension data and gadgets** (`BaseClass.c:303-360`, `LabelG.c:863-890`, `ExtObject.c:274-338`): *(status: partial: ExtObject pool, BaseClass free list, rotated XContext ids and a LabelGadget free list; the other gadgets still allocate per Get/SetValues)*
  - XContext's hash clusters on aligned pointers.
  - Each gadget Get/SetValues allocates two secondary objects and memcpys the widget.
  - Fix: store the ext-data stack in the instance, or use a mixed-pointer hash; add per-class free lists.
- [ ] **Render tables:** `ResConvert.c:481-530` uses `XtCacheNone` with the widget as a conversion argument, so the table is re-parsed per widget. The Xft font cache (`XmRenderT.c:1630-1720`) does about 10 `strcmp` per entry, grows by +1, and never evicts. Intern descriptions in a hash. *(status: partial: the Xft font cache is a per-display hash that is freed with the display; the render-table converter still parses per widget (caching it is not semantically safe))*
- [ ] **Traits** (`Trait.c`, 177 call sites): every lookup takes the process lock and probes a fixed 257-bucket hash. Replace with a lock-free per-class array indexed by trait id (about 25 traits). *(status: partial: an open-addressing hash replaces the fixed table; lookups still take the process lock)*
- [x] **Locks:** there are 673 `_XmProcessLock` and 416 `_XmAppLock` call sites, each a call into libXt even in single-threaded apps. Use an inline "threads initialised?" check.
- [ ] `Cache.c:80-114` `_XmCachePart` is a linear search. `Draw.c:66-100` builds one segment per pixel row for shadows; use `XFillRectangles` or a polygon instead. *(status: partial: _XmCachePart keeps records most recently used first; Draw.c shadows are unchanged (a polygon would not be pixel-identical, and it already sends 2 requests per call))*

### 3.3 Toolchain-level speed
- [x] After 2.2 (visibility), build release with `-fvisibility=hidden -fno-semantic-interposition -fno-plt`. *(status: export control is done with version scripts rather than -fvisibility=hidden; -fno-semantic-interposition in every optimized build, -fno-plt with -z now)*
  - The current build has **7,459** `R_X86_64_64` symbolic relocations, 3,114 of them against `_XmStrings`, plus 476 GOT loads for Xm-owned data. All are resolved at every process start.
  - `-fno-semantic-interposition` is currently applied only with LTO (`CMakeLists.txt:621`).
- [x] Enable `WITH_LTO` in the release preset; it is off by default.
- [x] Add a `WITH_PGO` option trained on `xmbench` and the widget smoke test.
- [ ] Measure the startup cost before and after with `LD_DEBUG=statistics` and `perf stat` on `hello_motif`. *(status: partial: relocation, symbol and startup counts are in doc/abi-policy.md; perf stat was not available)*

---

## Suggested execution order

| # | Milestone | Contents | Exit criterion |
|---|---|---|---|
| 1 | **Hygiene** | 0.1, 0.2 | Clean clone builds from a read-only source tree; `pkg-config` works; `.so.4` decision documented |
| 2 | **Tests online** | 1.1, the wiring part of 1.3 (ASan/UBSan/coverage flags, CI triggers) | CTest runs ≥ 66 cases under Xvfb in CI; `--no-tests=error` |
| 3 | **Security** | 0.3, 0.4, 0.5, plus fuzz targets and hostile-client tests (1.2 #4–5) | All P0 memory bugs fixed and each has a regression test; fuzzers run 1 hour with no crash |
| 4 | **Stable** | 1.2 #1–3 and #6–8, 2.1, 2.2 | Widget smoke test, UIL round trip and the XmString suite pass under ASan+UBSan; `abidiff` job green |
| 5 | **Mature** | 2.3, 2.4, 2.5 | `-Werror` with the curated list; config package; docs and README accurate |
| 6 | **Fast** | 3.0, then 3.1, 3.2, 3.3 | `xmbench` baseline published; zero round trips per text draw; List, Form and Container scale near-linearly; release built with hidden visibility and LTO |
