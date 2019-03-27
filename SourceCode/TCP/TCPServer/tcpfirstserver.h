#ifndef TCPFIRSTSERVER_H
#define TCPFIRSTSERVER_H
#include "headers.h"
//有客户端连接，就返回一条消息
void test()
{
    int listen_fd,conf_fd;

    //创建套接字
    listen_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(listen_fd==-1)
    {
        cout<<"socket create fail"<<endl;
        exit(0);
    }
    else
    {
        cout<<"create socket success"<<endl;
    }

    //绑定端口
    sockaddr_in ser_sock,cli_sock;
    ser_sock.sin_family=AF_INET;
    ser_sock.sin_addr.s_addr=INADDR_ANY;//inet_addr("127.0.0.1");
    ser_sock.sin_port=htons(8896);//host to net short/long

    if(bind(listen_fd,(sockaddr*)&ser_sock,sizeof(ser_sock))==-1)
    {
        cout<<"bind error"<<endl;
    }
    else
        cout<<"bind success"<<endl;

    //监听端口
    if(listen(listen_fd,5)==-1)
    {
        cout<<"listen error"<<endl;
    }
    else
        cout<<"listen success"<<endl;

    //等待连接
    socklen_t cli_len=sizeof(cli_sock);
    while(true)
    {
        conf_fd=accept(listen_fd,(sockaddr*)&cli_sock,&cli_len);
        if(conf_fd==-1)
        {
            cout<<"connect error"<<endl;
        }
        else
        {
            cout<<"ip= "<<inet_ntoa(cli_sock.sin_addr)<<" have connected"<<endl;
        }
        send(conf_fd,"hello I am server",strlen("hello I am server")+1,0);

        close(conf_fd);
    }
    close(listen_fd);
}
#endif // TCPFIRSTSERVER_H
