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

# 查找 Boost 库
# 确保 Boost 已安装在你的系统上，并且 CMake 能够找到它。
find_package(Boost CONFIG REQUIRED COMPONENTS system thread) # 建议使用大写 'Boost'
find_package(TBB REQUIRED)
find_package(OpenSSL REQUIRED)
# find_package(fmt REQUIRED)

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
target_compile_options(main PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
target_link_options(main PRIVATE -fsanitize=address,undefined)
