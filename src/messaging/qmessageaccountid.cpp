/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#include "qmessageaccountid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageAccountId

    \inmodule QtMessaging

    \ingroup messaging
    \since 1.0

    \brief The QMessageAccountId class provides a unique identifier for a QMessageAccount
    messaging account, within the scope of the messaging store.

    A QMessageAccountId can be constructed from a string, or converted to a string with toString().

    A QMessageAccountId instance can be tested for validity with isValid(), and compared to other
    instances for equality.

    If the message a QMessageAccountId identifies is removed from the messaging store then the
    identifier will not be reused.

    \sa QMessageAccount
*/

/*!
    \fn QMessageAccountId::QMessageAccountId()

    Creates an invalid identifier, toString() will return a null string.
*/

/*!
    \fn QMessageAccountId::QMessageAccountId(const QMessageAccountId& other)

    Constructs a copy of \a other.
    \since 1.0
*/

/*!
    \fn QMessageAccountId::QMessageAccountId(const QString& id)

    Constructs an identifier from \a id.

    \since 1.0
    \sa toString()
*/

/*!
    \fn QMessageAccountId::~QMessageAccountId()

    Destroys the identifier.
*/

/*!
    \internal
    \fn bool QMessageAccountId::operator==(const QMessageAccountId &other) const
    \since 1.0
*/

/*! \internal
    \since 1.0
*/
bool QMessageAccountId::operator!=(const QMessageAccountId &other) const
{
    return !operator==(other);
}

/*!
    \fn bool QMessageAccountId::operator<(const QMessageAccountId &other) const

    Returns true if this identifier is ordered before \a other using an implementation-defined ordering.
    \since 1.0
*/

/*!
    \internal
    \fn QMessageAccountId& QMessageAccountId::operator=(const QMessageAccountId &other)
    \since 1.0
*/

/*!
    \fn bool QMessageAccountId::toString() const

    Returns the string representation of this identifier.

    A null string should be returned if and only if the identifier is invalid.

    String representations of identifiers should not be used to test for equality, instead
    the equality operator should be used.
    \since 1.0
*/

/*!
    \fn bool QMessageAccountId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
    \since 1.0
*/

/*! \typedef QMessageAccountIdList

    \relates QMessageAccountId

    Qt-style synonym for QList<QMessageAccountId>
*/

QTM_END_NAMESPACE
