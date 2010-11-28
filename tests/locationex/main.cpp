#include <QtGui/QApplication>
#include "mainwindow.h"

#include <QProcessEnvironment>
#include <QtNetwork/QNetworkProxyFactory>
#include <QUrl>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString urlEnv = QProcessEnvironment::systemEnvironment().value("http_proxy");
    if (!urlEnv.isEmpty()) {
        QUrl url = QUrl(urlEnv, QUrl::TolerantMode);
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(url.host());
        proxy.setPort(url.port(8080));
        QNetworkProxy::setApplicationProxy(proxy);
    } else
        QNetworkProxyFactory::setUseSystemConfiguration(true);

    MainWindow w;
    w.show();

    return a.exec();
}
