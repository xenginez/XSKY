#include "MainWindow.h"

#include <QMessageBox>

typedef void( QAbstractSocket:: *error_t )( QAbstractSocket::SocketError );

MainWindow::MainWindow( QWidget * parent )
    : QMainWindow( parent )
{
    ui.setupUi( this );
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_pushButton_send_clicked()
{
    QAbstractSocket * socket;

    if( ui.comboBox_send_protocol->currentText() == "UDP" )
    {
        if( _UdpSocket != nullptr )
        {
            _UdpSocket->deleteLater();
        }

        _UdpSocket = new QUdpSocket( this );

        socket = _UdpSocket;
    }
    else
    {
        if( _TcpSocket != nullptr )
        {
            _TcpSocket->deleteLater();
        }

        _TcpSocket = new QTcpSocket( this );

        socket = _TcpSocket;
    }

    connect( socket, &QAbstractSocket::connected, [this, socket]()
    {
        socket->write( ui.textEdit_send_text->toPlainText().toLatin1() );
        socket->flush();
    } );
    connect( socket, &QAbstractSocket::readyRead, [this, socket]()
    {
        QMessageBox::about( this, QString::fromLocal8Bit( "俊辺" ), QString( socket->readAll() ) );
        socket->disconnected();
    } );
    connect( socket, (error_t) &QAbstractSocket::error, [this, socket]( QAbstractSocket::SocketError err )
    {
        QMessageBox::warning( this, QString::fromLocal8Bit( "少御" ), socket->errorString() );
    } );

    socket->connectToHost( ui.lineEdit_send_ip->text(), ui.spinBox_send_port->value() );
}

void MainWindow::on_pushButton_recv_clicked()
{
    if( ui.comboBox_recv_protocol->currentText() == "UDP" )
    {
        if( _UdpServer != nullptr )
        {
            _UdpServer->deleteLater();
        }

        _UdpServer = new QUdpSocket( this );

        _UdpServer->bind( ui.spinBox_recv_port->value() );

        connect( _UdpServer, &QAbstractSocket::readyRead, [this, socket = _UdpServer]()
        {
            ui.textEdit_recv_text->append( QString( socket->readAll() ) );

            socket->disconnected();
        } );
        connect( _UdpServer, (error_t) &QAbstractSocket::error, [this, socket = _UdpServer]( QAbstractSocket::SocketError err )
        {
            QMessageBox::warning( this, QString::fromLocal8Bit( "少御" ), socket->errorString() );
        } );
    }
    else
    {
        if( _TcpServer != nullptr )
        {
            _TcpServer->deleteLater();
        }

        _TcpServer = new QTcpServer( this );

        _TcpServer->listen( QHostAddress( "127.0.0.1" ), ui.spinBox_recv_port->value() );

        connect( _TcpServer, &QTcpServer::newConnection, [this, server = _TcpServer]()
        {
            if( server->hasPendingConnections() )
            {
                auto socket = server->nextPendingConnection();

                connect( socket, &QAbstractSocket::readyRead, [this, socket]()
                {
                    ui.textEdit_recv_text->append( QString( socket->readAll() ) );

                    socket->disconnected();
                } );
                connect( socket, (error_t) &QAbstractSocket::error, [this, socket]( QAbstractSocket::SocketError err )
                {
                    QMessageBox::warning( this, QString::fromLocal8Bit( "少御" ), socket->errorString() );
                } );
            }
        } );
        connect( _TcpServer, &QTcpServer::acceptError, [this, server = _TcpServer]( QAbstractSocket::SocketError err )
        {
            QMessageBox::warning( this, QString::fromLocal8Bit( "少御" ), server->errorString() );
        } );
    }
}
