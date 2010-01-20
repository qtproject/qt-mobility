#include <QApplication>
#include "qservicemanager.h"
#include <QTimer>

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

        QTimer::singleShot(1000, this, SLOT(killService()));
    }

public slots:
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
