#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QtCore>
#include <QtDBus/QDBusMessage>
#include "maemo/types.h"
#include "maemo/dbus-proxy.h"
#include "maemo/interfaces/cli-account-manager.h"
#include "maemo/ready-object.h"
#include "maemo/account.h"
#include "qtelephony.h"

QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace Tp
{
    class AccountManager : public StatefulDBusProxy
                        , public ReadyObject
                        , public RefCounted
    {
        Q_OBJECT
        Q_DISABLE_COPY(AccountManager)
    public:
        static const Feature FeatureCore;
        AccountManager(QDBusConnection busconnection, const QString busname, const QString &objectPath, QTelephonyCallListPrivate* callList);
        ~AccountManager();

    public:
        Tp::Client::AccountManagerInterface* pAccountManagerInterface;

    protected slots:
        void onAccountRemoved(const QDBusObjectPath& account);
        void onAccountValidityChanged(const QDBusObjectPath& account, bool valid);

    private:
        QList<AccountPtr> accountList;
        QTelephonyCallListPrivate* ptelephonyCallList;
    };
    typedef SharedPtr<AccountManager> AccountManagerPtr;
}
#endif // ACCOUNTMANAGER_H
