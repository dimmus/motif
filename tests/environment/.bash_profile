#!/bin/bash
#
#	Terminal Emulation - Converted from .login
#
# neither machine nor uname are supported on all our platforms
# but uname seems to win more often.

export MACHINE=$(uname)
if [ $? -ne 0 ]; then
	export MACHINE=$(machine 2>/dev/null)
fi
if [ $? -ne 0 ]; then
	unset MACHINE
	echo "Warning: cannot set machine type"
fi

# bash equivalent of noglob
set -f

case "$MACHINE" in
   mmax*)
	# eval `tset -I -Q -s -m :xterms` - bash equivalent
	stty erase "^?"
	stty susp "^Z"
	stty intr "^C"
	#informix stuff
	export INFORMIXDIR=/users/informix
	export SQLEXEC=$INFORMIXDIR/lib/sqlexec
	export DBPATH=/users/ifmxmgr/npirs/dbdir:/users/ifmxmgr/npirs/frm
	export DBDATE=MDY2/
	export PATH="${INFORMIXDIR}/bin:$PATH"
	#end informix stuff
        ;;
#hp 700 now
   HP-UX*|hp700*)
	stty erase "^H"
	stty intr "^C"
	stty kill "^U"
	stty susp "^Z"
	stty echoe
	# eval `tset -I -Q -s -m :xterm` - bash equivalent
        ;;
   SunOS*|sparc*)
	stty erase "^H"
	stty susp "^Z"
	stty intr "^C"
	# eval `tset -I -Q -s -m :xterm` - bash equivalent
	ulimit -c 512
	;;
   IRIX*|sgi*)
	stty erase "^H"
	stty kill "^U"
	stty susp "^Z"
	stty intr "^C"
	# eval `tset -I -Q -s -m :xterm` - bash equivalent
	ulimit -c 512
	;;
   osf1*|OSF1*)
	stty echoe
	stty erase "^?"
	stty kill "^U"
	stty intr "^C"
	# eval `tset -I -Q -s -m :xterm` - bash equivalent
	ulimit -c 512
	;;
   *)
	stty erase "^H"
	stty kill "^U"
	stty susp "^Z"
	stty intr "^C"
	# eval `tset -I -Q -s -m :xterm` - bash equivalent
	;;
esac

export	EDITOR=vi				# default editor 
export	MAILHOST=osf.osf.org 			# for nbiff 
export  TZ=est5edt
export  QA_REGDIR=20Final_reg
export	MB=/project/motif/build

# bash equivalent of unset noglob
set +f
# END of .bash_profile script 