#include "ServerListDialog.h"

#include <QMenu>
#include <QSettings>
#include <QCheckBox>

#include "Controller.h"

ServerListDialog::ServerListDialog( QWidget * parent )
	: QDialog( parent )
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	ui.treeWidget->setRootIsDecorated( false );
	ui.treeWidget->header()->setSectionResizeMode( QHeaderView::ResizeToContents );

	button_group = new QButtonGroup( this );

	auto select = Controller::Instance()->GetServer();

	QSettings settings( "config.ini", QSettings::IniFormat );
	int count = settings.beginReadArray( "Servers" );
	for( int i = 0; i < count; i++ )
	{
		settings.setArrayIndex( i );

		auto ip = settings.value( "ip", "" ).toString();
		auto port = settings.value( "port", 0 ).toInt();
		auto desc = settings.value( "desc", "" ).toString();

		QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
		item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
		ui.treeWidget->addTopLevelItem( item );

		auto check = new QCheckBox( this );

		item->setText( 0, ip );
		item->setText( 1, QString::number( port ) );
		item->setText( 2, desc );
		ui.treeWidget->setItemWidget( item, 3, check );

		button_group->addButton( check );

		if( QString( "%1:%2" ).arg( item->text( 0 ) ).arg( item->text( 1 ) ) == select )
		{
			check->setChecked( true );
		}
	}
	settings.endArray();
}

ServerListDialog::~ServerListDialog()
{
}

void ServerListDialog::on_pushButtonAccept_clicked()
{
	QSettings settings( "config.ini", QSettings::IniFormat );
	settings.beginWriteArray( "Servers" );
	for( int i = 0; i < ui.treeWidget->topLevelItemCount(); i++ )
	{
		settings.setArrayIndex( i );

		auto item = ui.treeWidget->topLevelItem( i );

		settings.setValue( "ip", item->text( 0 ) );
		settings.setValue( "port", item->text( 1 ).toInt() );
		settings.setValue( "desc", item->text( 2 ) );
		auto check = qobject_cast<QCheckBox *>( ui.treeWidget->itemWidget( item, 3 ) );

		if( check->isChecked() )
		{
			Controller::Instance()->SetServer( QString( "%1:%2" ).arg( item->text( 0 ) ).arg( item->text( 1 ) ) );
		}
	}
	settings.endArray();

	accept();
}

void ServerListDialog::on_pushButtonReject_clicked()
{
	reject();
}

void ServerListDialog::on_treeWidget_customContextMenuRequested( const QPoint & pos )
{
	QMenu menu( this );
	{
		menu.addAction( QString::fromLocal8Bit( "Ìí¼Ó" ), [this]()
		{
			QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
			item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
			ui.treeWidget->addTopLevelItem( item );

			auto check = new QCheckBox( this );

			item->setText( 0, "127.0.0.1" );
			item->setText( 1, QString::number( 48888 ) );
			item->setText( 2, "" );
			ui.treeWidget->setItemWidget( item, 3, check );

			button_group->addButton( check );
		} );
		if( ui.treeWidget->currentItem() != nullptr )
		{
			menu.addAction( QString::fromLocal8Bit( "É¾³ý" ), [this]()
			{
				auto item = ui.treeWidget->takeTopLevelItem( ui.treeWidget->currentIndex().row() );
				if( item != nullptr )
				{
					auto check = qobject_cast<QCheckBox *>( ui.treeWidget->itemWidget( item, 3 ) );
					button_group->removeButton( check );
					delete item;
				}
			} );
		}
	}
	menu.exec( QCursor::pos() );
}
