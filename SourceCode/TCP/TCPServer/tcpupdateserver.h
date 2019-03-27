#ifndef TCPUPDATESERVER_H
#define TCPUPDATESERVER_H
#include "headers.h"
#include<string>
//发送结构化数据
enum CMD
{
    CMD_LOGIN,
    CMD_LOGOUT
};
struct Header
{
    short dataLength;
    short cmd;
};
struct Login
{
    string name;
    string password;
};
struct LoginResult
{
    int result;
};
struct Logout
{
    string name;
};
struct LogoutResult
{
    int result;
};
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
    conf_fd=accept(listen_fd,(sockaddr*)&cli_sock,&cli_len);
    if(conf_fd==-1)
    {
        cout<<"accept error"<<endl;
    }
    else
    {
        cout<<"ip= "<<inet_ntoa(cli_sock.sin_addr)<<" have connected"<<endl;
    }

    while(true)
    {
        Header hea={};
        int recv_len=recv(conf_fd,(char *)&hea,sizeof(Header),0);
        if(recv_len<=0)
        {
            cout<<"client exit"<<endl;
            break;
        }
        cout<<"command "<<hea.cmd<<" "<<hea.dataLength<<endl;
        switch (hea.cmd)
        {
            case CMD_LOGIN:
            {
                    Login *login=new Login();
                    recv(conf_fd,(char*)&login,sizeof(Login),0);
                    //登录成功
                    LoginResult re={200};
                    send(conf_fd,(char*)&hea,sizeof(Header),0);
                    send(conf_fd,(char*)&re,sizeof(LoginResult),0);
             }
             break;
            case CMD_LOGOUT:
            {
            //在栈上创建，出现segmentaion fault
                    Logout *logout=new Logout();
                    recv(conf_fd,(char*)&logout,sizeof(Logout),0);
                    //登出成功
                    LogoutResult res={200};
                    send(conf_fd,(char *)&hea,sizeof(Header),0);
                    send(conf_fd,(char*)&res,sizeof(LogoutResult),0);
            }
            break;
            default:
                break;
        }
    }
    close(conf_fd);
    close(listen_fd);
}
#endif // TCPUPDATESERVER_H
