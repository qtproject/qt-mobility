/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
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
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
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
*/

/*!
    \fn QMessageAccountId::QMessageAccountId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/

/*!
    \fn QMessageAccountId::~QMessageAccountId()
    
    Destroys the identifier.
*/

/*!
    \internal
    \fn bool QMessageAccountId::operator==(const QMessageAccountId &other) const
*/

/*! \internal */
bool QMessageAccountId::operator!=(const QMessageAccountId &other) const
{
    return !operator==(other);
}

/*!
    \fn bool QMessageAccountId::operator<(const QMessageAccountId &other) const

    Returns true if this identifier is ordered before \a other using an implementation-defined ordering.
*/

/*!
    \internal
    \fn QMessageAccountId& QMessageAccountId::operator=(const QMessageAccountId &other)
*/

/*!
    \fn bool QMessageAccountId::toString() const

    Returns the string representation of this identifier.
    
    A null string should be returned if and only if the identifier is invalid.

    String representations of identifiers should not be used to test for equality, instead 
    the equality operator should be used.
*/

/*!
    \fn bool QMessageAccountId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/

/*! \typedef QMessageAccountIdList

    Qt-style synonym for QList<QMessageAccountId>
*/

QTM_END_NAMESPACE
