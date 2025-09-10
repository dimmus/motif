#!/bin/bash
#
#	Environment variables - Converted from .cshrc
#

export HOST=$(hostname)
export DISPLAY="${HOST}:0.0"
export MWMSHELL=/bin/bash

#shouldn't this already be set by .bash_profile?
export MACHINE=$(uname)
if [ $? -ne 0 ]; then
	export MACHINE=$(machine 2>/dev/null)
fi
if [ $? -ne 0 ]; then
	unset MACHINE
	echo "Warning: cannot set machine type"
fi

if [ "${MACHINE}" = "SunOS" ]; then
    export PURIFYHOME=/usr/local/apps/purify
fi

# bash equivalent settings
HISTSIZE=100				# length of history list
export PAGER='more'			# Set mail pager
set -o noclobber				# Be careful on overwriting
set -o ignoreeof				# Disallow ^D to logout
# filec equivalent - bash has filename completion by default
# fignore equivalent - bash doesn't have this, but we can set FIGNORE
export FIGNORE=".o:a.out:~:.BAK:\~:core"	# list of files/ext to ignore
export PS1="qauser@${HOST} \! \$ "	# PROPER PROMPT
# notify equivalent - bash doesn't have this

umask 022					# create files   rwxr-xr-x

#
#	Setup the PATH variable
#
export PATH=".:~/.bin:/usr/ode/bin_sc:/usr/ucb:/bin:/usr/bin:/usr/sbin:/usr/local/apps/CenterLine/bin:/usr/local/apps/purify:/usr/local/bin:/etc:/usr/etc:/usr/local/pub:/usr/local/pub/DEC:/usr/sccs:/usr/new:/usr/new/mh:/usr/server1/usr/bin:/usr/ode/bin_sc:/usr/local/CenterLine/bin:/project/motif/qa/qauser/qatools/auto_rep:/project/motif/build/tools/src/bin/share:/project/motif/qa/qauser/qatools/regression"

case "$MACHINE" in
   OSF1*|osf1*)
	if [ "$(uname -m)" = "alpha" ]; then
	    export PATH="$HOME/bin/OSF1.alpha:$HOME/bin:$PATH"
	else
	    if [ "${HOST}" = "pmin11" ]; then
		export PATH="/project/ot/newbin/mips.OSF1:$PATH:$HOME/bin/OSF1.mips/.:$HOME/bin:$HOME/qatools/vista:/project/tools/bin/mips.OSF1:/usr/bin/vista:/project/motif/qa/qauser/qatools/regression"
		export MANPATH="/usr/man:/project/tools/man:/usr/ode/man:/usr/bin/vista/vista/doc/man"
	    else
		export PATH="/project/ot/newbin/i386.OSF1:$PATH:$HOME/bin/OSF1/.:$HOME/bin:$HOME/qatools/vista:/project/tools/bin/i386.OSF1:/usr/vista/bin:/project/motif/qa/qauser/qatools/regression"
		export MANPATH="/usr/man:/project/tools/man:/usr/ode/man:/usr/vista/man"
	    fi
	fi
	if [ "${HOST}" != "chapman" ]; then
	    export VLM_HOST=pomerol
	fi
	;;
   mips*|ULTRIX*)
	export PATH="$PATH:$HOME/bin/Ultrix:/usr/local/pub/DEC:$HOME:/project/tools/bin/mips.ULTRIX"
	;;
   sparc*|SunOS*)
# This is temporary; to be removed as soon as Purify is installed on
# QA machines
        if [ "${HOST}" = "moonbeam" ]; then
		export PATH="/project/ot/newbin/sparc.SunOS:$PATH:$HOME/bin/Sparc:$HOME/bin:/usr/sccs:/project/tools/bin/sparc.SunOS:$HOME/qatools/memory"
	else
		export PATH="/project/ot/newbin/sparc.SunOS:$PATH:$HOME/bin/Sparc:$HOME/bin:/usr/sccs:/project/tools/bin/sparc.SunOS:$HOME/qatools/memory:/project/motif/qa/qauser/qatools/regression"
	fi
	if [ -n "$PURIFYHOME" ]; then
		export PATH="$PATH:$PURIFYHOME"
	fi
	export context=sparc_sunos
        ;;
   sgi*|IRIX*)
	export PATH="$PATH:$HOME/bin/IRIX:$HOME/bin"
	;;
#hp700 now
   HP-UX*|hp700*)
	export PATH="/project/ot/newbin/hp9000_700:$PATH:$HOME/bin/HPUX/.:$HOME/bin:/project/tools/bin/hp9000_700:/project/motif/qa/qauser/qatools/regression"
	export context=hp700_hpux
	export USER=qauser
	;;
   *)
	export PATH="$PATH:$HOME/bin"
	;;
esac

export OT_PROJECT=motif

if [ "$MACHINE" = "OSF1" ] && [ "$(uname -m)" = "alpha" ]; then
export XNLSPATH=/usr/lib/X11/nls
else
export XNLSPATH=/usr/lib/X11/nls
fi 