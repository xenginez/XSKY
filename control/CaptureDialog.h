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

	void on_tableWidget_cellClicked( int row, int column );

public:
	void closeEvent( QCloseEvent * e ) override;

private:
	QString GetAddress( const QByteArray & data );

	QString GetProtocol( const QByteArray & data );

	QString GetLength( const QByteArray & data );

	QString GetDescription( const QByteArray & data );

private:
	void CreateTree( const QByteArray & data );

	void CreateEth( const QByteArray & data );

	void CreateArp( const QByteArray & data );

	void CreateIP( const QByteArray & data );

	void CreateIcmp( const QByteArray & data );

	void CreateTcp( const QByteArray & data );

	void CreateUdp( const QByteArray & data );

	void CreateDNS( const QByteArray & data );

	void CreateHttp( const QByteArray & data );

private:
	Ui::CaptureDialog ui;
};
