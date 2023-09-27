#pragma once

#include <QDialog>
#include "ui_CaptureDialog.h"

class CaptureDialog : public QDialog
{
	Q_OBJECT

public:
	CaptureDialog(QWidget *parent = nullptr);
	~CaptureDialog();

public slots:
	void on_pushButton_clicked();

public:
	void closeEvent( QCloseEvent * e ) override;

private:
	Ui::CaptureDialog ui;
};
