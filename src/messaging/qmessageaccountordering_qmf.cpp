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

#include <qmailaccountsortkey.h>

QTM_BEGIN_NAMESPACE

class QMessageAccountOrderingPrivate
{
public:
    QMailAccountSortKey _key;

    //static QMessageAccountOrdering convert(const QMailAccountSortKey &key);
    static QMailAccountSortKey convert(const QMessageAccountOrdering &ordering);
};

/*
QMessageAccountOrdering QMessageAccountOrderingPrivate::convert(const QMailAccountSortKey &key)
{
    QMessageAccountOrdering result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailAccountSortKey QMessageAccountOrderingPrivate::convert(const QMessageAccountOrdering &ordering)
{
    return ordering.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageAccountOrdering convert(const QMailAccountSortKey &key)
{
    return QMessageAccountOrderingPrivate::convert(key);
}
*/

QMailAccountSortKey convert(const QMessageAccountOrdering &ordering)
{
    return QMessageAccountOrderingPrivate::convert(ordering);
}

}

QMessageAccountOrdering::QMessageAccountOrdering()
    : d_ptr(new QMessageAccountOrderingPrivate)
{
}

QMessageAccountOrdering::QMessageAccountOrdering(const QMessageAccountOrdering &other)
    : d_ptr(new QMessageAccountOrderingPrivate)
{
    this->operator=(other);
}

QMessageAccountOrdering::~QMessageAccountOrdering()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageAccountOrdering::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageAccountOrdering::isSupported() const
{
    return true; // stub
}

bool QMessageAccountOrdering::operator==(const QMessageAccountOrdering& other) const
{
    return d_ptr->_key == other.d_ptr->_key;
}

QMessageAccountOrdering& QMessageAccountOrdering::operator=(const QMessageAccountOrdering& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageAccountOrdering QMessageAccountOrdering::byName(Qt::SortOrder order)
{
    QMessageAccountOrdering ordering;
    ordering.d_ptr->_key = QMailAccountSortKey::name(order);
    return ordering;
}

QTM_END_NAMESPACE
