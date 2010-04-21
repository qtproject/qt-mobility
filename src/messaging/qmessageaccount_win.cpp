/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessageaccount_p.h"
#include "qmessagemanager.h"
#include "winhelpers_p.h"
#include <QDebug>

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

QMessageAccount::QMessageAccount()
    :d_ptr(new QMessageAccountPrivate(this))
{
}

QMessageAccount::QMessageAccount(const QMessageAccountId &id)
    :d_ptr(new QMessageAccountPrivate(this))
{
    *this = QMessageManager().account(id);
}

QMessageAccount::QMessageAccount(const QMessageAccount &other)
    :d_ptr(new QMessageAccountPrivate(this))
{
    this->operator=(other);
}

QMessageAccount& QMessageAccount::operator=(const QMessageAccount& other)
{
    if (&other != this)
        *d_ptr = *other.d_ptr;

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
    QMessageAccountId result;

    QMessageManager::Error code(QMessageManager::NoError);
    MapiSessionPtr mapiSession(MapiSession::createSession(&code));
    if (code == QMessageManager::NoError) {
        result = mapiSession->defaultAccountId(&code, type);
    }

    return result;
}

QTM_END_NAMESPACE
