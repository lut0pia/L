cmake_minimum_required(VERSION 3.10)
set(CMAKE_SUPPRESS_REGENERATION true)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Override platform if generating for Visual Studio

string(REGEX MATCH "Visual Studio" IS_VISUAL_STUDIO ${CMAKE_GENERATOR})
if(IS_VISUAL_STUDIO)
  set(CMAKE_GENERATOR_PLATFORM x64)
endif()

include(ExternalProject)

project(L)

# Configuration types

set(CMAKE_CONFIGURATION_TYPES "Debug;Development;Release" CACHE STRING "Configs" FORCE)
set(DEV_DBG $<NOT:$<CONFIG:Release>>)
add_compile_definitions($<${DEV_DBG}:L_DEBUG>)
set(CMAKE_EXE_LINKER_FLAGS_DEVELOPMENT ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE STRING "Configs")

# Compiler options

if(MSVC)
  add_compile_definitions(
    L_WINDOWS
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
    /WX /W4
    /fp:fast
  )
else()
  add_compile_definitions(
    L_UNIX
  )
  add_compile_options(
    -fno-operator-names
    -fno-exceptions
    -fno-rtti
    -Werror -Wall
    -ffast-math
  )
endif()
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

# Libraries

if(WIN32)
  link_libraries(ws2_32 $<${DEV_DBG}:dbghelp>)
else()
  link_libraries(X11 pthread)
endif()

# Vulkan

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
link_libraries(${Vulkan_LIBRARIES})

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
set_target_properties(
  core PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/smp
  RUNTIME_OUTPUT_DIRECTORY_DEVELOPMENT ${CMAKE_CURRENT_SOURCE_DIR}/smp
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/smp
  VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/smp
  OUTPUT_NAME_DEBUG Ldbg
  OUTPUT_NAME_DEVELOPMENT Ldev
  OUTPUT_NAME_RELEASE L
)

# Modules - function

function(add_module NAME)
  set(OPTARGS "")
  set(SINGLEARGS CONDITION)
  set(MULTIARGS
    SOURCES
    LINK_DIRS
    INCLUDE_DIRS
    LIBRARIES
    DBG_LIBRARIES
    RLS_LIBRARIES
    DEPENDENCIES
  )
  cmake_parse_arguments(M "${OPTARGS}" "${SINGLEARGS}" "${MULTIARGS}" ${ARGN})
  if(NOT DEFINED M_CONDITION)
    set(M_CONDITION 1) # Always use module if no condition set
  endif()

  add_library(mod_${NAME} ${M_SOURCES})
  set_target_properties(
    mod_${NAME} PROPERTIES
    EXCLUDE_FROM_ALL TRUE
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
  )

  # Core definition, link and directories
  target_compile_definitions(core PRIVATE $<${M_CONDITION}:L_USE_MODULE_${NAME}>)
  target_link_libraries(core PRIVATE $<${M_CONDITION}:mod_${NAME}>)
  target_link_directories(core PRIVATE $<${M_CONDITION}:${M_LINK_DIRS}>)

  # Module directories
  target_include_directories(mod_${NAME} PRIVATE ${M_INCLUDE_DIRS})
  target_link_directories(mod_${NAME} PRIVATE ${M_LINK_DIRS})

  # Libraries
  target_link_libraries(mod_${NAME} $<${M_CONDITION}:${M_LIBRARIES}>)
  foreach(LIB ${M_DBG_LIBRARIES})
    target_link_libraries(mod_${NAME} debug $<${M_CONDITION}:${LIB}>)
  endforeach()
  foreach(LIB ${M_RLS_LIBRARIES})
    target_link_libraries(mod_${NAME} optimized $<${M_CONDITION}:${LIB}>)
  endforeach()

  # Dependencies
  if(DEFINED M_DEPENDENCIES)
    add_dependencies(mod_${NAME} ${M_DEPENDENCIES})
  endif()
endfunction()

# Modules - iterate

set(MOD_INIT_GEN_PATH ${CMAKE_CURRENT_BINARY_DIR}/mod_init.gen)
file(WRITE ${MOD_INIT_GEN_PATH} "")
file(GLOB MOD_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/mod/*)
foreach(MOD_DIR ${MOD_DIRS})
  get_filename_component(MOD_NAME ${MOD_DIR} NAME)
  if(EXISTS ${MOD_DIR}/module.cmake)
    include(${MOD_DIR}/module.cmake)
  else()
    message(SEND_ERROR "Module ${MOD_NAME} is missing a module.cmake file")
  endif()
  file(APPEND ${MOD_INIT_GEN_PATH}
    "#if L_USE_MODULE_${MOD_NAME}\n"
    "{\n"
    "L_SCOPE_MARKER(\"${MOD_NAME}\");\n"
    "extern void ${MOD_NAME}_module_init();${MOD_NAME}_module_init();\n"
    "}\n"
    "#endif\n"
  )
endforeach()
configure_file(${MOD_INIT_GEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_init.gen)
