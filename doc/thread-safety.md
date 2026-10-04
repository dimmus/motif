# Thread safety of libXm and libMrm

This is the audit of the mutable static data of `src/lib/Xm` and
`src/lib/Mrm`: every file-scope and function-scope static (and every
exported global) that is not `const`, how it is protected, and what was
changed to make it safe.  Keep it up to date when you add a static.

## The model

Motif follows Xt.  A program that uses threads calls `XInitThreads` and
`XtToolkitThreadInitialize` first.  Then:

- Each application context has a lock, `XtAppLock`.  Every Xt and Motif
  function that takes a widget or a display takes the lock of its
  application context (`_XmAppLock`), so several threads may share one
  application context.  The usual design is one application context
  (and display) per thread.
- One process lock, `XtProcessLock`, protects data shared by all
  application contexts.  Motif's `_XmProcessLock` is that same
  recursive lock (it does nothing until `XtToolkitThreadInitialize` has
  been called).  It is always taken after an application lock, never
  before, except by code already holding the application lock of the
  same context.
- Xt itself holds the process lock while it initializes a widget class,
  while it calls a type converter through `_XtConvert`,
  `XtConvertAndStore` or a caching `XtCallConverter`, and while it calls
  an `XtRCallProc` resource default and copies the value it returns.

So a static is safe when it is (a) written only in class
initialization, (b) a converter or default-proc result buffer, (c) read
only, (d) created once under the process lock and only read after, (e)
used only under the process lock, or (f) thread-local because it only
carries a value from a call to the next one made by the same thread.

Three tools came out of the audit (internal, not exported):

- `_XmStartErrorTrap` / `_XmEndErrorTrap` (Xm.c): catch the X errors of
  the requests a thread makes, without swapping Xlib's process-wide
  error handler around them (see below).
- `_XmGetSubresources` (Xm.c): `XtGetSubresources` under the process
  lock.  Xt compiles a static resource list in place on first use under
  the application lock only, and Motif's lists are shared by all
  application contexts.
