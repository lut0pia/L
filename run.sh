#!/bin/sh

# Environment and parameters

mode=${1:-build}
configuration=${2:-development} # Configuration is development by default
(uname -s | grep -iqE "mingw|cygwin") && windows=true || windows=false

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

# Premake

mkdir pmk 2> /dev/null

if $windows ; then
  premake_url="https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-windows.zip"
  premake_bin=premake5.exe
  premake_action=vs2015
else
  premake_url="https://github.com/premake/premake-core/releases/download/v5.0.0-alpha14/premake-5.0.0-alpha14-linux.tar.gz"
  premake_bin=premake5
  premake_action=gmake2
fi

premake_ar=pmk/$(basename $premake_url)

if [ ! -f $premake_ar ] ; then
  echo Downloading premake archive: $premake_ar
  if $windows ; then
    powershell -command "iwr -outf $premake_ar $premake_url"
  else
    wget -O $premake_ar $premake_url
  fi
fi

if [ ! -f $premake_bin ] || [ $premake_bin -ot $premake_ar ] ; then
  echo Extracting premake binary from: $premake_ar
  if $windows ; then
    unzip -oq $premake_ar
  else
    tar xvzf $premake_ar
  fi
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
    cmd.exe /C "\"$VS140COMNTOOLS\vsvars32.bat\" && MSBuild /NOLOGO prj/$premake_action/L.sln /p:configuration=$configuration"
  else
    (cd prj/$premake_action && make config=$configuration -j 4) # Run make
  fi
  if [ $mode = "run" ] ; then
    (cd smp && ./$exe) # Execute program
  fi
fi

exit $?
