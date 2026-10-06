# 0. History of Motif Development

**Scope.** This chapter traces the Motif toolkit from the windowing
systems and vendor toolkits of the mid-1980s that it was assembled
from, through its years as the commercial Unix standard, its two
licensing crises, its maintenance by Integrated Computer Solutions
(ICS), its release under the LGPL in 2012, the end of upstream
development in 2017, and the community trees that continue it, this
one among them.  It explains *why* the toolkit is shaped the way the
rest of this guide describes: why it has a window manager and a
description language, why its keys are "virtual", why it looks like
Presentation Manager, why GTK exists, and why the sources of 2026 still
carry a 1989 copyright.

**On evidence.** The facts below were consolidated from publicly
available sources in October 2026 (release archives, press releases,
standards records, period manuals, mailing-list and news archives,
project repositories, and encyclopaedia articles).  Three grades of
support are distinguished in the text and in the reference list:
**primary** (a release artefact, repository record, standard or press
release read directly), **secondary** (a contemporaneous report or a
reference work, such as the Motif FAQ, LWN, Linux Journal or
Wikipedia), and **unverified** (claims found only in a single weak
source).  Where sources disagree, §0.13 says so.  Nothing in this
chapter should be quoted as established without checking the cited
source.

---

## 0.1 Prehistory: X, Xt and the look-and-feel market (1984-1988)

**The X Window System.** X was conceived at MIT in 1984 by Jim Gettys
(Project Athena, the DEC/MIT/IBM programme that needed a vendor-neutral
window system for a campus of heterogeneous workstations) and Bob
Scheifler (MIT Laboratory for Computer Science); the asynchronous
protocol of "X version 1" dates from June 1984 [W1].  X10 was the
first version in wide use.  X11, a redesign proposed by Smokey Wallace
and Gettys and built at DEC's Western Software Laboratory from 1986,
was released in September 1987 under the same permissive terms as its
predecessors, which is the single most important fact in this history:
X was *free*, and every company could build a toolkit on it [W1, X1].
The MIT X Consortium took over the releases in January 1988 [W1].

**The X Toolkit Intrinsics.** X11R2 (March 1988) was the first release
to carry the Xt Intrinsics, designed primarily by Joel McCormack of DEC
WSL with Ralph Swick of Project Athena, Charles Haynes, Mike Chow and
Paul Asente, and the Athena widget set (Xaw) by Swick and Chris
Peterson [X2, X3].  Xt is the object system described in chapter 1.1;
both of the toolkits that Motif was made from were Xt toolkits, and so
is Motif.  The same Xt acknowledgements credit later
internationalisation work to "Ellis Cohen, Daniel Dardailler and Vania
Joloboff of OSF", the engineers who would build Motif 1.2 [X2].

**The vendor toolkits.** By 1988 every workstation vendor had a GUI in
the field or in development:

- *Hewlett-Packard* contributed "the HP Widgets" to the MIT X tape and
  had a window manager, `hpwm`, with a three-dimensional look [X4, W2].
  The bevelled, light-from-the-top-left visual style was the work of
  Shiz Kobara, a visual designer in HP's User Interaction Design
  department from 1987 [K1].  HP also had *NewWave*, an object-oriented
  desktop on Windows 2.0 (1988), and, at Apollo, the beginnings of the
  Visual User Environment (VUE) that would later feed CDE [W3, W4].
- *Digital Equipment Corporation* announced the DECwindows programme in
  January 1987 with the *XUI* (X User Interface) toolkit and style,
  meant to be consistent across VMS and Ultrix [D1].  XUI introduced
  the *User Interface Language* (UIL), "a particularly important
  supplement to the X Window System" that let a programmer describe an
  interface declaratively and load it at run time through a resource
  manager [D2].  DECwindows shipped in VAX/VMS V5.1 in 1989 [W5].
- *Sun and AT&T* announced the *OPEN LOOK* specification in April 1988,
  implemented by OLIT (on Xt) and XView [W6].
- *HP and Microsoft* jointly built, for Unix and X, an implementation
  of *Presentation Manager*, the OS/2 GUI that followed IBM's Common
  User Access (CUA) guidelines.  The project had two products: *CXI*
  (Common X Interface), a toolkit and window manager, and *PM/X*, the
  PM API on Unix [W7].

The commercial stake was the Unix desktop standard, and the design
question of the day was whether Unix GUIs should behave like the PC
GUIs that developers and users already knew.  Motif's answer, "yes,
like Presentation Manager", was a deliberate bid for that developer
base [G1].

## 0.2 The Unix wars and the Open Software Foundation (1988)

In 1987-88 AT&T and Sun allied to merge System V, BSD and Xenix into
System V Release 4, with Sun's OPEN LOOK as its GUI.  Their competitors
read the alliance as an attempt to own Unix.  In May 1988 Apollo, Bull,
DEC, HP, IBM, Nixdorf and Siemens, the "Gang of Seven", formed the Open
Software Foundation (OSF), a not-for-profit joint venture to produce an
alternative operating system (OSF/1) and user environment; AT&T and Sun
answered with Unix International [W8, W9, L1].  OSF grew to over 300
employees within two years; Roger Gourd, formerly of DEC, was its
Vice-President of Engineering [W8].

