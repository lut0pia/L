#!/bin/sh

# Environment and parameters

mode=${1:-build}
configuration=${2:-dev} # Configuration is development by default
(uname -s | grep -iqE "mingw|cygwin") && windows=true || windows=false

# Functions

download () {
  if $windows ; then
    powershell -command "iwr -outf $1 $2"
  else
    wget -O $1 $2
  fi
}

extract () {
  if $windows ; then
    unzip -oq $1
  else
    tar xvzf $1
  fi
}

# Stats action

if [ $mode = "stats" ] ; then
  echo "Core file count:"
  git ls-files src | grep -v module/ | wc -l
  echo "Core line count:"
  git ls-files src | grep -v module/ | xargs cat | wc -l
  echo "Module count:"
  find src/module/* -maxdepth 1 -type d | wc -l
  echo "Module line count:"
  git ls-files src | grep module/ | xargs cat | wc -l
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
  premake_url="https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip"
  premake_bin=premake5.exe
  premake_action=vs$vsver
else
  premake_url="https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-linux.tar.gz"
  premake_bin=premake5
  premake_action=gmake2
fi

premake_ar=pmk/$(basename $premake_url)

if [ ! -f $premake_ar ] ; then
  echo Downloading premake archive: $premake_ar
  download $premake_ar $premake_url
fi

if [ ! -f $premake_bin ] || [ $premake_bin -ot $premake_ar ] ; then
  echo Extracting premake binary from: $premake_ar
  extract $premake_ar
  touch $premake_bin # Update mtime
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
