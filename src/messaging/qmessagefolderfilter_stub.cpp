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
#include "qmessagefolderfilter.h"
#include "qmessagefolderfilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageFolderFilter::QMessageFolderFilter()
{
}

QMessageFolderFilter::QMessageFolderFilter(const QMessageFolderFilter &other)
{
    Q_UNUSED(other)
}

QMessageFolderFilter::~QMessageFolderFilter()
{
}

QMessageFolderFilter& QMessageFolderFilter::operator=(const QMessageFolderFilter& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

void QMessageFolderFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    Q_UNUSED(matchFlags)
}

QMessageDataComparator::MatchFlags QMessageFolderFilter::matchFlags() const
{
    return 0;
}

bool QMessageFolderFilter::isEmpty() const
{
    return false; // stub
}

bool QMessageFolderFilter::isSupported() const
{
    return true; // TODO: Implement
}

QMessageFolderFilter QMessageFolderFilter::operator~() const
{
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::operator&(const QMessageFolderFilter& other) const
{
    Q_UNUSED(other)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::operator|(const QMessageFolderFilter& other) const
{
    Q_UNUSED(other)
    return QMessageFolderFilter(); // stub
}

const QMessageFolderFilter& QMessageFolderFilter::operator&=(const QMessageFolderFilter& other)
{
    Q_UNUSED(other)
    return *this;
}

const QMessageFolderFilter& QMessageFolderFilter::operator|=(const QMessageFolderFilter& other)
{
    Q_UNUSED(other)
    return *this;
}

bool QMessageFolderFilter::operator==(const QMessageFolderFilter& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFolderFilter(); // stub
}

QTM_END_NAMESPACE
