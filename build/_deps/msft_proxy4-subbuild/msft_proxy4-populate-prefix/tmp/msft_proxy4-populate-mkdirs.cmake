# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src")
  file(MAKE_DIRECTORY "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-src")
endif()
file(MAKE_DIRECTORY
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-build"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix/tmp"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix/src/msft_proxy4-populate-stamp"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix/src"
  "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix/src/msft_proxy4-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix/src/msft_proxy4-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/perdixky/Codes/SimpleChat/Client/build/_deps/msft_proxy4-subbuild/msft_proxy4-populate-prefix/src/msft_proxy4-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
