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
#include "qmessagefilter.h"
#include "qmessagefilter_p.h"

QTM_BEGIN_NAMESPACE

QMessageFilter::QMessageFilter()
{
}

QMessageFilter::QMessageFilter(const QMessageFilter &other)
{
    Q_UNUSED(other)
}

QMessageFilter::~QMessageFilter()
{
}

QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

void QMessageFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    Q_UNUSED(matchFlags)
}

QMessageDataComparator::MatchFlags QMessageFilter::matchFlags() const
{
    return 0;
}

bool QMessageFilter::isEmpty() const
{
    return false; // stub
}

bool QMessageFilter::isSupported() const
{
    return true; // TODO: Implement
}

QMessageFilter QMessageFilter::operator~() const
{
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::operator&(const QMessageFilter& other) const
{
    Q_UNUSED(other)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::operator|(const QMessageFilter& other) const
{
    Q_UNUSED(other)
    return QMessageFilter(); // stub
}

const QMessageFilter& QMessageFilter::operator&=(const QMessageFilter& other)
{
    Q_UNUSED(other)
    return *this;
}

const QMessageFilter& QMessageFilter::operator|=(const QMessageFilter& other)
{
    Q_UNUSED(other)
    return *this;
}

bool QMessageFilter::operator==(const QMessageFilter& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

QMessageFilter QMessageFilter::byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byId(const QMessageFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byType(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(mask)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byPriority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byStandardFolder(QMessage::StandardFolder folder, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(folder)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(filter)
    Q_UNUSED(cmp)
    return QMessageFilter(); // stub
}

QTM_END_NAMESPACE
