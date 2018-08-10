#!/bin/sh

if (uname -s | grep -iqE "mingw|cygwin") ; then
  ./premake5.exe vs2015
  start prj/vs2015/L.sln
  exit 0
fi

configuration=${1:-development} #Configuration is development by default
mode=${2:-build}

# Premake
(git clone --depth 1 https://github.com/premake/premake-core pmk) || # Attempt to clone
(!(git status pmk | grep up-to-date) && cd pmk && git pull) # Otherwise pull if not up-to-date

if [ ! -e premake5 ] || [ premake5 -ot pmk ] ; then # If it hasn't been built or is older than repo
	(cd pmk && make -f Bootstrap.mak linux) && # Build premake
	cp pmk/bin/release/premake5 premake5 # Copy binary
fi

# L
case $configuration in
  "dbg")
    exe="Ldbg"
    configuration=debug
  ;;
  "dev")
    exe="Ldev"
    configuration=development
  ;;
  "rls")
    exe="L"
    configuration=release
  ;;
esac

if
  ./premake5 gmake && # Run premake
  (cd prj/gmake && make -j 4 config=$configuration) # Run make
then
  if [ $mode = "run" ] ; then
    (cd smp && ./$exe) # Execute program
  fi
else
  exit $?
fi