- `_XmFreeDefaultRenderTable`, `_XmFlushColorCache` (called from the
  XmDisplay's destroy method) and the screen case of
  `_XmCleanPixmapCache`: per-display caches are emptied when the
  display goes, as a later display may get the same `Display` or
  `Screen` address.

## The test

`Threads.mtapps` (`src/tests/threads/mtapps.c`) runs two threads, each
with its own application context and display, that each build, realize,
exercise and destroy a widget tree several times: XmString, compound
text and render tables, font lists, parse tables, XmText (including the
clipboard), XmTextField, XmList, XmComboBox, XmSpinBox, XmScale, menu
bars, pulldown, popup and option menus with widget and gadget buttons,
XmContainer with icon gadgets, XmNotebook, drop site registration and
the drag context, XmClipboard copy and retrieve, file selection,
message and prompt dialogs, pixmaps, colours,
`XmGetSecondaryResourceData`, and an Mrm hierarchy (hellomotif.uid,
compiled by the uil tests) opened, fetched and closed on each display.
Displays are opened and closed again in each iteration, which is what
found the stale per-display caches.

It runs in every build (it found the crashes listed below) and is the
ThreadSanitizer test of a `WITH_TSAN` build, where any report fails it.
`src/tests/threads/tsan.supp` is used for that run; it suppresses
nothing at present (see the file for the policy: only libX11 and libXt
internals may be suppressed, each with its reason).  Under
AddressSanitizer the program turns off ASan's strict memcmp check (for
libX11's quark lookup) and suppresses three leaks that also happen
without threads; mtapps.c says which.

ThreadSanitizer only reports accesses not ordered by a lock both threads
took in between.  Xt takes the process lock very often, so many real
races of Motif statics are hidden from it: with the unlocked slot
allocation of ExtObject.c put back, for instance, it reports nothing.
It did catch the default-proc buffers below.  The audit is by reading;
the test catches what it can.

## Verdicts

`class-init`: written only by class initialization, under Xt's process
lock; read only afterwards.  `converter`: an Xt type converter's result
buffer (see below).  `default`: an `XtRCallProc` default's result
buffer; see below.  `read-only`:
never written (could be const).  `once`: created once under the
process lock, only read afterwards.  `locked`: every access under the
process lock.  `thread`: `_Thread_local`.  `fixed`: changed by this
audit; the change follows.

About `converter`: Xt calls converters, and copies the value into the
destination the caller gave, under its process lock; a caller of the
old `XtConvert`, or of `XtCallConverter` with a non-caching converter,
gets a pointer into the buffer, exactly as with Xt's own converters.
The `_XM_CONVERTER_DONE` macro (XmP.h) declares such a buffer (`buf`)
in every converter that uses it.

About `default`: Xt calls a resource default, and copies the value it
returns, under its process lock; but Motif also calls these procs
directly and reads the buffer afterwards (PushButtonGadget and the
other gadgets, ArrowButtonGadget, ScrollBar, RowColumn), which raced
(ThreadSanitizer saw it).  These buffers are now `_Thread_local`: Xt
copies the value in the thread that called the proc, and a direct
caller reads its own thread's buffer.

### Groups

| Files | Statics | Verdict |
|-------|---------|---------|
| all widget and gadget files | class records (`xm*ClassRec`), class pointers (`xm*WidgetClass`, `xm*GadgetClass`, `xm*ObjectClass`), base class and other class extension records, `XmCacheClassPart` records, resource lists (`resources`, `syn_resources`, `constraints`, `cache_resources`, `get_resources`, ...), action tables, trait records (`MenuSavvyRecord`, `*Transfer`, `*T` ...), default translation strings | class-init.  Two exceptions are handled where they occur: the per-widget switch of `tm_table` (MenuProc.c) and the counted swap of `initialize` by the BaseClass wrappers. |
| CascadeB, ClipWindow, ComboBox, DrawnB, Label, Notebook, PushB, RowColumn, SelectioB, SpinB, ToggleB | parsed translations and accelerators (`*_parsed`, `*Parsed`, `spinAccel`, `ClipWindowTranslations`) | class-init (ClassInitialize) |
| Container, CutPaste, Display, DragC, FileSB, I18List, Label, List, PrintS, Scale, Screen, SelectioB, TearOff, TextF, TextFSel, TextIn, TextSel, Transfer, TxtPropCv, VendorS, VirtKeys, Xm | `atom_names`, `names` (arrays of atom names for `XInternAtoms`) | read-only |
| Messages.c, MrmMessages.c | `_XmMsg*`, `XME_WARNING`, `_MrmMsg_*` | read-only (exported) |
| RepType.c | `*Names` tables, `StandardRepTypes` | read-only after class-init (`_XmRepTypeInstallConverters` sets `reverse_installed` under the lock) |
| ResConvert, PixConv, RepType, SpinB, VendorSE, IconG | `buf` of `_XM_CONVERTER_DONE` | converter |
| ButtonBox `option`, ColorS `mode`, Column `static_val`, Hierarchy `type`, IconButton `type`, Tree `connect`, `compress`, `lineStyle`, TabBox `result`, `static_val`, TabStack `result`, `static_val`, ResConvert `itsChild`, `tblptr` (two), `buf` (CvtStringToAtomList) | converter result buffers | converter |
| Hierarchy, IconButton, Tree | the `XtQE*` quarks and `haveQuarks` of their converters | converter (set on first call, under Xt's lock) |
| Color `new_value`, Container `result`, Direction `direction`, Display `thickness` (two), DragC `pixel` (two), Frame `child_type`, List `sb_display_policy`, Notebook `pixel`, `back_page_pos`, PanedW `indent`, PixConv `pixmap` (two), ResInd `unit_type`, Scale `direction`, `slider_visual`, `slider_mark`, `editable`, ScrollBar `direction`, `background`, `traversal`, `slider_visual`, `slider_mark`, `editable`, `highlight`, ScrolledW `placement`, `visual_policy`, SelectioB `type`, SeparatoG `pixmap`, TearOffB `default_height`, TextF and TextOut `cursor_pos_vis`, Tree `default_val` (two) | `XtRCallProc` result buffers | default; fixed: `_Thread_local` (Frame's `child_type` is a constant) |
| Display, DragBS, DragIcon, Container, IconG, MenuProc, Protocols, PrintS, TextF, TextFSel, TextIn, TextSel, DataF, Obso2_0, IconButton | lazily created `XContext`s (`displayContext`, `displayTo*Context`, `_XmTextualDragIconContext`, `dragIconInfoContext`, `large/smallIconContext`, `SaveTranslationsContext`, `allProtocolsMgrContext`, `PrintContextToWidget`, `_XmPrintScreenToShellContext`, `_XmTextFDestContext`, `_XmTextFDNDContext`, `_XmTextDestContext`, `_XmTextDNDContext`, `_XmDataF*Context`, `actualClassContext`, `worldObjectContext`, `stippleContext`, `primSelectContext`) | once (IconG and PrintS in ClassInitialize; Obso2_0's: fixed, were created without the lock) |
| Screen.c `_Xm*CursorIconQuark`, `_XmDefaultDragIconQuark`; Trait.c `XmQT*`; BaseClass `XmQmotif`; Display `_Xm_MOTIF_DRAG_AND_DROP_MESSAGE` | exported quarks and strings | class-init |
| Gadget, Manager, Primitive, BaseClass, Obso2_0 | `first_time`, `firstTime` of class initializers | class-init |
| Gadget, Primitive | `subresources` (tool tip) | compiled in place by Xt on first use: fixed, fetched with `_XmGetSubresources` |
| CascadeBG, DropSMgr, Gadget, IconG, LabelG, Primitive, PushBG, SeparatoG, Simple, TextIn, TextOut, ToggleBG, VendorS, VendorSE | static resource lists given to `XtGetSubresources` | fixed: `_XmGetSubresources` (ExtObject already held the lock) |
| Xpm*.c, nanosvg.h | `_XmxpmDataTypes`, `_XmxpmColorKeys`, `byteorderpixel`, RCS strings, `nsvg__colors` | read-only |
| XmTabList.c | `units_image`, `model_image`, `alignment_image` buffers | compiled only with `_XmDEBUG_XMTABLIST` |

### Individual statics

| File | Static | Verdict |
|------|--------|---------|
| BaseClass.c | `objectClassWrapper` | class-init (`_XmInitializeExtensions`, first call) |
| BaseClass.c | `resizeRefWContext`, `geoRefWContext` | fixed: were recreated by each of the five class initializers calling `_XmInitializeExtensions` while wrappers of other classes used them; now created once |
| BaseClass.c | `extToContextMap` | locked |
| BaseClass.c | `freeAssocData`, `numFreeAssocData` | locked |
| BaseClass.c | `shadowObjectClassRec` | fixed: `_XmTransformSubResources` changes its resource list for a moment and is reached from `XmGetSecondaryResourceData` at any time; now under the lock |
| BaseClass.c | `*Wrappers`, `*LeafWrappers` tables | read-only |
| BaseClass.c | `_Xm_fastPtr`, `_XmInheritClass` (exported) | read-only (`_Xm_fastPtr` is never set) |
| ButtonBox.c | `option` | converter |
| Color.c | `colorBlocks`, `lastColorBlock` (was `Color_Set`, `Set_Count`, `Set_Size`) | fixed: grows by blocks that do not move (`_XmSearchColorCache` and `_XmAddToColorCache` return pointers into it, which a realloc in another thread left dangling); entries of a display are dropped when its XmDisplay is destroyed; locked |
| Color.c | `default_set`, `default_set_count`, `default_set_size` | fixed: moved to file scope, flushed per display; locked |
| Color.c | `XmCOLOR_*_THRESHOLD`, `XmFOREGROUND_THRESHOLD`, `XmTHRESHOLDS_INITD` | fixed: set (per screen, shared) and used under the lock, marked initialized only once set |
| Color.c | `ColorRGBCalcProc` | fixed: read under the lock by `XmGetColorCalculation` and `XmSetColorCalculation` |
| Color.c | `color` (GetDefaultBackgroundColor), `background` (GetDefaultColors) | locked (callers hold the lock) |
| Color.c | `new_value` (XmeGetDefaultPixel) | default, fixed: `_Thread_local`; `XmeGetDefaultPixel`, also called by applications, takes the lock for the cache |
| ColorObj.c | `_XmColorObjCache` | once |
| ColorObj.c | `_XmColorObjCacheDisplay` | fixed: removed.  Every display's ColorObj was saved in the context table of the first display, and looked up there without the lock; it is now saved in its own display's table |
| ColorObj.c | `_XmDefaultColorObj` | fixed: read under the lock (ColObjFunc.c too), unpublished before its data is freed, cleared when its display closes |
| ColorObj.c | `xmColorObjClass->core_class.class_name` | fixed: set and used under one lock |
| ColorObj.c | `IconColorNames` | read-only |
| ColorS.c | `args` (SetSliders) | fixed: local |
| ColorS.c | `names`, `mode` | read-only; converter |
| Column.c | `in` (ChangeManaged), `label_widget` (ConstraintInitialize) | fixed: thread |
| Container.c | `Num_tab`, `Tab_pool` | locked; fixed: the size was read before taking the lock |
| Container.c | `x_deltas`, `y_deltas` | read-only |
| CutPaste.c | `cbProcTable`, `cbIdTable`, `maxCbProcs` | locked |
| CutPaste.c | `_passed_type` | fixed: thread |
| DataF.c | contexts, selection serial | locked (fixed in the previous round) |
| Display.c | `curDisplayClass` | locked |
| DragBS.c | `bad_window`, `oldErrorHandler`, `firstProtectRequest`, `errorWindow`, `RMW_ErrorFlag` | fixed: removed, error traps |
| DragBS.c | `first_time` (ReadAtomsTable) | locked; process-wide (one retry to recreate the Motif drag window for the first display that needs it) |
| DragBS.c | `stringTargets` | read-only |
| DragC.c | `current_dc` | fixed: removed, error trap |
| DragICC.c | `_XmByteOrderChar` (exported) | once (`_XmInitByteOrderChar`, from DisplayInitialize) |
| DragICC.c | `xdnd_version`, `xdnd_version_min` | read-only |
| DragIcon.c | `*CursorTable` | read-only |
| DragOverS.c | `mixed_cache` | fixed: searched without the lock and linked in before being filled; pixmap ids now compared only for icons of the same display |
| DrArrow.c | `allocated`, `top`, `cent`, `bot` | locked; fixed: the size was tested outside the lock |
| Draw.c | `segms`, `segm_count` | locked |
| DrawUtils.c | (none left) | fixed in the previous round |
| DropSMgr.c | `tmpRegion` (DetectImpliedClipper) | fixed: removed, never used |
| DropSMgr.c | `tmpR` (DetectAllClippers, IntersectWithWidgetAncestors) | locked |
| DropSMgr.c | `pR`, `testR` (IntersectWithDSInfoAncestors) | fixed: locked for the whole use (was locked statement by statement); `testR` removed, unused |
| DropSMgr.c | `testR`, `tmpR` (PointInDS) | fixed: locked for the whole use; one statement wrote `testR` without the lock |
| DropSMgr.c | `dsRegion`, `clipRegion`, `tmpRegion` (DoAnimation), `tmpRegion` (PutDSToStream) | fixed: locals.  DragUnder.c's animation kept pointers to them until the drop site was left; it now keeps copies |
| DropTrans.c | `which` | locked |
| DropTrans.c | `isValidStartDropTimerId` | compiled only with `CR1146` |
| ExtObject.c | `extarray` | fixed: slots taken and given back under the lock |
| FontS.c | `anyquark`, `anyquark2` | fixed: per call (and per widget: they came from the first widget's strings) |
| FontS.c | `GValidSizes`, `resolutions` | read-only |
| GrabShell.c | (IgnoreXErrors) | fixed: error traps |
| Hash.c | `FreeBucketList` | fixed: locked (shared by every table, some used without the lock) |
| Hierarchy.c | `args` | read-only |
| I18List.c | `global_current_widget` | fixed: thread (qsort comparator context) |
| I18List.c | `elist_q` | fixed: local (was written on every call) |
| I18List.c | `_params` | read-only |
| IconBox.c | `params` (two) | fixed: locals |
| IconBox.c | `G_any_cell` | read-only |
| IconButton.c | `pix_cache_list` | fixed: locked |
| IconButton.c | `stipple_cache` | fixed: removed; the stipple of a screen is kept in its display's context table (it was found again by a later display at the same address) |
| IconFile.c | `iconPath`, `bmPath`, `iconNameCache`, `cacheList` | locked; process-wide by design (paths from the environment) |
| IconG.c | `dummy` | fixed: thread (an `XFindContext` output) |
| ImageCache.c | `image_set`, `pixmap_set`, `pixmap_data_set`, `gc_set` | fixed: created once under the lock (two threads could both create them), looked up and filled under one lock in `_XmGetScaledPixmap` (entries were published before their pixmap was drawn), GC entries added once complete; when a screen goes its pixmaps are freed whatever their reference count |
| ImageCache.c | `colorCacheList`, `firstTime` | fixed: used under the lock held by `_XmGetScaledPixmap`; the colours of a display are forgotten when its screen goes |
| ImageCache.c | `built_in_image` | fixed: under the lock held by `_XmGetScaledPixmap` (its data pointer changes per call) |
| Label.c | `default_parsed`, `menu_parsed` | class-init |
| LabelG.c | `freeExtData`, `numFreeExtData` | locked |
| LabelG.c | `local_cache`, `local_cache_inited` | fixed: thread (`_XmAssignLabG_*` collect changes for the next `_XmReCacheLabG` of the same thread) |
| Log.c | `_log_domains`, `_log_domains_count`, `_log_domains_allocated`, `_print_cb`, `_print_cb_data`, `_log_level`, `_threads_enabled`, `_threads_inited`, `_main_thread` | fixed: the lock macros were empty; now one recursive pthread mutex (`_log_mutex`, made once through `_log_mutex_once`) |
| Log.c | `XM_LOG_DOMAIN_GLOBAL` (exported) | written by `XmLogInit` under the lock; read by applications without it (call `XmLogInit` before starting threads) |
| MapEvents.c | `modifierStrings`, `buttonEvents`, `keyEvents`, `initialized` | once |
| MenuProc.c | `menuProcEntry` | class-init (RowColumn) |
| MenuProc.c | class `tm_table` switched per widget | fixed: the process lock is held from `_XmSaveCoreClassTranslations` to `_XmRestoreCoreClassTranslations` |
| MenuShell.c | `check_set_save`, `check_set_offset1`, `check_set_offset2` | fixed: thread (state between the resource defaults of one widget, which Xt fetches in one thread but locks per resource) |
| Obso1_2.c | `rects`, `rect_count` | locked (previous round) |
| Obso1_2.c | `_XmMenuCursorContext`, `_XmTextEventBindings*` (exported) | read-only |
| Obso2_0.c | `XmCOLOR_*_THRESHOLD`, `XmTHRESHOLDS_INITD`, `default_set*`, `background` | fixed: locked, Xt conversions done outside the lock; `background` local |
| Obso2_0.c | `first_time`, `unitQ` (_XmSortResourceList) | fixed: the quark is looked up per call |
| Paned.c | `def_pos`, `params` | read-only |
| PixConv.c | `inited` | locked |
| Protocols.c | `allProtocolsMgrContext` | once |
| RCHook.c | `mono`, `color`, `colorPrim`, `init`, `screen` | fixed: locals.  They held the first menu bar's ColorObj values (and screen, gone with its display) for every menu bar |
| RCMenu.c | (SIF_ErrorHandler) | fixed: error trap |
| RCPopup.c | `popup_table`, `lasttarget` | locked |
| RepType.c | `DynamicRepTypes`, `DynamicRepTypeNumRecords` | locked (the converters run under Xt's lock) |
| ResConvert.c | `sFontLists`, `nsFontLists`, `maxnsFontLists` | fixed: moved to file scope; an entry is released when its XmDisplay is destroyed (a later display at the same address got the old fonts); it was also stored only when the table grew |
| ResConvert.c | `registered` | locked |
| ResEncod.c | `_encoding_registry_ptr`, registries | locked (registries read-only) |
| Scale.c | `null_region` | class-init |
| Screen.c | quarks | class-init |
| Simple.c | `SimpleMenuResources` | fixed: `_XmGetSubresources` |
| TabBox.c | `rect` (GetTabRectangle) | fixed: the callers pass a rectangle (and a NULL geometry no longer dereferences NULL) |
| TabBox.c, TabStack.c | translation strings, bitmaps, `tab_stack_filter`, `XiCosSinData` | read-only |
| Text.c | `nullsource`, `nullsourceptr` | class-init; read-only |
| Text.c | `context` (_XmCreateCutBuffers) | once |
| Text.c | `tell_output_force_display` | fixed: thread |
| TextOut.c | `posToXYCachedWidget`, `posToXYCachedPosition`, `posToXYCachedX`, `posToXYCachedY` | locked |
| TextSel.c, TextFSel.c | `prim_select` | fixed: per widget, in a context of the widget's display (one record for the process was overwritten by a transfer to any other widget) |
| TextSel.c, TextFSel.c | `insert_select` | locked by protocol: only one secondary transfer runs at a time (Transfer.c `secondary_lock`) |
| Trait.c | `TraitSlots`, `TraitMask`, `TraitInUse`, `TraitFilled`, `TraitTombstone`, `initialized` | locked; quarks class-init (traits work is in its own stream) |
| Transfer.c | `local_convert_flag`, `TB_internal` | fixed: thread |
| Transfer.c | `secondary_lock`, `secondary_event` | fixed: the lock is taken in the same critical section as the test |
| Transfer.c | `old_serial` | locked; process-wide |
| Transfer.c | `global_tc`, `free_tc` | fixed: unlinking was partly outside the lock |
| Transfer.c | `ConvertHashTable` | fixed: looked up and added under one lock (was published uninitialized in between) |
| Transfer.c | `DataIdDictionary` | locked |
| Transfer.c | `SIF_ErrorFlag` | fixed: removed, error trap |
| TraversalI.c | `SortReferenceGraph` | locked (qsort comparator context, set and used under the lock) |
| VendorS.c | `destroy_list`, `destroy_list_size`, `destroy_list_cnt` | locked |
| VendorS.c | `_XmDisplayHandle` | fixed: with `liveDisplays`, `numLiveDisplays` (locked) and `threadDisplay` (thread): `_XmGetDefaultDisplay` returns the display this thread last created a shell on while its XmDisplay exists, else the latest live one; it used to be cleared by the first XmDisplay's destruction only, whatever it held |
| VendorS.c | `previousWarningHandler` | locked; Motif's warning handler is installed in the first application context only (Xt's handlers are per context), as before |
| VendorS.c | `_XmVersionString`, `default_unspecified_shell_int` | read-only |
| VirtKeys.c | binding tables | read-only |
| Xm.c | `NumLockMask`, `ScrollLockMask` (exported) | never written by the library (only by the exported `_XmInitModifiers`, which nothing calls) |
| Xm.c | `iso10646_atoms` | locked; dropped when the display closes |
| Xm.c | `threadErrorTraps` | thread |
| Xm.c | `trapPreviousHandler`, `numErrorTraps` | atomic; locked |
| XmExtUtil.c | `pixmapCache` | fixed: removed with the two unused static functions using it |
| XmExtUtil.c | `xm_std_filter`, `xm_std_constraint_filter`, `pixmap_bits` | read-only |
| XmIm.c | `XmImResList` | locked |
| XmRenderT.c | `quarks`, `num_quarks`, `found`, `table`, `QString`, `Qfont` (GetResources) | locked |
| XmRenderT.c | `CVTtransfervector`, `CVTtvinited` | locked |
| XmRenderT.c | `_XmXftDisplays` | locked; dropped when the display closes |
| XmRenderT.c | render table and rendition reference counts | fixed: the copy and add functions changed them under the application lock only, the free functions under the process lock only; all now hold the process lock |
| XmString.c | `_tag_cache`, `_cache_count`, `_cache_size`, `_old_tag_caches` | fixed: the array never moves (a larger copy replaces it, the old ones are kept), since `_XmEntryRendBegins` and friends return pointers into it used without the lock; the pointer is atomic; locked otherwise |
| XmString.c | `locale`, `cache_str`, `str`, `opt_str` (two), `rend`, `default_dir_pattern`, `table` | locked |
| XmTabList.c | `quarks` (_XmCreateTab) | fixed: per call (were set without a lock, a reader could see some of them) |
| Xmos.c | `dirCacheName`, `dirCacheNameLen`, `dirCache`, `numCacheAlloc`, `numCacheEntries` | locked |
| Xmos.c | `homeDir`, `empty` | once |
| Xmos.c | `method_table`, `_XmSDEFAULT_*` | read-only |

### libMrm

Every public Mrm function (`MrmOpenHierarchy*`, `MrmCloseHierarchy`,
`MrmRegisterNames*`, `MrmRegisterClass*`, `MrmFetch*`, `MrmInitialize`)
holds the process lock (and the application lock when it has a
display) for the whole call, so the statics below are only reached under
it.  The one exception was `DisplayDestroyCallback` (Mrmwcrw.c), run
when an XmDisplay is destroyed, which looked up and removed a name in
the shared name table without the lock: fixed.

| File | Static | Verdict |
|------|--------|---------|
| MrmIbuffer.c | `idb__buffer_pool_vec`, `idb__buffer_activity_count`, `idb__buffer_pool_size` | locked |
| Mrmerror.c | `urm__latest_error_code`, `urm__latest_error_msg`, `urm__err_out` | locked; process-wide (the last error of any thread) |
| Mrmerror.c | `urm_codes_codstg`, `urm_codes_invalidcode` | read-only |
| Mrmhier.c | `uidPath`, `uidSubs`, `first` | locked |
| Mrminit.c | `urm__initialize_complete` | locked |
| Mrmwci.c | `cldesc_hash_inited`, `cldesc_hash_table`, `hash_hash_inited`, `hash_az_hash_table`, `wci_cldesc_list` | locked (fixed in `DisplayDestroyCallback`) |
| Mrmwcrw.c | `staticNull`, `urm__cw_tree_nodes` | locked |

## X errors

Xlib's error handler is process-wide.  Code that installs its own
around a request and restores the previous one afterwards breaks with
threads: the handlers are restored out of order (the application's
handler can be lost for good) and one thread's errors reach the other
one's handler.  `_XmStartErrorTrap` installs one handler while any
thread has a trap; it keeps an error only when the thread reading it
has a trap for that display, for a request made since the trap started
(and of the error code and resource asked for, if any), and passes any
other error to the handler that was installed before.  With Xt's
locking, the thread that reads a reply is the one holding the
application lock, which made the request.

## Limits

- Interfaces without a display argument (`XmCvtXmStringToCT`,
  `XmCvtCTToXmString`, the unit conversions, XmString drawing and
  measuring with a render table made without a display, `MrmOpenHierarchy`
  without a display, the obsolete colour and image functions) use
  `_XmGetDefaultDisplay`: the display the calling thread last created a
  shell on.  A thread that uses such an interface before creating any
  shell gets another thread's display.
- Converters and resource defaults return pointers to static buffers,
  as Xt's own do; use them through Xt.
- Only one secondary selection transfer runs at a time in a process.
- The clipboard protocol (a lock property on the root window) is not
  atomic between clients, whether they are threads or processes.
- libXt compiles a resource list in place under the application lock
  only; Motif's own lists go through `_XmGetSubresources`, but an
  application's static lists given to `XtGetSubresources` from two
  application contexts have the same problem.
