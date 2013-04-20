#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QPrinter>
#include <QDesktopServices>
#include <QDebug>
#include <windows.h>
#include <winspool.h>
#include <QFile>
#include <QByteArray>
#include <QIODevice>
#include <QPrinterInfo>
#include <QList>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QSslConfiguration>
#include <QTimer>
extern void display_last_error(DWORD dw);
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void print(QString path, QByteArray ba);
public slots:
    void on_printButtonClicked();
    void on_urlUpdated(QString);
    void on_userIdUpdated(QString);
    void on_updateButtonClicked();
    void setPrinter(QString path);
    void on_printInfoFetched(QNetworkReply * rep);
    void on_authenticationRequired(QNetworkReply * reply,QAuthenticator * auth);
    void on_timerTimeout();
    void on_printoutFetched(QNetworkReply * rep);
private:
    Ui::MainWindow *ui;
    QSettings * settings;
    QTimer * timer;
};

#endif // MAINWINDOW_H
