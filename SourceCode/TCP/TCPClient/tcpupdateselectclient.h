#ifndef TCPUPDATESELECTCLIENT_H
#define TCPUPDATESELECTCLIENT_H

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
#include <thread>
#include <iostream>

#define SERVERIP "127.0.0.1"
#define PORT 10001
#define BUFFER_SIZE 512
bool isrunning=true;
void scanf_MSG(int sockfd)
{
    while(1)
    {
        char buf[128]={'\0'};
        scanf("%s",buf);
        if(strcmp(buf,"exit")==0)
        {
            isrunning=false;
            std::cout<<"thread exit"<<std::endl;
            return;
        }
        send(sockfd,buf,strlen(buf),0);
    }
}

int test()
{
    int sockfd;
    struct sockaddr_in server;
    char sendbuf[BUFFER_SIZE];
    bzero(&sendbuf,sizeof(sendbuf));
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    bzero(&server,sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVERIP);


    if(connect(sockfd,(struct sockaddr *)&server,sizeof(struct sockaddr)) < 0)
    {
            perror("connect failed.\n");
            return -1;
    }
    std::cout<<"connect success"<<std::endl;
    fd_set fdread;
    timeval t={1,0};
    //启动输入线程
    std::thread newthread(scanf_MSG,sockfd);
    newthread.detach();
    while(isrunning)
    {
        FD_ZERO(&fdread);
        FD_SET(sockfd,&fdread);
        if(select(sockfd+1,&fdread,NULL,NULL,&t)<0)
        {
            std::cout<<"select error"<<std::endl;
            break;
        }
        if(FD_ISSET(sockfd,&fdread))
        {
            FD_CLR(sockfd,&fdread);
            char buf[128]={};
            if(recv(sockfd,buf,128,0)<0)
            {
                std::cout<<"disconnect with server"<<std::endl;
                break;
            }
            std::cout<<buf<<std::endl;
        }
    }
    close(sockfd);
    return 0;
}


#endif // TCPUPDATESELECTCLIENT_H
