#pragma once

#include <QDialog>
#include "ui_DomainListDialog.h"

class DomainListDialog : public QDialog
{
	Q_OBJECT

public:
	DomainListDialog(QWidget *parent = nullptr);
	~DomainListDialog();

public slots:
	void on_pushButtonAccept_clicked();

	void on_pushButtonReject_clicked();

	void on_treeWidget_customContextMenuRequested( const QPoint & pos );

private:
	Ui::DomainListDialog ui;
};
