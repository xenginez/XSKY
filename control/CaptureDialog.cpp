#include "CaptureDialog.h"

#include "Controller.h"

CaptureDialog::CaptureDialog( QWidget * parent )
	: QDialog( parent )
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	connect( Controller::Instance(), &Controller::captureItem, [this]( const QByteArray & data )
	{
		int row = ui.tableWidget->rowCount();
		ui.tableWidget->insertRow( row );

	} );
}

CaptureDialog::~CaptureDialog()
{
}

void CaptureDialog::closeEvent( QCloseEvent * e )
{
	if( Controller::Instance()->IsCapture() )
	{
		Controller::Instance()->StopCapture();
	}
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
		QString setting;
		{

		}
		Controller::Instance()->StartCapture( setting );
		ui.pushButton->setText( QString::fromLocal8Bit( "停止捕获" ) );
	}
}
