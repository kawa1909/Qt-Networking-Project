#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


private slots:
    void on_pushButton_LianJie_clicked();

    void Data_From_Server();


    void on_pushButton_FaSong_clicked();

    void on_pushButton_DuanKai_clicked();

private:
    Ui::Widget *ui;
    QTcpSocket *socket;
    QTimer *connectionTimer;
};
#endif // WIDGET_H
