#include "chatclient.h"
#include "ui_chatclient.h"
#include <QDebug>
#include <QMetaType>


ChatClient::ChatClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ChatClient)
{
    ui->setupUi(this);
    ui->textBrowser->hide();
    ui->SendB->hide();
    ui->SendB->hide();
    ui->note->hide();
    ui->NameToShow->hide();
    ui->userList->hide();
    ui->sendText->hide();
    bzero(sendbuf,sizeof(sendbuf));
    bzero(recvbuf,sizeof(recvbuf));
    qRegisterMetaType<std::string>("std::string");

    connect(this,SIGNAL(new_client_connect(std::string)),this,SLOT(update_user_list(std::string)));
    connect(this,SIGNAL(new_message_recv(std::string)),this,SLOT(update_message_show(std::string)));
}

ChatClient::~ChatClient()
{
    ::close(this->sockfd);
    delete ui;
}

int ChatClient::Send_Login(QString name,QString password)
{
        bzero(sendbuf,sizeof(sendbuf));
        sockfd = socket(AF_INET,SOCK_STREAM,0);
        bzero(&server,sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(PORT);
        server.sin_addr.s_addr = inet_addr(SERVERIP);


        if(::connect(sockfd,(struct sockaddr *)&server,sizeof(struct sockaddr)) < 0)
        {
                qDebug() <<"connect failed.\n";
                return false;
        }
        sprintf(sendbuf,"Login %s %s",name.toStdString().data(),password.toStdString().data());
        send(sockfd, sendbuf, sizeof(sendbuf), 0);
        bzero(sendbuf,sizeof(sendbuf));
        recv(sockfd,recvbuf,sizeof(recvbuf),0);
        if(recvbuf[0]=='t'&&recvbuf[1]=='r'&&recvbuf[2]=='u'&&recvbuf[3]=='e')
        {
            bzero(recvbuf,sizeof(recvbuf));
            return true;
        }
        bzero(recvbuf,sizeof(recvbuf));
        ::close(this->sockfd);
        return false;
}

void ChatClient::on_RegisterB_clicked()
{

}

void ChatClient::on_LoginB_clicked()
{
    QString register_name= ui->UserNameE->toPlainText();
    QString register_password= ui->PasswordE->toPlainText();

    if(!Send_Login(register_name,register_password))
    {
        ui->note->show();
        ui->note->setText("Name or Password error");
        ui->PasswordE->clear();
        return;
    }
    //it is ok,then recv and send
    std::thread(&ChatClient::send_data_message,this).detach();
    std::thread(&ChatClient::recv_data_watch,this).detach();
    ui->PasswordE->hide();
    ui->PassWordL->hide();
    ui->UserNameE->hide();
    ui->UserNameL->hide();
    ui->RegisterB->hide();
    ui->LoginB->hide();
    ui->textBrowser->show();
    ui->SendB->show();
    ui->note->hide();
    QString s=QString("Welcome  ")+register_name;
    ui->NameToShow->setText(s);
    ui->NameToShow->show();
    ui->userList->show();
    ui->sendText->show();
}

void ChatClient::on_SendB_clicked()
{
    if(ui->sendText->toPlainText()==""||ui->userList->currentItem()==NULL){
        return;
    }
    sprintf(sendbuf,"%s %s",ui->userList->currentItem()->text().toStdString().data(),ui->sendText->toPlainText().toStdString().data());
    qDebug()<<QString(sendbuf);
    //this->Send_Message(ui->userList->currentItem()->text().toStdString(),ui->sendText->toPlainText().toStdString());
    {
        std::unique_lock<std::mutex> lock{this->mu};
        this->notify_send_message.notify_one();
    }
    ui->sendText->clear();
}

void ChatClient::recv_data_watch()
{
    for(;;)
    {
        int len=recv(this->sockfd,this->recvbuf,sizeof(this->recvbuf),0);
        std::string is_newclient="";
        int i=0;
        for(;i<9;i++)
        {
            is_newclient+=recvbuf[i];
        }
        if(strcmp(is_newclient.data(),"newclient")==0)
        {
            i++;
            std::string new_name="";
            for(;recvbuf[i]!='\0';i++)
                new_name+=recvbuf[i];
            emit new_client_connect(new_name);
        }
        else
        {
           std::string message(recvbuf);
           emit new_message_recv(message);
        }
        bzero(this->recvbuf,sizeof(this->recvbuf));
    }
}

void ChatClient::update_user_list(std::string name)
{
        ui->userList->addItem(QString(name.data()));
}

void ChatClient::update_message_show(std::string message)
{
    ui->textBrowser->append(QString(message.data()));
}

void ChatClient::send_data_message()
{
    for(;;)
    {
        {
            std::unique_lock<std::mutex> lock{this->mu};
            this->notify_send_message.wait(lock);
        }
        qDebug()<<"\ncsd";
        send(this->sockfd,this->sendbuf,sizeof(this->sendbuf),0);
        bzero(sendbuf,sizeof(sendbuf));
    }
}
