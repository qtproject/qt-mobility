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
#include "qmessageaddress.h"
#include "qmessageaddress_p.h"
#include "addresshelper_p.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageAddress

    \brief The QMessageAddress class provides an interface for a message address.

    \ingroup messaging
   
    A message address consists of an addressee string and a type.
*/    

/*!
    \enum QMessageAddress::Type

    This enum type is used to describe the type of a message address.
    
    \value System             A system address.
    \value Phone              A telephony address.
    \value Email              An Email, Internet Message Format address.
    \value InstantMessage     An Instant Messaging address.
    
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
    Constructs a message address with the given \a type and \a addressee.
*/
QMessageAddress::QMessageAddress(Type type, const QString &addressee)
    : d_ptr(new QMessageAddressPrivate(this))
{
    d_ptr->type = type;
    d_ptr->addressee = addressee;
}

/*!
    Constructs a copy of \a other.
*/
QMessageAddress::QMessageAddress(const QMessageAddress &other)
    : d_ptr(new QMessageAddressPrivate(this))
{
    this->operator=(other);
}

/*! \internal */
QMessageAddress& QMessageAddress::operator=(const QMessageAddress& other)
{
    if (&other != this) {
        d_ptr->addressee = other.d_ptr->addressee;
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

/*! \internal */
bool QMessageAddress::operator==(const QMessageAddress& other) const
{
    return ((d_ptr->type == other.d_ptr->type) && (d_ptr->addressee == other.d_ptr->addressee));
}

/*! \internal */
bool QMessageAddress::operator!=(const QMessageAddress& other) const
{
    return !operator==(other);
}

/*!
    Returns the addressee.

    \sa setAddressee()
*/
QString QMessageAddress::addressee() const
{
    return d_ptr->addressee;
}

/*!
    Sets the addressee to \a addressee.

    \sa addressee()
*/
void QMessageAddress::setAddressee(const QString &addressee)
{
    d_ptr->addressee = addressee;
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

/*!
    Parses an email address into name, address and suffix parts.

    * \a name is set to the name part of the email address.
    * \a address is set to the address part of the email address.
    * \a suffix is set to the suffix part of the email address.
    
    If the starting delimeter between the name and address part of the email address is found 
    then * \a startDelimeterFound is set to true; otherwise * \a startDelimeterFound is set to false;

    If the starting delimeter is not found, then the parsing is ambiguous and both * \a name and
    * \a address will be set to the input \a emailAddress.

    If the ending delimeter of the address part of the email address is found 
    then * \a endDelimeterFound is set to true; otherwise * \a endDelimeterFound is set to false;

*/
void QMessageAddress::parseEmailAddress(const QString &emailAddress, QString *name, QString *address, QString *suffix, bool *startDelimeterFound, bool *endDelimeterFound)
{
    QString *aName(name);
    QString strName;
    if (!aName)
        aName = &strName;

    QString *aAddress(address);
    QString strAddress;
    if (!aAddress)
        aAddress = &strAddress;

    QString *aSuffix(suffix);
    QString strSuffix;
    if (!aSuffix)
        aSuffix = &strSuffix;

    bool *aStartDelimeterFound(startDelimeterFound);
    bool ignored1;
    if (!aStartDelimeterFound)
        aStartDelimeterFound = &ignored1;

    bool *aEndDelimeterFound(endDelimeterFound);
    bool ignored2;
    if (!aEndDelimeterFound)
        aEndDelimeterFound = &ignored2;

    QString emailAddressCopy(emailAddress);
    qParseMailbox(emailAddressCopy, *aName, *aAddress, *aSuffix, *aStartDelimeterFound, *aEndDelimeterFound);
}

/*! \typedef QMessageAddressList

    Qt-style synonym for QList<QMessageAddress>
*/

QTM_END_NAMESPACE
