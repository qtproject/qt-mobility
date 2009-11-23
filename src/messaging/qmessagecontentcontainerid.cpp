/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessagecontentcontainerid.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageContentContainerId
    \ingroup messaging

    \preliminary
    \brief The QMessageContentContainerId class provides a unique identifier for 
    QMessageContentContainer internet media (MIME) type entities, within the scope of the
    containing message.

    A QMessageContentContainerId can be constructed from a string, or converted to a string 
    with toString().

    The identifier of the containing (parent) message is return by messageId()

    A QMessageContentContainerId instance can be tested for validity with isValid(), and compared 
    to other instances for equality.
    
    If the object a QMessageContentContainerId identifies is removed then the identifier may 
    be reused.
    
    If the QMessageContentContainerId refers to an object that has a parent container, and the 
    contents of that parent container are modified by removing or prepending contents (i.e. 
    message attachments are removed or a message body is created) or the parent container 
    itself is removed then the identifier may become invalid, or be reused to reference a 
    different object entirely.
    
    \sa QMessageContentContainer
*/

/*!
    \fn QMessageContentContainerId::QMessageContentContainerId()

    Creates an invalid identifier, toString() will return a null string;
*/

/*!
    \fn QMessageContentContainerId::QMessageContentContainerId(const QMessageContentContainerId& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QMessageContentContainerId::QMessageContentContainerId(const QString& id)

    Constructs an identifier from \a id.
    
    \sa toString()
*/

/*!
    \fn QMessageContentContainerId::~QMessageContentContainerId()
    
    Destroys the identifier.
*/

/*!
    \internal
    \fn QMessageContentContainerId& QMessageContentContainerId::operator=(const QMessageContentContainerId &other)
*/

/*!
    \internal
    \fn bool QMessageContentContainerId::operator==(const QMessageContentContainerId &other) const
*/

/*!
    \internal
*/
bool QMessageContentContainerId::operator!=(const QMessageContentContainerId &other) const
{
    return !operator==(other);
}

/*!
    \fn bool QMessageContentContainerId::toString() const

    Returns the string representation of this identifier.
    
    A null string should be returned if and only if the identifier is invalid.

    String representations of identifiers should not be used to test for equality, instead 
    the equality operator should be used.
*/

/*!
    \fn bool QMessageContentContainerId::isValid() const

    Returns true if this identifier is valid; otherwise returns false.
*/

/*! \typedef QMessageContentContainerIdList

    Qt-style synonym for QList<QMessageContentContainerId>
*/

QTM_END_NAMESPACE
