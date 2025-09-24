set_languages("c++latest")
-- add_requires("msgpack-c")
add_requires("boost", {configs = {thread = true, system = true, serialization = true, context = true, asio = true}})
add_requires("spdlog")
add_requires("stdexec-git")
add_requires("reflect-cpp", {configs = {msgpack = true}})
add_requires("openssl3")
if is_plat("linux") then
  add_requires("gtk4", {system = true})
  add_requires("webkitgtk-6.0", {system = true})
  set_toolchains("gcc")
end

if is_plat("windows") then
  add_requires("webview2-nuget 1.0.3485.44", {system = false})
  set_toolchains("llvm")
end

add_requires("webview-git")


target("Client")
  if is_plat("windows") then
    set_runtimes("MD")
  end
  add_includedirs("include", { public = false })
  add_files("src/main.cpp", "src/**/*.cpp")
  add_packages("openssl3", "boost", "stdexec-git", "webview-git", "reflect-cpp", "spdlog")

-- Headless stdio bridge used by Electron main process
target("ChatDaemon")
  set_kind("binary")
  set_targetdir("bin")
  if is_plat("windows") then
    set_runtimes("MD")
  end
  add_includedirs("include", { public = false })
  add_files("src/daemon.cpp", "src/Utils/Logger.cpp", "src/Async/Loop.cpp")
  add_packages("openssl3", "boost", "stdexec-git", "reflect-cpp", "spdlog")

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
    assert(package:has_cxxincludes("stdexec/execution.hpp", { configs = { languages = "c++23" } }))
    -- assert(package:has_cxxincludes("asioexec/use_sender.hpp", { configs = { languages = "c++23" } }))
    -- assert(package:has_cxxincludes("execpools/asio/asio_thread_pool.hpp", { configs = { languages = "c++23" } }))
  end)

package("webview-git")
    set_homepage("https://github.com/webview/webview")
    set_description("Tiny cross-platform webview library for C/C++ to build modern desktop GUIs.")
    set_license("MIT")

    add_urls("https://github.com/webview/webview.git")
    add_versions("latest", "master")

    -- 允许选择 WebKitGTK API
    add_configs("webkitgtk_api", {
        description = "Select WebKitGTK API version (6.0 / 4.1 / 4.0)",
        default = "6.0",
        values = { "6.0", "4.1", "4.0" }
    })

    -- （可选）是否构建共享库
    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean"})

    set_policy("package.cmake_generator.ninja", false)

    add_deps("cmake")

    -- 通用 on_load：根据平台和配置追加依赖/链接
    on_load(function (package)
        local plat = package:plat()
        local api  = package:config("webkitgtk_api")

        if plat == "linux" then
            -- 依据 API 选择系统依赖 (假设这些已经由 xmake or 系统 pkg-config 提供)
            if api == "6.0" then
                -- WebKitGTK 6.0 + GTK4
                package:add("deps", "gtk4", "webkitgtk-6.0", {system = true})
            elseif api == "4.1" then
                -- WebKitGTK 4.1 + GTK3
                package:add("deps", "gtk3", "webkit2gtk-4.1", {system = true})
            else
                -- WebKitGTK 4.0 + GTK3
                package:add("deps", "gtk3", "webkit2gtk-4.0", {system = true})
            end
            package:add("links", "dl")
        elseif plat == "windows" then
            -- 依赖 webview2 nuget 包
            package:add("deps", "webview2-nuget 1.0.3485.44", {system = false})
            package:add("syslinks", "advapi32", "ole32", "shell32", "shlwapi", "user32", "version")
        elseif plat == "macosx" then
            package:add("frameworks", "WebKit")
            package:add("links", "dl")
        elseif plat == "mingw" or plat == "msys" then
            package:add("syslinks", "advapi32", "ole32", "shell32", "shlwapi", "user32", "version")
        end
    end)

    on_install("windows", "linux", "macosx", "mingw", "msys", function (package)
        local configs = {
            "-DWEBVIEW_BUILD_TESTS=OFF",
            "-DWEBVIEW_BUILD_EXAMPLES=OFF",
            "-DWEBVIEW_BUILD_DOCS=OFF",
            "-DWEBVIEW_INSTALL_TARGETS=ON",
            "-DWEBVIEW_ENABLE_CHECKS=OFF",
            "-DWEBVIEW_BUILD_AMALGAMATION=OFF"
        }

        -- 共享 / 静态（上游有对应选项）
        if package:config("shared") then
            table.insert(configs, "-DWEBVIEW_BUILD_SHARED_LIBRARY=ON")
            table.insert(configs, "-DWEBVIEW_BUILD_STATIC_LIBRARY=OFF")
        else
            table.insert(configs, "-DWEBVIEW_BUILD_SHARED_LIBRARY=OFF")
            table.insert(configs, "-DWEBVIEW_BUILD_STATIC_LIBRARY=ON")
        end

        table.insert(configs, "-DCMAKE_BUILD_TYPE=" .. (package:debug() and "Debug" or "Release"))

        if package:is_plat("linux") then
            -- 根据 API 确定模块名（对应 CMake 脚本里的判断）
            local api = package:config("webkitgtk_api")
            local module
            if api == "6.0" then
                module = "webkitgtk-6.0"
            elseif api == "4.1" then
                module = "webkit2gtk-4.1"
            else
                module = "webkit2gtk-4.0"
            end
            -- 强制指定模块，避免被默认首选列表覆盖
            table.insert(configs, "-DWEBVIEW_WEBKITGTK_MODULE_NAME=" .. module)
        end

        import("package.tools.cmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:has_cxxincludes("webview/webview.h", {configs = {languages = "c++14"}}))
    end)

package("webview2-nuget")
    -- 设置包的主页和描述
    set_homepage("https://developer.microsoft.com/en-us/microsoft-edge/webview2/")
    set_description("Build web-powered apps with Microsoft Edge WebView2")

    -- WebView2 是 Windows-only 的
    set_plat("windows")

    -- 添加从 NuGet 获取源码的 URL
    -- 我们通过在文件名后添加 .zip 后缀，让 xmake 知道 nupkg 文件本质上是 zip 压缩包
    add_urls("https://www.nuget.org/api/v2/package/Microsoft.Web.WebView2/1.0.3485.44", {filename = "Microsoft.Web.WebView2.zip"})

    add_versions("1.0.3485.44", "bc09150b179246ac90189649b13be8e6b11b3ac200e817e18df106e1f3cf489e")

    -- 定义安装过程
    -- 当 xmake 下载并解压源码后，会执行此函数
    on_install(function (package)
        -- 将头文件拷贝到包的安装目录的 include 文件夹下
        os.cp("build/native/include/WebView2.h", package:installdir("include"))

        -- 将不同架构的静态链接库拷贝到对应的 lib 文件夹下
        -- package:arch() 会获取当前编译的架构，如 "x64", "x86", "arm64"
        local arch = package:arch()
        if arch == "x86" then
            -- nuget 包里的 32 位架构文件夹名是 x86
            os.cp("build/native/x86/WebView2LoaderStatic.lib", package:installdir("lib"))
        elseif arch == "x64" then
            os.cp("build/native/x64/WebView2LoaderStatic.lib", package:installdir("lib"))
        elseif arch == "arm64" then
            os.cp("build/native/arm64/WebView2LoaderStatic.lib", package:installdir("lib"))
        end
    end)
    on_test(function(package)
      assert(package:has_cxxincludes("WebView2.h", { configs = { languages = "c++14" } }))
    end)
