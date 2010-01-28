#include <QApplication>
#include <qservicetyperegister.h>
#include <qservicecontrol.h>
#include <instancemanager_p.h>
#include "qservicemanager.h"
#include <QDebug>

QTM_USE_NAMESPACE

class TestService : public QObject 
{
    Q_OBJECT
    Q_SERVICE(TestService, "com.nokia.qt.interface", "3.4")
public:

    TestService(QObject* parent = 0) 
        : QObject(parent)
    {
    }

Q_SIGNALS:
    void testSignal();
public slots:
    void testSlot()
    {
        qDebug() << "TestService::testSlot() called";
    }

    void testSlotWithArgs(const QByteArray& d, int a, const QVariant& variant)
    {
        QString output("%1, %2, %3");
        output = output.arg(d.constData()).arg(a).arg(variant.toString());
        qDebug() << "TestService::testSlotWithArgs(" << output << ") called";
    }

};
class TestService2 : public QObject 
{
    Q_OBJECT
    Q_SERVICE(TestService2, "com.nokia.qt.interface", "3.5")
public:

    /*Q_INVOKABLE*/ TestService2(QObject* parent = 0) 
        : QObject(parent)
    {
    }
Q_SIGNALS:
    void testSignal();

public slots:
    void testSlot() {
        qDebug() << "TestService2::testSlot() called";
    }
    
    void testSlotWithArgs(const QByteArray& d, int a, const QVariant& variant)
    {
        QString output("%1, %2, %3");
        output = output.arg(d.constData()).arg(a).arg(variant.toString());
        qDebug() << "TestService2::testSlotWithArgs(" << output << ") called";
    }
};

void unregisterExampleService()
{
    QServiceManager m;
    m.removeService("IPCExampleService");
}

void registerExampleService()
{
    unregisterExampleService();
    QServiceManager m;
    const QString path = QCoreApplication::applicationDirPath() + "/xmldata/ipcexampleservice.xml";
    bool r = m.addService(path);
    if (!r)
        qWarning() << "Cannot register IPCExampleService";
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    registerExampleService();

    QServiceTypeRegister::registerType<TestService>(QServiceTypeRegister::SharedInstance);
    QServiceTypeRegister::registerType<TestService2>(QServiceTypeRegister::UniqueInstance);

    InstanceManager* in = InstanceManager::instance();
    QList<QServiceTypeIdent> idents = in->allIdents();
    foreach(QServiceTypeIdent i, idents) {
        QUuid uid;
        QObject* testService = in->createObjectInstance( i, uid );
        qWarning() << i.first << i.second << testService << uid;
        testService = in->createObjectInstance( i, uid );
        qWarning() << i.first << i.second << testService << uid;
    }


    QServiceControl* control = new QServiceControl();
    control->publishServices("qt_sfw_example_ipc_ident");
    int res =  app.exec();
    delete control;
    unregisterExampleService();
    return res;
}

#include "main.moc"
