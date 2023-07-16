#pragma once

#include <QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

public slots:
	void on_pushButton_clicked();

private:
	void sync( const QString& addr, const std::function<void( bool, QByteArray )> & callback );
	void route( const QString & addr, const QByteArray & data, const std::function<void(bool, QByteArray )>& callback );
	void server( const QString & addr, const QByteArray & data, const std::function<void( bool, QByteArray )> & callback );

protected:
	void closeEvent( QCloseEvent * e ) override;

private:
	Ui::MainWindow ui;
};
