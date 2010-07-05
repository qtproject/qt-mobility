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
#include "qmessageaccount.h"
#include "qmfhelpers_p.h"

#include <qmailaccount.h>
#include <qmailstore.h>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

QTM_BEGIN_NAMESPACE

class QMessageAccountPrivate
{
public:
    QMailAccount _account;

    static QMessageAccount convert(const QMailAccount &account);
    //static QMailAccount convert(const QMessageAccount &account);
};

QMessageAccount QMessageAccountPrivate::convert(const QMailAccount &account)
{
    QMessageAccount result;
    result.d_ptr->_account = account;
    return result;
}

/*
QMailAccount QMessageAccountPrivate::convert(const QMessageAccount &account)
{
    return account.d_ptr->_account;
}
*/

namespace QmfHelpers {

QMessageAccount convert(const QMailAccount &account)
{
    return QMessageAccountPrivate::convert(account);
}

/*
QMailAccount convert(const QMessageAccount &account)
{
    return QMessageAccountPrivate::convert(account);
}
*/

}

QMessageAccount::QMessageAccount()
    : d_ptr(new QMessageAccountPrivate)
{
}

QMessageAccount::QMessageAccount(const QMessageAccountId &id)
    : d_ptr(new QMessageAccountPrivate)
{
    *this = QMessageManager().account(id);
}

QMessageAccount::QMessageAccount(const QMessageAccount &other) 
    : d_ptr(new QMessageAccountPrivate) 
{
    this->operator=(other);
}

QMessageAccount& QMessageAccount::operator=(const QMessageAccount& other)
{
    if (&other != this) {
        d_ptr->_account = other.d_ptr->_account;
    }

    return *this;
}

QMessageAccount::~QMessageAccount()
{
    delete d_ptr;
}

QMessageAccountId QMessageAccount::id() const
{
    return convert(d_ptr->_account.id());
}

QString QMessageAccount::name() const
{
    return d_ptr->_account.name();
}

QMessage::TypeFlags QMessageAccount::messageTypes() const
{
    return convert(d_ptr->_account.messageType());
}

QMessageAccountId QMessageAccount::defaultAccount(QMessage::Type type)
{
    if (QMailStore *store = QMailStore::instance()) {
        QMailAccountKey typeKey(QMailAccountKey::messageType(convert(type)));
        QMailAccountKey enabledKey(QMailAccountKey::status(QMailAccount::Enabled));

        // See if there is a preferred sender
        QMailAccountKey preferredKey(QMailAccountKey::status(QMailAccount::PreferredSender));
        foreach (const QMailAccountId &id, store->queryAccounts(typeKey & enabledKey & preferredKey)) {
            return convert(id);
        }

        // See if there are any sending accounts for this type
        QMailAccountKey capableKey(QMailAccountKey::status(QMailAccount::CanTransmit));
        foreach (const QMailAccountId &id, store->queryAccounts(typeKey & enabledKey & capableKey)) {
            return convert(id);
        }
    }

    return QMessageAccountId();
}

QTM_END_NAMESPACE
