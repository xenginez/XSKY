#include "CaptureDialog.h"

#include <QDateTime>

#include "Controller.h"
#include "../common/common.h"
#include "QHexView/document/qhexcursor.h"
#include "QHexView/document/buffer/qmemorybuffer.h"

CaptureDialog::CaptureDialog( QWidget * parent )
	: QDialog( parent )
{
	ui.setupUi(this);

	setWindowFlags( ( windowFlags() & ~Qt::WindowContextHelpButtonHint ) | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint );

	ui.hexView->setReadOnly( true );

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

	QByteArray data = "111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";

	int row = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow( row );

	QTableWidgetItem * item = new QTableWidgetItem( QTime::currentTime().toString( "hh:mm:ss.zzz" ) );
	item->setData( Qt::UserRole + 1, data );
	ui.tableWidget->setItem( row, 0, item );
	ui.tableWidget->setItem( row, 1, new QTableWidgetItem( GetAddress( data ) ) );
	ui.tableWidget->setItem( row, 2, new QTableWidgetItem( GetProtocol( data ) ) );
	ui.tableWidget->setItem( row, 3, new QTableWidgetItem( GetLength( data ) ) );
	ui.tableWidget->setItem( row, 4, new QTableWidgetItem( GetDescription( data ) ) );
}

CaptureDialog::~CaptureDialog()
{
}

void CaptureDialog::on_pushButton_clicked()
{
	if( Controller::Instance()->IsCapture() )
	{
		ui.lineEdit->setEnabled( true );
		ui.toolButton->setEnabled( true );
		Controller::Instance()->StopCapture();
		ui.pushButton->setText( QString::fromLocal8Bit( "开始捕获" ) );
	}
	else
	{
		ui.lineEdit->setEnabled( false );
		ui.toolButton->setEnabled( false );
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

void CaptureDialog::on_treeWidget_itemClicked( QTreeWidgetItem * item, int column )
{
	auto point = item->data( 0, Qt::UserRole + 1 ).value<QPoint>();
	auto line_sz = ui.hexView->document()->cursor()->position().lineWidth;

	int beg_col = point.x() % line_sz;
	int end_col = point.y() % line_sz;
	int beg_line = point.x() / line_sz;
	int end_line = point.y() / line_sz;

	ui.hexView->document()->cursor()->moveTo( beg_line, beg_col );
	ui.hexView->document()->cursor()->select( end_line, end_col );
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
	ui.treeWidget->setProperty( "@Data", data );
	ui.hexView->setDocument( QHexDocument::fromMemory<QMemoryBuffer>( data, ui.hexView ) );

	CreateEth( data );
}

void CaptureDialog::CreateEth( const QByteArray & data )
{
	const protocol_eth * eth = (const protocol_eth *) data.constData();
	if( eth != nullptr )
	{
		QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
		QString text = QString( "Ethernet, Src: %1:%2:%3:%4:%5:%6, Dst: %7:%8:%9:%10:%11:%12" )
			.arg( eth->src.mac[0], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->src.mac[1], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->src.mac[2], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->src.mac[3], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->src.mac[4], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->src.mac[5], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->dst.mac[0], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->dst.mac[1], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->dst.mac[2], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->dst.mac[3], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->dst.mac[4], 2, 16, QLatin1Char( '0' ) )
			.arg( eth->dst.mac[5], 2, 16, QLatin1Char( '0' ) );
		item->setText( 0, text );
		item->setData( 0, Qt::UserRole + 1, QPoint( 0, sizeof( protocol_eth ) ) );
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
