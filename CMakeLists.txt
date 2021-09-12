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
set(RLS $<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>)
add_compile_definitions(
  L_DBG=$<CONFIG:Debug>
  L_RLS=${RLS}
  L_LINUX=$<PLATFORM_ID:Linux>
  L_WINDOWS=$<PLATFORM_ID:Windows>
)
set(CMAKE_EXE_LINKER_FLAGS_DEVELOPMENT ${CMAKE_EXE_LINKER_FLAGS_DEBUG} CACHE INTERNAL "Configs")

# Compiler options

option(L_USE_ASAN "Use address sanitizer" OFF)

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

  if(${L_USE_ASAN})
    add_compile_options(/fsanitize=address)
  endif()
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

  if(${L_USE_ASAN})
    add_compile_options(-fsanitize=address)
  endif()
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
option(L_USE_COMPRESSION "Use compression for archived resources" ON)
target_compile_definitions(core PRIVATE L_USE_DEBUG_ALLOC=$<BOOL:${L_USE_DEBUG_ALLOC}>)
target_compile_definitions(core PRIVATE L_USE_MALLOC=$<BOOL:${L_USE_MALLOC}>)
target_compile_definitions(core PRIVATE L_USE_COMPRESSION=$<BOOL:${L_USE_COMPRESSION}>)

# Testing

enable_testing()
add_test(
  NAME all_tests
  COMMAND $<TARGET_FILE:core> --run_all_tests
)

# Externals - function

function(add_external NAME)
  set(OPTARGS
    CMAKE
    CONFIG_INSTALL_DIR)
  set(SINGLEARGS
    GIT_REPOSITORY
    GIT_TAG)
  set(MULTIARGS
    INCLUDE_DIRS
    LIBRARIES DBG_LIBRARIES RLS_LIBRARIES
    CMAKE_ARGS
  )
  cmake_parse_arguments(E "${OPTARGS}" "${SINGLEARGS}" "${MULTIARGS}" ${ARGN})

  # Setup install directory
  set(EXT_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/ext/${NAME})
  if(E_CONFIG_INSTALL_DIR)
    # External needs multiple install directories for each config
    set(EXT_INSTALL_DIR ${EXT_INSTALL_DIR}/$<CONFIG>)
  endif()

  if(E_CMAKE) # Build with CMake via ExternalProject
    ExternalProject_Add(
      ext_${NAME}
      GIT_REPOSITORY ${E_GIT_REPOSITORY}
      GIT_TAG ${E_GIT_TAG}
      UPDATE_COMMAND ""
      CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${EXT_INSTALL_DIR}
        ${E_CMAKE_ARGS}
    )
  else() # No build process, only fetch from git
    ExternalProject_Add(
      ext_${NAME}
      GIT_REPOSITORY ${E_GIT_REPOSITORY}
      GIT_TAG ${E_GIT_TAG}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND ""
      UPDATE_COMMAND ""
      INSTALL_COMMAND ""
      SOURCE_DIR ${EXT_INSTALL_DIR}
    )
  endif()

  # Include directories
  if(DEFINED E_INCLUDE_DIRS)
    list(TRANSFORM E_INCLUDE_DIRS PREPEND ${EXT_INSTALL_DIR}/)
    set_property(GLOBAL PROPERTY EXT_${NAME}_INCLUDE_DIRS ${E_INCLUDE_DIRS})
  else()
    set_property(GLOBAL PROPERTY EXT_${NAME}_INCLUDE_DIRS ${EXT_INSTALL_DIR})
  endif()

  # Libraries
  foreach(LIBS IN ITEMS LIBRARIES DBG_LIBRARIES RLS_LIBRARIES)
    list(TRANSFORM E_${LIBS} PREPEND ${EXT_INSTALL_DIR}/)
    set_property(GLOBAL PROPERTY EXT_${NAME}_${LIBS} ${E_${LIBS}})
  endforeach()
endfunction()

# Modules - settings

option(L_ENABLE_ALL_MODULES "Enable all modules" OFF)

# Modules - function

