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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderordering.h"
#include "qmessagefolderordering_p.h"

bool QMessageFolderOrderingPrivate::lessThan(const QMessageFolderOrdering &ordering,
                                             const QMessageFolder &folder1, const QMessageFolder &folder2)
{
    QMessageFolderOrderingPrivate *d(ordering.d_ptr);
    
    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        Qt::SortOrder order((*it).second);
        ++it;

        const QMessageFolder *left;
        const QMessageFolder *right;
        if (order == Qt::AscendingOrder) {
            left = &folder1;
            right = &folder2; 
        } else {
            left = &folder2;
            right = &folder1; 
        }

        switch (field)
        {
        case Name: {
            if (left->displayName() < right->displayName()) {
                return true;
            } else if (right->displayName() < left->displayName()) {
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

QMessageFolderOrdering::QMessageFolderOrdering()
 : d_ptr(new QMessageFolderOrderingPrivate(this))
{
}

QMessageFolderOrdering::QMessageFolderOrdering(const QMessageFolderOrdering &other)
 : d_ptr(new QMessageFolderOrderingPrivate(this))
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
        d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
    }

    return *this;
}

bool QMessageFolderOrdering::isEmpty() const
{
    return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageFolderOrdering::isSupported() const
{
    return true;
}

QMessageFolderOrdering QMessageFolderOrdering::operator+(const QMessageFolderOrdering& other) const
{
    QMessageFolderOrdering sum;
    sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
    return sum;
}

QMessageFolderOrdering& QMessageFolderOrdering::operator+=(const QMessageFolderOrdering& other)
{
    if (&other == this) {
        return *this;
    }
    
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
    
    return *this;
}

bool QMessageFolderOrdering::operator==(const QMessageFolderOrdering& other) const
{
    return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

QMessageFolderOrdering QMessageFolderOrdering::byDisplayName(Qt::SortOrder order)
{
    QMessageFolderOrdering result;
    QPair<QMessageFolderOrderingPrivate::Field, Qt::SortOrder> fieldOrder(QMessageFolderOrderingPrivate::Name, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QMessageFolderOrdering QMessageFolderOrdering::byPath(Qt::SortOrder order)
{
    QMessageFolderOrdering result;
    QPair<QMessageFolderOrderingPrivate::Field, Qt::SortOrder> fieldOrder(QMessageFolderOrderingPrivate::Path, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}
#endif
