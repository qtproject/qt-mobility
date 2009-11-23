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
#include "qmessageaccountordering_p.h"

QTM_BEGIN_NAMESPACE

QMessageAccountOrderingPrivate::QMessageAccountOrderingPrivate(QMessageAccountOrdering *ordering)
    :q_ptr(ordering),
    _empty(true),
    _order(Qt::AscendingOrder)
{
}

QMessageAccountOrdering::QMessageAccountOrdering()
    :d_ptr(new QMessageAccountOrderingPrivate(this))
{
}

QMessageAccountOrdering::~QMessageAccountOrdering()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageAccountOrdering::QMessageAccountOrdering(const QMessageAccountOrdering &other)
    :d_ptr(new QMessageAccountOrderingPrivate(this))
{
    this->operator=(other);
}

bool QMessageAccountOrdering::isEmpty() const
{
    return d_ptr->_empty;
}

bool QMessageAccountOrdering::isSupported() const
{
    return true;
}

bool QMessageAccountOrdering::operator==(const QMessageAccountOrdering& other) const
{
    return ((d_ptr->_empty == other.d_ptr->_empty) &&
            (d_ptr->_order == other.d_ptr->_order) &&
            (d_ptr->_empty == other.d_ptr->_empty));
}

QMessageAccountOrdering& QMessageAccountOrdering::operator=(const QMessageAccountOrdering& other)
{
    if (&other != this) {
        *d_ptr = *other.d_ptr;
    }

    return *this;
}

QMessageAccountOrdering QMessageAccountOrdering::byName(Qt::SortOrder order)
{
    QMessageAccountOrdering result;
    result.d_ptr->_empty = false;
    result.d_ptr->_order = order;
    return result;
}

QTM_END_NAMESPACE
