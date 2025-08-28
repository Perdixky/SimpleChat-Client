cmake_minimum_required(VERSION 3.30) # 或者更高，例如 3.10.0, 3.15.0 等
set(CMAKE_CXX_STANDARD 23) # 设置 C++ 标准为 C++20
project(SimpleChat CXX) # 建议指定语言

include(cmake/CPM.cmake)
# 现在 CPMAddPackage 命令就可用了
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

# CPMAddPackage(
#   NAME OpenSSL
#   GIT_REPOSITORY https://github.com/openssl/openssl.git
#   GIT_TAG master
# )

find_package(OpenSSL REQUIRED)

CPMAddPackage(
  NAME Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG master
)

CPMAddPackage(
  NAME fmtlog
  GITHUB_REPOSITORY MengRao/fmtlog
  GIT_TAG main
)

# CPMAddPackage(
#   NAME reflectcpp
#   GITHUB_REPOSITORY getml/reflect-cpp
#   GIT_TAG main
# )
# 收集源文件
# 确保 'src/' 目录下有 .cpp 文件
file(GLOB_RECURSE source_files PRIVATE "src/*.cpp")
add_executable(main ${source_files}) # 修正：使用 ${source_files} 解引用变量
target_include_directories(main PRIVATE "include")

# 链接库
target_link_libraries(main
  PRIVATE STDEXEC::stdexec # 根据 stdexec 的 CMakeLists.txt 提供的目标名称
  PRIVATE ftxui::screen
  PRIVATE ftxui::component
  PRIVATE Boost::system
  PRIVATE Boost::asio
  # PRIVATE reflectcpp
  # PRIVATE fmt::fmt
  PRIVATE fmtlog-static
  PRIVATE TBB::tbb
  PRIVATE OpenSSL::SSL
)

add_executable(test test.cpp)
target_include_directories(test PRIVATE "include")
target_link_libraries(test
  PRIVATE STDEXEC::stdexec # 根据 stdexec 的 CMakeLists.txt 提供的目标名称
  PRIVATE ftxui::screen
  PRIVATE ftxui::component
  PRIVATE Boost::system
  # PRIVATE reflectcpp
  # PRIVATE fmt::fmt
  PRIVATE fmtlog-static
  PRIVATE TBB::tbb
  PRIVATE OpenSSL::SSL
)

if (CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
  target_compile_options(main PRIVATE -fsanitize=address,undefined)
  target_link_options(main PRIVATE -fsanitize=address,undefined)
endif()
