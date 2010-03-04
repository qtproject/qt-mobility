#include <QCoreApplication>
#include <QTimer>
#include <qservicetyperegister.h>
#include <qservicecontrol.h>
#include "qservicemanager.h"
#include <QDebug>

#include "qservicefilter.h" //only used to test custom metatype

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceFilter);

class SharedTestService : public QObject 
{
    Q_OBJECT
    Q_SERVICE(SharedTestService, "com.nokia.qt.ipcunittest", "3.4")
    Q_PROPERTY(QString value READ value WRITE setValue RESET resetValue NOTIFY valueChanged SCRIPTABLE true DESIGNABLE true STORED true); 
public:

    SharedTestService(QObject* parent = 0) 
        : QObject(parent)
    {
    }

    QString value() const
    {
        qDebug() << "Reading property";
        return m_value;
    }

    void setValue(const QString& value)
    {
        qDebug() << "Writing property";
        m_value = value;
        emit valueChanged();
    }

    void resetValue()
    {
        qDebug() << "Resetting property";
        m_value = "<empty>";
        emit valueChanged();
    }



    Q_INVOKABLE QString testFunctionWithReturnValue(int input)
    {
        qDebug() << "SharedTestService::testFunctionWithReturnValue(" << input << ")";
        QString output("%1 x 3 = %2");
        output = output.arg(input).arg(input*3);
        return output;
    }
    
    Q_INVOKABLE QVariant testFunctionWithVariantReturnValue()
    {
        qDebug() << "SharedTestService::testFunctionWithVariantReturnValue()";
        return QVariant('4');
    }

    Q_INVOKABLE QServiceFilter testFunctionWithCustomReturnValue()
    {
        qDebug() << "SharedTestService::testFunctionWithCustomReturnValue()";
        QServiceFilter f("com.nokia.qt.ipcunittest", "6.7");
        f.setServiceName("MyService");
        return f;

    }

Q_SIGNALS:
    void signalWithIntParam(int);
    void signalWithVariousParam(QVariant,QString,QServiceFilter);
    void valueChanged();
public slots:

    Q_INVOKABLE void triggerSignalWithIntParam()
    {
        QTimer::singleShot(2000, this, SLOT(triggerSignalWithIntParamExecute()));
    }
    
    void triggerSignalWithVariousParam()
    {
        QTimer::singleShot(2000, this, SLOT(triggerSignalWithVariousParamExecute()));
    }

    void triggerSignalWithIntParamExecute()
    {
        qDebug() << "Emitting SharedTestService::signalWithIntParam()";
        emit signalWithIntParam( 5 );
    }

    void triggerSignalWithVariousParamExecute()
    {
        QServiceFilter f("com.nokia.qt.ipcunittest", "6.7");
        f.setServiceName("MyService");
        qDebug() << "Emitting SharedTestService::signalWithVariousParam()";
        emit signalWithVariousParam( QVariant(), QString("string-value"), f );
    }
    void testSlot()
    {
        qDebug() << "SharedTestService::testSlot() called";
    }

    void testSlotWithArgs(const QByteArray& d, int a, const QVariant& variant)
    {
        QString output("%1, %2, %3");
        output = output.arg(d.constData()).arg(a).arg(variant.toString());
        qDebug() << "SharedTestService::testSlotWithArgs(" << output << ") called" << variant.isValid();
    }
    
    void testSlotWithCustomArg(const QServiceFilter& f)
    {
        QString output("%1: %2 - %3.%4");
        output = output.arg(f.serviceName()).arg(f.interfaceName())
                .arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << "SharedTestService::testSlotWithCustomArg(" << output << ") called";
    }
    
    void testSlotWithUnknownArg(const QServiceInterfaceDescriptor& )
    {
        qDebug() << "SharedTestService::testSlotWithUnknownArg(const QServiceInterfaceDescriptor& d)";
    }
private:
    QString m_value;

};
class UniqueTestService : public QObject 
{
    Q_OBJECT
    Q_SERVICE(UniqueTestService, "com.nokia.qt.ipcunittest", "3.5")
    Q_CLASSINFO("UniqueTestService", "First test");
    Q_CLASSINFO("Key", "Value");

    Q_PROPERTY(QString value READ value WRITE setValue RESET resetValue NOTIFY valueChanged SCRIPTABLE true DESIGNABLE true STORED true); 
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged);
    Q_ENUMS(Priority);
    Q_PROPERTY(ServiceFlag serviceFlags READ flags WRITE setFlags);
    Q_FLAGS(ServiceFlag ServiceFlags);
