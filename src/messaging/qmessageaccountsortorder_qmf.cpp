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
#include "qmessageaccountsortorder.h"

#include <qmailaccountsortkey.h>

QTM_BEGIN_NAMESPACE

class QMessageAccountSortOrderPrivate
{
public:
    QMailAccountSortKey _key;

    //static QMessageAccountSortOrder convert(const QMailAccountSortKey &key);
    static QMailAccountSortKey convert(const QMessageAccountSortOrder &sortOrder);
};

/*
QMessageAccountSortOrder QMessageAccountSortOrderPrivate::convert(const QMailAccountSortKey &key)
{
    QMessageAccountSortOrder result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailAccountSortKey QMessageAccountSortOrderPrivate::convert(const QMessageAccountSortOrder &sortOrder)
{
    return sortOrder.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageAccountSortOrder convert(const QMailAccountSortKey &key)
{
    return QMessageAccountSortOrderPrivate::convert(key);
}
*/

QMailAccountSortKey convert(const QMessageAccountSortOrder &sortOrder)
{
    return QMessageAccountSortOrderPrivate::convert(sortOrder);
}

}

QMessageAccountSortOrder::QMessageAccountSortOrder()
    : d_ptr(new QMessageAccountSortOrderPrivate)
{
}

QMessageAccountSortOrder::QMessageAccountSortOrder(const QMessageAccountSortOrder &other)
    : d_ptr(new QMessageAccountSortOrderPrivate)
{
    this->operator=(other);
}

QMessageAccountSortOrder::~QMessageAccountSortOrder()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageAccountSortOrder::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageAccountSortOrder::isSupported() const
{
    return true; // stub
}

bool QMessageAccountSortOrder::operator==(const QMessageAccountSortOrder& other) const
{
    return d_ptr->_key == other.d_ptr->_key;
}

QMessageAccountSortOrder& QMessageAccountSortOrder::operator=(const QMessageAccountSortOrder& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageAccountSortOrder QMessageAccountSortOrder::byName(Qt::SortOrder order)
{
    QMessageAccountSortOrder sortOrder;
    sortOrder.d_ptr->_key = QMailAccountSortKey::name(order);
    return sortOrder;
}

QTM_END_NAMESPACE
