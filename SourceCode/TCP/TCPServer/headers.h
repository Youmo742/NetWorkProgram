#ifndef HEADERS_H
#define HEADERS_H
#include <iostream>
#include<string>

#ifdef _WIN32
    #include <windows.h>
    #include <winSock2.h>
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <string.h>
    #include <sys/types.h>
#endif
using namespace std;
#endif // HEADERS_H
