#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Gutenberg");
    settings = new QSettings("gutenberg.ini", QSettings::IniFormat);
    ui->urlEdit->setText(settings->value("url").toString());
    ui->userIdEdit->setText(settings->value("user-id").toString());
    QList<QPrinterInfo> printer_list = QPrinterInfo::availablePrinters();
    for (int i = 0; i < printer_list.length(); i++) {
        QPrinterInfo info = printer_list.at(i);
        QString name = info.printerName();
        qDebug() << "Adding " << name << " to combo box";
        ui->printerComboBox->addItem(name,QVariant(name));
        qDebug() << "added...";
        //ui->printerComboBox->addItem(name,QVariant(name));
     }
    timer = new QTimer(this);
    connect(
        timer,
        SIGNAL(timeout()),
        this,
        SLOT(on_timerTimeout())
    );
    settings->beginGroup("printer-info");
        int interval = settings->value("interval").toInt();
        if (interval) {
            qDebug() << "Interval is " << interval;
            timer->start(interval * 1000);
        }
    settings->endGroup();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::print(QString path, QByteArray ba) {
    qDebug() << " Printing to device named: " << path;
    qDebug() << "FileContent is: " << ba;
    BOOL     bStatus = FALSE;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob = 0L;
    DWORD      dwBytesWritten = 0L;
    HANDLE     hPrinter;
    wchar_t * name = new wchar_t[path.length()+1];
    path.toWCharArray(name);
    name[path.length()] = 0;
    bStatus = OpenPrinter(name,&hPrinter, NULL);

    if (bStatus) {
        DocInfo.pDocName = L"My Document";
        DocInfo.pOutputFile = NULL;
        DocInfo.pDatatype = L"RAW";
        dwJob = StartDocPrinter( hPrinter, 1, (LPBYTE)&DocInfo );
        if (dwJob > 0) {
            bStatus = StartPagePrinter(hPrinter);
            if (bStatus) {
                bStatus = WritePrinter(hPrinter,ba.data(),ba.length(),&dwBytesWritten);
                EndPagePrinter(hPrinter);
                qDebug() << " Page Ended";
            } else {
                qDebug() << "could not start printer";
            }
            EndDocPrinter(hPrinter);
            qDebug() << "DocPrinter ended";
        } else {
            qDebug() << "Couldn't create job";
        }
        ClosePrinter(hPrinter);
    } else {
        DWORD dw = GetLastError();
        qDebug() << "Could not open printer";
        qDebug() << QString::number(dw);
        display_last_error(dw);
    } // end if bStatus
    if (dwBytesWritten != ba.length()) {
        DWORD dw = GetLastError();
        qDebug() << "Wrong number of bytes";

        qDebug() << QString::number(dw);
        display_last_error(dw);
    } else {
        qDebug() << " Bytes were written";
    }
}
void MainWindow::on_printButtonClicked() {
    qDebug() << "print now button clicked";
    on_timerTimeout();
}
void MainWindow::setPrinter(QString path) {
    qDebug() << "Setting Printer to: " << path;
    settings->setValue("printer-name",path);
}
void MainWindow::on_urlUpdated(QString u) {
    qDebug() << "Setting url to: " << u;
    settings->setValue("url",u);
}
void MainWindow::on_userIdUpdated(QString u) {
    qDebug() << "Setting user-id to: " << u;
    settings->setValue("user-id",u);
}
void MainWindow::on_updateButtonClicked() {
    qDebug() << "update button clicked";
    QNetworkRequest request(QUrl::fromUserInput(settings->value("url").toString()));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QNetworkAccessManager * manager = new QNetworkAccessManager(this);
    connect(manager,
            SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this,
            SLOT(on_authenticationRequired(QNetworkReply*,QAuthenticator*))
    );
    QByteArray data;
    QUrl params;
    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);
    params.addQueryItem("id",settings->value("user-id").toString());
    data = params.encodedQuery();
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(on_printInfoFetched(QNetworkReply*)));
    manager->post(request,data);
}
void MainWindow::on_printInfoFetched(QNetworkReply *rep) {
    qDebug() << "Attempting to read reply";
    QString body = rep->readAll();
    qDebug() << "Reply is: " << body;
    QStringList lines = body.split("\n");
    QRegExp rx("([\\w\-]+)\\:(.+)");
    foreach(QString line,lines) {
        int pos = 0;
        while( ( pos = rx.indexIn(line,pos) ) != -1 ) {
            QString key = rx.cap(1);
            QString val = rx.cap(2);
            qDebug() << "Key is: " << key << " value is: " << val;
            settings->beginGroup("printer-info");
                settings->setValue(key,val);
            settings->endGroup();
            pos += rx.matchedLength();
        }
    }
    qDebug() << "Setting up timer";
    settings->beginGroup("printer-info");
        timer->stop();
        int interval = settings->value("interval").toInt();
        if (interval) {
             qDebug() << "Printer Info Interval is " << interval;
            timer->start(interval * 1000);
        } else {
            qDebug() << "interval is not good " << interval;
        }
    settings->endGroup();

}
void MainWindow::on_authenticationRequired(QNetworkReply *reply, QAuthenticator *auth) {
    qDebug() << "Authentication required";
    auth->setUser(settings->value("username").toString());
    auth->setPassword(settings->value("password").toString());
}
void MainWindow::on_timerTimeout() {
    qDebug() << "Timer fired";
    settings->beginGroup("printer-info");
    QNetworkRequest request(QUrl::fromUserInput(settings->value("url").toString()));
    QNetworkAccessManager * manager = new QNetworkAccessManager(this);
    connect(manager,
            SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
            this,
            SLOT(on_authenticationRequired(QNetworkReply*,QAuthenticator*))
    );

    QSslConfiguration c = request.sslConfiguration();
    c.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(c);

    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(on_printoutFetched(QNetworkReply*)));
    manager->get(request);
    settings->endGroup();
}
void MainWindow::on_printoutFetched(QNetworkReply *rep) {
    qDebug() << "Printout Fetched";
    QByteArray ba = rep->readAll();
    qDebug() << ba;
    QString path = settings->value("printer-name").toString();
    if (ba.length() > 1) {
        this->print(path,ba);
    }
}
