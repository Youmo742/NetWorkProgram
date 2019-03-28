#define _WINSOCK_DEPRECATED_NO_WARNINGS
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
		cout << "¼ÓÔØÌ×½Ó×Ö¿âÊ§°Ü" << endl;
		return 0;
	}
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion != 2))
	{
		WSACleanup();
		return 0;
	}

	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	if (sockClient == INVALID_SOCKET)
	{
		cout << "µ÷ÓÃsockÊ§°Ü" << endl;
		return 0;
	}

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("192.168.0.107");
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = (htons)(9000);

	//int err2 = bind(sockClient, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));

		connect(sockClient, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR));
			char recvBuff[50];
			recv(sockClient, recvBuff, sizeof(recvBuff), 0);
			cout << recvBuff << endl;

			/*char sendBuff[50];
			cin >> sendBuff;
			send(sockClient, sendBuff, strlen(sendBuff) + 1, 0);
			*/
			closesocket(sockClient);
			WSACleanup();
		return 0;
}