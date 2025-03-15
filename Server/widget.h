#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QTcpServer>
#include<QDebug>
#include <QAbstractSocket>
#include <QNetworkInterface>
#include <QTcpSocket>
#include "mycombobox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //创建一个QTcpServer类的指针
    QTcpServer* server;

public  slots:
    void new_Client_internetSignal();

    void on_pushButton_JianTin_clicked();

    void on_pushButton_TinZhiJianTin_clicked();

    void on_pushButton_FaSong_clicked();

    void on_ReadyRead();

    void check_ClientConnect();

    void mycomboBox_refresh();

private slots:

    void on_comboBoxchildren_activated(int index);

private:
    Ui::Widget *ui;
    int chidIndex;
    QList<QTcpSocket*> clientList;//存储多个客户端连接
};
#endif // WIDGET_H
