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
        }
        checkServiceObject();
    }

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

        QString result;
        QString o("Invoking testFunctionWithReturnValue( int i = %1 ) returned '%2'");
        QMetaObject::invokeMethod(service, "testFunctionWithReturnValue",
              Q_RETURN_ARG(QString, result), Q_ARG(int, 5));
        o = o.arg(5).arg(result);
        qDebug() << o;

        //test QVariant return types

        QTimer::singleShot(1000, this, SLOT(killService()));
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
