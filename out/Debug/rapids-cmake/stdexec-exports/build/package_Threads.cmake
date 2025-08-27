#=============================================================================
# find_dependency Search for Threads
#
# Make sure we search for a build-dir config module for the project
set(possible_package_dir "")
if(possible_package_dir AND NOT DEFINED Threads_DIR)
  set(Threads_DIR "${possible_package_dir}")
endif()

find_dependency(Threads)

if(possible_package_dir)
  unset(possible_package_dir)
endif()
#=============================================================================
