{
  "targets": [{
    "target_name": "session_addon",
    "sources": [
      "src/Native/addon.cpp",
      "src/Async/Loop.cpp",
      "src/Utils/Logger.cpp"
    ],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include\")",
      "include/",
      "/usr/include/boost",
      "<!@(pkg-config --cflags spdlog)"
    ],
    "libraries": [
      "<!@(pkg-config --libs spdlog)",
      "-lboost_system",
      "-lssl",
      "-lcrypto"
    ],
    "cflags!": [ "-fno-exceptions" ],
    "cflags_cc!": [ "-fno-exceptions" ],
    "cflags": [ "-std=c++20" ],
    "cflags_cc": [ "-std=c++20" ],
    "defines": [
      "NAPI_DISABLE_CPP_EXCEPTIONS",
      "NAPI_VERSION=6"
    ],
    "conditions": [
      ["OS=='linux'", {
        "libraries": [
          "-Wl,-rpath,/usr/lib/x86_64-linux-gnu"
        ]
      }]
    ]
  }]
}