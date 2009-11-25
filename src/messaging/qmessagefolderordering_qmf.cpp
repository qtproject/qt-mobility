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
#include "qmessagefolderordering.h"

#include <qmailfoldersortkey.h>

QTM_BEGIN_NAMESPACE

class QMessageFolderOrderingPrivate
{
public:
    QMailFolderSortKey _key;

    //static QMessageFolderOrdering convert(const QMailFolderSortKey &key);
    static QMailFolderSortKey convert(const QMessageFolderOrdering &ordering);
};

/*
QMessageFolderOrdering QMessageFolderOrderingPrivate::convert(const QMailFolderSortKey &key)
{
    QMessageFolderOrdering result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailFolderSortKey QMessageFolderOrderingPrivate::convert(const QMessageFolderOrdering &ordering)
{
    return ordering.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageFolderOrdering convert(const QMailFolderSortKey &key)
{
    return QMessageFolderOrderingPrivate::convert(key);
}
*/

QMailFolderSortKey convert(const QMessageFolderOrdering &ordering)
{
    return QMessageFolderOrderingPrivate::convert(ordering);
}

}

QMessageFolderOrdering::QMessageFolderOrdering()
    : d_ptr(new QMessageFolderOrderingPrivate)
{
}

QMessageFolderOrdering::QMessageFolderOrdering(const QMessageFolderOrdering &other)
    : d_ptr(new QMessageFolderOrderingPrivate)
{
    this->operator=(other);
}

QMessageFolderOrdering::~QMessageFolderOrdering()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFolderOrdering& QMessageFolderOrdering::operator=(const QMessageFolderOrdering& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

bool QMessageFolderOrdering::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageFolderOrdering::isSupported() const
{
    return true; // stub
}

QMessageFolderOrdering QMessageFolderOrdering::operator+(const QMessageFolderOrdering& other) const
{
    QMessageFolderOrdering ordering;
    ordering.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return ordering;
}

QMessageFolderOrdering& QMessageFolderOrdering::operator+=(const QMessageFolderOrdering& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

bool QMessageFolderOrdering::operator==(const QMessageFolderOrdering& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

QMessageFolderOrdering QMessageFolderOrdering::byDisplayName(Qt::SortOrder order)
{
    QMessageFolderOrdering ordering;
    ordering.d_ptr->_key = QMailFolderSortKey::displayName(order);
    return ordering;
}

QMessageFolderOrdering QMessageFolderOrdering::byPath(Qt::SortOrder order)
{
    QMessageFolderOrdering ordering;
    ordering.d_ptr->_key = QMailFolderSortKey::path(order);
    return ordering;
}

QTM_END_NAMESPACE
