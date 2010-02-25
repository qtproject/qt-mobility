#include <QApplication>
#include "qservicemanager.h"
#include "qservicefilter.h"
#include <QTimer>
#include <QMetaObject>
#include <QMetaMethod>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceFilter)

class Test : public QObject 
{
    Q_OBJECT
public:
    Test() : QObject()
    {
        QServiceManager manager;
        service = manager.loadInterface("com.nokia.qt.interface");
        if (!service)  {
            qWarning() << "Cannot find service. Error:" << manager.error();
            QTimer::singleShot(1000, this, SLOT(killProcess()));
        } else {
            checkServiceObject();
        }
    }

Q_SIGNALS:
    void clientsignal();

public slots:
    void checkServiceObject()
    {
        const QMetaObject* mo = service->metaObject();
        qDebug() << "ServiceObject class: " << mo->className() << mo->superClass() << mo->superClass()->className();
        qDebug() << "Methods:" << mo->methodCount()- mo->methodOffset() << "(" << mo->methodCount() << ")";
        for (int i=0; i< mo->methodCount(); i++) {
            QMetaMethod method = mo->method(i);
            QString type;
            switch(method.methodType()) {
            case QMetaMethod::Signal:
                type = "signal"; break;
            case QMetaMethod::Slot:
                type = "slot"; break;
            case QMetaMethod::Constructor:
                type = "constrcutor"; break;
            case QMetaMethod::Method:
                type = "method"; break;
            }
            qDebug() << "    " << i << "." << method.signature() << type;
        }
        qDebug() << "Properties:" << mo->propertyCount()- mo->propertyOffset() << "(" << mo->propertyCount() << ")";
        for(int i=0; i< mo->propertyCount(); i++) {
            QMetaProperty property = mo->property(i);
            QString info = "Readable: %1 Resettable: %2 Writeable: %3 Designable: %4 Scriptable: %5 User: %6 Stored: %7 Constant: %8 Final: %9 HasNotify: %10 EnumType: %11 FlagType: %12";
            info = info.arg(property.isReadable()).arg(property.isResettable()).arg(property.isWritable());
            info = info.arg(property.isDesignable()).arg(property.isScriptable()).arg(property.isUser());
            info = info.arg(property.isStored()).arg(property.isConstant()).arg(property.isFinal());
            info = info.arg(property.hasNotifySignal()).arg(property.isEnumType()).arg(property.isFlagType());

            qDebug() << "    " << i << "." << property.name() << "Type:" << property.typeName() << info;
        }


        QTimer::singleShot(1000, this, SLOT(useService()));
    }

