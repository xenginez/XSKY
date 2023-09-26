#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void on_actionStart_triggered( bool checked );
    void on_actionStop_triggered( bool checked );
    void on_actionExit_triggered( bool checked );
    void on_actionProtocol_triggered( bool checked );
    void on_actionDomainList_triggered( bool checked );
    void on_actionServerList_triggered( bool checked );
    void on_labelConnect_clicked();

protected:
    bool eventFilter( QObject * o, QEvent * e ) override;

private:
    Ui::MainWindowClass ui;
};
