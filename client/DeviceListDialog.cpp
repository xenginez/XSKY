#include "DeviceListDialog.h"

#include <QMenu>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>

#include "Controller.h"

namespace
{
	QStringList ComboItemList =
	{
		QString::fromLocal8Bit( "放行" ),
		QString::fromLocal8Bit( "代理" ),
		QString::fromLocal8Bit( "屏蔽" ),
	};
}

DeviceListDialog::DeviceListDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	ui.treeWidget->setRootIsDecorated( false );
	ui.treeWidget->header()->setSectionResizeMode( QHeaderView::ResizeToContents );

	auto devices = Controller::Instance()->GetDeviceList();
	for( auto it : devices )
	{
		auto list = it.split( "|" );

		QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
		item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
		ui.treeWidget->addTopLevelItem( item );

		auto combo = new QComboBox( this );
		combo->addItems( ComboItemList );
		combo->setCurrentIndex( list[1].toInt() );

		item->setText( 0, list[0] );
		ui.treeWidget->setItemWidget( item, 1, combo );
		item->setText( 2, list[2] );
	}
}

DeviceListDialog::~DeviceListDialog()
{
}

void DeviceListDialog::on_pushButtonAccept_clicked()
{
	QStringList devices;
	for( int i = 0; i < ui.treeWidget->topLevelItemCount(); i++ )
	{
		auto item = ui.treeWidget->topLevelItem( i );

		if( !item->text( 0 ).isEmpty() )
			devices.push_back( QString( "%1|%2|%3" ).arg( item->text( 0 ) ).arg( qobject_cast<QComboBox *>( ui.treeWidget->itemWidget( item, 1 ) )->currentIndex() ).arg( item->text( 2 ) ) );
	}
	Controller::Instance()->SetDeviceList( devices );

	accept();
}

void DeviceListDialog::on_pushButtonReject_clicked()
{
	reject();
}

void DeviceListDialog::on_treeWidget_itemChanged( QTreeWidgetItem * item, int column )
{
	if( column == 0 && !item->text( 0 ).isEmpty() )
	{
		if( !QRegExp( R"([a-fA-F0-9]{2}(:[a-fA-F0-9]{2}){5})" ).exactMatch( item->text( 0 ) ) )
		{
			QMessageBox::warning( this, QString::fromLocal8Bit( "警告" ), QString::fromLocal8Bit( "输入的MAC地址格式不正确，请重新输入！" ) );
			item->setText( 0, "00:00:00:00:00:00" );
		}
	}
}

void DeviceListDialog::on_treeWidget_customContextMenuRequested( const QPoint & pos )
{
	QMenu menu( this );
	{
		menu.addAction( QString::fromLocal8Bit( "添加" ), [this]()
		{
			QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
			item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
			ui.treeWidget->addTopLevelItem( item );

			auto combo = new QComboBox( this );
			combo->addItems( ComboItemList );
			combo->setCurrentText( ComboItemList[0] );

			item->setText( 0, "00:00:00:00:00:00" );
			ui.treeWidget->setItemWidget( item, 1, combo );
			item->setText( 2, "" );
		} );
		if( ui.treeWidget->currentItem() != nullptr )
		{
			menu.addAction( QString::fromLocal8Bit( "删除" ), [this]()
			{
				auto item = ui.treeWidget->takeTopLevelItem( ui.treeWidget->currentIndex().row() );
				if( item != nullptr ) delete item;
			} );
		}
	}
	menu.exec( QCursor::pos() );
}
