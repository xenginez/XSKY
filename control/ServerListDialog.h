#pragma once

#include <QDialog>
#include <QButtonGroup>

#include "ui_ServerListDialog.h"

class ServerListDialog : public QDialog
{
	Q_OBJECT

public:
	ServerListDialog(QWidget *parent = nullptr);
	~ServerListDialog();

public slots:
	void on_pushButtonAccept_clicked();

	void on_pushButtonReject_clicked();

	void on_treeWidget_customContextMenuRequested( const QPoint & pos );

private:
	Ui::ServerListDialog ui;
	QButtonGroup * button_group;
};
