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

/*!
    \class QMessageAccount

    \brief The QMessageAccount class represents a messaging account in the messaging store.
    
    \ingroup messaging

    A QMessageAccount is a logical entity that groups messages according to the 
    method by which they are sent and received.  An account may either transmit messages 
    receive messages or both.
    
    The QMessageAccount class is used for accessing properties of the account related 
    to dealing with the account's folders and messages, rather than for modifying 
    the account itself.
    
    \sa QMessageManager::account()
*/

/*!
    \fn QMessageAccount::QMessageAccount()
    
    Creates an empty and invalid account object.
*/

/*!
    \fn QMessageAccount::QMessageAccount(const QMessageAccountId &id)

    
    Convenience constructor that creates a QMessageAccount by loading the data from the messaging 
    store as specified by the QMessageAccountId \a id. If the account does not exist in the 
    store, then this constructor will create an empty and invalid QMessageAccount.
    
    \sa QMessageManager::account()
*/

/*!
    \fn QMessageAccount::QMessageAccount(const QMessageAccount &other)

    Constructs a copy of \a other.
*/

/*! 
    \internal
    \fn QMessageAccount::operator=(const QMessageAccount& other)
*/

/*!
    \fn QMessageAccount::~QMessageAccount()
    
    Destroys the account object.
*/

/*!
    \fn QMessageAccount::id() const
    
    Returns the identifier for this account.
*/

/*!
    \fn QMessageAccount::name() const
    
    Returns the name of the account for display purposes.
*/

/*!
    \fn QMessageAccount::messageTypes() const
    
    Returns the types of messages this account handles.
*/

/*!
    \fn QMessageAccount::defaultAccount(QMessage::Type type)
    
    Returns the default account for messages of type \a type.
*/
