#include "DomainListDialog.h"

#include <QMenu>
#include <QComboBox>

#include "Controller.h"

namespace
{
	QStringList ComboItemList =
	{
		QString::fromLocal8Bit( "代理" ),
		QString::fromLocal8Bit( "放行" ),
		QString::fromLocal8Bit( "屏蔽" ),
	};
}

DomainListDialog::DomainListDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	setWindowFlags( windowFlags() & ~Qt::WindowContextHelpButtonHint );

	ui.treeWidget->setRootIsDecorated( false );
	ui.treeWidget->header()->setSectionResizeMode( QHeaderView::ResizeToContents );

	auto domains = Controller::Instance()->GetDomainList();
	for( auto it : domains )
	{
		auto list = it.split( "|" );

		QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
		item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
		ui.treeWidget->addTopLevelItem( item );

		auto combo = new QComboBox( this );
		combo->addItems( ComboItemList );
		ui.treeWidget->setItemWidget( item, 1, combo );

		item->setText( 0, list[0] );
		combo->setCurrentIndex( list[1].toInt() );
		item->setText( 2, list[2] );
	}
}

DomainListDialog::~DomainListDialog()
{
}

void DomainListDialog::on_pushButtonAccept_clicked()
{
	QStringList domains;
	for( int i = 0; i < ui.treeWidget->topLevelItemCount(); i++ )
	{
		auto item = ui.treeWidget->topLevelItem( i );

		domains.push_back( QString( "%1|%2|%3" ).arg( item->text( 0 ) ).arg( qobject_cast<QComboBox *>( ui.treeWidget->itemWidget( item, 1 ) )->currentIndex() ).arg( item->text( 2 ) ) );
	}
	Controller::Instance()->SetDomainList( domains );

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
		menu.addAction( QString::fromLocal8Bit( "添加" ), [this]()
		{
			QTreeWidgetItem * item = new QTreeWidgetItem( ui.treeWidget );
			item->setFlags( item->flags() | Qt::ItemFlag::ItemIsEditable );
			ui.treeWidget->addTopLevelItem( item );

			auto combo = new QComboBox( this );
			combo->addItems( ComboItemList );
			combo->setCurrentText( ComboItemList[0] );

			item->setText( 0, "www.baidu.com" );
			ui.treeWidget->setItemWidget( item, 1, combo );
			item->setText( 2, "baidu" );
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
