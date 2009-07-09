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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccount.h"
#include "qmessageaccount_p.h"

/*!
    \class QMessageAccount

    \preliminary
    \brief The QMessageAccount class represents a messaging account in the messaging store.
    
    \ingroup messaging

    A QMessageAccount is a logical entity that groups messages according to the 
    method by which they are sent and received.  An account may either transmit messages 
    receive messages or both.
    
    The QMessageAccount class is used for accessing properties of the account related 
    to dealing with the account's folders and messages, rather than for modifying 
    the account itself.
    
    \sa QMessageStore::account()
*/

/*!
    \fn QMessageAccount::QMessageAccount()
    
    Creates an empty and invalid account object.
*/
QMessageAccount::QMessageAccount()
{
}

/*!
    \fn QMessageAccount::QMessageAccount(const QMessageAccountId &id)

    
    Convenience constructor that creates a QMessageAccount by loading the data from the messaging 
    store as specified by the QMessageAccountId \a id. If the account does not exist in the 
    store, then this constructor will create an empty and invalid QMessageAccount.
    
    \sa QMessageStore::account()
*/
QMessageAccount::QMessageAccount(const QMessageAccountId &id)
{
    Q_UNUSED(id)
}

/*!
    \fn QMessageAccount::~QMessageAccount()
    
    Destroys the account object.
*/
QMessageAccount::~QMessageAccount()
{
}

/*!
    \fn QMessageAccount::id() const
    
    Returns the identifier for this account.
*/
QMessageAccountId QMessageAccount::id() const
{
    return QMessageAccountId();  // stub
}

/*!
    \fn QMessageAccount::name() const
    
    Returns the name of the account for display purposes.
*/
QString QMessageAccount::name() const
{
    return QString(); // stub
}

/*!
    \fn QMessageAccount::fromAddress() const
    
    Returns the address from which the account's outgoing messages should be reported as originating.
*/
QMessageAddress QMessageAccount::fromAddress() const
{
    return QMessageAddress(); // stub
}

/*!
    \fn QMessageAccount::types() const
    
    Returns the types of messages this account deals with.
*/
QMessage::TypeFlags QMessageAccount::types() const
{
    return QMessage::None; // stub
}

/*!
    \fn QMessageAccount::signature() const
    
    Return the signature for the account if one exists and is enabled; otherwise returns an empty 
    string.
*/
QString QMessageAccount::signature() const
{
    return QString(); // stub
}

/*!
    \fn QMessageAccount::defaultAccount(QMessage::Type type)
    
    Returns the default account for messages of type \a type.
*/
QMessageAccountId QMessageAccount::defaultAccount(QMessage::Type type)
{
    Q_UNUSED(type)
    return QMessageAccountId(); // stub
}
