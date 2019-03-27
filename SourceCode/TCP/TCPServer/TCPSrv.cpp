#include <WinSock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main()
{
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

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "调用sock失败" << endl;
		return 0;
	}

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

	listen(sock, 5);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	while (1)
	{

		SOCKET sockConn = accept(sock, (SOCKADDR *)&addrClient, 
				& len);

		char s[50];
		cin >> s;
		send(sockConn, s, strlen(s)+1, 0);

		char recvBuff[50];
		recv(sockConn, recvBuff, sizeof(recvBuff), 0);
		cout << recvBuff << endl;
	}
		closesocket(sockConn);
		WSACleanup();
	return 0;
}
