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
#include "qmessageaccountordering.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QMessageAccountOrdering

    \preliminary
    \brief The QMessageAccountOrdering class defines the parameters used for sorting a subset of 
    queried accounts from the messaging store.
    \ingroup messaging

    A QMessageAccountOrdering is composed of a account property to sort and a sort order. 
    The QMessageAccountOrdering class is used in conjunction with the QMessageManager::queryAccounts() 
    function to sort account results according to the criteria defined by the ordering.
    
    \sa QMessageManager, QMessageAccountFilter
*/

/*!
    \fn QMessageAccountOrdering::QMessageAccountOrdering()
  
    Create a QMessageAccountOrdering with specifying matching parameters.

    A default-constructed ordering (one for which isEmpty() returns true) sorts no accounts. 

    The result of combining an empty ordering with a non-empty ordering is the same as the original 
    non-empty ordering.

    The result of combining two empty keys is an empty ordering.
*/

/*!
    \fn QMessageAccountOrdering::QMessageAccountOrdering(const QMessageAccountOrdering &other)
  
    Constructs a copy of \a other.
*/

/*!
    \fn QMessageAccountOrdering::~QMessageAccountOrdering()
    
    Destroys the ordering.
*/

/*!
    \internal
    \fn QMessageAccountOrdering::operator=(const QMessageAccountOrdering& other)
*/
    
/*!
    \fn QMessageAccountOrdering::isEmpty() const
  
    Returns true if the ordering remains empty after default construction; otherwise returns false.
*/

/*!
    \fn QMessageAccountOrdering::isSupported() const
  
    Returns true if the ordering is supported on the current platform; otherwise returns false.
*/

/*!
    \internal
    \fn QMessageAccountOrdering::operator==(const QMessageAccountOrdering& other) const
*/

/*!
    \internal
*/
bool QMessageAccountOrdering::operator!=(const QMessageAccountOrdering& other) const
{
    return !operator==(other);
}

/*!
    \fn QMessageAccountOrdering::byName(Qt::SortOrder order)
  
    Returns an ordering that sorts accounts by name, according to \a order.

    \sa QMessageAccount::name()
*/

QTM_END_NAMESPACE
