/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"
#include "qmessagemanager.h"
#include "qmfstore_maemo6_p.h"
#include "telepathyengine_maemo6_p.h"

QTM_BEGIN_NAMESPACE

QMessageAccount QMessageAccountPrivate::from(const QMessageAccountId &id, const QString &name, const QMessageAddress &address, const QMessage::TypeFlags &types)
{
    QMessageAccount result;
    result.d_ptr->_id = id;
    result.d_ptr->_name = name;
    result.d_ptr->_address = address;
    result.d_ptr->_types = types;
    return result;
}

QMessageAccountPrivate* QMessageAccountPrivate::implementation(const QMessageAccount &account)
{
    return account.d_ptr;
}

QMessageAccount::QMessageAccount()
 : d_ptr(new QMessageAccountPrivate(this))
{
}

QMessageAccount::QMessageAccount(const QMessageAccountId &id)
 : d_ptr(new QMessageAccountPrivate(this))
{
    *this = QMessageManager().account(id);
}

QMessageAccount::QMessageAccount(const QMessageAccount &other)
 : d_ptr(new QMessageAccountPrivate(this))
{
    this->operator=(other);
}

QMessageAccount& QMessageAccount::operator=(const QMessageAccount& other)
{
    if (&other != this) {
        *d_ptr = *other.d_ptr;
    }

    return *this;
}

QMessageAccount::~QMessageAccount()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageAccountId QMessageAccount::id() const
{
    return d_ptr->_id;
}

QString QMessageAccount::name() const
{
    return d_ptr->_name;
}

QMessage::TypeFlags QMessageAccount::messageTypes() const
{
    return d_ptr->_types;
}

QMessageAccountId QMessageAccount::defaultAccount(QMessage::Type type)
{
    QMessageAccountId accountId;

    switch (type)
    {
    case QMessage::Email:  {
            accountId = QMFStore::instance()->defaultAccount();
            break;
        }
    case QMessage::Sms:  {
            accountId = TelepathyEngine::instance()->defaultAccount(type);
            break;
        }
    default: ;
    }

    return accountId;
}

QTM_END_NAMESPACE
