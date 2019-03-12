# coding:utf-8
import socket
import re

from multiprocessing import Process

HTML_ROOT_DIR="./ht"

def handle_client(client_socket):
    """
    处理客户端请求
    """
    request_data=client_socket.recv(1024)
    print("request data\n"+request_data.decode("utf-8"))

    request_lines=request_data.decode("utf-8").splitlines()

    request_lines_start=request_lines[0]
    print(request_lines_start)
    # 提取文件名
    file_name=re.match(r"\w+ +(/[^ ]*) ",request_lines_start).group(1)
    # 打开文件，读取内容
    print(file_name)
    try:
        file=open(HTML_ROOT_DIR+file_name,"r")
    except:
        response_start_line="HTTP/1.1 404 NOT FOUND\r\n"
        response_headers="Server: MyServer\r\n"
        response_body="File Not found"
    else:
        file_date = file.read().encode("utf-8")
        file.close()
        # 构造响应体
        response_start_line="HTTP/1.1 200 OK\r\n"
        response_headers="Server: MyServer\r\n"
        response_body=file_date.decode("utf-8")
    # 形成响应报文
    response=response_start_line+response_headers+"\r\n"+response_body

    # 发信息
    client_socket.send(bytes(response,"utf-8"))
    # 关闭链接
    client_socket.close()


if __name__=="__main__":
    # 1 建立一个tcp socket服务器
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET,socket.SO_REUSEADDR,1)
    server_socket.bind(("", 8000))
    server_socket.listen(128)

while True:
    client_socket,client_address = server_socket.accept()
    print("%s %s 连接了" % client_address)
    handle_process=Process(target=handle_client,args=(client_socket,))
    handle_process.start()
    client_socket.close()