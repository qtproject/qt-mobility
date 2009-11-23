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
#include "qmessageordering.h"
#include "qmessageordering_p.h"


QTM_BEGIN_NAMESPACE

QMessageOrdering::QMessageOrdering()
{
}

QMessageOrdering::QMessageOrdering(const QMessageOrdering &other)
{
    Q_UNUSED(other)
}

QMessageOrdering::~QMessageOrdering()
{
}

QMessageOrdering& QMessageOrdering::operator=(const QMessageOrdering& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

bool QMessageOrdering::isEmpty() const
{
    return false; // stub
}

bool QMessageOrdering::isSupported() const
{
    return true; // stub
}

QMessageOrdering QMessageOrdering::operator+(const QMessageOrdering& other) const
{
    Q_UNUSED(other)
    return QMessageOrdering(); // stub
}

QMessageOrdering& QMessageOrdering::operator+=(const QMessageOrdering& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

bool QMessageOrdering::operator==(const QMessageOrdering& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

QMessageOrdering QMessageOrdering::byType(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::bySender(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::byRecipients(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::bySubject(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::byTimeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::byReceptionTimeStamp(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::byStatus(QMessage::Status flag, Qt::SortOrder order)
{
    Q_UNUSED(flag)
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::byPriority(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QMessageOrdering QMessageOrdering::bySize(Qt::SortOrder order)
{
    Q_UNUSED(order)
    return QMessageOrdering(); // stub
}

QTM_END_NAMESPACE
