#include "CaptureDialog.h"

#include <QDateTime>

#include "Controller.h"
#include "../common/protocol.h"

CaptureDialog::CaptureDialog( QWidget * parent )
	: QDialog( parent )
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	connect( Controller::Instance(), &Controller::captureItem, [this]( const QByteArray & data )
	{
		int row = ui.tableWidget->rowCount();
		ui.tableWidget->insertRow( row );

		QTableWidgetItem * item = new QTableWidgetItem( QTime::currentTime().toString( "hh:mm:ss.zzz" ) );
		item->setData( Qt::UserRole + 1, data );
		ui.tableWidget->setItem( row, 0, item );
		ui.tableWidget->setItem( row, 1, new QTableWidgetItem( GetAddress( data ) ) );
		ui.tableWidget->setItem( row, 2, new QTableWidgetItem( GetProtocol( data ) ) );
		ui.tableWidget->setItem( row, 3, new QTableWidgetItem( GetLength( data ) ) );
		ui.tableWidget->setItem( row, 4, new QTableWidgetItem( GetDescription( data ) ) );
	} );
}

CaptureDialog::~CaptureDialog()
{
}

void CaptureDialog::on_pushButton_clicked()
{
	if( Controller::Instance()->IsCapture() )
	{
		Controller::Instance()->StopCapture();
		ui.pushButton->setText( QString::fromLocal8Bit( "开始捕获" ) );
	}
	else
	{
		Controller::Instance()->StartCapture( ui.lineEdit->text() );
		ui.pushButton->setText( QString::fromLocal8Bit( "停止捕获" ) );
	}
}

void CaptureDialog::on_tableWidget_cellClicked( int row, int column )
{
	auto item = ui.tableWidget->item( row, 0 );
	if( item != nullptr )
	{
		CreateTree( item->data( Qt::UserRole + 1 ).toByteArray() );
	}
}

void CaptureDialog::closeEvent( QCloseEvent * e )
{
	if( Controller::Instance()->IsCapture() )
	{
		Controller::Instance()->StopCapture();
	}
}

QString CaptureDialog::GetAddress( const QByteArray & data )
{
	return QString();
}

QString CaptureDialog::GetProtocol( const QByteArray & data )
{
	return QString();
}

QString CaptureDialog::GetLength( const QByteArray & data )
{
	return QString();
}

QString CaptureDialog::GetDescription( const QByteArray & data )
{
	return QString();
}

void CaptureDialog::CreateTree( const QByteArray & data )
{
	ui.treeWidget->clear();
	CreateEth( data );
	ui.treeWidget->setProperty( "@Data", data );
}

void CaptureDialog::CreateEth( const QByteArray & data )
{
	const protocol_eth * eth = (const protocol_eth *) data.constData();
	if( eth != nullptr )
	{
		QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
		QString text = QString( "Ethernet, Src: %1, Dst: %2" ).arg( "" ).arg( "" );
		item->setText( 0, text );
		item->setData( 0, Qt::UserRole + 1, QSize( 0, 0 ) );
		ui.treeWidget->addTopLevelItem( item );


	}
}

void CaptureDialog::CreateArp( const QByteArray & data )
{
}

void CaptureDialog::CreateIP( const QByteArray & data )
{
}

void CaptureDialog::CreateIcmp( const QByteArray & data )
{
}

void CaptureDialog::CreateTcp( const QByteArray & data )
{
}

void CaptureDialog::CreateUdp( const QByteArray & data )
{
}

void CaptureDialog::CreateDNS( const QByteArray & data )
{
}

void CaptureDialog::CreateHttp( const QByteArray & data )
{
}
