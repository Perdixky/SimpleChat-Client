# Install script for directory: /home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/proxy" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/proxy.h"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/proxy_macros.h"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/proxy_fmt.h"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/proxy/v4" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/v4/proxy.ixx"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/v4/proxy.h"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/v4/proxy_macros.h"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src/include/proxy/v4/proxy_fmt.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/msft_proxy4/msft_proxy4Targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/msft_proxy4/msft_proxy4Targets.cmake"
         "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-build/CMakeFiles/Export/0d99fc5f7230140cc773567a9c3dbaa5/msft_proxy4Targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/msft_proxy4/msft_proxy4Targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/share/msft_proxy4/msft_proxy4Targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/msft_proxy4" TYPE FILE FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-build/CMakeFiles/Export/0d99fc5f7230140cc773567a9c3dbaa5/msft_proxy4Targets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/msft_proxy4" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-build/cmake/msft_proxy4Config.cmake"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-build/cmake/msft_proxy4ConfigVersion.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
