# 基于UDP协议的客户端和服务器端的程序编写   
## 服务器端程序   
---------------------------   
**1 加载套接字库**   
```C++   
WORD versionRequested;
WSADATA wsadata;//套接字版本的信息在这个结构体中
versionRequested = MAKEWORD(2, 2);//MAKEWORD方便获得version的正确值
int err = WSAStartup(versionRequested, &wsadata);//调用成功，则返回一个0
if (err != 0)
	{
		return 0;
	}
if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)//若版本不符合，则释放socket库的加载
	{
		WSACleanup();
		return 0;
	}
```
**WSADATA结构体的定义**   
```C++
typedef struct WSAData {
  WORD           wVersion;//低位字节版本
  WORD           wHighVersion;//高位字节版本
  char           szDescription[WSADESCRIPTION_LEN+1];
  char           szSystemStatus[WSASYS_STATUS_LEN+1];
  unsigned short iMaxSockets;//不适用
  unsigned short iMaxUdpDg;//不使用
  char FAR       *lpVendorInfo;
} WSADATA, *LPWSADATA;
```
--------------------------------------
**2 创建套接字**   
```C++
SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
if (sock == INVALID_SOCKET)
	{
		cout << "socket创建失败" << WSAGetLastError() << endl;
		return 0;
	}
```
**其中**   
```C++
SOCKET WSAAPI socket(
  _In_ int af,//地址组，对于TCP/IP来说，一般都是AF_INET
  _In_ int type,//套接字类型 UDP为SOCK_GRAME,TCP为SOCK_STREAM
  _In_ int protocol//与地址组相关的协议，若指定为0，则自适应套接字类型
  //若函数调用失败，返回一个INVALID_SOCKET.
);
```
---------------------------------
**3 绑定套接字到本地地址和端口上**   
```C++
SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr =(htonl)(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);
	int err1=bind(sock, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	if (err1 == SOCKET_ERROR)
	{
		cout << "绑定失败" << endl;
		return 0;
	}
```
**sockaddr&sockaddr_in结构体的定义**   
**用sockaddr—in代替sockaddr，方便填写**
```C++
struct sockaddr { 
        ushort sa_family; 
        char sa_data [14]; 
}; 

struct sockaddr_in { 
        short sin_family; //地址族，对于IP地址这里还是AF_INET
        u_short sin_port; //端口号
        struct in_addr sin_addr; //in_addr是一个联合体
                                 //主机IP地址，需要一个网络字节序的值，若希望唯一，可以用inet—addr来转换，这里转换用htonl
                                 //设置为INADDR_ANY，允许向任何分配给本机IP地址的主机发送信息。
        char sin_zero [8]; 
};
```
--------------------------------
**4 为连接的客户机创建一个套接字**
```C++
SOCKADDR_IN addrClient;
int len=sizeof(SOCKADDR);//这一步到下面不管是接受数据还是发送数据，都会用到。
```
---------------------------------
**5 发送或接受数据**
```C++
    char SendBuf[50];
		sendto(sock, s, strlen(s)+1, 0,(SOCKADDR *)&addrClient, len);
    char  recvBuf[50];
    recvfrom(sock,s,sizeof(recvBuf),(SOCKADDR *)&addrClient,&len);
```
------------------------------
**6 关闭连接，释放套接字**
```C++
closesocket(sockSrv);
WSACleanup();
```
-----------------------
### 至此，一个简单的服务端建立完毕
-------------------------------------
## 客户端的编程

**1 加载套接字库**

------------------------

**2 建立连接套接字**

------------------------
**3 发送和接受数据**

--------------------------
**4 关闭套接字**

------------------
