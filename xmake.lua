set_languages("c++latest")
-- Deprecated: webview and stdio daemon are removed in favor of native Electron addon.
-- Keep packages for potential reuse.
add_requires("boost", {configs = {thread = true, system = true, serialization = true, context = true, asio = true}})
add_requires("spdlog")
add_requires("stdexec-git")
add_requires("reflect-cpp", {configs = {msgpack = true}})
add_requires("openssl3")
add_requires("node-addon-api")

target("native")
  add_rules("nodejs.module")
  add_files("src/*/*.cpp")
  add_includedirs("include", {public = true})
  add_packages("boost", "spdlog", "stdexec-git", "reflect-cpp", "openssl3", "node-addon-api")

package("stdexec-git")
  set_kind("library", { headeronly = true })
  set_homepage("https://github.com/NVIDIA/stdexec")
  set_description("`std::execution`, the proposed C++ framework for asynchronous and parallel programming. ")
  set_license("Apache-2.0")

  add_urls("https://github.com/Perdixky/stdexec.git")

  add_versions("latest", "main")

  set_policy("package.cmake_generator.ninja", false)

  add_deps("cmake")

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
    assert(package:has_cxxincludes("stdexec/execution.hpp", { configs = { languages = "c++23" } }))
    -- assert(package:has_cxxincludes("asioexec/use_sender.hpp", { configs = { languages = "c++23" } }))
    -- assert(package:has_cxxincludes("execpools/asio/asio_thread_pool.hpp", { configs = { languages = "c++23" } }))
  end)
