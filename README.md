<div align="center">
  <img src="SimpleChat.svg" width="200" />
</div>

# SimpleChat-Client

这是一个简单的聊天室客户端程序，使用 C++ 编写，旨在探索网络编程和客户端开发。

## 项目状态

⚠️ 请注意：本项目目前正处于早期开发阶段，许多功能尚未实现或不完整。这仅仅是一个用于学习和实践的半成品，需要更多时间来开发和完善。

## 开发与运行（Electron + xmake 原生模块）

本项目客户端使用 Electron + 原生 C++ Addon（Node-API）。UI 从 `electron/index.html` 加载；渲染层通过 `window.bridge.invoke(name, ...args)` 调用，由主进程转发到 C++ 的 `Session` 方法（PascalCase）。

### 先决条件
- Node.js 与 npm
- C/C++ 工具链（gcc/clang 或 MSVC）
- OpenSSL 开发头/库（Linux 通常为 `libssl-dev`）
- xmake（用于构建原生模块）

### 构建与启动（开发模式）
```bash
# 构建 Debug 原生模块
xmake f -m debug && xmake build

# 启动 Electron 应用
cd electron
npm start
```

### 打包（electron-builder）
```bash
# 构建 Release 原生模块
xmake f -m release && xmake build

# 打包应用（prepack 会将 native.node 复制到 electron/）
cd electron
npm run pack
```
打包逻辑：
- `electron/scripts/prepare-native.js` 会把 `build/<os>/<arch>/release/native.node` 复制到 `electron/native.node`
- `electron-builder` 会把它打进 `resources/native.node`
- 运行时主进程优先从 `resources/native.node` 加载

### 说明
- 后端 TLS CA 路径在 `include/Network/Connection.hpp` 配置；请按本机证书调整。
- 旧的 node-gyp 流程与构建产物已移除，统一改为 xmake 构建。
- 旧的 WebView/stdio 守护版本已废弃；仅保留 Electron 原生模块方案。
