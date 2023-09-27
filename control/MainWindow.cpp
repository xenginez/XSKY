#include "MainWindow.h"

#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

#include "Controller.h"
#include "CaptureDialog.h"
#include "ProtocolDialog.h"
#include "ServerListDialog.h"
#include "DomainListDialog.h"

MainWindow::MainWindow( QWidget * parent )
    : QMainWindow( parent )
{
    ui.setupUi(this);

    ui.actionStart->setEnabled( !Controller::Instance()->IsStartService() );
    ui.actionStop->setEnabled( Controller::Instance()->IsStartService() );
    ui.labelServer->setText( Controller::Instance()->GetServer() );
    ui.labelStatus->setText( QString::fromLocal8Bit( Controller::Instance()->IsStartService() ? "已开启" : "已关闭" ) );
    ui.labelConnect->setPixmap( QPixmap( Controller::Instance()->IsStartService() ? ":/icons/stop.png" : ":/icons/start.png" ) );

    ui.labelConnect->installEventFilter( this );

    connect( Controller::Instance(), &Controller::configChanged, [this]()
    {
        ui.actionStart->setEnabled( !Controller::Instance()->IsStartService() );
        ui.actionStop->setEnabled( Controller::Instance()->IsStartService() );
        ui.labelServer->setText( Controller::Instance()->GetServer() );
        ui.labelStatus->setText( QString::fromLocal8Bit( Controller::Instance()->IsStartService() ? "已开启" : "已关闭" ) );
        ui.labelConnect->setPixmap( QPixmap( Controller::Instance()->IsStartService() ? ":/icons/stop.png" : ":/icons/start.png" ) );
    } );
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionStart_triggered( bool checked )
{
    Controller::Instance()->StartService();
}

void MainWindow::on_actionStop_triggered( bool checked )
{
    Controller::Instance()->StopService();
}

void MainWindow::on_actionCapture_triggered( bool checked )
{
    CaptureDialog dialog( this );
    dialog.exec();
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