set(MODULE_LIST "")
function(add_module NAME)
  set(OPTARGS "")
  set(SINGLEARGS CONDITION DEFAULT_ENABLED)
  set(MULTIARGS
    INCLUDE_DIRS SYSTEM_INCLUDE_DIRS
    LIBRARIES DBG_LIBRARIES RLS_LIBRARIES
    DEPENDENCIES
    MOD_DEPENDENCIES OPT_MOD_DEPENDENCIES
    EXT_DEPENDENCIES
  )
  cmake_parse_arguments(M "${OPTARGS}" "${SINGLEARGS}" "${MULTIARGS}" ${ARGN})

  if(NOT DEFINED M_DEFAULT_ENABLED)
    if(DEFINED M_EXT_DEPENDENCIES) # Modules are disabled by default if they have no external dependencies
      set(M_DEFAULT_ENABLED OFF)
    else()
      set(M_DEFAULT_ENABLED ON)
    endif()
  endif()

  if(NOT DEFINED M_CONDITION)
    set(M_CONDITION 1) # Always use module if no condition set
  endif()

  string(TOUPPER ${NAME} NAME_UPPER)
  option(L_ENABLE_${NAME_UPPER} "Enable module ${NAME}" ${M_DEFAULT_ENABLED})
  if(NOT L_ENABLE_${NAME_UPPER} AND NOT L_ENABLE_ALL_MODULES)
    return()
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
  add_compile_definitions(L_USE_MODULE_${NAME}=${M_CONDITION}) # Define for everyone
  target_link_libraries(core PRIVATE $<${M_CONDITION}:mod_${NAME}>)

  # Dependencies
  if(DEFINED M_DEPENDENCIES)
    add_dependencies(mod_${NAME} ${M_DEPENDENCIES})
  endif()
  foreach(MOD_DEP ${M_MOD_DEPENDENCIES})
    # Force enable dependency module
    string(TOUPPER ${MOD_DEP} MOD_DEP_UPPER)
    set(L_ENABLE_${MOD_DEP_UPPER} ON CACHE BOOL "Enable module ${NAME} (forced by dependency)" FORCE)
    target_include_directories(mod_${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mod/${MOD_DEP}/include)
    target_link_libraries(mod_${NAME} mod_${MOD_DEP})
  endforeach()
  foreach(MOD_DEP ${M_OPT_MOD_DEPENDENCIES})
    target_include_directories(mod_${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mod/${MOD_DEP}/include)
  endforeach()
  foreach(EXT_DEP ${M_EXT_DEPENDENCIES})
    # Read external definition if not already defined
    if(NOT TARGET ext_${EXT_DEP})
      include(${CMAKE_CURRENT_SOURCE_DIR}/ext/${EXT_DEP}.cmake)
    endif()
    add_dependencies(mod_${NAME} ext_${EXT_DEP})

    # Include directories
    get_property(EXT_INCLUDE_DIRS GLOBAL PROPERTY EXT_${EXT_DEP}_INCLUDE_DIRS)
    list(APPEND M_SYSTEM_INCLUDE_DIRS ${EXT_INCLUDE_DIRS})

    # Libraries
    foreach(LIBS IN ITEMS LIBRARIES DBG_LIBRARIES RLS_LIBRARIES)
      get_property(EXT_LIBRARIES GLOBAL PROPERTY EXT_${EXT_DEP}_${LIBS})
      list(APPEND M_${LIBS} ${EXT_LIBRARIES})
    endforeach()
  endforeach()

  # Module directories
  target_include_directories(mod_${NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/mod/${NAME}/include) # Own include directory
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

  set(MODULE_LIST ${MODULE_LIST} ${NAME} PARENT_SCOPE)
endfunction()

# Modules - iterate

file(GLOB MOD_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/mod/*)
foreach(MOD_DIR ${MOD_DIRS})
  include(${MOD_DIR}/module.cmake)
endforeach()

# Modules - mod list file

set(MOD_LIST_GEN_PATH ${CMAKE_CURRENT_BINARY_DIR}/mod_list.gen)
file(WRITE ${MOD_LIST_GEN_PATH} "// This file was generated by CMake, do not modify manually\n")
foreach(MOD_NAME ${MODULE_LIST})
  string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
  if(L_ENABLE_${MOD_NAME_UPPER} OR L_ENABLE_ALL_MODULES)
    file(APPEND ${MOD_LIST_GEN_PATH} "MOD(${MOD_NAME})\n")
  endif()
endforeach()

configure_file(${MOD_LIST_GEN_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src/mod_list.gen)
