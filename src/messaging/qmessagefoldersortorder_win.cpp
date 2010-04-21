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
#include "qmessagefoldersortorder.h"
#include "qmessagefoldersortorder_p.h"

QTM_BEGIN_NAMESPACE

bool QMessageFolderSortOrderPrivate::lessthan(const QMessageFolderSortOrder &sortOrder, const QMessageFolder &l, const QMessageFolder &r)
{
    QMessageFolderSortOrderPrivate *d(sortOrder.d_ptr);

    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        Qt::SortOrder order((*it).second);
        ++it;

        const QMessageFolder *left;
        const QMessageFolder *right;
        if (order == Qt::AscendingOrder) {
            left = &l;
            right = &r; 
        } else {
            left = &r;
            right = &l; 
        }

        switch (field)
        {
        case Name: {
            if (left->name() < right->name()) {
                return true;
            } else if (right->name() < left->name()) {
                return false;
            } else {
                continue;
            }
        }
        case Path: {
            if (left->path() < right->path()) {
                return true;
            } else if (right->path() < left->path()) {
                return false;
            } else {
                continue;
            }
        }
        } // switch
    }
    return false; // equality
}

QMessageFolderSortOrder::QMessageFolderSortOrder()
    : d_ptr(new QMessageFolderSortOrderPrivate(this))
{
}

QMessageFolderSortOrder::QMessageFolderSortOrder(const QMessageFolderSortOrder &other)
    : d_ptr(new QMessageFolderSortOrderPrivate(this))
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
        d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
    }

    return *this;
}

bool QMessageFolderSortOrder::isEmpty() const
{
    return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageFolderSortOrder::isSupported() const
{
    return true;
}

QMessageFolderSortOrder QMessageFolderSortOrder::operator+(const QMessageFolderSortOrder& other) const
{
    QMessageFolderSortOrder sum;
    sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
    return sum;
}

QMessageFolderSortOrder& QMessageFolderSortOrder::operator+=(const QMessageFolderSortOrder& other)
{
    if (&other == this)
        return *this;
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
    return *this;
}

bool QMessageFolderSortOrder::operator==(const QMessageFolderSortOrder& other) const
{
    return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

QMessageFolderSortOrder QMessageFolderSortOrder::byName(Qt::SortOrder order)
{
    QMessageFolderSortOrder result;
    QPair<QMessageFolderSortOrderPrivate::Field, Qt::SortOrder> fieldOrder(QMessageFolderSortOrderPrivate::Name, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QMessageFolderSortOrder QMessageFolderSortOrder::byPath(Qt::SortOrder order)
{
    QMessageFolderSortOrder result;
    QPair<QMessageFolderSortOrderPrivate::Field, Qt::SortOrder> fieldOrder(QMessageFolderSortOrderPrivate::Path, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QTM_END_NAMESPACE
