#!/bin/sh

# Environment and parameters

mode=${1:-build}
configuration=${2:-dev} # Configuration is development by default
(uname -s | grep -iqE "mingw|cygwin|msys|windows") && windows=true || windows=false

# Stats action

if [ $mode = "stats" ] ; then
  printf "Core file count: "
  find src/* -type f | grep -v module/ | grep -E "\.(h|hpp|cpp|inl|def)$" | wc -l
  printf "Core line count: "
  find src/* -type f | grep -v module/ | grep -E "\.(h|hpp|cpp|inl|def)$" | xargs cat | wc -l
  printf "Module count: "
  find src/module/* -maxdepth 0 -type d | wc -l
  printf "Module line count: "
  find src/module/* -type f | grep -E "\.(h|hpp|cpp|inl|def)$" | xargs cat | wc -l
  exit 0
fi

# Configuration

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

# Detect Visual Studio version

if $windows ; then
  #for ver in 2019 2017 ; do
  #  for flavor in Enterprise Professional Community ; do
  #    if [ -e "$SYSTEMDRIVE/Program Files (x86)/Microsoft Visual Studio/$ver/$flavor" ] ; then
  #      vsver=$ver
  #      vsvars="$SYSTEMDRIVE/Program Files (x86)/Microsoft Visual Studio/$ver/$flavor/Common7/Tools/VsDevCmd.bat"
  #      break 2
  #    fi
  #  done
  #done

  if [ "$vsver" == "" ] ; then
    if [ "$VS140COMNTOOLS" != "" ] ; then
      vsver=2015
      vsvars="$VS140COMNTOOLS/vsvars32.bat"
    fi
  fi

  if [ "$vsver" == "" ] ; then
    echo "Could not detect any Visual Studio installation"
    exit 1
  fi

  echo "Detected Visual Studio version: $vsver"
fi

# Premake

mkdir -p pmk

if $windows ; then
  premake_bin=ext/bin/premake5.exe
  premake_action=vs$vsver
else
  premake_bin=ext/bin/premake5
  premake_action=gmake2
fi

if ./$premake_bin $premake_action ; then # Run premake
  if [ $mode = "open" ] ; then
    if $windows; then
      start ./prj/$premake_action/L.sln
      exit 0
    fi
  fi
  if $windows ; then
    cmd.exe /C "\"$vsvars\" && MSBuild /NOLOGO prj/$premake_action/L.sln /p:configuration=$configuration"
  else
    (cd prj/$premake_action && make config=$configuration -j 4) # Run make
  fi

  success=$?

  if [ $success = 0 ] && [ $mode = "run" ] ; then
    (cd smp && ./$exe) # Execute program
    success=$?
  fi

  exit $success
fi

exit $?