OSF's method was not to design from scratch but to run a *Request For
Technology* (RFT): solicit existing technologies, then adopt one or
combine several [W10].  Motif was the first product that process
delivered; OSF/1 itself did not ship until October 1990 [G1, T1].

## 0.3 The Request for Technology and the making of Motif (1988-1989)

Forty products were submitted to the user-environment RFT and
twenty-three were short-listed, including OPEN LOOK, XUI, CXI and PM/X
[W10].  On December 30, 1988 OSF announced that its user environment
would be based on DEC's toolkit technology combined with "the joint
Hewlett-Packard/Microsoft submission of H-P's 3-D appearance and
Microsoft's Presentation Manager-compatible behavior (window manager)",
to be merged by "a joint team of OSF, DEC and HP engineers" [G1, E1].
In the usual summary: the *look* (HP's bevels), the *window manager*
(from hpwm/CXI) and the *CUA-compatible behaviour* (Microsoft's
licence of PM semantics) came from the HP/Microsoft side; the *widget
API* and *UIL/Mrm* came from DEC's XUI [W10, W5].  PM/X was rejected in
favour of the XUI API, and HP and Microsoft continued PM/X separately
for a while before abandoning it [W7].  One hobbyist timeline dates the
announcement of the name "OSF/Motif" to January 11, 1989; no source
explains the choice of the name [U1].

The people who can be named from the sources: Shiz Kobara (HP, visual
design); Roger Gourd (OSF engineering); Ellis Cohen (OSF, May 1989 to
December 1994), Vania Joloboff (head of Motif development at the OSF
Research Institute) and Daniel Dardailler (OSF, later W3C), the OSF
engineers credited in the Xt and X11R6 acknowledgements [K1, P1, P2,
X2].  IBM, a founding member, shaped the behaviour through CUA rather
than through code that the sources identify.

**Motif 1.0** shipped in 1989, most probably in September: HP's
*OSF/Motif Programmer's Guide* for the HP 9000 is dated September 1989,
and the Motif Window Manager article gives the same month [B1, W2].
The release contained the toolkit (libXm), UIL and Mrm, mwm, and the
first *OSF/Motif Style Guide* (Revision 1.0, Prentice Hall 1990), the
document that made Motif a *look and feel* and not only a library
[B2].  The CUA inheritance is visible to this day in the virtual
keysyms `osfActivate`, `osfCancel`, `osfHelp` and `osfMenuBar`
(chapter 3): the Style Guide specified behaviour in terms of these
abstract keys so that PM semantics could be mapped onto every vendor's
keyboard [M1].

## 0.4 Motif 1.1 and 1.2: the standard takes shape (1990-1995)

| Release | Date | Content | Sources |
|---------|------|---------|---------|
| 1.1 | 1990-91 (month not found) | Rebased on X11R4; Style Guide revision 1.1 | [F1, B3] |
| 1.2.0 | April 1992 | Based on X11R5; internationalisation (text input and output, locale resources, compound text); drag and drop; tear-off menus; binary compatibility with 1.1 applications; Style Guide revision 1.2 (September 30, 1992) with the Level One certification checklist | [F1, O1, B4] |
| 1.2.1, 1.2.2, 1.2.5 | Sept 1992, March 1993, June 15, 1995 | Bug-fix releases; 1.2.5 became the base of Solaris 2.6's libXm | [F1, S1] |

Motif 1.2 is the release that most of the toolkit's lifetime
applications were written against, and three of its mechanisms are the
subject of later chapters: the drag and drop protocol (chapter 6.2),
the internationalised text widgets (chapter 6.1) and the virtual
bindings (chapter 3).

**Standardisation.** The Motif API became IEEE Std 1295-1993,
"X Window System, Modular Toolkit Environment", published in 1993 and
withdrawn on March 6, 2000, when the IEEE stopped maintaining it [I1,
I2].  X/Open published the CAE Specification C320, *Motif Toolkit
API*, in March 1995, later re-issued by The Open Group as a Technical
Standard "aligned with IEEE 1295" [TO1].  Motif was thus an *open
standard* with a single *proprietary implementation*, a combination
that defined its next decade.  By 1993 OSF was reporting a million
copies shipped and twenty-nine certified implementations, figures that
come from OSF's own news posts and could not be independently checked
[U2].

## 0.5 COSE, CDE and the vendors (1993-1997)

In March 1993 HP, IBM, SCO, Sun, Unix System Laboratories and Univel
formed the Common Open Software Environment (COSE) to end the Unix
wars by converging on common specifications, and in June 1993 HP, IBM,
SunSoft and USL announced the *Common Desktop Environment* (CDE),
"based on the Motif widget toolkit"; Sun agreed to phase out
OpenWindows, which ended the GUI war in Motif's favour [W11, W12, W6].
HP donated much of VUE to CDE; CDE 1.0 shipped in 1995 (Novell's
UnixWare version was announced in March 1995) and The Open Group's CDE
2.1 of February 1997 was its last major release [W4, C1, N1, TO2].

Every commercial Unix then shipped Motif: HP-UX (VUE 3.0 in 1992, then
CDE), AIX, Digital UNIX/Tru64 and VMS (DECwindows Motif replaced XUI in
August 1991, and mwm became the default DECwindows window manager in
OpenVMS 6.0), SGI IRIX (the Indigo Magic desktop, with SGI's own
*Motif 2.1 Porting Guide*), Solaris from 2.6 (1997), SCO and IXI's
X.desktop [W5, W13, S1, S2].  A 1998 survey called Motif "the single
most widely used toolkit in the Unix world" and noted that users
confused Motif with the GUI itself; The Open Group's Motif 2.1 data
sheet claimed more than 200 hardware and software platforms [R1,
TO3].  CDE's `dtwm` is a descendant of mwm, and CDE's own widgets
(`DtEditor`, `DtHelp`, ...) are Motif subclasses, which is why this
tree still carries stub `Dt/` headers and keeps exported the `_Xm`
functions that CDE's sources declare by hand (chapter 1,
[doc/abi-policy.md](../abi-policy.md)).

