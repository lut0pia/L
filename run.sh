#!/bin/sh

# Environment and parameters

mode=${1:-build}
config=${2:-dev} # Configuration is development by default
(uname -s | grep -iqE "mingw|cygwin|msys|windows") && windows=true || windows=false

# Stats action

if [ $mode = "stats" ] ; then
  printf "Core file count: "
  find src/* -type f | grep -E "\.(h|hpp|cpp|inl|def)$" | wc -l
  printf "Core line count: "
  find src/* -type f | grep -E "\.(h|hpp|cpp|inl|def)$" | xargs cat | wc -l
  printf "Module count: "
  find mod/* -maxdepth 0 -type d | wc -l
  printf "Module line count: "
  find mod/* -type f | grep -E "\.(h|hpp|cpp|inl|def)$" | xargs cat | wc -l
  exit 0
fi

# Configuration

case $config in
  "dbg")
    exe="Ldbg"
    config=Debug
  ;;
  "dev")
    exe="Ldev"
    config=Development
  ;;
  "rls")
    exe="L"
    config=Release
  ;;
esac

mkdir -p bld
if (cd bld && cmake -DCMAKE_BUILD_TYPE=$config ..) ; then # Run CMake
  if [ $mode = "open" ] ; then
    if $windows; then
      start ./bld/L.sln
      exit 0
    fi
  fi

  cmake --build bld --config $config

  success=$?

  if [ $success = 0 ] && [ $mode = "run" ] ; then
    (cd smp && ./$exe) # Execute program
    success=$?
  fi

  exit $success
fi

exit $?
