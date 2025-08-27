# Install script for directory: /home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src

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

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/perdixky/Codes/SimpleChat/Client/build/_deps/rapids-cmake-build/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exec" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/any_sender_of.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/async_scope.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/at_coroutine_exit.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/create.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/env.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/finally.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/fork_join.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/inline_scheduler.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/into_tuple.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/just_from.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/libdispatch_queue.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/materialize.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/on.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/on_coro_disposition.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/repeat_effect_until.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/repeat_n.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/reschedule.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/scope.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence_senders.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/single_thread_context.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/start_now.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/static_thread_pool.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/system_context.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/task.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/timed_scheduler.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/timed_thread_scheduler.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/trampoline_scheduler.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/variant_sender.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/when_any.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exec/__detail" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__atomic_intrusive_queue.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__atomic_ref.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__basic_sequence.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__bit_cast.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__bwos_lifo_queue.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__numa.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__system_context_default_impl.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__system_context_default_impl_entry.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__system_context_replaceability_api.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/__xorshift.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/__detail/intrusive_heap.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exec/linux" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/linux/io_uring_context.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/linux/memory_mapped_region.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/linux/safe_file_descriptor.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exec/linux/__detail" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/linux/__detail/memory_mapped_region.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/linux/__detail/safe_file_descriptor.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exec/sequence" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence/any_sequence_of.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence/empty_sequence.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence/ignore_all_values.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence/iterate.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/sequence/transform_each.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/exec/windows" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/windows/filetime_clock.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/exec/windows/windows_thread_pool.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/execpools" TYPE FILE FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/execpools/thread_pool_base.hpp")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/stdexec" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/concepts.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/coroutine.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/execution.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/functional.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/stop_token.hpp"
    )
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/stdexec/__detail" TYPE FILE FILES
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__any_receiver_ref.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__as_awaitable.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__awaitable.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__basic_sender.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__bulk.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__completion_signatures.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__concepts.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__config.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__connect_awaitable.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__continues_on.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__cpo.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__debug.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__diagnostics.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__domain.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__ensure_started.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__env.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__execute.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__execution_fwd.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__execution_legacy.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__force_include.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__inline_scheduler.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__into_variant.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__intrusive_mpsc_queue.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__intrusive_ptr.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__intrusive_queue.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__intrusive_slist.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__just.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__let.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__manual_lifetime.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__meta.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__on.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__operation_states.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__optional.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__preprocessor.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__ranges.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__read_env.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__receiver_adaptor.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__receiver_ref.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__receivers.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__run_loop.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__schedule_from.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__schedulers.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__scope.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__sender_adaptor_closure.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__sender_introspection.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__senders.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__senders_core.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__shared.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__spin_loop_pause.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__split.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__start_detached.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__starts_on.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__stop_token.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__stopped_as_error.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__stopped_as_optional.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__submit.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__sync_wait.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__tag_invoke.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__then.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__transfer_just.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__transform_completion_signatures.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__transform_sender.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__tuple.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__type_traits.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__unstoppable.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__upon_error.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__upon_stopped.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__utility.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__variant.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__when_all.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__with_awaitable_senders.hpp"
    "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-src/include/stdexec/__detail/__write_env.hpp"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/include/stdexec_version_config.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/libsystem_context.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "stdexec" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec/stdexec-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec/stdexec-targets.cmake"
         "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/CMakeFiles/Export/6b4098dc7f0643ed4f67fee387cf87ba/stdexec-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec/stdexec-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec/stdexec-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec" TYPE FILE FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/CMakeFiles/Export/6b4098dc7f0643ed4f67fee387cf87ba/stdexec-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec" TYPE FILE FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/CMakeFiles/Export/6b4098dc7f0643ed4f67fee387cf87ba/stdexec-targets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "stdexec" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/stdexec" TYPE DIRECTORY FILES "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/rapids-cmake/stdexec/export/stdexec/")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/perdixky/Codes/SimpleChat/Client/build/_deps/stdexec-build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
