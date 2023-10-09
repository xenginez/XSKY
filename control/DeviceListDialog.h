#pragma once

#include <QDialog>
#include "ui_DeviceListDialog.h"

class DeviceListDialog : public QDialog
{
	Q_OBJECT

public:
	DeviceListDialog(QWidget *parent = nullptr);
	~DeviceListDialog();

public slots:
	void on_pushButtonAccept_clicked();

	void on_pushButtonReject_clicked();

	void on_treeWidget_itemChanged( QTreeWidgetItem * item, int column );

	void on_treeWidget_customContextMenuRequested( const QPoint & pos );

private:
	Ui::DeviceListDialog ui;
};
