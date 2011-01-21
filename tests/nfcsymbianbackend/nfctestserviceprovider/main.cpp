/*
============================================================================
 Name		: nfctestserviceprovider.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main GUI Application
============================================================================
*/

#include "nfctestserviceprovider.h"

#include <QtGui>
#include <QApplication>
#include <qremoteserviceregister.h>
#include <qservicemanager.h>

#include <QDebug>
#include <qnearfieldmanager.h>
#include <qndefmessage.h>
#include <qnearfieldtarget.h>

#include <iostream>
#include <fstream>

using namespace std;

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void MyOutputHandler(QtMsgType type, const char *msg) {
    static int fd = -1;
      if (fd == -1)
          fd = ::open("E:\\nfctestserviceprovider.log", O_WRONLY | O_CREAT);

      ::write(fd, msg, strlen(msg));
      ::write(fd, "\n", 1);
      ::fsync(fd);

      switch (type) {
      case QtFatalMsg:
          abort();
      }
}

QTM_USE_NAMESPACE

nfctestserviceprovider* w;

class MyContentHandler : public QObject
{
    Q_OBJECT

    
signals:
        void userHandleMessage(const QNdefMessage& msg, QNearFieldTarget* target);
        
public slots:
        void handleMessage(const QNdefMessage& msg, QNearFieldTarget* target)
        {
         w->setWindowTitle( "nfc test service provider" );
         w->showMaximized();
         qInstallMsgHandler(MyOutputHandler);
         qDebug() << " MyContentHandler handleMessage was called." << endl; 
         qInstallMsgHandler(0);
        }
        
public:
    MyContentHandler(QObject* parent = 0)
        : QObject(parent)
    {
    qDebug() << " MyContentHandler constructed !!!!!" << endl;
      connect(this, SIGNAL(userHandleMessage(const QNdefMessage& , QNearFieldTarget* )),
            this, SLOT(handleMessage(const QNdefMessage& , QNearFieldTarget* )));
    }

};

void unregisterExampleService()
{
    QServiceManager m;
    m.removeService("nfctestserviceprovider");
}

void registerExampleService()
{
    unregisterExampleService();
    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/nfctestserviceprovider.xml";
    qWarning() << "xml path:" << path;
    if (!m.addService(path))
        {
        qWarning() << "Cannot register service provider" << path;
        }
    else
        {
        qDebug() << " Register ok" << endl;
        }
}

Q_DECLARE_METATYPE(QMetaType::Type);



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    w = new nfctestserviceprovider();
    
    qInstallMsgHandler(MyOutputHandler);
    
    qRegisterMetaType<QNearFieldTarget*>("QNearFieldTarget*"); 
    // qRegisterMetaType<QNearFieldTarget>("QNearFieldTarget"); 
    qRegisterMetaType<QNdefMessage>("QNdefMessage"); 

    
    qDebug() << "app.applicationName: " << app.applicationName() << "QCoreApplication:: applicaitonname" << QCoreApplication::applicationName() << endl;
    qDebug() << "app.applicationName: " << app.applicationFilePath() << endl;

   
    registerExampleService();
    
    MyContentHandler handler;
    QNearFieldManager manager;
    
    qDebug() << " begin to register target detect handler" << endl;

    manager.registerTargetDetectedHandler(&handler, SIGNAL(userHandleMessage(QNdefMessage, QNearFieldTarget*)));
    
    qDebug() << " register target detect handler ok" << endl;

    int ret = app.exec();
    delete w;
    return ret;
}

#include "main.moc"
