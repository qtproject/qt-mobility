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
#include "qmessagefoldersortorder.h"

#include <qmailfoldersortkey.h>

QTM_BEGIN_NAMESPACE

class QMessageFolderSortOrderPrivate
{
public:
    QMailFolderSortKey _key;

    //static QMessageFolderSortOrder convert(const QMailFolderSortKey &key);
    static QMailFolderSortKey convert(const QMessageFolderSortOrder &sortOrder);
};

/*
QMessageFolderSortOrder QMessageFolderSortOrderPrivate::convert(const QMailFolderSortKey &key)
{
    QMessageFolderSortOrder result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailFolderSortKey QMessageFolderSortOrderPrivate::convert(const QMessageFolderSortOrder &sortOrder)
{
    return sortOrder.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageFolderSortOrder convert(const QMailFolderSortKey &key)
{
    return QMessageFolderSortOrderPrivate::convert(key);
}
*/

QMailFolderSortKey convert(const QMessageFolderSortOrder &sortOrder)
{
    return QMessageFolderSortOrderPrivate::convert(sortOrder);
}

}

QMessageFolderSortOrder::QMessageFolderSortOrder()
    : d_ptr(new QMessageFolderSortOrderPrivate)
{
}

QMessageFolderSortOrder::QMessageFolderSortOrder(const QMessageFolderSortOrder &other)
    : d_ptr(new QMessageFolderSortOrderPrivate)
{
    this->operator=(other);
}

QMessageFolderSortOrder::~QMessageFolderSortOrder()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFolderSortOrder& QMessageFolderSortOrder::operator=(const QMessageFolderSortOrder& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

bool QMessageFolderSortOrder::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageFolderSortOrder::isSupported() const
{
    return true; // stub
}

QMessageFolderSortOrder QMessageFolderSortOrder::operator+(const QMessageFolderSortOrder& other) const
{
    QMessageFolderSortOrder sortOrder;
    sortOrder.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return sortOrder;
}

QMessageFolderSortOrder& QMessageFolderSortOrder::operator+=(const QMessageFolderSortOrder& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

bool QMessageFolderSortOrder::operator==(const QMessageFolderSortOrder& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

QMessageFolderSortOrder QMessageFolderSortOrder::byName(Qt::SortOrder order)
{
    QMessageFolderSortOrder sortOrder;
    sortOrder.d_ptr->_key = QMailFolderSortKey::displayName(order);
    return sortOrder;
}

QMessageFolderSortOrder QMessageFolderSortOrder::byPath(Qt::SortOrder order)
{
    QMessageFolderSortOrder sortOrder;
    sortOrder.d_ptr->_key = QMailFolderSortKey::path(order);
    return sortOrder;
}

QTM_END_NAMESPACE
