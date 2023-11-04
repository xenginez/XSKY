#pragma once

#include <QDialog>
#include "ui_ProtocolDialog.h"

class ProtocolDialog : public QDialog
{
	Q_OBJECT

public:
	ProtocolDialog(QWidget *parent = nullptr);
	~ProtocolDialog();

public slots:
	void on_pushButtonAccept_clicked();

	void on_pushButtonReject_clicked();

private:
	Ui::ProtocolDialog ui;
};
