# Copyright Abel Sinkovics (abel@sinkovics.hu)  2012.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

set(CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required

cmake_minimum_required(VERSION 2.6)
project(mpllibs)
set(CMAKE_PROJECT_NAME mpllibs)
set(MAJOR_VERSION 1)
set(MINOR_VERSION 0)
set(PATCH_VERSION 0)

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Pandoc)
include(Files)

enable_testing()

include_directories(".")

# Warning level
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

  # Boost.Config says that variadic macros are available when they should not be
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros")

  message("Setting warning level for GCC")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

  # Boost.Config says that variadic macros are available when they should not be
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-variadic-macros")

  message("Setting warning level for Clang")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  subdirs(libs)
else()
  # Documentation generation is supported on Unix only
  subdirs(libs doc)
  
  # build the documentation website
  set(WEBSITE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/website)
  
  dir(${WEBSITE_ROOT})
  
  copy_file(${CMAKE_SOURCE_DIR} style.css ${WEBSITE_ROOT})
  
  set(WEBSITE_FILES ${WEBSITE_ROOT}/style.css)

  foreach(F index build)
    add_custom_command(
      OUTPUT ${WEBSITE_ROOT}/${F}.html
      COMMAND cat ${CMAKE_SOURCE_DIR}/${F}.html | sed 's/\\"libs\\//"/g' | sed 's/\\/html\\//\\//g' > ${WEBSITE_ROOT}/${F}.html
      DEPENDS ${CMAKE_SOURCE_DIR}/${F}.html ${WEBSITE_ROOT}
    )

    set(WEBSITE_FILES ${WEBSITE_FILES} ${WEBSITE_ROOT}/${F}.html)
  endforeach(F)
  
  # The libraries
  foreach(L metamonad metaparse safe_printf)
    set(DIR ${CMAKE_SOURCE_DIR}/libs/${L})
    file(GLOB FILES ${DIR}/doc/*.md)
    list(REMOVE_ITEM FILES ${DIR}/doc/index.md)

    dir(${WEBSITE_ROOT}/${L})
    
    foreach(F ${FILES})
      get_filename_component(FN_BASE ${F} NAME_WE)
      get_filename_component(P ${WEBSITE_ROOT}/${L} ABSOLUTE)

      set(FILENAME ${FN_BASE}.html)
      set(DST ${P}/${FILENAME})
      copy_file(
        ${DIR}/html
        ${FILENAME}
        ${P}
        ${L}_documentation
      )
      set(WEBSITE_FILES ${WEBSITE_FILES} ${DST})
    endforeach(F)

    add_custom_command(
      OUTPUT ${P}/index.html
      COMMAND cat ${DIR}/html/index.html | sed 's/\\.\\.\\/\\.\\.\\/\\.\\.\\/index\\.html/..\\/index.html/' > ${P}/index.html
      DEPENDS ${L}_documentation ${DIR}/html/index.html ${P}
    )
    set(WEBSITE_FILES ${WEBSITE_FILES} ${P}/index.html)
  
    copy_file(${DIR}/html style.css ${P})
    set(WEBSITE_FILES ${WEBSITE_FILES} ${P}/style.css)

  endforeach(L)
  
  add_custom_target(website DEPENDS base_documentation ${WEBSITE_FILES})
endif()


# Installation
macro(install_with_dir PREFIX_TO_CUT)
 
 string(LENGTH ${PREFIX_TO_CUT} PREFIX_LEN)

 foreach(F ${ARGN})
   string(REGEX MATCH "(.*)[/\\]" DIR ${F})
   string(LENGTH ${DIR} DIR_LEN)
   math(EXPR DIR_KEEP "${DIR_LEN} - ${PREFIX_LEN}")
   string(SUBSTRING ${DIR} ${PREFIX_LEN} ${DIR_KEEP} DIR2)
   install(FILES ${F} DESTINATION include/${DIR2})
 endforeach(F)
          
endmacro(install_with_dir)

set(CMAKE_INSTALL_PREFIX "/usr")

file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/mpllibs/*.hpp)
install_with_dir(${CMAKE_CURRENT_SOURCE_DIR} ${HEADER_FILES})

# Debian package
if(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")
  include(InstallRequiredSystemLibraries)
   
  set(CPACK_SET_DESTDIR "on")
  set(CPACK_PACKAGING_INSTALL_PREFIX "/tmp")
  set(CPACK_GENERATOR "DEB")
   
  set(CPACK_PACKAGE_DESCRIPTION "Mpllibs")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ template metaprogramming library collection")
  set(CPACK_PACKAGE_VENDOR "Abel Sinkovics")
  set(CPACK_PACKAGE_CONTACT "abel@sinkovics.hu")
  set(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
  set(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
  set(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
  set(CPACK_PACKAGE_FILE_NAME "lib${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}-dev")
  set(CPACK_SOURCE_PACKAGE_FILE_NAME "lib${CMAKE_PROJECT_NAME}-${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}-dev")

  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libboost-dev")
   
  set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
  set(CPACK_DEBIAN_PACKAGE_SECTION "devel")
  set(CPACK_DEBIAN_ARCHITECTURE "all") # Header-only
   
  set(CPACK_COMPONENTS_ALL headers)
  include(CPack)
   
endif(EXISTS "${CMAKE_ROOT}/Modules/CPack.cmake")



