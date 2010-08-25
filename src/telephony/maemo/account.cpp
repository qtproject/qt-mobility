#include "qtelephony.h"
#include "maemo/account.h"
#include "maemo/cli-constants.h"
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/pending-operation.h"

#include <QtCore/QDebug>

namespace Tp
{
    const Feature Account::FeatureCore = Feature(QLatin1String(Account::staticMetaObject.className()), 0, true);

    Account::Account(QDBusConnection busconnection, const QString busname, const QString &objectPath, QTelephonyCallListPrivate* callList)
        : StatefulDBusProxy(busconnection, busname, objectPath)
        , ReadyObject(this, FeatureCore)
        , ptelephonyCallList(callList)
    {
        qDebug() << " Account::Account(";
        qDebug() << "- QDBusConnection base service: " << busconnection.baseService();
        qDebug() << "- QDBusConnection name: " << busconnection.name();
        qDebug() << "- objectPath: " << objectPath;
        qDebug() << "- DBusProxy.busName: " << this->busName();
        qDebug() << "- DBusProxy.objectPath: " << this->objectPath();

        //Create Account Manager interface
        pAccountInterface = new Tp::Client::AccountInterface(this->dbusConnection(),this->busName(), this->objectPath());
        QString connectionpath = pAccountInterface->Connection().path();
        qDebug() << "- connection bus " << connectionpath;
        qDebug() << "- connection path " << connectionpath;
        connection = ConnectionPtr(new Connection(QDBusConnection::sessionBus(), PATH2BUS(connectionpath), connectionpath, ptelephonyCallList));
    }

    Account::~Account()
    {
        qDebug() << "Account::~Account";
        if(pAccountInterface)
            delete pAccountInterface;
    }
}
