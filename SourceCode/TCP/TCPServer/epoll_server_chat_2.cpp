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
#include <iostream>
#include<fcntl.h>

#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 10001

std::unordered_map<std::string, int> clients_name_find_fd;
std::unordered_map<int, std::string> clients_fd_find_name;

int server_fd = 0;
int epoll_server_fd = 0;

void do_run(int client_fd_var)
{
    char recvbuf[1024] = {'\0'};
    char temp_recvbuf[64] = {'\0'};
    int len = 0;
    while (1)
    {
        int recv_len = recv(client_fd_var, temp_recvbuf, sizeof(temp_recvbuf), 0);
        if (recv_len < 0)
        {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
            { //errno为EAGAIN和EWOULDBLOCK时表示数据已经读取完毕可以进行下一次epoll
                std::cout << "epoll again" << std::endl;
                break;
            }
            goto here;
        }
        else if(recv_len==0)
        {
            here:
            fprintf(stdout, "client socket %d close or error\n", client_fd_var);
            if (epoll_ctl(epoll_server_fd, EPOLL_CTL_DEL, client_fd_var, NULL) < 0)
            {
                fprintf(stdout, "some error happend\n");
                exit(-1);
            }
            std::string del_name = (*clients_fd_find_name.find(client_fd_var)).second;
            {
                std::unique_lock<std::mutex> lock();
                clients_fd_find_name.erase(client_fd_var);
                clients_name_find_fd.erase(del_name);
            }
            close(client_fd_var);
            return;
        }
        std::cout<<recv_len<<std::endl;
        memcpy(recvbuf + len, temp_recvbuf, recv_len);
        len+=recv_len;
        bzero(temp_recvbuf,sizeof(temp_recvbuf));
    }
    //std::cout<<recvbuf<<std::endl;
    // if (recv_len <= 0)
    // {
    //     fprintf(stdout, "client socket %d close or error\n", client_fd_var);
    //     if (epoll_ctl(epoll_server_fd, EPOLL_CTL_DEL, client_fd_var, NULL) < 0)
    //     {
    //         fprintf(stdout, "some error happend\n");
    //         exit(-1);
    //     }
    //     std::string del_name = (*clients_fd_find_name.find(client_fd_var)).second;
    //     {
    //         std::unique_lock<std::mutex> lock();
    //         clients_fd_find_name.erase(client_fd_var);
    //         clients_name_find_fd.erase(del_name);
    //     }
    //     close(client_fd_var);

    //     return;
    // }

    //it is Login Message?
    std::string is_Login = "";
    for (int i = 0; i < 5; i++)
    {
        is_Login += recvbuf[i];
    }
    //if it is register

    //if it is Login
    if (is_Login == "Login")
    {
        int i = 6;
        std::string username = "";
        for (; recvbuf[i] != ' '; i++)
        {
            username += recvbuf[i];
        }
        {
            std::unique_lock<std::mutex> lock();
            clients_name_find_fd.insert(std::make_pair(username, client_fd_var));
            clients_fd_find_name.insert(std::make_pair(client_fd_var, username));
        }

        send(client_fd_var, "true", strlen("true") + 1, 0);

        //new client in ,Notify others to update there list
        std::this_thread::sleep_for(std::chrono::microseconds(500));
        std::string message="";
        // for (auto t : clients_fd_find_name)
        //     std::cout << t.first << " ";
        // std::cout << std::endl;
        // for (auto v : clients_name_find_fd)
        //     std::cout << v.first << " ";
        // std::cout << std::endl;
        for (auto t : clients_fd_find_name)
        {
            if (t.first == client_fd_var)
            {
                for (auto v : clients_name_find_fd)
                {
                    if (v.first != username)
                    {

                        message = std::string("newclient ") + v.first;
                        send(t.first, message.data(), message.size() + 1, 0);
                        std::this_thread::sleep_for(std::chrono::microseconds(500));
                    }
                }
            }
            else
            {
                message = std::string("newclient ") + username;
                send(t.first, message.data(), message.size() + 1, 0);
                std::this_thread::sleep_for(std::chrono::microseconds(500));
            }
        }
        return;
    }

    //It send message to others
    std::string to_user = "";
    std::string words = "";
    int i = 0, index_of_user = 0;

    for (; recvbuf[i] != ' '; i++)
    {
        to_user += recvbuf[i];
    }
    index_of_user = i;
    i++;
    for (; i < len - index_of_user; i++)
    {
        words += recvbuf[i];
    }
    std::string from_username = (*clients_fd_find_name.find(client_fd_var)).second;
    words = from_username + " : " + words;

    // std::cout << words << std::endl;
    // std::cout << to_user << std::endl;

    auto it = clients_name_find_fd.find(to_user);
    if (it != clients_name_find_fd.end())
    {
        int send_to_fd = (*it).second;
        std::cout << send_to_fd << std::endl;
        send(send_to_fd, words.data(), words.size() + 1, 0);
    }
}

int main(int argc, char *argv[])
{

    //create server socket
    threadpool pool(20);
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        printf("create socket error\n");
        exit(-1);
    }
    int reuse = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
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
    epoll_server_fd = epoll_create(1024);
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

    //char read_buff[512] = {'\0'};
    int client_fd_var;
    for (;;)
    {
        int number_ready = epoll_wait(epoll_server_fd, come_events, 1024, -1);
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
                fcntl(client_fd,F_SETFL,O_NONBLOCK);
                client_event.events = EPOLLIN | EPOLLET;
                client_event.data.fd = client_fd;
                if (epoll_ctl(epoll_server_fd, EPOLL_CTL_ADD, client_fd, &client_event) < 0)
                {
                    printf("client epoll add in error\n");
                    close(server_fd);
                    close(epoll_server_fd);
                    close(client_fd);
                    exit(-1);
                }
            }
            else
            {
                client_fd_var = come_events[i].data.fd;
                pool.commit(do_run,client_fd_var);
                //std::thread t(do_run, client_fd_var);
                //do_run(client_fd_var,epoll_server_fd);
                //t.detach();
            }
        }
    }

    close(server_fd);
    close(epoll_server_fd);
}