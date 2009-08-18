/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaddress.h"
#include "qmessageaddress_p.h"

/*!
    \class QMessageAddress

    \preliminary
    \brief The QMessageAddress class provides an interface for a message address.

    \ingroup messaging
   
    A message address consists of a recipient string and a type.
*/    

/*!
    \enum QMessageAddress::Type

    This enum type is used to describe the type of a message address.
    
    \value System   A system address.
    \value Phone    A telephony address.
    \value Email    An Email, Internet Message Format address.
    \value Xmpp     An XMPP, Extensible Messaging and Presence Protocol address.
    
    \sa type(), setType()
*/

/*!
    Constructs an empty message address.
*/
QMessageAddress::QMessageAddress()
    : d_ptr(new QMessageAddressPrivate(this))
{
    d_ptr->type = QMessageAddress::System;
}

/*!
    Constructs a message address with the given recipient \a recipient and type \a type.
*/
QMessageAddress::QMessageAddress(const QString &recipient, Type type)
    : d_ptr(new QMessageAddressPrivate(this))
{
    d_ptr->recipient = recipient;
    d_ptr->type = type;
}

/*!
    Constructs a copy of \a other.
    
    \sa operator=()
*/
QMessageAddress::QMessageAddress(const QMessageAddress &other)
    : d_ptr(new QMessageAddressPrivate(this))
{
    this->operator=(other);
}

/*!
    Assign the value of the QMessageAddress \a other to this.
*/
const QMessageAddress& QMessageAddress::operator=(const QMessageAddress& other)
{
    if (&other != this) {
        d_ptr->recipient = other.d_ptr->recipient;
        d_ptr->type = other.d_ptr->type;
    }

    return *this;
}

/*!
    Destroys the message address.
*/
QMessageAddress::~QMessageAddress()
{
    delete d_ptr;
    d_ptr = 0;
}

/*!
    Returns true if the message address is equal to that contained by \a other.
*/
bool QMessageAddress::operator==(const QMessageAddress& other) const
{
    return ((d_ptr->type == other.d_ptr->type) && (d_ptr->recipient == other.d_ptr->recipient));
}

/*! \internal */
bool QMessageAddress::operator!=(const QMessageAddress& other) const
{
    return !operator==(other);
}

/*!
    Returns the recipient.

    \sa setRecipient()
*/
QString QMessageAddress::recipient() const
{
    return d_ptr->recipient;
}

/*!
    Sets the recipient to \a recipient.

    \sa recipient()
*/
void QMessageAddress::setRecipient(const QString &recipient)
{
    d_ptr->recipient = recipient;
}

/*!
    Returns the type of the message address.

    \sa setType()
*/
QMessageAddress::Type QMessageAddress::type() const
{
    return d_ptr->type;
}

/*!
    Sets the type of the message address to \a type.

    \sa type()
*/
void QMessageAddress::setType(Type type)
{
    d_ptr->type = type;
}


/*! \typedef QMessageAddressList

    Qt-style synonym for QList<QMessageAddress>
*/


