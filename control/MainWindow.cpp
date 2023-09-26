#include "MainWindow.h"

#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "Controller.h"
#include "WaitDialog.h"
#include "ProtocolDialog.h"
#include "ServerListDialog.h"
#include "DomainListDialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QTimer::singleShot( 10, [this]()
    {
        WaitDialog * wait = new WaitDialog( this );
        Controller::Instance()->GetConfig( [this, wait]( QString data )
        {
            ui.labelServer->setText( Controller::Instance()->GetServer() );
            ui.labelStatus->setText( QString::fromLocal8Bit( Controller::Instance()->IsStartService() ? "�ѿ���" : "�ѹر�" ) );
            ui.labelConnect->setPixmap( QPixmap( Controller::Instance()->IsStartService() ? ":/icons/stop.png" : ":/icons/start.png" ) );

            wait->deleteLater();
        } );
        wait->exec();
    } );

    ui.labelConnect->installEventFilter( this );
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionStart_triggered( bool checked )
{
    Controller::Instance()->StartService( [this]( bool result )
    {
        ui.actionStart->setEnabled( !result );
        ui.actionStop->setEnabled( result );
        ui.labelConnect->setPixmap( QPixmap( ":/icons/stop.png" ) );
        ui.labelStatus->setText( QString::fromLocal8Bit( "�ѿ���" ) );
    } );
}

void MainWindow::on_actionStop_triggered( bool checked )
{
    Controller::Instance()->StopService( [this]( bool result )
    {
        ui.actionStart->setEnabled( result );
        ui.actionStop->setEnabled( !result );
        ui.labelConnect->setPixmap( QPixmap( ":/icons/start.png" ) );
        ui.labelStatus->setText( QString::fromLocal8Bit( "�ѹر�" ) );
    } );
}

void MainWindow::on_actionExit_triggered( bool checked )
{
    close();
}

void MainWindow::on_actionProtocol_triggered( bool checked )
{
    ProtocolDialog dialog( this );
    dialog.exec();
}

void MainWindow::on_actionDomainList_triggered( bool checked )
{
    DomainListDialog dialog( this );
    dialog.exec();
}

void MainWindow::on_actionServerList_triggered( bool checked )
{
    ServerListDialog dialog( this );
    dialog.exec();
}

bool MainWindow::eventFilter( QObject * o, QEvent * e )
{
    if( o == ui.labelConnect && e->type() == QEvent::MouseButtonRelease )
    {
        on_labelConnect_clicked();
    }

    return QMainWindow::eventFilter( o, e );
}

void MainWindow::on_labelConnect_clicked()
{
    if( Controller::Instance()->IsStartService() )
    {
        on_actionStop_triggered( true );
    }
    else
    {
        on_actionStart_triggered( true );
    }
}
