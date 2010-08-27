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
#include "qmessageaccountfilter.h"
#include "qmessageaccountfilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageAccountFilter::QMessageAccountFilter()
{
}

QMessageAccountFilter::QMessageAccountFilter(const QMessageAccountFilter &other)
{
    Q_UNUSED(other)
}

QMessageAccountFilter::~QMessageAccountFilter()
{
}

QMessageAccountFilter& QMessageAccountFilter::operator=(const QMessageAccountFilter& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

void QMessageAccountFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    Q_UNUSED(matchFlags)
}

QMessageDataComparator::MatchFlags QMessageAccountFilter::matchFlags() const
{
    return 0;
}

bool QMessageAccountFilter::isEmpty() const
{
    return false; // stub
}

bool QMessageAccountFilter::isSupported() const
{
    return true; // TODO: Implement
}

QMessageAccountFilter QMessageAccountFilter::operator~() const
{
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::operator&(const QMessageAccountFilter& other) const
{
    Q_UNUSED(other)
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::operator|(const QMessageAccountFilter& other) const
{
    Q_UNUSED(other)
    return QMessageAccountFilter(); // stub
}

const QMessageAccountFilter& QMessageAccountFilter::operator&=(const QMessageAccountFilter& other)
{
    Q_UNUSED(other)
    return *this;
}

const QMessageAccountFilter& QMessageAccountFilter::operator|=(const QMessageAccountFilter& other)
{
    Q_UNUSED(other)
    return *this;
}

bool QMessageAccountFilter::operator==(const QMessageAccountFilter& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::byId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilter(); // stub
}

QMessageAccountFilter QMessageAccountFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageAccountFilter(); // stub
}

QTM_END_NAMESPACE
