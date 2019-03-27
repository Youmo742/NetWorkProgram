
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <algorithm>

#define PORT 10001
#define RT_ERR (-1)
#define RT_OK 0
#define SERVERIP "127.0.0.1"
#define LISTEN_QUEUE 10
#define BUFFER_SIZE 1024
//服务器使用select模型，作为
int test()
{
    std::vector<int>v;
    int listenfd, connsockfd;
    char readbuf[BUFFER_SIZE];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenfd < 0)
    {
            fprintf(stderr, "socket function failed.\n");
            exit(RT_ERR);
    }

    struct  sockaddr_in serveraddr, clientaddr;
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT);
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);

    unsigned int client_len = sizeof(struct sockaddr_in);
    if(bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_in)) < 0)
    {
            fprintf(stderr, "bind function failed.\n");
            close(listenfd);
            exit(RT_ERR);
    }

    if(listen(listenfd,LISTEN_QUEUE) < 0)
    {
            fprintf(stderr, "listen function failed.\n");
            close(listenfd);
            exit(RT_ERR);
    }
    fprintf(stdout, "The server IP is %s, listen on port: %d\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));

    fd_set readfdset;
    while(1)
    {
        FD_ZERO(&readfdset);
        for(auto fd:v)
        {
            FD_SET(fd, &readfdset);
        }
        FD_SET(listenfd, &readfdset);

        if(!(select(FD_SETSIZE, &readfdset, NULL, NULL, NULL) > 0))
        {
            fprintf(stderr, "select function failed.\n");
            close(listenfd);
            exit(RT_ERR);
        }
        if(FD_ISSET(listenfd, &readfdset))
        {
//            fprintf(stdout, "fd is %d, listenfd is %d\n", fd, listenfd);
            std::cout<<"new client"<<std::endl;
            if((connsockfd = accept(listenfd, (struct sockaddr*)&clientaddr, &client_len)) < 0)
            {
                fprintf(stderr, "accept function failed.\n");
                exit(RT_ERR);
            }
            v.push_back(connsockfd);
            fprintf(stdout, "It is a new session from IP:%s port:%d\n",inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
        }
         for(int fd=listenfd+1;fd<FD_SETSIZE;fd++)
         {
             if(FD_ISSET(fd, &readfdset))
             {
                 if(recv(fd, readbuf, BUFFER_SIZE, 0) > 0)
                 {
                     fprintf(stdout, "recv message: %s\n", readbuf);
                 }
                 else
                 {
                     FD_CLR(fd,&readfdset);
                     auto t=std::find(v.begin(),v.end(),fd);
                     v.erase(t);
                     close(fd);
                     fprintf(stdout, "client socket %d close\n", fd);
                 }
             }
        }//end of for
    }//end of while
    close(listenfd);
    return 0;
}

