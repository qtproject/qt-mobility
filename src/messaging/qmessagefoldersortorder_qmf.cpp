/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
