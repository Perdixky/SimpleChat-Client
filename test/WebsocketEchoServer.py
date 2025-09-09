import asyncio
import websockets
import ssl
import pathlib
import json
import msgpack

# 证书和私钥文件的路径
CERT_FILE = "/home/perdixky/Codes/SimpleChat-Client/safety/certificate.crt"
KEY_FILE = "/home/perdixky/Codes/SimpleChat-Client/safety/private.key"

# 服务器监听的主机和端口
HOST = "localhost"
PORT = 8080

# --- 最终修正的函数 ---
async def echo(websocket):
    """
    处理新的 WebSocket 连接。
    
    此函数针对 websockets v15.0.1 及更高版本的行为编写：
    1. 它接受单个 'websocket' 参数，这符合 v10.0 以来的标准。
    2. 它不依赖于在此环境中缺失的 'websocket.path' 属性。
    3. 它从 'websocket.request_headers' 手动解析路径，这是最可靠的后备方案。
    """
    # path = "未知路径"
    # try:
    #     # HTTP/2 和 HTTP/1.1 的 WebSocket 握手都会在请求头中包含路径信息。
    #     # 对于现代实现，它通常存储在 ':path' 这个伪头中。
    #     if websocket.request_headers and ':path' in websocket.request_headers:
    #         path = websocket.request_headers[':path']
    #     else:
    #         # 如果没有找到 ':path'，可以打印所有头信息以供调试，找出路径存储的位置
    #         print(f"DEBUG: 未找到':path'伪头, 所有头信息: {websocket.request_headers}")
    #
    # except Exception as e:
    #     print(f"无法从请求头中解析路径: {e}")

    # print(f"新连接来自: {websocket.remote_address} (请求路径: {path})")
    
    async for message in websocket:
        try:
            print(f"收到来自 {websocket.remote_address} 的消息: {message}")
            data = msgpack.unpackb(message, raw=False)
            method = data.get("method")

            if method == "Echo":
                await websocket.send(message)
                print(f"已将消息 '{message}' 回送给 {websocket.remote_address}")
            elif method == "SignIn":
                response = {
                    "id": data.get("id"),
                    "success": True
                }
                print(f"处理 SignIn 请求, 准备发送确认: {msgpack.packb(response)}")
                await websocket.send(msgpack.packb(response))
                print(f"已发送 SignIn 确认给 {websocket.remote_address}")
            else:
                await websocket.send(message)
                print(f"未知 method, 已将原消息 '{message}' 回送给 {websocket.remote_address}")

        except websockets.exceptions.ConnectionClosedOK:
            print(f"连接 {websocket.remote_address} 正常关闭。")
        except websockets.exceptions.ConnectionClosedError as e:
            print(f"连接 {websocket.remote_address} 异常关闭: {e}")
        except msgpack.UnpackException:
            print(f"收到的消息不是有效的Msgpack格式: {message}")
            await websocket.send(message) # Or send an error message
        except Exception as e:
            print(f"处理 {websocket.remote_address} 连接时发生错误: {e}")
        finally:
            print(f"连接 {websocket.remote_address} 已断开。")

async def main():
    """
    主函数：配置 SSL 上下文并启动 WSS 服务器。
    """
    print(f"正在配置 SSL 上下文...")
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)

    cert_path = pathlib.Path(CERT_FILE)
    key_path = pathlib.Path(KEY_FILE)

    if not cert_path.is_file():
        raise FileNotFoundError(f"证书文件不存在: {cert_path.resolve()}")
    if not key_path.is_file():
        raise FileNotFoundError(f"私钥文件不存在: {key_path.resolve()}")
        
    ssl_context.load_cert_chain(str(cert_path), str(key_path))
    print(f"SSL 上下文配置成功。")

    print(f"正在启动 WSS Echo 服务器在 wss://{HOST}:{PORT}")
    async with websockets.serve(
        echo,
        HOST,
        PORT,
        ssl=ssl_context  # 使用与 asyncio 兼容的 'ssl' 参数
    ) as server:
        await asyncio.Future()

if __name__ == "__main__":
    try:
        print(f"正在使用的 websockets 版本: {websockets.__version__}")
        asyncio.run(main())
    except KeyboardInterrupt:
        print("\n服务器已停止。")
    except Exception as e:
        print(f"服务器启动失败: {e}")
