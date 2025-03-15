#include "ui_widget.h"
#include "widget.h"
#include <QIODevice>
#include <QNetworkProxy>
#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //固定控件
    this->setLayout(ui->verticalLayout_3);
    socket = new QTcpSocket(this);

    socket->setProxy(QNetworkProxy::NoProxy);  // **禁用代理**

    //接受服务端的套接字
    connect(socket, &QIODevice::readyRead, this, &Widget::Data_From_Server);

}

Widget::~Widget()
{
    delete ui;
}

//连接服务端
void Widget::on_pushButton_LianJie_clicked()
{
    ui->textEdit_JieShou->insertPlainText("尝试连接...\n");

    // 设置定时器，超时时间 3000ms（3秒）
    connectionTimer = new QTimer(this);
    connectionTimer->setSingleShot(true);

    // 绑定定时器超时处理（输出超时信息）
    connect(connectionTimer, &QTimer::timeout, this, [=]() {
        qDebug() << "连接超时！";
        ui->textEdit_JieShou->insertPlainText("连接超时！\n");
    });

    // 启动定时器
    connectionTimer->start(3000);

    // 进行连接
    socket->connectToHost(ui->lineEdit_IP->text(), ui->lineEdit_Port->text().toInt());

    if(socket->waitForConnected(3000)) {
        // 连接成功，停止定时器
        connectionTimer->stop();
        qDebug() << "客户端连接成功！";
        ui->textEdit_JieShou->insertPlainText("客户端连接成功！\n");
        // ✅ 禁用连接按钮，防止重复连接
        ui->pushButton_LianJie->setEnabled(false);
        // ✅ 启用断开按钮，让用户可以断开
        ui->pushButton_DuanKai->setEnabled(true);
    } else {
        // 连接失败，可能是超时，也可能是其他错误
        if(socket->error() == QAbstractSocket::SocketTimeoutError) {
            qDebug() << "连接超时！";
            ui->textEdit_JieShou->insertPlainText("连接超时！\n");
        } else {
            qDebug() << "客户端连接失败：" << socket->errorString();
            ui->textEdit_JieShou->insertPlainText("客户端连接失败：" + socket->errorString() + "\n");
        }
    }
}

//接收来自服务端的消息
void Widget::Data_From_Server()
{
    //    QByteArray data = socket->readAll();
    //    qDebug() << "收到服务端消息：" << data;
    //    ui->textEdit_JieShou->insertPlainText(data);
    QByteArray data = socket->readAll();
    QString receivedText = QString::fromUtf8(data);

    // 在客户端窗口显示接收到的消息（红色）
    ui->textEdit_JieShou->append("<font color='red'>服务器: " + receivedText + "</font>");
}

//发送消息给服务端
void Widget::on_pushButton_FaSong_clicked()
{
    //    socket->write(ui->textEdit_FaSong->toPlainText().toUtf8());
    //    ui->textEdit_FaSong->clear();
    QString sendText = ui->textEdit_FaSong->toPlainText();
    if(sendText.isEmpty()) {
        qDebug() << "发送内容为空！";
        return;
    }

    // 发送数据
    socket->write(sendText.toUtf8());

    // 在客户端窗口显示已发送的消息（黑色）
    ui->textEdit_JieShou->append("<font color='black'>我: " + sendText + "</font>");

    // 清空输入框
    ui->textEdit_FaSong->clear();
}

void Widget::on_pushButton_DuanKai_clicked()
{
    socket->disconnectFromHost();

    if (socket->state() == QAbstractSocket::UnconnectedState || socket->waitForDisconnected(3000)) {
        qDebug() << "客户端已断开连接！";

        // ✅ 重新启用连接按钮
        ui->pushButton_LianJie->setEnabled(true);
        // ✅ 禁用断开按钮
        ui->pushButton_DuanKai->setEnabled(false);
    }
}
