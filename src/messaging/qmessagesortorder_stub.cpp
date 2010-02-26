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
#include "qmessagesortorder.h"
#include "qmessagesortorder_p.h"


QTM_BEGIN_NAMESPACE

QMessageSortOrder::QMessageSortOrder()
{
}

QMessageSortOrder::QMessageSortOrder(const QMessageSortOrder &other)
{
    Q_UNUSED(other)
}

QMessageSortOrder::~QMessageSortOrder()
{
}

QMessageSortOrder& QMessageSortOrder::operator=(const QMessageSortOrder& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

bool QMessageSortOrder::isEmpty() const
{
    return false; // stub
}

bool QMessageSortOrder::isSupported() const
{
    return true; // stub
}

QMessageSortOrder QMessageSortOrder::operator+(const QMessageSortOrder& other) const
{
    Q_UNUSED(other)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder& QMessageSortOrder::operator+=(const QMessageSortOrder& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

bool QMessageSortOrder::operator==(const QMessageSortOrder& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

QMessageSortOrder QMessageSortOrder::byType(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::bySender(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::byRecipients(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::bySubject(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::byTimeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::byReceptionTimeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
    Q_UNUSED(flag)
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::byPriority(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QMessageSortOrder QMessageSortOrder::bySize(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageSortOrder(); // stub
}

QTM_END_NAMESPACE
