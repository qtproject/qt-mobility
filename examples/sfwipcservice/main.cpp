#include <QApplication>
#include <qservicetyperegister.h>
#include <qservicecontrol.h>
#include <instancemanager_p.h>
#include <QDebug>

QTM_USE_NAMESPACE

class TestService : public QObject 
{
    Q_OBJECT
    Q_SERVICE(TestService, "com.nokia.qt.interface", "3.4")
public:

    Q_INVOKABLE TestService(QObject* parent = 0) 
        : QObject(parent)
    {
    }

};
class TestService2 : public QObject 
{
    Q_OBJECT
    Q_SERVICE(TestService2, "com.nokia.qt.interface", "3.5")
public:

    TestService2(QObject* parent = 0) 
        : QObject(parent)
    {
    }
};


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
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
    return app.exec();
    delete control;
}

#include "main.moc"
