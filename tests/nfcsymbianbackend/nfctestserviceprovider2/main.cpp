/*
============================================================================
 Name		: nfctestserviceprovider2.cpp
 Author	  : 
 Copyright   : Your copyright notice
 Description : Main GUI Application
============================================================================
*/

#include "nfctestserviceprovider2.h"

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
          fd = ::open("E:\\nfctestserviceprovider2.log", O_WRONLY | O_CREAT);

      ::write(fd, msg, strlen(msg));
      ::write(fd, "\n", 1);
      ::fsync(fd);

      switch (type) {
      case QtFatalMsg:
          abort();
      }
}

QTM_USE_NAMESPACE

nfctestserviceprovider2* w;

class MyContentHandler : public QObject
{
    Q_OBJECT

    
signals:
        void userHandleMessage(const QNdefMessage& msg, QNearFieldTarget* target);
        
public slots:
        void handleMessage(const QNdefMessage& msg, QNearFieldTarget* target)
        {
        QFile m_file("E:\\testserviceprovider2.dat");
        m_file.open(QIODevice::ReadWrite | QIODevice::Append);
        QDataStream m_dataStream(&m_file);
        QByteArray msgArray = msg.toByteArray();
        m_dataStream << msgArray;
                         
        w->close();
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
    m.removeService("nfctestserviceprovider2");
}

void registerExampleService()
{
    unregisterExampleService();
    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/nfctestserviceprovider2.xml";
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
    
    w = new nfctestserviceprovider2();
    w->setWindowTitle( "nfc test service provider 2" );
    w->showMaximized();
    
    qInstallMsgHandler(MyOutputHandler);
    
    qRegisterMetaType<QNearFieldTarget*>("QNearFieldTarget*"); 
    qRegisterMetaType<QNdefMessage>("QNdefMessage"); 

    registerExampleService();
    
    MyContentHandler handler;
    QNearFieldManager manager;
    
    int handle = manager.registerTargetDetectedHandler(&handler, SIGNAL(userHandleMessage(QNdefMessage, QNearFieldTarget*)));
    
    QFile m_file("E:\\testserviceprovider2.dat");
    m_file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream m_textStream(&m_file);
    m_textStream << "register handle return " << QString::number(handle);
            
    int ret = app.exec();
    
    manager.unregisterTargetDetectedHandler(handle);
    delete w;
    return ret;
}

#include "main.moc"
