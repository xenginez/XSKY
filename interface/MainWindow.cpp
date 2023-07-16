#include "MainWindow.h"

#include <QSettings>
#include <QTcpSocket>
#include <QMessageBox>
#include <QRegExpValidator>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.statusBar->addWidget( ui.label_route );
	ui.statusBar->addWidget( ui.label_server );
	ui.statusBar->addWidget( ui.label_message_port );
	ui.statusBar->addWidget( ui.label_control_port );

	QRegExp rx( "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:\\d{5}" );
	ui.lineEdit_route->setValidator( new QRegExpValidator( rx, this ) );
	ui.lineEdit_server->setValidator( new QRegExpValidator( rx, this ) );

	QSettings settings( "config.ini", QSettings::IniFormat );
	{
		ui.lineEdit_route->setText( settings.value( "route_addr", "" ).toString() );
		ui.lineEdit_server->setText( settings.value( "server_addr", "" ).toString() );
	}
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_pushButton_clicked()
{
	if ( ui.lineEdit_route->text().isEmpty() )
	{
		QMessageBox::warning( nullptr, QString::fromLocal8Bit( "错误" ), QString::fromLocal8Bit( "路由器地址不能为空！" ) );
		return;
	}
	if ( ui.lineEdit_server->text().isEmpty() )
	{
		QMessageBox::warning( nullptr, QString::fromLocal8Bit( "错误" ), QString::fromLocal8Bit( "服务器地址不能为空！" ) );
		return;
	}

	ui.label_msg->setText( QString::fromLocal8Bit( "正在同步数据中，请稍后……" ) );
	ui.label_route->setText( QString::fromLocal8Bit( "路由状态：查询中" ) );
	ui.label_server->setText( QString::fromLocal8Bit( "服务状态：查询中" ) );
	ui.label_message_port->setText( QString::fromLocal8Bit( "数据端口：查询中" ) );
	ui.label_control_port->setText( QString::fromLocal8Bit( "控制端口：查询中" ) );

	sync( ui.lineEdit_route->text(), []( bool, QByteArray msg )
	{
	} );
	sync( ui.lineEdit_server->text(), []( bool, QByteArray msg )
	{
	} );
}

void MainWindow::sync( const QString & addr, const std::function<void( bool, QByteArray )> & callback )
{
	QTcpSocket * socket = new QTcpSocket( this );
	socket->connectToHost( addr.left( addr.indexOf( ':' ) ), addr.right( addr.size() - addr.indexOf( ':' ) - 1 ).toUShort() );

	connect( socket, &QTcpSocket::connected, [this, socket]()
	{

	} );
	connect( socket, QOverload<QAbstractSocket::SocketError>::of( &QTcpSocket::error ), [this, socket, callback]( QAbstractSocket::SocketError err )
	{
		callback( false, socket->errorString().toUtf8() );
		socket->deleteLater();
	} );
	connect( socket, &QTcpSocket::readyRead, [this, socket, callback]()
	{
		
		callback( true, socket->readAll() );
		socket->deleteLater();
	} );
}

void MainWindow::route( const QString & addr, const QByteArray & data, const std::function<void( bool, QByteArray )> & callback )
{

}

void MainWindow::server( const QString & addr, const QByteArray & data, const std::function<void( bool, QByteArray )> & callback )
{

}

void MainWindow::closeEvent( QCloseEvent * e )
{
	QMainWindow::closeEvent( e );

	QSettings settings( "config.ini", QSettings::IniFormat );

	settings.setValue( "route_addr", ui.lineEdit_route->text() );
	settings.setValue( "server_addr", ui.lineEdit_server->text() );
}