## 0.6 Motif 2.0 and 2.1: the technical leap, and The Open Group (1994-1997)

OSF announced **Motif 2.0** on June 21, 1994 and shipped it in August
1994 [F2].  It is the release that produced most of the architecture
this guide describes:

- new widgets: `XmContainer`, `XmNotebook`, `XmIconGadget`,
  `XmSpinBox`, `XmComboBox`, `XmCSText` (dropped again in 2.1), a
  thermometer-style `XmScale` [F2, F3];
- the *Uniform Transfer Model*: one API (`XmTransferValue`, the
  `XmQTtransfer` trait) for primary and secondary selection, clipboard
  and drag and drop [F3];
- *render tables* (`XmRenderTable`, `XmRendition`), replacing font
  lists, with colours, underlining and tab lists for multi-column
  lists [F3];
- *traits*, the interface mechanism of chapter 1.2, written because
  the new widgets needed to cooperate with children of unknown class
  [F3].

OSF and X/Open merged in 1996 to form **The Open Group**, which has
owned the Motif copyright and trademark since [W14, W8].  Its first
release was **Motif 2.1**, announced March 6, 1997: thread-safe
libraries (the `_XmAppLock`/`_XmProcessLock` discipline of chapter 1),
X11R6 session management and ICCCM compliance, and the `XmPrintShell`
print widget for the X Print Server [TO4].  The CDE 2.1 release of the
previous month unified CDE with Motif 2.1 [TO2].

## 0.7 Licensing, and the free-software response (1994-2000)

Motif was never free.  OSF and The Open Group sold *source* licences to
vendors and collected *per-copy royalties* on binaries.  The Open
Group's 1997 price list, as summarised from the archived page, gave
US$17,000 for a source licence with full distribution rights, US$9,500
for developer distribution rights, US$2,000 for limited rights, and
US$40 per floating user licence in object-code royalties [TO5].  On
Linux, where no vendor paid the fee, Motif was sold separately: Metro
Link's "Motif Complete" and SWiM at US$149, "Red Hat Motif 2.1"
(announced March 2, 1998, US$149, sub-licensed from Metro Link), and
Xi Graphics' CDE desktops at US$199.95 [LJ1, LJ2, RH1, LJ3].

The consequences for free software were immediate:

- **LessTif** (1994, Chris Toshok and The Hungry Programmers) was an
  LGPL re-implementation that "aims to be source compatible" with
  Motif 1.2 and later 2.1; "the license of Motif was the main
  motivation" [W15, LT1].  Richard Stallman records that LessTif
  "became powerful enough to support most Motif applications only in
  1997" [GNU1].  It carried Mozilla, nedit, xpdf and OpenDX on Linux
  for a decade, released its final version 0.95.2 on May 27, 2009, and
  recommends the real Motif sources since 2012 [LT2, LT1].
- **GTK** exists because of Motif: GIMP 0.54 (January 1996) required
  Motif 1.2, whose licence made distribution "to a lot of users
  impossible", so Peter Mattis wrote the GIMP Toolkit in July 1996; GTK+
  1.0 followed in April 1998 and GNOME chose it specifically to avoid
  Motif [GIMP1, RH2].  Qt's first release (May 1995) and KDE's choice
  of it, and Tk's Motif-like look, complete the picture of a Unix
  desktop that routed around the proprietary standard [W16].

