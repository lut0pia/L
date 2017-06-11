#!/bin/sh

# Premake
(git clone https://github.com/premake/premake-core pmk) || # Attempt to clone
(!(git status pmk | grep up-to-date) && cd pmk && git pull) # Otherwise pull if not up-to-date

if [ ! -e premake5 ] || [ premake5 -ot pmk ] ; then # If it hasn't been built or is older than repo
	(cd pmk && make -f Bootstrap.mak linux) && # Build premake
	cp pmk/bin/release/premake5 premake5 # Copy binary
fi

# L
configuration=${1:-development} #Configuration is development by default
case $configuration in
   "debug") exe="Ldbg"
   ;;
   "development") exe="Ldev"
   ;;
   "release") exe="L"
   ;;
esac
./premake5 gmake && # Run premake
(cd prj/gmake && make -j 4 config=$configuration) && # Run make
(cd smp && ./$exe) # Execute program
