#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QtCore>
#include <QtDBus/QDBusMessage>
#include "maemo/types.h"
#include "maemo/dbus-proxy.h"
#include "maemo/interfaces/cli-account-manager.h"
#include "maemo/interfaces/cli-account.h"
#include "maemo/ready-object.h"
#include "maemo/connection.h"
#include "qtelephonycallinfo.h"
#include "qtelephony.h"


QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace Tp
{
    class Account : public StatefulDBusProxy
                        , public ReadyObject
                        , public RefCounted
    {
        Q_OBJECT
        Q_DISABLE_COPY(Account)
    public:
        static const Feature FeatureCore;
        Account(QDBusConnection busconnection, const QString busname, const QString &objectPath, QTelephonyCallListPrivate* callList);
        ~Account();

    public:
        Tp::Client::AccountInterface* pAccountInterface;
        Tp::ConnectionPtr connection;
        QTelephonyCallListPrivate* ptelephonyCallList;
    };
    typedef SharedPtr<Account> AccountPtr;
}
#endif // ACCOUNTMANAGER_H
