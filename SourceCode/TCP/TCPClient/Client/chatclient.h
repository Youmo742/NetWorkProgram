#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QMainWindow>
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
#include <condition_variable>

namespace Ui {
class ChatClient;
}

class ChatClient : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChatClient(QWidget *parent = 0);
    int Send_Login(QString name,QString password);
    //int Send_Message(QString to_user,QString words);

    void recv_data_watch();
    void send_data_message();
    ~ChatClient();

private slots:
    void on_RegisterB_clicked();


    void on_LoginB_clicked();

    void on_SendB_clicked();

    void update_user_list(std::string name);

    void update_message_show(std::string message);
signals:
    void new_client_connect(std::string name);
    void new_message_recv(std::string message);


private:
    Ui::ChatClient *ui;
    #define SERVERIP "127.0.0.1"
    #define PORT 10001
    #define BUFFER_SIZE 512
    int sockfd;
    sockaddr_in server;
    char sendbuf[BUFFER_SIZE];
    char recvbuf[BUFFER_SIZE];
    std::mutex mu;
    std::condition_variable notify_send_message;
};

#endif // CHATCLIENT_H
