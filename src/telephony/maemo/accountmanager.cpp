/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QDebug>

#include "qtelephony.h"
#include "maemo/accountmanager.h"
#include "maemo/constants.h"
#include "maemo/qtelephonycalllist_maemo_p.h"
#include "maemo/pendingoperation.h"

namespace DBus
{
    const Feature AccountManager::FeatureCore = Feature(QLatin1String(AccountManager::staticMetaObject.className()), 0, true);

    AccountManager::AccountManager(QDBusConnection busconnection, const QString busname, const QString &objectPath, QTelephonyCallListPrivate* callList)
        : StatefullDBusProxy(busconnection, busname, objectPath)
        , ReadyObject(this, FeatureCore)
        , ptelephonyCallList(callList)
    {
        qDebug() << " AccountManager::AccountManager(";

        if(isValid()){
            //Create Account Manager interface
            pIAccountManager = new DBus::Interfaces::IAccountManager(this->dbusConnection(),busName(), this->objectPath());
            if(pIAccountManager->isValid()){
                DBus::Interfaces::ObjectPathList opl = pIAccountManager->ValidAccounts();
                /*
                 e.g.
                "/org/freedesktop/Telepathy/Account/ring/tel/ring"
                "/org/freedesktop/Telepathy/Account/spirit/skype/wolfgang_2ebeck100"
                */
                foreach(const QDBusObjectPath& objp, opl){
                    accountList.push_back(AccountPtr(new Account(QDBusConnection::sessionBus(), busname, objp.path(), ptelephonyCallList)));
                }
                connect(pIAccountManager, SIGNAL(AccountRemoved(QDBusObjectPath)), SLOT(onAccountRemoved(QDBusObjectPath)));
                connect(pIAccountManager, SIGNAL(AccountValidityChanged(QDBusObjectPath,bool)), SLOT(onAccountValidityChanged(QDBusObjectPath,bool)));
            }
        }
    }

    AccountManager::~AccountManager()
    {
        qDebug() << "AccountManager::~AccountManager";
        accountList.clear();
        if(pIAccountManager)
            delete pIAccountManager;
    }

    void AccountManager::onAccountRemoved(const QDBusObjectPath& account)
    {
        qDebug() << "AccountManager::onAccountRemoved";
        int found = -1;
        for(int i = 0; i < accountList.count(); i++){
            if(accountList[i].data()->objectPath() == account.path()){
                found = i;
                break;
            }
        }
    }

    void AccountManager::onAccountValidityChanged(const QDBusObjectPath& account, bool valid)
    {
        qDebug() << "AccountManager::onAccountValidityChanged";
        onAccountRemoved(account);
        if(valid){
            accountList.push_back(AccountPtr(new Account(QDBusConnection::sessionBus(), busName(), account.path(), ptelephonyCallList)));
        }
    }
}//DBus