public:

    enum ServiceFlag {
        FirstBit = 0x01,
        SecondBit = 0x02,
        ThirdBit = 0x04
    };
    Q_DECLARE_FLAGS(ServiceFlags, ServiceFlag)

    ServiceFlags flags() const { return m_flags; }
    void setFlags(ServiceFlags f) {
        m_flags = f;
    }


    enum Priority { High, Low, VeryLow };
    void setPriority(Priority p) {
        m_priority = p;
        emit priorityChanged();
    }
    Priority priority() const
    {
        return m_priority;
    }

    QString value() const
    {
        qDebug() << "Reading property";
        return m_value;
    }

    void setValue(const QString& value)
    {
        qDebug() << "Writing property";
        m_value = value;
        emit valueChanged();
    }

    void resetValue()
    {
        qDebug() << "Resetting value";
        m_value = "<empty>";
        emit valueChanged();
    }



    /*Q_INVOKABLE*/ UniqueTestService(QObject* parent = 0) 
        : QObject(parent), m_priority(UniqueTestService::High), m_flags(ThirdBit)
    {
    }

    Q_INVOKABLE QString testFunctionWithReturnValue(int input)
    {
        qDebug() << "UniqueTestService::testFunctionWithReturnValue(" << input << ")";
        QString output("%1 x 3 = %2");
        output = output.arg(input).arg(input*3);
        return output;
    }

    Q_INVOKABLE QVariant testFunctionWithVariantReturnValue()
    {
        qDebug() << "UniqueTestService::testFunctionWithVariantReturnValue()";
        return  QVariant('4');
    }

    Q_INVOKABLE QServiceFilter testFunctionWithCustomReturnValue()
    {
        qDebug() << "TestService::testFunctionWithCustomReturnValue()";
        QServiceFilter f("com.nokia.qt.ipcunittest", "6.7");
        f.setServiceName("MyService");
        return f;

    }

Q_SIGNALS:
    void signalWithIntParam(int);
    void signalWithVariousParam(QVariant,QString,QServiceFilter);
    void valueChanged();
    void priorityChanged();

public slots:

    void triggerSignalWithIntParam()
    {
        QTimer::singleShot(2000, this, SLOT(triggerSignalWithIntParamExecute()));
    }

    void triggerSignalWithVariousParam()
    {
        QTimer::singleShot(2000, this, SLOT(triggerSignalWithVariousParamExecute()));
    }

    void triggerSignalWithIntParamExecute()
    {
        qDebug() << "Emitting UniqueTestService::signalWithIntParam()";
        emit signalWithIntParam( 5 );
    }
    
    void triggerSignalWithVariousParamExecute()
    {
        QServiceFilter f("com.nokia.qt.ipcunittest", "6.7");
        f.setServiceName("MyService");
        qDebug() << "Emitting UniqueTestService::signalWithVariousParam()";
        emit signalWithVariousParam( QVariant(), QString("string-value"), f );
    }

    void testSlot() {
        qDebug() << "UniqueTestService::testSlot() called";
    }
    
    void testSlotWithArgs(const QByteArray& d, int a, const QVariant& variant)
    {
        QString output("%1, %2, %3");
        output = output.arg(d.constData()).arg(a).arg(variant.toString());
        qDebug() << "UniqueTestService::testSlotWithArgs(" << output << ") called";
    }

    void testSlotWithCustomArg(const QServiceFilter& f)
    {
        QString output("%1: %2 - %3.%4");
        output = output.arg(f.serviceName()).arg(f.interfaceName())
                .arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << "UniqueTestService::testSlotWithCustomArg(" << output << ") called";
    }
    
    void testSlotWithUnknownArg(const QServiceInterfaceDescriptor& )
    {
        qDebug() << "UniqueTestService::testSlotWithUnknownArg(const QServiceInterfaceDescriptor& d)";
    }
private:
    QString m_value;
    Priority m_priority;
    ServiceFlags m_flags;
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

    QServiceTypeRegister::registerType<SharedTestService>(QServiceTypeRegister::SharedInstance);
    QServiceTypeRegister::registerType<UniqueTestService>(QServiceTypeRegister::UniqueInstance);


    //this only works
    QServiceControl* control = new QServiceControl();
    control->publishServices("qt_sfw_example_ipc_unittest");
#ifdef Q_OS_SYMBIAN
    qDebug("OTR Calling RProcess::rendezvous TODO if needed and correct place");
    RProcess::Rendezvous(KErrNone); 
#endif
    int res =  app.exec();
    delete control;
    unregisterExampleService();
    return res;
}


#include "main.moc"
