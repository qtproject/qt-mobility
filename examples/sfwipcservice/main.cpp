#include <QApplication>
#include <QTimer>
#include <qservicetyperegister.h>
#include <qservicecontrol.h>
#include <instancemanager_p.h>
#include "qservicemanager.h"
#include <QDebug>

#include "qservicefilter.h" //only used to test custom metatype

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceFilter);

class TestService : public QObject 
{
    Q_OBJECT
    Q_SERVICE(TestService, "com.nokia.qt.interface", "3.4")
public:

    TestService(QObject* parent = 0) 
        : QObject(parent)
    {
    }

    Q_INVOKABLE QString testFunctionWithReturnValue(int input)
    {
        qDebug() << "TestService::testFunctionWithReturnValue(" << input << ")";
        QString output("%1 x 3 = %2");
        output = output.arg(input).arg(input*3);
        return output;
    }
    
    Q_INVOKABLE QVariant testFunctionWithVariantReturnValue()
    {
        qDebug() << "TestService::testFunctionWithVariantReturnValue()";
        return QVariant('4');
    }

    Q_INVOKABLE QServiceFilter testFunctionWithCustomReturnValue()
    {
        qDebug() << "TestService::testFunctionWithCustomReturnValue()";
        QServiceFilter f("com.nokia.qt.testservice", "6.7");
        f.setServiceName("MyService");
        return f;

    }

Q_SIGNALS:
    void signalWithIntParam(int);
public slots:

    Q_INVOKABLE void triggerSignalWithIntParam()
    {
        QTimer::singleShot(2000, this, SLOT(triggerSignalWithIntParamExecute()));
    }

    void triggerSignalWithIntParamExecute()
    {
        qDebug() << "Emitting TestService::signalWithIntParam()";
        emit signalWithIntParam( 5 );
    }


    void testSlot()
    {
        qDebug() << "TestService::testSlot() called";
    }

    void testSlotWithArgs(const QByteArray& d, int a, const QVariant& variant)
    {
        QString output("%1, %2, %3");
        output = output.arg(d.constData()).arg(a).arg(variant.toString());
        qDebug() << "TestService::testSlotWithArgs(" << output << ") called" << variant.isValid();
    }
    
    void testSlotWithCustomArg(const QServiceFilter& f)
    {
        QString output("%1: %2 - %3.%4");
        output = output.arg(f.serviceName()).arg(f.interfaceName())
                .arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << "TestService::testSlotWithCustomArg(" << output << ") called";
    }
    
    void testSlotWithUnknownArg(const QServiceInterfaceDescriptor& )
    {
        qDebug() << "TestService::testSlotWithUnknownArg(const QServiceInterfaceDescriptor& d)";
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

    Q_INVOKABLE QString testFunctionWithReturnValue(int input)
    {
        qDebug() << "TestService2::testFunctionWithReturnValue(" << input << ")";
        QString output("%1 x 3 = %2");
        output = output.arg(input).arg(input*3);
        return output;
    }

    Q_INVOKABLE QVariant testFunctionWithVariantReturnValue()
    {
        qDebug() << "TestService2::testFunctionWithVariantReturnValue()";
        return  QVariant('4');
    }

    Q_INVOKABLE QServiceFilter testFunctionWithCustomReturnValue()
    {
        qDebug() << "TestService::testFunctionWithCustomReturnValue()";
        QServiceFilter f("com.nokia.qt.testservice", "6.7");
        f.setServiceName("MyService");
        return f;

    }

Q_SIGNALS:
    void signalWithIntParam(int);


public slots:
    
    void triggerSignalWithIntParam()
    {
        //qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAASSSSSSSSSSSSs";
        //emit signalWithIntParam( 5 );
        QTimer::singleShot(2000, this, SLOT(triggerSignalWithIntParamExecute()));
    }

    void triggerSignalWithIntParamExecute()
    {
        qDebug() << "Emitting TestService2::signalWithIntParam()";
        emit signalWithIntParam( 5 );
    }

    void testSlot() {
        qDebug() << "TestService2::testSlot() called";
    }
    
    void testSlotWithArgs(const QByteArray& d, int a, const QVariant& variant)
    {
        QString output("%1, %2, %3");
        output = output.arg(d.constData()).arg(a).arg(variant.toString());
        qDebug() << "TestService2::testSlotWithArgs(" << output << ") called";
    }

    void testSlotWithCustomArg(const QServiceFilter& f)
    {
        QString output("%1: %2 - %3.%4");
        output = output.arg(f.serviceName()).arg(f.interfaceName())
                .arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << "TestService2::testSlotWithCustomArg(" << output << ") called";
    }
    
    void testSlotWithUnknownArg(const QServiceInterfaceDescriptor& )
    {
        qDebug() << "TestService2::testSlotWithUnknownArg(const QServiceInterfaceDescriptor& d)";
    }
Q_SIGNALS:
    //void signalWithIntParam1(int);
    //void signalWithIntParam4(int);
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
        qWarning() << "Cannot register IPCExampleService" << path;
}

Q_DECLARE_METATYPE(QMetaType::Type);

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    qRegisterMetaType<QServiceFilter>();
    qRegisterMetaTypeStreamOperators<QServiceFilter>("QServiceFilter");

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
