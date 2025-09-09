<div align="center">
  <img src="Chat.svg" width="200" />
</div>

# SimpleChat-Client

这是一个简单的聊天室客户端程序，使用 C++ 编写，旨在探索网络编程和客户端开发。

## ⚠️ 项目状态

请注意：本项目目前正处于早期开发阶段，许多功能尚未实现或不完整。这仅仅是一个用于学习和实践的半成品，需要更多时间来开发和完善。

## 编译指南

本项目使用 CMake 进行构建。在编译之前，请确保您的系统上已安装 C++ 编译器 (如 GCC, Clang) 和 CMake。

### 编译步骤

1.  **克隆仓库**
    ```bash
    git clone https://github.com/Perdixky/SimpleChat-Client.git SimpleChat-Client
    cd SimpleChat-Client
    ```

2.  **创建构建目录**
    我们建议在项目根目录之外创建一个构建目录，以保持源码树的清洁。
    ```bash
    mkdir build
    cd build
    ```

3.  **运行 CMake 配置**
    此步骤会检查编译环境并生成构建系统。需要您本地拥有OpenSSL库，CMake配置会自动处理其他依赖项的下载（建议您在本地安装有Boost以防止长时间的拉取和编译过程）。
    ```bash
    cmake .. -G Ninja
    ```

4.  **编译项目**
    使用 CMake 的构建命令来编译项目。
    ```bash
    cmake --build .
    ```

5.  **运行**
    编译成功后，可执行文件将生成在 `build` 目录中。
