#include "WaitDialog.h"

WaitDialog::WaitDialog( QWidget * parent )
	: QDialog( parent )
{
	ui.setupUi( this );

	setWindowOpacity( 0.8 );
	setWindowFlags( Qt::Dialog | Qt::FramelessWindowHint );

	auto p = parent;
	while( p->parentWidget() != nullptr )
		p = p->parentWidget();

	setGeometry( p->geometry() );
}

WaitDialog::~WaitDialog()
{
}
