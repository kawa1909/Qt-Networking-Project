#include "widget.h"
#include "ui_widget.h"
#include "mycombobox.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    server =  new QTcpServer(this);

    connect(ui->comboBoxchildren, &MyComboBox::on_ComboBox_clicked, this, &Widget::mycomboBox_refresh);

    this->setLayout(ui->verticalLayout_3);

    //    //固定combobox格式
    //    ui->comboBox_IP->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);
    //    ui->comboBox_XieYi->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLength);


    connect(server, &QTcpServer::newConnection, this, &Widget::new_Client_internetSignal);

    //点击开启监听按钮后，停止监听启动
    ui->pushButton_TinZhiJianTin->setEnabled(false);

    //QList<QHostAddress> QNetworkInterface::allAddresses()
    QList<QHostAddress> address = QNetworkInterface::allAddresses();
    for(auto add : address){
        if(add.protocol() == QAbstractSocket::IPv4Protocol){
            ui->comboBox_IP->addItem(add.toString());
        }
    }

}


Widget::~Widget()
{
    delete ui;
}


//建立了新的连接信号
void Widget::new_Client_internetSignal()
{
    qDebug() << "new ClientIn!";
    mycomboBox_refresh();  // 连接后立刻刷新客户端列表
    //判断是否有客户端信号挂起
    if(server->hasPendingConnections()){
        //qDebug() << "有客户端信号正在挂起!";
        //处理客户端套接字
        QTcpSocket* clientSocket = server->nextPendingConnection();
        if(clientSocket){
            qDebug() << "有客户端连接！IP：" << clientSocket->peerAddress().toString().toUtf8().constData()
                     << "Port:" << clientSocket->peerPort();
            ui->textEdit_JieShou->insertPlainText("客户端IP:" + clientSocket->peerAddress().toString()
                                                  + "客户端Port:" + QString::number(clientSocket->peerPort()) + "\n");
            //连接接受到的新数据
            connect(clientSocket, &QIODevice::readyRead, this, &Widget::on_ReadyRead);
            //监听客户端是否断开
            connect(clientSocket, &QAbstractSocket::disconnected, this, &Widget::check_ClientConnect);

            clientList.append(clientSocket); // 存储连接的客户端
        }else{
            qDebug() << "没有待处理的客户端连接";
        }
    }else{
        qDebug() << "ClientERROR";
    }
}


//开启监听
void Widget::on_pushButton_JianTin_clicked()
{
    //先检测是否输入了端口
    if(ui->lineEdit_Port->text().isEmpty()){
        qDebug() << "请先输入端口号再开启监听！";
        return;
    }

    //获取服务端当前状态
    if(!server->isListening()){
        //        //telnet 172.20.10.2 8000
        //        //server->QTcpServer::listen(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);//端口号8080~15000
        //        QHostAddress addr("172.20.10.2");//10.189.86.127
        int Port = ui->lineEdit_Port->text().toInt();
        if(!server->listen(QHostAddress(ui->comboBox_IP->currentText()), Port)){
            qDebug() << "监听失败！";
        }else{
            qDebug() << "监听成功！";
            ui->pushButton_JianTin->setEnabled(false);
            ui->pushButton_TinZhiJianTin->setEnabled(true);
        }
    }
}


//停止监听
void Widget::on_pushButton_TinZhiJianTin_clicked()
{
    server->close();
    qDebug() << "服务端停止监听!";
    ui->pushButton_JianTin->setEnabled(true);
    ui->pushButton_TinZhiJianTin->setEnabled(false);
}


//发送信息
void Widget::on_pushButton_FaSong_clicked()
{
    QString sendText = ui->textEdit_FaSong->toPlainText();

    //判断是否连接成功再发送消息
    if(!server->isListening()){
        qDebug() << "无连接信号，信息发送失败！";
    }

    QList<QTcpSocket*> clientSockets = server->findChildren<QTcpSocket*>();

    //判断发送文本是否为空
    if(!sendText.isEmpty()){
        //如果用户不选择所有客户端发送消息
        if(ui->comboBoxchildren->currentText() != "all"){
            if (chidIndex < 0 || chidIndex >= clientList.size()) {
                qDebug() << "请先选择你要发送信息的客户端! " << chidIndex;
                return;
            }
            //对选定的客户端进行发送消息
            clientSockets[chidIndex]->write(ui->textEdit_FaSong->toPlainText().toStdString().c_str());
        }else{
            for(auto tmp : clientSockets){
                tmp->write(ui->textEdit_FaSong->toPlainText().toStdString().c_str());
            }
        }
        // 在服务端窗口显示发送的消息（蓝色）
        ui->textEdit_JieShou->append("<font color='black'>服务器: " + sendText + "</font>");
    }else{
        qDebug() << "请先输入要发送的信息！";
    }
    ui->textEdit_FaSong->clear();
}

//接收数据信号
void Widget::on_ReadyRead()
{
    //    //定义槽函数发送的信号
    //    QTcpSocket* tmpSock = qobject_cast<QTcpSocket*>(sender());
    //    QByteArray RevData =  tmpSock->readAll();//读取文本发过来的数据
    //    ui->textEdit_JieShou->insertPlainText(RevData);
    // 获取触发信号的客户端
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket) return;

    // 读取客户端数据
    QByteArray data = clientSocket->readAll();
    QString receivedText = QString::fromUtf8(data);

    // 显示客户端发送的消息（黑色）
    ui->textEdit_JieShou->append("<font color='red'>客户端: " + receivedText + "</font>");
}

//检测客户端是否断开连接了
void Widget::check_ClientConnect()
{
    //获取触发信号的对象
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if(clientSocket){
        qDebug() << "客户端断开连接：" << clientSocket->peerAddress().toString().toUtf8().constData();
        clientList.removeOne(clientSocket); // 从列表中移除
        clientSocket->deleteLater(); // 释放资源

        ui->textEdit_JieShou->insertPlainText("客户端断开连接!IP:" + clientSocket->peerAddress().toString() + "\n");
    }
}

//鼠标左键事件，选择特定的客户端发送消息
void Widget::mycomboBox_refresh()
{
    ui->comboBoxchildren->clear();//对之前遗留的客户端端口号清除
    QList<QTcpSocket*> clientSockets = server->findChildren<QTcpSocket*>();//查找server的所有子对象
    for(auto tmp : clientSockets){
        ui->comboBoxchildren->addItem(QString::number(tmp->peerPort()));//读取端口号
    }
    ui->comboBoxchildren->addItem("all");
}

//读取combobox中的下标索引
void Widget::on_comboBoxchildren_activated(int index)
{
    chidIndex = index;
}
