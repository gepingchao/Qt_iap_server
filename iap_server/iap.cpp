#include "iap.h"
extern crc8 mycrc;
namespace Ui {
class iap_server;
}

bool iap_server::is_file_exit(QString filename)
{
    QFile check_file(filename);
    if(check_file.exists())
    {
        //QMessageBox::information(this,"测试","找到文件"+filename);
        return true;
    }else{
        //QMessageBox::critical(this,"测试","文件不存在");
        return false;}
}

void iap_server::check_updata_file()
{
    QString filename;
    for(int i=0;i<0XFF;i++)
    {
        filename=QString("update%1").arg(i);
        if(is_file_exit("update_file/" + filename))
        {
            ui->file_choose->addItem(tr("update%1").arg(i));
        }
    }
    ui->file_choose->setCurrentIndex(ui->file_choose->count()-1);
}

QByteArray creat_iap_head(void)
{
    QByteArray head;
    head.resize(8);
    head[0] = 0XA5;
    head[1] = 0X5A;
    return head;
}

QByteArray creat_iap_packet(QByteArray insert_parameter,QByteArray insert_data)
{
    int insert_data_length=insert_data.size();
    int insert_parameter_length=insert_parameter.size();
    int packet_length=0;
    QByteArray data = creat_iap_head();
    packet_length = data.size()+insert_data_length+insert_parameter_length;
    data.resize(packet_length);

    data[2] = packet_length>>8 & 0X000000FF;
    data[3] = packet_length & 0X000000FF;

    data[4] = insert_data_length>>8 & 0X000000FF;
    data[5] = insert_data_length & 0X000000FF;

    data[6] = insert_parameter_length & 0X000000FF;

    for(int i=0; i<insert_parameter_length; i++)
    {data[7+i] = insert_parameter.at(i);}
    for(int i=0; i<insert_data_length; i++)
    {data[7+insert_parameter_length+i] = insert_data.at(i);}
    mycrc.SetCRC8Sub(data.data(),packet_length-1);
    return data;
}

void iap_server::get_device_version(void)
{
    QByteArray send_data;
    QByteArray parameter;
    QByteArray data;
    parameter.resize(1);
    parameter[0] = 0XF0;
    data.resize(0);
    send_data = creat_iap_packet(parameter,data);
    send_data_to_client(send_data);
}

void iap_server::send_update_information(void)
{
    int update_version = 0;
    int update_size = 0;
    if(ui->file_choose->currentText().isEmpty())
    {
        return;
    }
    update_version = (ui->file_choose->currentText().split('e')[1]).toInt();
    G_App_file->setFileName("update_file/"+ui->file_choose->currentText());
    G_App_file->open(QIODevice::ReadOnly);
    update_size = G_App_file->size();

    ui->progressBar_download->setRange(0,update_size/256);
    ui->progressBar_download->setVisible(true);

    QByteArray send_data;
    QByteArray parameter;
    QByteArray data;
    parameter.resize(7);
    parameter[0] = 0XF1;
    parameter[1] =  update_version >>8 & 0X000000FF;
    parameter[2] =  update_version & 0X000000FF;
    parameter[3] =  update_size >>24 & 0X000000FF;
    parameter[4] =  update_size >>16 & 0X000000FF;
    parameter[5] =  update_size >>8 & 0X000000FF;
    parameter[6] =  update_size& 0X000000FF;
    data.resize(0);
    send_data = creat_iap_packet(parameter,data);
    send_data_to_client(send_data);
}

void iap_server::deal_get_version_information(QByteArray data)
{
    int version = 0;
    if(data.at(6)>2)
    {
        version |= data.at(8)<<8 & 0X0000FF00;
        version |= data.at(9) & 0X000000FF;

        QMessageBox msg(this);
        msg.setStyleSheet("font:10pt '宋体'");
        msg.setWindowTitle("准备升级");
        msg.setText(QString("检测到当前设备正在使用的固件版本为:%1!\r\n是否升级?").arg(version));
        msg.setIcon(QMessageBox::Warning);
        QAbstractButton *yes = msg.addButton("确定",QMessageBox::ActionRole);
        QAbstractButton *no = msg.addButton("取消",QMessageBox::RejectRole);
        msg.exec();

        if(msg.clickedButton() == yes)
        {
            //ui->updata->setDisabled(true);
            send_update_information();
        }
        if(msg.clickedButton() == no)
        {
        }
    }
}

int read_iap_page_data_by_wifi(QFile* &app, int num,char* buff ,int &read_num)
{
    int offset;
    offset = num *256;
    if(offset > app->size())
    {
        return 0;
    }
    app->seek(offset);
    read_num = app->read(buff,256);
    return 1;
}

void iap_server::transmission_iap_data_by_wifi(QFile* &app,int num)
{
    int read_num = 0;
    char iap_buf[256] = {0};
    if(read_iap_page_data_by_wifi(app,num,(iap_buf),read_num))
    {
        QByteArray iap_data;
        QByteArray iap_parameter;
        if(read_num>0)
        {
            iap_data.resize(read_num);
            iap_parameter.resize(3);
            iap_parameter[0] = 0XF2;
            iap_parameter[1] = num>>8 & 0X000000FF;
            iap_parameter[2] = num & 0X000000FF;
            for(int i = 0; i<read_num; i++)
            {iap_data[i] = iap_buf[i];}
            send_data_to_client(creat_iap_packet(iap_parameter,iap_data));
        }
    }
}

void iap_server::deal_data_from_client(QByteArray data)
{
    unsigned char function;
    int page_num = 0;
    function = (unsigned char)data.at(7);
    switch(function)//function
    {
    case 0XF0:
        deal_get_version_information(data);
    break;

    case 0XF1:
    break;

    case 0XF2:
        page_num |= data.at(8)<<8 & 0X0000FF00;
        page_num |= data.at(9)& 0X000000FF;
        ui->progressBar_download->setValue(page_num);
        transmission_iap_data_by_wifi(G_App_file,page_num);
        if(page_num == ui->progressBar_download->maximum())
        {
            myHelper::ShowMessageBoxInfo("升级完成!");
            ui->progressBar_download->setVisible(false);
        }
    break;

    case 0XF3:
    break;

    case 0XF4:
    break;

    case 0XF5:
    break;

    default:
        break;
    }
}