**Open Motif.** On May 15, 2000 The Open Group released Motif 2.1.30
as "Open Motif" under the *Open Group Public License*, which allowed
royalty-free use and redistribution only on operating systems that were
themselves open source [LWN1, SD1].  The Open Group's own FAQ admitted
that this failed clause 8 of the Open Source Definition ("License Must
Not Be Specific to a Product"); the Free Software Foundation's
statement *The Motif License* found it "neither free software nor open
source", objected to the shrink-wrap acceptance and the platform
restriction, and concluded that linking GPL programs with it still
violated the GPL [TO6, GNU2].  Open Motif did get onto distribution
CDs, as LWN predicted, but in *non-free* sections: Debian kept it there
until 2012, Fedora dropped it in 2006 [LWN1, DEB1, LWN2].

## 0.8 ICS and Open Motif 2.2 and 2.3 (1998-2010)

In 1998 The Open Group chose Integrated Computer Solutions (ICS), the
makers of the Builder Xcessory GUI builder, to support its Motif source
licensees; ICS ran the MotifZone developer site, forums, binaries and
Bugzilla, and became the de-facto maintainer [ICS1].

**Open Motif 2.2.0** (January 28, 2002) merged the ICS extension widget
set into the toolkit: `XmButtonBox`, `XmColorSelector`, `XmColumn`,
`XmDataField`, `XmDropDown`, `XmFontSelector`, `XmIconBox`,
`XmIconButton`, `XmMultiList`, `XmOutline`, `XmPaned` and
`XmTabStack`, plus tool tips [RN1].  These are the classes in
chapter 1's taxonomy whose sources keep a different coding style, and
`XmDataField`, originally a *copy* of `XmTextField`, is the subject of
chapter 3.1.

**Open Motif 2.3.0** (beta November 2005; release May 2007, announced
June 7, 2007) brought the toolkit into the FreeType era: anti-aliased
client-side fonts through Xft (`XmFONT_IS_XFT`), UTF-8 and the
`UTF8_STRING` target, text-plus-pixmap labels, and PNG and JPEG images
handled like XPM [ICS2, OSN1].  Chapter 5 describes the Xft rendering
path that this added.  2.3.1 (September 2008), 2.3.2 (March 2009) and
2.3.3 (March 2010) were bug-fix releases, all still under the Open
Group Public License and headed jointly "The Open Group and Integrated
Computer Solutions" [ICS3].  The 2.1 branch received maintenance
releases 2.1.31 (2004) and 2.1.32 (2005) for CDE users [U3].

## 0.9 LGPL (2012) and the 2.3.x tail (2012-2017)

The relicensing came from the CDE side.  Peter Howkins campaigned for
years, with a petition, for The Open Group to open-source CDE and
Motif; on August 6, 2012 The Open Group released CDE under the LGPL
and handed it to a community led by Howkins and Jon Trulson [REG1,
CDE1].  On **October 23, 2012** ICS and The Open Group published
**Motif 2.3.4** on SourceForge under the **LGPL version 2.1**, as "a
major bug fix release" with more than forty fixes [SF1, ICS4].  The
licence that had kept Motif out of free systems for twenty-three years
was gone; LessTif lost its reason to exist, Debian moved `motif`
2.3.4 into main and removed `lesstif2` in 2013, and Fedora 19 shipped
`motif-2.3.4` [W15, DEB1, DEB2].

Four more upstream releases followed, all on SourceForge: 2.3.5
(March 17, 2016: a reimplemented drop-down list, List crash fixes),
2.3.6 (June 11, 2016: rendering during scrolling, parallel builds),
2.3.7 (March 27, 2017: option menu and popup crashes) and **2.3.8
(December 5, 2017)**, seven fixes including TextField cursor and
XQuartz issues, tested on Fedora 15 and Solaris 10 [SF1, SF2].  After
2.3.8 the upstream repository shows three commits by Oleksii
Chernyavskyi of ICS, the last on February 16, 2023 updating the bundled
Xpm code to 3.5.12, and no release [SF3].  The bug tracker named in the
2.3.8 release notes, `bugs.motifzone.org`, is reported by later
maintainers as gone [SF2, GH1].

## 0.10 After upstream: community trees and this one (2017-2026)

With no upstream, maintenance moved to individual trees, each
starting from the 2.3.8 sources:

- **Jon Trulson** (the CDE maintainer) forked the SourceForge tree on
  GitHub in August 2022 "because ICS MotifZone isn't really
  maintaining this any more", keeping the original in a `motifzone`
  branch [GH2].
- **Tim Hentenaar**'s fork (July 2025) describes itself as actively
  maintained and adds Unicode support for `XmString`, Xcursor with SVG
  and PNG cursors, Xrandr and Xinerama, transparent XDND, a Mesa GL
  drawing area, and collects dormant upstream and Gentoo fixes [GH1].
- **Alexander Pampuchin** maintains **EMWM**, the Enhanced Motif Window
  Manager (from November 2020; version 2.1 in August 2026), a fork of
  mwm with Xinerama/Xrandr, Xft fonts, workspaces and EWMH, packaged in
  FreeBSD ports and NetBSD pkgsrc [GH3, FC1].
- **This tree** (Dmitri Chudinov, from August 2025) continues 2.3.8 as
  Motif 2.5.0 (October 4, 2026), with the modernisation summarised in
  chapter 1 §1.8: a CMake build, a security review of every parser of
  untrusted data, exported-symbol version scripts and SONAME 5, a test
  suite, fuzzers and CI, and the performance work of chapters 3 and 5
  [LOC1].  Its [AUTHORS](../../AUTHORS) file credits the Hentenaar and
  Pampuchin trees and Olivier Fourdan's work as sources it draws on.
- CDE itself continues at SourceForge (2.5.3 in September 2026), and
  NsCDE reproduces the CDE look on FVWM without Motif code [CDE2, GH4].

**Security.** Motif has always carried its own copy of the XPM image
library (`src/lib/Xm/Xpm*.c`, by Arnaud Le Hors of Groupe Bull), so
every libXpm vulnerability is a libXm vulnerability.  X.Org's advisory
of January 17, 2023 (CVE-2022-46285, an infinite loop on an unclosed
comment; CVE-2022-44617, a runaway loop on a zero-width image;
CVE-2022-4883, running `uncompress`/`gunzip` found through `$PATH`) and
of October 3, 2023 (CVE-2023-43788 and CVE-2023-43789, out-of-bounds
reads) apply to it, and Ubuntu's tracker lists all of them, with two
2026 XPM issues and the 2005 libUil overflow CVE-2005-3964, against the
`motif` package [XO1, XO2, UB1].  Upstream's only response was the 2023
update to Xpm 3.5.12, which predates both fixed versions [SF3].  This
tree's [SECURITY.md](../../SECURITY.md) lists the five XPM CVEs as fixed
and describes the broader review that followed from the same threat
model: any X client on the display can feed Motif data.

**Motif today.** It is the toolkit of xpdf's classic versions, NEdit
and its continuation XNEdit, Grace, DDD, old BRL-CAD and GNU Electric,
and of scientific, control-system and EDA software that was written
once and must keep running [ICS5, FB1].  On Wayland desktops Motif
programs run through Xwayland; there is no native Wayland backend, and
claims that Wayland "implements Xlib" are wrong [AW1].  The
2024 OSNews argument that "there's a market out there for a modern
X11/Motif-based desktop distribution", made on the strength of EMWM,
is the most optimistic recent assessment [OSN2].

## 0.11 Technical lineage

Where the parts of the toolkit described in this guide came from:

| Component | Origin | Chapter |
|-----------|--------|---------|
| Xt object model, resources, translations | X Consortium / DEC WSL, X11R2 (1988) | 1.1, 1.3 |
| 3-D bevelled look, shadow drawing | HP (Shiz Kobara, 1987-89), hpwm and the HP widgets | 2 |
| Window manager (mwm) | HP's hpwm via the HP/Microsoft CXI submission | 1 |
| CUA-style behaviour: keyboard traversal, tab groups, virtual keys, menu and dialog semantics | Microsoft Presentation Manager / IBM CUA via CXI; specified in the Style Guide | 3 |
| Widget API, UIL, Mrm, UID files | DEC XUI (DECwindows, 1987-89) | 1, 1.3 |
| Internationalised text, drag and drop, tear-off menus | OSF, Motif 1.2 (1992) on X11R5 | 6.1, 6.2 |
| Traits, Uniform Transfer Model, render tables, Container/Notebook/ComboBox/SpinBox, class extensions and hooks | OSF, Motif 2.0 (1994) | 1.2, 3.3, 4, 5 |
| Thread safety, print shell, X11R6 session management | The Open Group, Motif 2.1 (1997) | 1 |
| ButtonBox, ColorSelector, Column, DataField, DropDown, FontSelector, IconBox, IconButton, MultiList, Outline, Paned, TabStack, tool tips | ICS extension widgets, Open Motif 2.2 (2002) | 1, 3.1 |
| Xft fonts, UTF-8, PNG and JPEG | ICS, Open Motif 2.3 (2007) | 5 |
| SVG images (nanosvg) | Post-2.3.8 community work | 5 |
| CMake build, security review, symbol versioning, tests, performance work, DataField as a subclass | This tree, 2.4 and 2.5 (2025-26) | 1, 3.1, 3.2, 5.1, 6.1 |

## 0.12 Condensed timeline

| Date | Event |
|------|-------|
| 1984-06 | X Window System begins at MIT |
| 1987-01 | DEC announces DECwindows and XUI, with UIL |
| 1987-09 | X11R1 |
| 1988-03 | X11R2: Xt Intrinsics and Athena widgets |
| 1988-04 | Sun and AT&T announce OPEN LOOK |
| 1988-05 | OSF founded by Apollo, Bull, DEC, HP, IBM, Nixdorf, Siemens |
| 1988-12-30 | OSF picks DEC's toolkit plus HP/Microsoft's look, window manager and behaviour |
| 1989 (Sept) | Motif 1.0 with mwm; Style Guide 1.0 |
| 1990-91 | Motif 1.1 on X11R4 |
| 1991-08 | DECwindows Motif replaces XUI on VMS |
| 1992-04 | Motif 1.2: i18n, drag and drop, tear-off menus |
| 1993 | IEEE 1295-1993; COSE formed (March); CDE announced (June) |
| 1994-08 | Motif 2.0: traits, UTM, render tables, new widgets |
| 1994 | LessTif begins |
| 1995-03 | X/Open C320 *Motif Toolkit API*; CDE 1.0 |
| 1996 | OSF and X/Open merge into The Open Group; GTK written for GIMP |
| 1997-03-06 | Motif 2.1: thread-safe, X11R6, print shell |
| 1998 | ICS becomes The Open Group's Motif support partner; Red Hat Motif 2.1 at US$149 |
| 2000-03-06 | IEEE 1295 withdrawn |
| 2000-05-15 | Open Motif 2.1.30 under the Open Group Public License |
| 2002-01-28 | Open Motif 2.2.0 with the ICS widgets |
| 2007-05/06 | Open Motif 2.3.0: Xft, UTF-8, PNG, JPEG |
| 2008-2010 | 2.3.1, 2.3.2, 2.3.3 |
| 2009-05-27 | LessTif 0.95.2, final release |
| 2012-08-06 | CDE released under the LGPL |
| 2012-10-23 | Motif 2.3.4 under LGPL 2.1 on SourceForge |
| 2013 | Debian and Fedora replace lesstif/openmotif with motif |
| 2016-03-17, 2016-06-11, 2017-03-27 | 2.3.5, 2.3.6, 2.3.7 |
| 2017-12-05 | Motif 2.3.8, the last upstream release |
| 2020-11 | EMWM started |
| 2022-08 | Trulson fork on GitHub |
| 2023-01-17, 2023-10-03 | X.Org libXpm advisories affecting Motif |
| 2023-02-16 | Last upstream commit (Xpm 3.5.12) |
| 2025-07 | Hentenaar fork |
| 2025-08-21 | This tree started |
| 2026-10-04 | Motif 2.5.0 |

## 0.13 Open questions and conflicting claims

1. **Motif 1.0's release month.** September 1989 is supported by HP's
   manual date and the Motif Window Manager article; one recollection
   says July.  No OSF press release was available.
2. **The naming announcement** of January 11, 1989 rests on a single
   hobbyist timeline; the origin of the name "Motif" was not found in
   any source.
3. **Motif 1.1's release date** was not found; manuals date from
   1990-91.
4. **IEEE 1295**: the standard is dated 1993; one summary says IEEE
   "adopted" Motif in 1994, probably the publication lag.
5. **The Open Group's formation month** (February 1996) comes from one
   reference site; others give the year only.
6. **Open Motif 2.1.31 and 2.1.32** dates (April 26, 2004; August 5,
   2005) come from search summaries of vendor pages.
7. **Licensing figures.** The 1997 price list is a search summary of
   the archived Open Group page; OSF-era figures such as "US$7,500 plus
   US$43 per copy" and the "600 companies in 25 countries" statistic
   appear only in unreadable Usenet archives or AI-generated pages and
   are not repeated here as fact.
8. **"One million copies by mid-1993"** and **"29 certified
   implementations"** are attributed to OSF news posts in an MIT
   archive that could not be read.
9. **Roger Gourd's role.**  His position as OSF's VP of Engineering is
   well attested; "directed the Motif effort" appears in
   encyclopaedia-derived text only.
10. **Olivier Fourdan's Motif tree**, credited in this repository's
    AUTHORS, could not be located publicly; his known work is on Xfce
    and Xwayland.
11. **A Motif-based Mathematica front end** is often mentioned and was
    not confirmed by any source found.
12. **The MotifZone bug tracker's** current status was not tested.

## References

Primary (read directly):

- [SF1] Motif project files on SourceForge, release dates 2.3.4 to 2.3.8: <https://sourceforge.net/projects/motif/files/>
- [SF2] Upstream release notes (RELNOTES, 2.3.4-2.3.8): <https://sourceforge.net/p/motif/code/ci/master/tree/RELNOTES>
- [SF3] Upstream git log, commits after 2.3.8: <https://sourceforge.net/p/motif/code/ci/master/log/>
- [LT1] LessTif home page: <https://lesstif.sourceforge.net/>
- [LT2] LessTif release notes: <https://lesstif.sourceforge.net/ReleaseNotes.html>
- [CDE2] CDE project on SourceForge: <https://sourceforge.net/projects/cdesktopenv/>
- [GH1] T. Hentenaar's Motif tree: <https://github.com/thentenaar/motif>
- [GH2] J. Trulson's Motif tree: <https://github.com/jontrulson/motif>
- [GH3] EMWM: <https://github.com/alx210/emwm>
- [GH4] NsCDE: <https://github.com/NsCDE/NsCDE>
- [UB1] Ubuntu security tracker, package `motif`: <https://ubuntu.com/security/cves?package=motif>
- [LOC1] This tree: [README](../../README.md), [CHANGELOG](../../CHANGELOG.md), [AUTHORS](../../AUTHORS), [SECURITY.md](../../SECURITY.md)

Secondary (contemporaneous reports, reference works, archived vendor pages):

- [W1] Wikipedia, *X Window System*: <https://en.wikipedia.org/wiki/X_Window_System>
- [W2] Wikipedia, *Motif Window Manager*: <https://en.wikipedia.org/wiki/Motif_Window_Manager>
- [W3] Wikipedia, *NewWave*: <https://en.wikipedia.org/wiki/NewWave>
- [W4] Wikipedia, *Visual User Environment*: <https://en.wikipedia.org/wiki/Visual_User_Environment>
- [W5] Wikipedia, *DECwindows*: <https://en.wikipedia.org/wiki/DECwindows>
- [W6] Wikipedia, *OPEN LOOK*: <https://en.wikipedia.org/wiki/OPEN_LOOK>
- [W7] Wikipedia, *Presentation Manager*: <https://en.wikipedia.org/wiki/Presentation_Manager>
- [W8] Wikipedia, *Open Software Foundation*: <https://en.wikipedia.org/wiki/Open_Software_Foundation>
- [W9] Wikipedia, *Unix International*: <https://en.wikipedia.org/wiki/Unix_International>
- [W10] Wikipedia, *Motif (software)*: <https://en.wikipedia.org/wiki/Motif_(software)>
- [W11] Wikipedia, *Common Open Software Environment*: <https://en.wikipedia.org/wiki/Common_Open_Software_Environment>
- [W12] Wikipedia, *Common Desktop Environment*: <https://en.wikipedia.org/wiki/Common_Desktop_Environment>
- [W13] Wikipedia, *OpenWindows*: <https://en.wikipedia.org/wiki/OpenWindows>
- [W14] Wikipedia, *The Open Group*: <https://en.wikipedia.org/wiki/The_Open_Group>
- [W15] Wikipedia, *LessTif*: <https://en.wikipedia.org/wiki/LessTif>
- [W16] Wikipedia, *Qt (software)*, *Tk (software)*: <https://en.wikipedia.org/wiki/Qt_(software)>, <https://en.wikipedia.org/wiki/Tk_(software)>
- [X1] X.Org release history: <https://www.x.org/wiki/Releases/History/>
- [X2] Xt Intrinsics specification, acknowledgements: <https://www.x.org/releases/X11R7.7/doc/libXt/intrinsics.html>
- [X3] T. Dickey, *Athena widgets* history: <https://invisible-island.net/athena_widgets/>
- [X4] comp.windows.x FAQ, "What are the HP Widgets?": <http://web.mit.edu/cdsdev/html/Faq/comp.windows.x/q.18>
- [K1] S. Kobara, "Designing the OSF/MOTIF Graphical User Interface", *Design Management Journal* 4 (1993): <https://onlinelibrary.wiley.com/doi/abs/10.1111/j.1948-7169.1993.tb00122.x>
- [D1] M. Good, "Developing the XUI Style": <https://michaelgood.info/publications/usability/developing-the-xui-style/>
- [D2] *Digital Technical Journal* vol. 2 no. 3 (1990), the DECwindows program: <https://vmssoftware.com/docs/dtj-v02-03-1990.pdf>
- [G1] GUIdebook, reprint of a period article on OSF/Motif: <https://guidebookgallery.org/articles/osfmotif>
- [E1] Encyclopedia.com, *OSF/Motif*: <https://www.encyclopedia.com/computing/dictionaries-thesauruses-pictures-and-press-releases/osfmotif>
- [L1] *Open Software Foundation v. USF&G*, 1st Cir. 2002 (recites OSF's formation): <https://caselaw.findlaw.com/court/us-1st-circuit/1002905.html>
- [T1] OSF/1 1.0 release, October 1990: <https://www.tech-insider.org/unix/research/1990/1023.html>
- [P1] V. Joloboff, colloquium biography: <https://cse.ucsd.edu/about/colloquium/2016-2017/vania-joloboff>
- [P2] E. Cohen, professional profile (OSF 1989-94): <https://www.linkedin.com/in/ellis-cohen-34366/>
- [B1] HP, *OSF/Motif Programmer's Guide*, September 1989: <https://bitsavers.org/pdf/hp/9000_hpux/x11/98794-90005_OSF_Motif_Programmers_Guide_Sep89.pdf>
- [B2] OSF, *Motif Style Guide* Rev. 1.0 (Prentice Hall, 1990), ISBN 0-13-640491-X
- [B3] OSF, *Motif Style Guide* Rev. 1.1 (1991), ISBN 0-13-640616-5
- [B4] OSF, *Motif Style Guide* Rev. 1.2 (1992), ISBN 0-13-643123-2; scan: <http://bitsavers.informatik.uni-stuttgart.de/pdf/openSoftwareFoundation/motif/OSF_Motif_Style_Guide_Revision_1.2_1993.pdf>
- [M1] *VirtualBindings(3)* manual page, also in [`doc/man/man3`](../man/man3): <https://manpages.debian.org/testing/libmotif-dev/VirtualBindings.3.en.html>
- [F1] Motif FAQ, "What are the Motif versions?": <https://users.polytech.unice.fr/~buffa/cours/X11_Motif/motif-faq/part1/faq-doc-17.html>
- [F2] Motif FAQ / comp.windows.x.motif, Motif 2.0 announcement: <https://groups.google.com/g/comp.windows.x.motif/c/uAUr8pi56HA>
- [F3] *Motif Programming Manual* vol. 6A, 2nd ed., chapters 3, 24, 25 (UTM, render tables, traits): <https://www.ist.co.uk/motif/books/vol6A/ch-24.fm.html>
- [O1] *Motif Programming Manual* vol. 6A, chapter 3 (Motif 1.2 features): <https://www.oreilly.com/openbook/motif/vol6a/Vol6a_html/ch03.html>
- [S1] Oracle, Solaris 2.6 Motif notes: <https://docs.oracle.com/cd/E19504-01/805-0037/6j03u2hqe/index.html>
- [S2] SGI, *Motif 2.1 Porting Guide* (007-3951-001): <https://irix7.com/techpubs/007-3951-001.pdf>
- [I1] IEEE Std 1295-1993: <https://ieeexplore.ieee.org/document/7362077>
- [I2] IEEE SA record (withdrawn 2000-03-06): <https://standards.ieee.org/ieee/1295/1969>
- [TO1] The Open Group, *Motif Toolkit API* (X/Open C320, March 1995): <https://pubs.opengroup.org/onlinepubs/009667999/toc.pdf>
- [TO2] The Open Group, CDE 2.1 press release (1997-02-05): <http://www.opengroup.org/tech/desktop/Press_Releases/cde2.1ga.htm>
- [TO3] The Open Group, Motif 2.1 data sheet: <http://www.opengroup.org/motif/motif.data.sheet.htm>
- [TO4] The Open Group, Motif 2.1 press release (1997-03-06): <http://www.opengroup.org/tech/desktop/Press_Releases/motif2.1ga.htm>
- [TO5] The Open Group, Motif 2.1 price list: <http://www.opengroup.org/tech/desktop/ordering/motif.price.list.htm>
- [TO6] The Open Group, Open Motif FAQ and licence: <https://archive.opengroup.org/openmotif/faq.html>
- [C1] CDE project wiki, Q&A: <https://sourceforge.net/p/cdesktopenv/wiki/QandA/>
- [N1] Novell, CDE 1.0 for UnixWare press release (March 1995): <https://www.novell.com/news/press/archive/1995/03/pr00020.html>
- [R1] "GUI toolkits: what are your options?" (1998): <https://www.linux.co.cr/desktops/review/1998/03-a.html>
- [LJ1] Linux Journal, Metro Link Motif: <https://www.linuxjournal.com/article/3211>
- [LJ2] Linux Journal, Red Hat Motif: <https://www.linuxjournal.com/article/3218>
- [LJ3] Linux Journal, Xi Graphics maXimum cde/OS: <https://www.linuxjournal.com/article/3249>
- [RH1] Red Hat press release, Red Hat Motif 2.1 (1998-03-02): <https://www.redhat.com/en/about/press-releases/press-motif>
- [RH2] M. Clasen, GTK+ history notes (GTK+ 1.0, April 1998): <https://people.redhat.com/mclasen/Usenix04/notes/x29.html>
- [GIMP1] GIMP prehistory (Motif dependency, birth of GTK): <https://www.gimp.org/about/prehistory.html>
- [GNU1] R. Stallman, *The GNU Project* (LessTif in 1997): <https://www.gnu.org/gnu/thegnuproject.html>
- [GNU2] FSF, *The Motif License*: <https://www.gnu.org/philosophy/motif.html>
- [LWN1] LWN, Open Motif licence, 2000-05-18: <https://lwn.net/2000/0518/bigpage.php3>
- [LWN2] LWN, Fedora drops openmotif (2006): <https://lwn.net/Articles/197744/>
- [SD1] Slashdot, "Motif Released To The Open Source Community", 2000-05-15: <https://tech.slashdot.org/story/00/05/15/1229207/motif-released-to-the-open-source-community>
- [ICS1] ICS, About MotifZone: <https://motif.ics.com/about/>
- [ICS2] ICS, "OpenMotif 2.3 released": <https://motif.ics.com/forum/openmotif-23-released/>
- [ICS3] ICS, release notes 2.3.0-2.3.3: <https://motif.ics.com/sites/default/files/release.txt>, <https://motif.ics.com/open-motif-233-release-notes>
- [ICS4] ICS, Motif 2.3.4 release notes: <https://motif.ics.com/motif-234-release-notes/>
- [ICS5] ICS, applications built with Motif: <https://motif.ics.com/book/export/html/31/>
- [OSN1] OSNews, "OpenMotif 2.3 released" (2007): <https://www.osnews.com/story/18156/openmotif-23-released/>
- [OSN2] OSNews, "There's a market out there for a modern X11/Motif-based desktop distribution" (2024-12-15): <https://www.osnews.com/story/141358/theres-a-market-out-there-for-a-modern-x11-motif-based-desktop-distribution/>
- [RN1] Open Motif 2.2.x release notes (mirror): <https://web.mit.edu/ghudson/dev/nokrb/third/openmotif/RELNOTES>
- [REG1] The Register, "CDE goes open source" (2012-08-09): <https://www.theregister.com/2012/08/09/cde_goes_opensource/>
- [CDE1] P. Howkins, CDE campaign page: <https://www.marutan.net/cde/>
- [DEB1] Debian, lesstif2-to-motif transition: <https://wiki.debian.org/lesstif2motifTransition>
- [DEB2] Fedora package `motif`: <https://packages.fedoraproject.org/pkgs/motif/motif-devel/index.html>
- [FC1] EMWM home: <https://fastestcode.org/emwm.html>
- [FB1] FreeBSD forums, "Currently working Motif applications": <https://forums.freebsd.org/threads/currently-working-motif-applications.69392/>
- [XO1] X.Org advisory, libXpm, 2023-01-17: <https://lists.x.org/archives/xorg-announce/2023-January/003312.html>
- [XO2] X.Org advisory, libXpm and libX11, 2023-10-03: <https://lists.x.org/archives/xorg-announce/2023-October/003424.html>
- [AW1] Arch wiki, *Wayland* (Xwayland and legacy toolkits): <https://wiki.archlinux.org/title/Wayland>

Unverified (single weak source; see §0.13):

- [U1] OSF timeline (naming on 1989-01-11): <https://www.krsaborio.net/unix/osf.htm>
- [U2] OSF news archive at MIT (shipment and certification figures): <https://diswww.mit.edu/menelaus.mit.edu/osf-news/>
- [U3] Open Motif 2.1.31/2.1.32 dates: <http://www.motifdeveloper.com/news/news23.html>, <https://www.ist-inc.com/motif/download/openmotif_download.html>
