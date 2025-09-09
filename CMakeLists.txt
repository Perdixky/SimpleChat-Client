cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 23)
project(SimpleChat CXX) # 建议指定语言

include(cmake/CPM.cmake)

CPMAddPackage(
  NAME stdexec
  GITHUB_REPOSITORY NVIDIA/stdexec
  GIT_TAG main
  OPTIONS "STDEXEC_ENABLE_ASIO ON" "STDEXEC_BUILD_EXAMPLES OFF"
)

CPMAddPackage(
  NAME ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
  GIT_TAG main
)

CPMAddPackage(
  NAME TBB
  GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB.git
  GIT_TAG v2022.2.0
  OPTIONS "TBB_TEST OFF"
)

find_package(OpenSSL REQUIRED)

# CPMAddPackage(
#   NAME fmtlog
#   GITHUB_REPOSITORY MengRao/fmtlog
#   GIT_TAG main
# )

CPMAddPackage(
  NAME reflectcpp
  GITHUB_REPOSITORY getml/reflect-cpp
  GIT_TAG main
  OPTIONS "REFLECTCPP_MSGPACK ON"
)

# CPMAddPackage(
#   NAME Slint
#   GITHUB_REPOSITORY slint-ui/slint
#   GIT_TAG release/1
# )

CPMAddPackage(
  NAME webview
  GIT_REPOSITORY https://github.com/webview/webview
  GIT_TAG master
)
find_package(Boost REQUIRED COMPONENTS system thread log log_setup)

# 收集源文件
# 确保 'src/' 目录下有 .cpp 文件
file(GLOB_RECURSE source_files PRIVATE "src/*.cpp")
add_executable(main ${source_files}) # 修正：使用 ${source_files} 解引用变量
target_include_directories(main PRIVATE "include")
# slint_target_sources(main include/GUI/Login.slint)
# if(WIN32)
#   add_custom_command(TARGET main POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:my_application> $<TARGET_FILE_DIR:my_application> COMMAND_EXPAND_LISTS)
# endif()
target_compile_definitions(main PRIVATE REFLECTCPP_USE_STD_EXPECTED BOOST_LOG_DYN_LINK)
# 链接库
target_link_libraries(main
  PRIVATE STDEXEC::stdexec # 根据 stdexec 的 CMakeLists.txt 提供的目标名称
  PRIVATE ftxui::screen
  PRIVATE ftxui::component
  PRIVATE Boost::system
  PRIVATE Boost::log
  PRIVATE Boost::log_setup
  PRIVATE Boost::thread
  # PRIVATE Boost::asio
  # PRIVATE Boost::uuid
  PRIVATE reflectcpp::reflectcpp
  # PRIVATE fmt::fmt
  # PRIVATE fmtlog-static
  PRIVATE TBB::tbb
  PRIVATE OpenSSL::SSL
  PRIVATE webview::core
  # PRIVATE Slint::Slint
)

add_executable(test test.cpp)
target_include_directories(test PRIVATE "include")
target_link_libraries(test
  PRIVATE STDEXEC::stdexec # 根据 stdexec 的 CMakeLists.txt 提供的目标名称
  PRIVATE ftxui::screen
  PRIVATE ftxui::component
  PRIVATE Boost::system
  PRIVATE reflectcpp
  PRIVATE Boost::log
  PRIVATE Boost::log_setup
  PRIVATE Boost::thread
  # PRIVATE fmt::fmt
  # PRIVATE fmtlog-static
  PRIVATE TBB::tbb
  PRIVATE OpenSSL::SSL
  PRIVATE webview::core
  # PRIVATE Slint::Slint
)
# slint_target_sources(test include/GUI/Test.slint)
# if(WIN32)
#   add_custom_command(TARGET test POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:my_application> $<TARGET_FILE_DIR:my_application> COMMAND_EXPAND_LISTS)
# endif()
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" ON)
if (ENABLE_SANITIZERS AND CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message("Enabling AddressSanitizer and UndefinedBehaviorSanitizer")
    set(SAN_FLAGS -fsanitize=address,undefined -fno-omit-frame-pointer -O1 -g)
    target_compile_options(main PRIVATE ${SAN_FLAGS})
    target_link_options(main PRIVATE -fsanitize=address,undefined)
endif()
