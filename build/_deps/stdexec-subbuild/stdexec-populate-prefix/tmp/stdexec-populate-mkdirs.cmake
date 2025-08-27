# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src")
  file(MAKE_DIRECTORY "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src")
endif()
file(MAKE_DIRECTORY
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix/tmp"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix/src/stdexec-populate-stamp"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix/src"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix/src/stdexec-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix/src/stdexec-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-subbuild/stdexec-populate-prefix/src/stdexec-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
