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

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <QtCore>
#include <QtDBus/QDBusMessage>

#include "maemo/types.h"
#include "maemo/statefulldbusproxy.h"
#include "maemo/interfaces/iaccountmanager.h"
#include "maemo/readyobject.h"
#include "maemo/account.h"
#include "qtelephony.h"

QTM_BEGIN_NAMESPACE
     class QTelephonyCallListPrivate;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

namespace DBus
{
    class AccountManager : public StatefullDBusProxy
                        , public ReadyObject
                        , public ReferenceCounter
    {
        Q_OBJECT
        Q_DISABLE_COPY(AccountManager)
    public:
        static const Feature FeatureCore;
        AccountManager(QDBusConnection busconnection, const QString busname, const QString &objectPath, QTelephonyCallListPrivate* callList);
        ~AccountManager();

    public:
        DBus::Interfaces::IAccountManager* pIAccountManager;

    protected slots:
        void onAccountRemoved(const QDBusObjectPath& account);
        void onAccountValidityChanged(const QDBusObjectPath& account, bool valid);

    private:
        QList<AccountPtr> accountList;
        QTelephonyCallListPrivate* ptelephonyCallList;
    };
    typedef SharedPointer<AccountManager> AccountManagerPtr;
}
#endif // ACCOUNTMANAGER_H
