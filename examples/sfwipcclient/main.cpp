#include <QApplication>
#include "qservicemanager.h"
#include <QTimer>
#include <QMetaObject>
#include <QMetaMethod>

QTM_USE_NAMESPACE

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

        qDebug() << "Invoking testSlotWithArgs(QByteArray, int, QVariant)";
        QMetaObject::invokeMethod( service, "testSlotWithArgs",
              Q_ARG(QByteArray, "array"), Q_ARG(int, 5), Q_ARG(QVariant, "dddd"));

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
    Test* t = new Test();
    t->setParent(&app);

    return app.exec();
}

#include "main.moc"
