#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
using namespace std;
#pragma comment(lib,"ws2_32.lib")
int main()
{
	WORD versionRequested;
	WSADATA wsadata;
	int err;
	versionRequested = MAKEWORD(2, 2);
	err = WSAStartup(versionRequested, &wsadata);
	if (err != 0)
	{
		return 0;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
	{
		WSACleanup();
		return 0;
	}
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		cout << "socket´´½¨Ê§°Ü" << WSAGetLastError() << endl;
		return 0;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.0.107");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port =6001;
	int err1 = bind(sock, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
	if (err1 == SOCKET_ERROR)
	{
		cout << "°ó¶¨Ê§°Ü" << endl;
		return 0;
	}
	/*
	SOCKADDR_IN addrCon;
	addrCon.sin_addr.S_un.S_addr = inet_addr("192.168.0.107");
	addrCon.sin_port = 6001;
	addrCon.sin_family = AF_INET;
	*/
	while (1)
	{
		char s[50];
		int n=recv(sock, s, sizeof(s), 0);
		if (n > 0)
		{
			cout << s << endl;
		}
	}
	return 0; 
}
