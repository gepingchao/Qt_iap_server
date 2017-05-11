#include "iap_server.h"
#include "ui_iap_server.h"

iap_server::iap_server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::iap_server)
{
    ui->setupUi(this);
    this->init();
    this->check_updata_file();
}

iap_server::~iap_server()
{
    delete ui;
}

crc8 mycrc;

void iap_server::init()
{
    my_timer = new QTimer(this);
    G_App_file = new QFile(this);
    myHelper::SetsysCode();

    G_App_file = new QFile(this);

    tcpClient = new QTcpSocket(this);
    tcpClient->abort();
    connect(tcpClient,SIGNAL(readyRead()),this,SLOT(ReadData()));
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ReadError(QAbstractSocket::SocketError)));

    tcpServer = new myTcpServer(this);
    connect(tcpServer,SIGNAL(ClientConnect(int,QString,int)),this,SLOT(ClientConnect(int,QString,int)));
    connect(tcpServer,SIGNAL(ClientDisConnect(int,QString,int)),this,SLOT(ClientDisConnect(int,QString,int)));
    connect(tcpServer,SIGNAL(ClientReadData(int,QString,int,QByteArray)),this,SLOT(ClientReadData(int,QString,int,QByteArray)));

    ui->file_choose->setDisabled(true);
    ui->operat_device->setDisabled(true);
    //timer
    connect(my_timer,SIGNAL(timeout()),this,SLOT(timer_updata()));
    my_timer->start(1000);

    ui->progressBar_download->setRange(0,49999);
    ui->progressBar_download->setValue(0);
    ui->progressBar_download->setVisible(false);

    on_start_server_clicked();
}


void iap_server::ReadData()
{
    QByteArray buffer = tcpClient->readAll();
    if(!buffer.isEmpty())
    {

    }
}
void iap_server::ReadError(QAbstractSocket::SocketError)
{
    tcpClient->disconnectFromHost();
}

void iap_server::ClientReadData(int clientID, QString IP, int Port, QByteArray data)
{
    if(!data.isEmpty())
    {
        //读取到数据
        deal_data_from_client(data);
    }
}

void iap_server::ClientConnect(int clientID, QString IP, int Port)
{
    ui->operat_device->addItem(tr("%1:%2:%3").arg(clientID).arg(IP).arg(Port));
}

void iap_server::ClientDisConnect(int clientID, QString IP, int Port)
{
    ui->operat_device->removeItem(ui->operat_device->findText(tr("%1:%2:%3").arg(clientID).arg(IP).arg(Port)));
}



int iap_server::send_data_to_client(QByteArray data)
{
    QString target = ui->operat_device->currentText();
    int clientID = target.split(":")[0].toInt();
    tcpServer->SendData(clientID,data);
}


void iap_server::timer_updata()
{
}

void iap_server::on_start_server_clicked()
{
    if(ui->start_server->text() == "启动服务器")
    {
        bool ok  = tcpServer->listen(QHostAddress::Any,8086);
        if(ok)
        {
            ui->start_server->setText("关闭服务器");
        }else{
            myHelper::ShowMessageBoxError("端口被占用,服务器启动失败!");
        }
    }else{
        tcpServer->CloseAllClient();
        tcpServer->close();
        ui->start_server->setText("启动服务器");
    }
}

void iap_server::on_updata_clicked()
{
    get_device_version();
}

void iap_server::on_change_file_clicked()
{
    if(ui->change_file->text() == "更改")
    {
        ui->file_choose->setDisabled(false);
        ui->updata->setDisabled(true);
        ui->change_file->setText("确定");
    }
    else
    {
        ui->file_choose->setDisabled(true);
        ui->change_file->setText("更改");
        if(ui->change_device->text()=="更改")
        {ui->updata->setDisabled(false);}
    }
}


void iap_server::on_change_device_clicked()
{if(ui->change_device->text() == "更改")
    {
        ui->operat_device->setDisabled(false);
        ui->updata->setDisabled(true);
        ui->change_device->setText("确定");
    }
    else
    {
        ui->operat_device->setDisabled(true);
        ui->change_device->setText("更改");
        if(ui->change_file->text()=="更改")
        {ui->updata->setDisabled(false);}
    }
}

