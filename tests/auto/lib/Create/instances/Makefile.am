# Instances library for Motif tests
# Converted from imake to autotools

noinst_LTLIBRARIES = libinstances.la

libinstances_la_SOURCES = \
	ApplicationS.c \
	ArrowB.c \
	ArrowBG.c \
	BulletinB.c \
	BulletinBD.c \
	CascadeB.c \
	CascadeBG.c \
	ComboBox.c \
	Command.c \
	Container.c \
	DialogS.c \
	DrawingA.c \
	DrawnB.c \
	DrpDnCmBx.c \
	DrpDnLstCmBx.c \
	ErrorD.c \
	FileSB.c \
	FileSD.c \
	Form.c \
	FormD.c \
	Frame.c \
	IconGadget.c \
	InfoD.c \
	Label.c \
	LabelG.c \
	List.c \
	MainW.c \
	MenuB.c \
	MenuS.c \
	MessageB.c \
	MessageD.c \
	Notebook.c \
	OptionM.c \
	PanedW.c \
	PopupM.c \
	PopupS.c \
	PromptD.c \
	PulldownM.c \
	PushB.c \
	PushBG.c \
	QuestionD.c \
	RadioB.c \
	RowC.c \
	Sash.c \
	Scale.c \
	ScrollB.c \
	ScrolledL.c \
	ScrolledT.c \
	ScrolledW.c \
	SelectionB.c \
	SelectionD.c \
	Separator.c \
	SeparatorG.c \
	SpinBox.c \
	TearOffButton.c \
	TemplateD.c \
	Text.c \
	TextF.c \
	ToggleB.c \
	ToggleBG.c \
	WarningD.c \
	WorkA.c \
	WorkingD.c

libinstances_la_CPPFLAGS = \
	-I$(top_srcdir)/tests/auto/lib/Create \
	-I$(top_srcdir)/tests/auto/lib/common \
	-I$(top_srcdir)/tests/auto/lib/Synth \
	-I$(top_srcdir)/tests/auto/lib/Visual \
	-I$(top_srcdir)/tests/auto/lib/Util \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/lib \
	-I$(top_builddir)/include \
	-I$(top_srcdir)/include \
	$(X_CFLAGS)

libinstances_la_LIBADD = \
	$(top_builddir)/lib/Xm/libXm.la \
	$(X_LIBS)

# Clean up generated files
MAINTAINERCLEANFILES = Makefile.in 