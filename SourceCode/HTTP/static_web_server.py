# coding:utf-8
import socket

from multiprocessing import Process


def handle_client(client_socket):
    """
    处理客户端请求
    """
    request_data=client_socket.recv(1024)
    print("request data"+str(request_data))

    # 构造响应体
    response_start_line="HTTP/1.1 200 OK\r\n"
    response_headers="Server: MyServer\r\n"
    response_body="Hello World"
    response=response_start_line+response_headers+"\r\n"+response_body

    # 发信息
    client_socket.send(bytes(response,"utf-8"))
    # 关闭链接
    client_socket.close()


if __name__=="__main__":
    # 1 建立一个tcp socket服务器
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(("", 8000))
    server_socket.listen(128)

while True:
    client_socket,client_address = server_socket.accept()
    print("%s %s 连接了" % client_address)
    handle_process=Process(target=handle_client,args=(client_socket,))
    handle_process.start()
    client_socket.close()