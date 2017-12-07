# TCP协议 网络服务器和编程实现   
&ensp;&ensp;TCP 是面向连接的稳定的传输的协议，不同于UDP，TCP只有建立连接后，才可以发送数据。相比UDP，TCP服务器端的   
程序多了监听 `(lisen)`步骤以及等待客服端的连接`accept`步骤。   
## 服务器端的建立   
---------------------
1 加载套接字库   
```C++
WORD wVersionrequsted;
	WSADATA wsadata;
	wVersionrequsted = MAKEWORD(2, 2);
	int err1 = WSAStartup(wVersionrequsted, &wsadata);
	if (err1 != 0)
	{
		cout << "加载套接字库失败" << endl;
		return 0;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion != 2))
	{
		WSACleanup();
		return 0;
	}
```
--------------------------
2 创建套接字   
```C++
SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "调用sock失败" << endl;
		return 0;
	}
```
----------------------
3 绑定套接字   
```C++
SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = (htons)(9000);

	int err2 = bind(sock, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	if (err2 == SOCKET_ERROR)
	{
		cout << "绑定失败" << endl;
		return 0;
	}
```
-------------------------------
4 监听套接字   
```C++
listen(sock, 5);//其中，sock表示绑定的套接字
                //5表示，允许接受客户端的最大数量
```
---------------------------
5 处理客户端的请求
```C++
int len=sizeof(SOCKADDR);
SOCKET sockConn = accept(sock, (SOCKADDR *)&addrClient, &len);
```
--------------------
6 发送和接受数据
```C++
    char sendBuf[50];
		cin >>sendBuf;
		send(sockConn, s, strlen(s)+1, 0);

		char recvBuff[50];
		recv(sockConn, recvBuff, sizeof(recvBuff), 0);
		cout << recvBuff << endl;
		
```
------------------
7 关闭连接和释放套接字   
```C++
closesocket(sockConn);
WSACleanup();
```
### 至此，服务器端建立成功   

#### 客户端的建立   

------------------------
1 加载套接字库

---------------------

2 创建套接字

--------------------

3 建立与服务器端的连接   

-----------------------------
```C++
connect(sockClient, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
````
------------------------------
4 发送和接受数据
```C++
char recvBuff[50];
recv(sockClient, recvBuff, sizeof(recvBuff), 0);

har sendBuff[50];
send(sockClient, sendBuff, strlen(sendBuff) + 1, 0);
```
----------------------
5 关闭连接和释放套接字   

-----------------------