    void useService()
    {
        qDebug() << "Invoking testSlot()";
        QMetaObject::invokeMethod( service, "testSlot" );

        qDebug() << "Invoking testSlotWithArgs(QByteArray, int, QVariant) - default variant";
        QVariant test;
        QMetaObject::invokeMethod( service, "testSlotWithArgs",
              Q_ARG(QByteArray, "array"), Q_ARG(int, 5), Q_ARG(QVariant, test));

        qDebug() << "Invoking testSlotWithArgs(QByteArray, int, QVariant) ";
        test = QVariant(QString("teststring"));
        QMetaObject::invokeMethod( service, "testSlotWithArgs",
              Q_ARG(QByteArray, "array"), Q_ARG(int, 5), Q_ARG(QVariant, test));
        QServiceFilter filter("com.myInterface" , "4.5");
        filter.setServiceName("MyService");

        qDebug() << "Invoking testSlotWithCustomArg(QServiceFilter)";
        QMetaObject::invokeMethod( service, "testSlotWithCustomArg",
              Q_ARG(QServiceFilter, filter));

        //we expect this to fail
        QServiceInterfaceDescriptor desc;
        qDebug() << "Invoking testSlotWithUnknownArg(QServiceInterfaceDescriptor)";
        QMetaObject::invokeMethod( service, "testSlotWithUnknownArg",
              Q_ARG(QServiceInterfaceDescriptor, desc));

        //test known return type
        QString result;
        QString o("Invoking testFunctionWithReturnValue( int i = %1 ) returned '%2'");
        QMetaObject::invokeMethod(service, "testFunctionWithReturnValue",
              Q_RETURN_ARG(QString, result), Q_ARG(int, 4));
        o = o.arg(4).arg(result);
        qDebug() << o;

        //test QVariant return types
        QVariant varResult;
        o = "Invoking testFunctionWithVariantReturnValue() returned '%1'";
        QMetaObject::invokeMethod(service, "testFunctionWithVariantReturnValue",
              Q_RETURN_ARG(QVariant, varResult));
        o = o.arg(varResult.toInt()); //should return false
        qDebug() << o;

        //test custom return type
        QServiceFilter f;
        QMetaObject::invokeMethod(service, "testFunctionWithCustomReturnValue",
              Q_RETURN_ARG(QServiceFilter, f));
        o = "Invoking testFunctionWithCustomReturnValue return: %1: %2 - %3.%4";
        o = o.arg(f.serviceName()).arg(f.interfaceName()).arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << o;


        //signal support from service to client
        //we'll call a function which will trigger the signal on the server side
        connect(service, SIGNAL(signalWithIntParam(int)), this, SLOT(slotSignalWithIntParam(int)));
        QMetaObject::invokeMethod(service, "triggerSignalWithIntParam");

    }

    void slotSignalWithIntParam(int i)
    {
        qDebug() << "signalWithIntParam(int) received. value:" << i;
        
        //test signal with QVariant type
        connect(service, SIGNAL(signalWithVariousParam(QVariant,QString,QServiceFilter)), this, SLOT(slotSignalWithVariousParam(QVariant,QString,QServiceFilter)));
        QMetaObject::invokeMethod(service, "triggerSignalWithVariousParam");
    }

    void slotSignalWithVariousParam(const QVariant& v, const QString& s, const QServiceFilter& f)
    {
        QString o = "%1: %2 - %3.%4";
        o = o.arg(f.serviceName()).arg(f.interfaceName()).arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << "signalWithVariousParam(QVariant,QString,QServiceFilter) received. values:" << v << s << o;

        //signal support from client to service
        connect(this, SIGNAL(clientsignal()), service, SLOT(testSlot()));
        QTimer::singleShot(2000, this, SLOT(triggerClientSignal()));
    }

    void triggerClientSignal()
    {
        qDebug() << "Triggering signal on client side";
        emit clientsignal();

        const int pIndex = service->metaObject()->indexOfProperty("value");
        const QMetaProperty prop = service->metaObject()->property(pIndex);
        if (prop.isValid()) {
            if ( prop.hasNotifySignal())
                QMetaObject::connect(service, prop.notifySignalIndex(), this, metaObject()->indexOfMethod("propertyChanged()"));
            qDebug() << "Property value:" << service->property("value");
            qDebug() << "Changing property to: " << "QWERTY";
            service->setProperty("value", "QWERTY");
            qDebug() << "New property value:" << service->property("value");
            qDebug() << "Resetting property";
            prop.reset(service);
            qDebug() << "New property value:" << service->property("value");
            qDebug() << "Property designable:" << prop.isDesignable();
        }

        QTimer::singleShot(5000, this, SLOT(killService()));
    }

    void propertyChanged()
    {
        qDebug() << "Property change detected";
    }

    void killService()
    {
        delete service;
        QTimer::singleShot(1000, this, SLOT(killProcess()));
    }

    void killProcess()
    {
        QTimer::singleShot(1000, qApp, SLOT(quit()));
    }

private:
    QObject* service;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    qRegisterMetaType<QServiceFilter>();
    qRegisterMetaTypeStreamOperators<QServiceFilter>("QServiceFilter");
    Test* t = new Test();
    t->setParent(&app);

    return app.exec();
}

#include "main.moc"
