dnl XM_COLOR
dnl will check if terminal supports color and if color is wanted by user.
dnl
dnl Used Variables:
dnl     WANT_COLOR: if no, forces no color output.
dnl     TERM: used to check if color should be enabled.
dnl
dnl Defined Variables:
dnl     COLOR_YES: to be used in positive/yes conditions
dnl     COLOR_NO: to be used in negative/no conditions
dnl     COLOR_OTHER: to be used to highlight some other condition
dnl     COLOR_RESET: to reset color
dnl     want_color: yes or no
AC_DEFUN([XM_COLOR],
[dnl
case "$TERM" in
   xterm|xterm-color|xterm-256color|Eterm|aterm|kterm|rxvt*|screen|gnome|interix)
      want_color="${WANT_COLOR:-yes}"
      ;;
   *)
      want_color="no"
      ;;
esac

### echo compatibility

## the BSD echo does not have the -e option (it is the default behaviour)
echo_e=
if test "`echo -e x`" = "x"; then
   echo_e=-e
fi
AC_SUBST([ECHO_E], [${echo_e}])

if test "${want_color}" = "yes"; then
   COLOR_YES=`echo $echo_e "\033@<:@1;32m"`
   COLOR_NO=`echo $echo_e "\033@<:@1;31m"`
   COLOR_OTHER=`echo $echo_e "\033@<:@1;36m"`
   COLOR_RESET=`echo $echo_e "\033@<:@0m"`
else
   COLOR_YES=""
   COLOR_NO=""
   COLOR_OTHER=""
   COLOR_RESET=""
fi
])

dnl XML_ADD_FEATURE(PKG, NAME, [VALUE])
dnl if VALUE is not specified, will use ${have_name} instead.
dnl
dnl Defined Variables:
dnl     features_pkg
AC_DEFUN([XM_ADD_FEATURE],
[dnl
AC_REQUIRE([XM_COLOR])dnl
m4_pushdef([DOWNPKG], m4_translit([$1], [-A-Z], [_a-z]))dnl
m4_pushdef([DOWNNAME], m4_translit([$2], [-A-Z], [_a-z]))dnl

value="m4_if($3, , [${have_]m4_defn([DOWNNAME])[:-${want_]m4_defn([DOWNNAME])[}}], [$3])"
case "${value}" in
   yes)
      tmp="${COLOR_YES}+$2${COLOR_RESET}"
      ;;
   no)
      tmp="${COLOR_NO}-$2${COLOR_RESET}"
      ;;
   *)
      tmp="${COLOR_OTHER}$2=${value}${COLOR_RESET}"
      ;;
esac
if test -z "${features_[]m4_defn([DOWNPKG])}"; then
   features_[]m4_defn([DOWNPKG])="${tmp}"
else
   features_[]m4_defn([DOWNPKG])="${features_[]m4_defn([DOWNPKG])} ${tmp}"
fi
m4_popdef([DOWNNAME])dnl
m4_popdef([DOWNPKG])dnl
])