set_languages("c++latest")
-- add_requires("msgpack-c")
add_requires("python", {system = true})
add_requires("boost", {system = false, configs = {shared = true, system = true, serialization = true, context = true, asio = true, log = true, log_setup = true, thread = true}})
add_requires("stdexec-git")
add_requires("reflect-cpp", {configs = {msgpack = true}})
add_requires("openssl3")
if is_plat("linux") then
  add_requires("gtk4", {system = true})
  add_requires("webkitgtk-6.0", {system = true})
end
add_requires("webview-git")


target("Client")
add_includedirs("include", { public = false })
add_files("src/main.cpp", "src/**/*.cpp")
add_packages("python", "openssl3", "boost", "stdexec-git", "webview-git", "reflect-cpp")

package("stdexec-git")
  set_kind("library", { headeronly = true })
  set_homepage("https://github.com/NVIDIA/stdexec")
  set_description("`std::execution`, the proposed C++ framework for asynchronous and parallel programming. ")
  set_license("Apache-2.0")

  add_urls("https://github.com/Perdixky/stdexec.git")

  add_versions("latest", "main")

  set_policy("package.cmake_generator.ninja", false)

  add_deps("cmake", "boost")

  if on_check then
    on_check("windows", function(package)
      import("core.base.semver")

      local vs_toolset = package:toolchain("msvc"):config("vs_toolset")
      assert(vs_toolset and semver.new(vs_toolset):minor() >= 30, "package(stdexec): need vs_toolset >= v143")
    end)
  end

  on_install("windows", "linux", "macosx", "mingw", "msys", function(package)
    if package:has_tool("cxx", "cl") then
      package:add("cxxflags", "/Zc:__cplusplus", "/Zc:preprocessor")
    end

    local configs = { "-DSTDEXEC_BUILD_EXAMPLES=OFF", "-DSTDEXEC_BUILD_TESTS=OFF", "-DSTDEXEC_ENABLE_ASIO=ON" }
    table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
    table.insert(configs, "-DBUILD_SHARED_LIBS=" .. (package:config("shared") and "ON" or "OFF"))
    import("package.tools.cmake").install(package, configs)
  end)

  on_test(function(package)
    assert(package:has_cxxincludes("asioexec/use_sender.hpp", { configs = { languages = "c++23" } }))
    assert(package:has_cxxincludes("stdexec/execution.hpp", { configs = { languages = "c++23" } }))
    assert(package:has_cxxincludes("execpools/asio/asio_thread_pool.hpp", { configs = { languages = "c++23" } }))
  end)

package("webview-git")
  set_homepage("https://github.com/webview/webview")
  set_description("Tiny cross-platform webview library for C/C++ to build modern desktop GUIs.")
  set_license("MIT")

  add_urls("https://github.com/webview/webview.git")

  add_versions("latest", "master")

  set_policy("package.cmake_generator.ninja", false)

  add_deps("cmake")
  if is_plat("linux") then
    add_deps("gtk4", "webkitgtk-6.0", {system = true})
  end

  -- on_load 会在最终用户项目加载此包时执行，用于传递链接信息等
  on_load(function(package)
    -- macOS 需要链接 WebKit.framework
    if is_plat("macosx") then
      package:add("frameworks", "WebKit")
      package:add("links", "dl") -- 根据文档，macOS 也需要链接 dl
    end
    -- Windows 需要链接一系列系统库
    if is_plat("windows") then
      package:add("syslinks", "advapi32", "ole32", "shell32", "shlwapi", "user32", "version")
    end
    -- Linux 需要链接 dl 库
    if is_plat("linux") then
      package:add("links", "dl")
    end
  end)

  on_install("windows", "linux", "macosx", "mingw", "msys", function(package)
    local configs = {
      "-DWEBVIEW_BUILD_TESTS=OFF",
      "-DWEBVIEW_BUILD_EXAMPLES=OFF",
      "-DWEBVIEW_BUILD_DOCS=OFF",
      "-DWEBVIEW_INSTALL_TARGETS=ON",
      "-DWEBVIEW_BUILD_STATIC_LIBRARY=ON",
      "-DWEBVIEW_BUILD_SHARED_LIBRARY=OFF",
      "-DWEBVIEW_ENABLE_CHECKS=OFF",
      "-DWEBVIEW_BUILD_AMALGAMATION=OFF"
    }
    table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))
    import("package.tools.cmake").install(package, configs)
  end)
  
  on_test(function(package)
    assert(package:has_cxxincludes("webview/webview.h", { configs = { languages = "c++14" } }))
  end)
