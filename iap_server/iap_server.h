#ifndef IAP_SERVER_H
#define IAP_SERVER_H

#include <QMainWindow>
#include <QThread>
#include<QTime>
#include<QTimer>
#include <QDirModel>
#include <QFile>
#include <strstream>
#include "myhelper.h"
#include "mytcpclient.h"
#include "mytcpserver.h"
#include "crc.h"
#include"iap.h"

namespace Ui {
class iap_server;
}

class iap_server : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit iap_server(QWidget *parent = 0);
    ~iap_server();
    
private slots:
    void on_start_server_clicked();

    void on_updata_clicked();

private:
    Ui::iap_server *ui;


private:
    QTcpSocket *tcpClient;
    myTcpServer *tcpServer;
    QTimer *my_timer;
    QFile *G_App_file;

private:
    void init();
    bool is_file_exit(QString filename);
    void check_updata_file();
    void deal_data_from_client(QByteArray data);
    int send_data_to_client(QByteArray data);
    void get_device_version(void);
    void deal_get_version_information(QByteArray data);
    void send_update_information(void);
    void transmission_iap_data_by_wifi(QFile* &app,int num);

private slots:

    void ReadData();
    void ReadError(QAbstractSocket::SocketError);

    void ClientReadData(int clientID,QString IP,int Port,QByteArray data);
    void ClientConnect(int clientID,QString IP,int Port);
    void ClientDisConnect(int clientID,QString IP,int Port);
    void timer_updata();
    void on_change_file_clicked();
    void on_change_device_clicked();
};

#endif // IAP_SERVER_H



