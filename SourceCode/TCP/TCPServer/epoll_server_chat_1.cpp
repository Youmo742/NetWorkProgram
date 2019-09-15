#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include "thread_pool.hpp"
#include <string>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 10001

std::unordered_map<const char *, int> clients;

void do_run(int fd)
{
    char recvbuf[512]={'\0'};
    int recv_len=recv(fd,recvbuf,sizeof(recvbuf),0);
    std::string is_Login = "";
    for (int i = 0; i < 5; i++)
    {
        is_Login += recvbuf[i];
    }
    //if it is register

    //if it is Login
    if (is_Login == "Login")
    {
        int i = 5;
        std::string username = "";
        for (; recvbuf[i] != ' '; i++)
        {
            username += recvbuf[i];
        }
        clients.insert(std::make_pair(username.data(), fd));
        send(fd, "true", strlen("true") + 1, 0);
        return;
    }
    std::string to_user = "";
    std::string words = "";
    int i=0,index_of_user=0;
    for (; recvbuf[i] != ' '; i++)
    {
        to_user += recvbuf[i];
    }
    index_of_user=i;
    for(;i<strlen(recvbuf)-index_of_user;i++){
        words+=recvbuf[i];
    }
    auto it=clients.find(to_user.data());
    if(it!=clients.end()){
        int send_to_fd = (*it).second;
        send(send_to_fd,words.data(),words.size(),0);
    }
}

int main(int argc, char *argv[])
{

    //create server socket
    threadpool pool(20);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("create socket error\n");
        exit(-1);
    }

    //bind server port
    sockaddr_in server_addrin;
    bzero(&server_addrin, sizeof(server_addrin));
    server_addrin.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addrin.sin_family = AF_INET;
    server_addrin.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (sockaddr *)&server_addrin, sizeof(sockaddr_in)) < 0)
    {
        printf("bind error\n");
        close(server_fd);
        exit(-1);
    }

    //listen server fd
    if (listen(server_fd, 25) < 0)
    {
        printf("listen error\n");
        close(server_fd);
        exit(-1);
    }

    //create epoll
    int epoll_server_fd = epoll_create(1024);
    if (epoll_server_fd < 0)
    {
        printf("create epoll error\n");
        close(server_fd);
        exit(-1);
    }
    //register listen fd in
    epoll_event server_events;
    server_events.data.fd = server_fd;
    server_events.events = EPOLLIN;

    if (epoll_ctl(epoll_server_fd, EPOLL_CTL_ADD, server_fd, &server_events) < 0)
    {
        printf("add server fd in epoll error");
        close(server_fd);
        close(epoll_server_fd);
        exit(-1);
    }

    epoll_event come_events[1024];

    sockaddr_in client_addrin;
    socklen_t client_len;
    epoll_event client_event;

    char read_buff[512] = {'\0'};
    for (;;)
    {
        int number_ready = epoll_wait(epoll_server_fd, come_events, sizeof(come_events), -1);

        if (number_ready < 0)
        {
            printf("epoll wait error\n");
            close(server_fd);
            close(epoll_server_fd);
            exit(-1);
        }
        for (int i = 0; i < number_ready; i++)
        {
            if (!come_events[i].events & EPOLLIN)
                continue;

            //a new client come
            if (come_events[i].data.fd == server_fd)
            {
                client_len = sizeof(sockaddr_in);

                int client_fd = accept(server_fd, (sockaddr *)&client_addrin, &client_len);
                if (client_fd < 0)
                {
                    printf("client accept error\n");
                    continue;
                }
                fprintf(stdout, "It is a new session from IP:%sport:%d\n", inet_ntoa(client_addrin.sin_addr), ntohs(client_addrin.sin_port));
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;
                if (epoll_ctl(epoll_server_fd, EPOLL_CTL_ADD, client_fd, &client_event) < 0)
                {
                    printf("client epoll add in error\n");
                    close(client_fd);
                    continue;
                }
            }
            else
            {
                int client_fd_var = come_events[i].data.fd;
     
                int read_len = recv(client_fd_var, read_buff, sizeof(read_buff), 0);

                if (read_len <= 0)
                {
                    fprintf(stdout, "client socket %d close or error\n", client_fd_var);
                    if (epoll_ctl(epoll_server_fd, EPOLL_CTL_DEL, client_fd_var, NULL) < 0)
                    {
                        exit(-1);
                    }
                    close(client_fd_var);
                }
                else
                {
                    //TODO
                    //send it to thread pool
                    // fprintf(stdout, "read message from %d %s\n", client_fd_var, read_buff);
                    // pool.commit([client_fd_var](){
                    //     send(client_fd_var,"hello I have recved\n",strlen("hello I have recved"),0);
                    // });
                    
                }
            }
        }
    }

    close(server_fd);
    close(epoll_server_fd);
}