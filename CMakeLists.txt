cmake_minimum_required(VERSION 3.10)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Override platform if generating for Visual Studio

string(REGEX MATCH "Visual Studio" IS_VISUAL_STUDIO ${CMAKE_GENERATOR})
if(IS_VISUAL_STUDIO)
  set(CMAKE_GENERATOR_PLATFORM x64)
endif()

include(CheckCXXCompilerFlag)
include(ExternalProject)

project(L)

# Configuration types

set(DEV_DBG $<NOT:$<CONFIG:Release>>)
add_compile_definitions(
  L_DBG=$<CONFIG:Debug>
  L_RLS=$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>
  L_LINUX=$<PLATFORM_ID:Linux>
  L_WINDOWS=$<PLATFORM_ID:Windows>
)
set(CMAKE_EXE_LINKER_FLAGS_DEVELOPMENT ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE INTERNAL "Configs")

# Compiler options

function(add_compile_options_safe)
  foreach(OPT ${ARGV})
    check_cxx_compiler_flag(${OPT} HAS_COMPILER_OPTION_${OPT})
    if(HAS_COMPILER_OPTION_${OPT})
      add_compile_options(${OPT})
    endif()
  endforeach()
endfunction()

if(MSVC)
  add_compile_definitions(
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    NOCRYPT
    _CRT_SECURE_NO_WARNINGS
    _WINSOCK_DEPRECATED_NO_WARNINGS
    _HAS_EXCEPTIONS=0
  )

  string(REPLACE "/EHsc" "/EHs-c-" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  add_compile_options(
    /GR-
    /fp:fast
    /WX /W4
  )
  add_link_options(
    $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
  )
else()
  add_compile_options(
    -fno-operator-names
    -fno-exceptions
    -fno-rtti
    -ffast-math
    -Werror -Wall
  )
  add_compile_options_safe(
    -Wno-class-memaccess
    -Wno-missing-braces
    -Wno-missing-field-initializers
    -Wno-type-limits
    -Wno-unused-function
  )
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Libraries

if(WIN32)
  link_libraries(ws2_32 $<${DEV_DBG}:dbghelp>)
else()
  link_libraries(X11 pthread)
endif()

# Source files

file(GLOB_RECURSE CORE_SOURCES src/*.cpp src/*.inl src/*.h)
if(WIN32)
  list(FILTER CORE_SOURCES EXCLUDE REGEX .*_unix.cpp)
else()
  list(FILTER CORE_SOURCES EXCLUDE REGEX .*_win.cpp)
endif()
if(MSVC)
  list(APPEND CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resource.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/L.natvis
  )
endif()
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src FILES ${CORE_SOURCES})

# Binary

add_executable(core ${CORE_SOURCES})
set_property(DIRECTORY . PROPERTY VS_STARTUP_PROJECT core)
set(L_GAME_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/smp" CACHE PATH "Game data directory")
set_target_properties(
  core PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${L_GAME_DIRECTORY}
  RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${L_GAME_DIRECTORY}
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${L_GAME_DIRECTORY}
  RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${L_GAME_DIRECTORY}
  VS_DEBUGGER_WORKING_DIRECTORY ${L_GAME_DIRECTORY}
  OUTPUT_NAME_DEBUG Ldbg
  OUTPUT_NAME_RELWITHDEBINFO Ldev
  OUTPUT_NAME_RELEASE L
  OUTPUT_NAME_MINSIZEREL Lmin
)

# Engine settings

option(L_USE_DEBUG_ALLOC "Use system page allocator to track memory issues (expect disastrous performance)" OFF)
option(L_USE_MALLOC "Use malloc/free instead of custom allocator" OFF)
target_compile_definitions(core PRIVATE L_USE_DEBUG_ALLOC=$<BOOL:${L_USE_DEBUG_ALLOC}>)
target_compile_definitions(core PRIVATE L_USE_MALLOC=$<BOOL:${L_USE_MALLOC}>)

# Modules - function

set(MODULE_LIST "")
function(add_module NAME)
  set(OPTARGS "")
  set(SINGLEARGS CONDITION)
  set(MULTIARGS
    LINK_DIRS INCLUDE_DIRS SYSTEM_INCLUDE_DIRS
    LIBRARIES DBG_LIBRARIES RLS_LIBRARIES
    DEPENDENCIES MOD_DEPENDENCIES
  )
  cmake_parse_arguments(M "${OPTARGS}" "${SINGLEARGS}" "${MULTIARGS}" ${ARGN})
  if(NOT DEFINED M_CONDITION)
    set(M_CONDITION 1) # Always use module if no condition set
  endif()

  file(GLOB_RECURSE MOD_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/*.cpp
    ${CMAKE_CURRENT_LIST_DIR}/*.inl
    ${CMAKE_CURRENT_LIST_DIR}/*.h)

  add_library(mod_${NAME} ${MOD_SOURCES})
  set_target_properties(
    mod_${NAME} PROPERTIES
    EXCLUDE_FROM_ALL TRUE
  )

  # Core definition and link
  target_compile_definitions(core PRIVATE L_USE_MODULE_${NAME}=${M_CONDITION})
  target_link_libraries(core PRIVATE $<${M_CONDITION}:mod_${NAME}>)
  target_link_directories(core PRIVATE ${M_LINK_DIRS})

  # Module directories
  target_include_directories(mod_${NAME} PRIVATE ${M_INCLUDE_DIRS})
  target_include_directories(mod_${NAME} SYSTEM PRIVATE ${M_SYSTEM_INCLUDE_DIRS})

  # Libraries
  set(MOD_LIB_PATTERN "^(.*/)(${CMAKE_STATIC_LIBRARY_PREFIX})?([^.]*)$")
  set(MOD_LIB_REPLACE "\\1${CMAKE_STATIC_LIBRARY_PREFIX}\\3${CMAKE_STATIC_LIBRARY_SUFFIX}")
  list(TRANSFORM M_LIBRARIES REPLACE ${MOD_LIB_PATTERN} ${MOD_LIB_REPLACE})
  list(TRANSFORM M_DBG_LIBRARIES REPLACE ${MOD_LIB_PATTERN} ${MOD_LIB_REPLACE})
  list(TRANSFORM M_RLS_LIBRARIES REPLACE ${MOD_LIB_PATTERN} ${MOD_LIB_REPLACE})
  target_link_libraries(mod_${NAME} ${M_LIBRARIES})
  foreach(LIB ${M_DBG_LIBRARIES})
    target_link_libraries(mod_${NAME} debug ${LIB})
  endforeach()
  foreach(LIB ${M_RLS_LIBRARIES})
    target_link_libraries(mod_${NAME} optimized ${LIB})
  endforeach()

  # Dependencies
  if(DEFINED M_DEPENDENCIES)
    add_dependencies(mod_${NAME} ${M_DEPENDENCIES})
  endif()
  foreach(MOD_DEP ${M_MOD_DEPENDENCIES})
    target_include_directories(mod_${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mod/${MOD_DEP}/include)
    target_link_libraries(mod_${NAME} mod_${MOD_DEP})
  endforeach()

  set(MODULE_LIST ${MODULE_LIST} ${NAME} PARENT_SCOPE)
endfunction()

# Modules - iterate

file(GLOB MOD_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/mod/*)
foreach(MOD_DIR ${MOD_DIRS})
  get_filename_component(MOD_NAME ${MOD_DIR} NAME)
  option(L_ENABLE_${MOD_NAME} "Enable module ${MOD_NAME}" ON)
  if(L_ENABLE_${MOD_NAME})
    include(${MOD_DIR}/module.cmake)
  endif()
endforeach()

# Modules - mod list file

set(MOD_LIST_GEN_PATH ${CMAKE_CURRENT_BINARY_DIR}/mod_list.gen)
file(WRITE ${MOD_LIST_GEN_PATH} "// This file was generated by CMake, do not modify manually\n")
foreach(MOD_NAME ${MODULE_LIST})
  if(L_ENABLE_${MOD_NAME})
    file(APPEND ${MOD_LIST_GEN_PATH} "MOD(${MOD_NAME})\n")
  endif()
endforeach()

configure_file(${MOD_LIST_GEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_list.gen)
