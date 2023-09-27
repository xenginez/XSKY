#pragma once

#include <QtWidgets/QMainWindow>

#include <QTimer>
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>

#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_pushButton_send_clicked();

    void on_pushButton_recv_clicked();

private:
    Ui::MainWindowClass ui;

    QUdpSocket * _UdpSocket = nullptr;
    QUdpSocket * _UdpServer = nullptr;
    QTcpSocket * _TcpSocket = nullptr;
    QTcpServer * _TcpServer = nullptr;
};
