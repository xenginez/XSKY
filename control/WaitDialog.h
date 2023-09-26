#pragma once

#include <QDialog>
#include "ui_WaitDialog.h"

class WaitDialog : public QDialog
{
	Q_OBJECT

public:
	WaitDialog(QWidget *parent = nullptr);
	~WaitDialog();

private:
	Ui::WaitDialog ui;
};
