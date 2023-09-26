#include "DomainListDialog.h"

#include <QMenu>
#include <QSettings>

DomainListDialog::DomainListDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	ui.treeWidget->setRootIsDecorated( false );
	ui.treeWidget->header()->setSectionResizeMode( QHeaderView::ResizeToContents );

	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "XSKY" );
	int count = settings.beginReadArray( "Domains" );
	for( int i = 0; i < count; i++ )
	{
		settings.setArrayIndex( i );

		QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
		item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
		ui.treeWidget->addTopLevelItem( item );

		item->setText( 0, settings.value( "domain", "" ).toString() );
		item->setText( 1, settings.value( "desc", "" ).toString() );
	}
	settings.endArray();
}

DomainListDialog::~DomainListDialog()
{
}

void DomainListDialog::on_pushButtonAccept_clicked()
{
	QSettings settings( QSettings::NativeFormat, QSettings::UserScope, "XSKY" );
	settings.beginWriteArray( "Domains" );
	for( int i = 0; i < ui.treeWidget->topLevelItemCount(); i++ )
	{
		settings.setArrayIndex( i );

		auto item = ui.treeWidget->topLevelItem( i );

		settings.setValue( "domain", item->text( 0 ) );
		settings.setValue( "desc", item->text( 1 ) );
	}
	settings.endArray();

	accept();
}

void DomainListDialog::on_pushButtonReject_clicked()
{
	reject();
}

void DomainListDialog::on_treeWidget_customContextMenuRequested( const QPoint & pos )
{
	QMenu menu( this );
	{
		menu.addAction( QString::fromLocal8Bit( "Ìí¼Ó" ), [this]()
		{
			QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
			item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
			ui.treeWidget->addTopLevelItem( item );

			item->setText( 0, "www.baidu.com" );
			item->setText( 1, "baidu" );
		} );
		if( ui.treeWidget->currentItem() != nullptr )
		{
			menu.addAction( QString::fromLocal8Bit( "É¾³ý" ), [this]()
			{
				auto item = ui.treeWidget->takeTopLevelItem( ui.treeWidget->currentIndex().row() );
				if( item != nullptr ) delete item;
			} );
		}
	}
	menu.exec( QCursor::pos() );
}
