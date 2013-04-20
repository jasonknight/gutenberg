#include <QtGui/QApplication>
#include "mainwindow.h"
#include <windows.h>
#include <QDebug>
extern void display_last_error(DWORD dw) {
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process
    QString text;
    text.fromWCharArray((LPTSTR)lpMsgBuf);
    qDebug() << "display_last_error: " << text << " - " << QString::number(dw);
    LocalFree(lpMsgBuf);
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
