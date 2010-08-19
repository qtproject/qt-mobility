#include "qtelephony.h"
#include "maemo/accountmanager.h"
#include "maemo/cli-constants.h"
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/pending-operation.h"

#include <QtCore/QDebug>

namespace Tp
{
    const Feature AccountManager::FeatureCore = Feature(QLatin1String(AccountManager::staticMetaObject.className()), 0, true);

    AccountManager::AccountManager(QDBusConnection busconnection, const QString busname, const QString &objectPath, QTelephonyCallListPrivate* callList)
        : StatefulDBusProxy(busconnection, busname, objectPath)
        , ReadyObject(this, FeatureCore)
        , ptelephonyCallList(callList)
    {
        qDebug() << " AccountManager::AccountManager(";
        qDebug() << "- QDBusConnection base service: " << busconnection.baseService();
        qDebug() << "- QDBusConnection name: " << busconnection.name();
        qDebug() << "- objectPath: " << objectPath;
        qDebug() << "- DBusProxy.busName: " << this->busName();
        qDebug() << "- DBusProxy.objectPath: " << this->objectPath();

        //Create Account Manager interface
        pAccountManagerInterface = new Tp::Client::AccountManagerInterface(this->dbusConnection(),this->busName(), this->objectPath());
        Tp::ObjectPathList opl = pAccountManagerInterface->ValidAccounts();
        qDebug() << "- read Valid Accounts";
        /*
         e.g.
        "/org/freedesktop/Telepathy/Account/ring/tel/ring"
        "/org/freedesktop/Telepathy/Account/spirit/skype/wolfgang_2ebeck100"
        */
        foreach(const QDBusObjectPath& objp, opl){
            qDebug() << "-- path " << objp.path();
            qDebug() << "-- bus " << PATH2BUS(objp.path());
            accountList.push_back(AccountPtr(new Account(QDBusConnection::sessionBus(), busname, objp.path(), ptelephonyCallList)));
        }
        connect(pAccountManagerInterface, SIGNAL(AccountRemoved(QDBusObjectPath)), SLOT(onAccountRemoved(QDBusObjectPath)));
        connect(pAccountManagerInterface, SIGNAL(AccountValidityChanged(QDBusObjectPath,bool)), SLOT(onAccountValidityChanged(QDBusObjectPath,bool)));
    }

    AccountManager::~AccountManager()
    {
        qDebug() << "AccountManager::~AccountManager";
        accountList.clear();
        if(pAccountManagerInterface)
            delete pAccountManagerInterface;
    }

    void AccountManager::onAccountRemoved(const QDBusObjectPath& account)
    {

    }

    void AccountManager::onAccountValidityChanged(const QDBusObjectPath& account, bool valid)
    {

    }
}
