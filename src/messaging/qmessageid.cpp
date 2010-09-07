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
#include "qmessageid.h"


QTM_BEGIN_NAMESPACE

/*!
    \class QMessageId
    
    \inmodule QtMessaging
    
    \ingroup messaging

    \brief The QMessageId class provides a unique identifier for a QMessage message within the 
    scope of the messaging store.

    A QMessageId can be constructed from a string, or converted to a string with toString().

    A QMessageId instance can be tested for validity with isValid(), and compared to other instances
    for equality.
    
    If the message a QMessageId identifies is removed from the messaging store then the identifier 
    will not be reused.
    
    The QMessageId implementation should be as small as is practical for the underlying platform.
    
    \sa QMessage, QMessageManager
*/

/*!
    \fn QMessageId::QMessageId()

    Creates an invalid identifier, toString() will return a null string.
*/

/*!
    \fn QMessageId::QMessageId(const QMessageId& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QMessageId::QMessageId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/

/*!
    \fn QMessageId::~QMessageId()
    
    Destroys the identifier.
*/

/*!
    \internal
    \fn QMessageId& QMessageId::operator=(const QMessageId &other)
*/

/*!
    \internal
    \fn bool QMessageId::operator==(const QMessageId &other) const
*/

/*! \internal */
bool QMessageId::operator!=(const QMessageId &other) const
{
    return !operator==(other);
}

/*!
    \fn bool QMessageId::operator<(const QMessageId &other) const

    Returns true if this identifier is ordered before \a other using an implementation-defined ordering.
*/

/*!
    \fn bool QMessageId::toString() const

    Returns the string representation of this identifier.
    
    A null string should be returned if and only if the identifier is invalid.
    
    String representations of identifiers should not be used to test for equality, instead 
    the equality operator should be used.
*/

/*!
    \fn bool QMessageId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/

/*! \typedef QMessageIdList

    Qt-style synonym for QList<QMessageId>
*/

static const int registrationId = qRegisterMetaType<QMessageId>();

QTM_END_NAMESPACE
