/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmfhelpers_p.h"

#include <qmailaccountkey.h>
#include <qmailfolderkey.h>
#include <qmailmessagekey.h>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

QTM_BEGIN_NAMESPACE

class QMessageFilterPrivate
{
public:
    QMessageFilterPrivate() : _key(), _matchFlags(0) {}

    QMailMessageKey _key;
    QMessageDataComparator::MatchFlags _matchFlags;

    //static QMessageFilter convert(const QMailMessageKey &key);
    static QMailMessageKey convert(const QMessageFilter &key);
};

/*
QMessageFilter QMessageFilterPrivate::convert(const QMailMessageKey &key)
{
    QMessageFilter result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailMessageKey QMessageFilterPrivate::convert(const QMessageFilter &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageFilter convert(const QMailMessageKey &key)
{
    return QMessageFilterPrivate::convert(key);
}
*/

QMailMessageKey convert(const QMessageFilter &key)
{
    return QMessageFilterPrivate::convert(key);
}

}

namespace {

// QMF stores dates in localtime
QDateTime adjusted(const QDateTime &dt)
{
    if (dt.timeSpec() == Qt::LocalTime ) {
        return dt;
    }

    return dt.toLocalTime();
}

}

QMessageFilter::QMessageFilter()
    : d_ptr(new QMessageFilterPrivate)
{
}

QMessageFilter::QMessageFilter(const QMessageFilter &other)
    : d_ptr(new QMessageFilterPrivate)
{
    this->operator=(other);
}

QMessageFilter::~QMessageFilter()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFilter& QMessageFilter::operator=(const QMessageFilter& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
        d_ptr->_matchFlags = other.d_ptr->_matchFlags;
    }

    return *this;
}

void QMessageFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
}

QMessageDataComparator::MatchFlags QMessageFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageFilter::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageFilter::isSupported() const
{
    return !d_ptr->_matchFlags;
}

QMessageFilter QMessageFilter::operator~() const
{
    QMessageFilter result;
    result.d_ptr->_key = ~d_ptr->_key;
    return result;
}

QMessageFilter QMessageFilter::operator&(const QMessageFilter& other) const
{
    QMessageFilter result;
    result.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    result.d_ptr->_matchFlags = d_ptr->_matchFlags | other.d_ptr->_matchFlags; // matchFlags not supported
    return result;
}

QMessageFilter QMessageFilter::operator|(const QMessageFilter& other) const
{
    QMessageFilter result;
    result.d_ptr->_key = d_ptr->_key | other.d_ptr->_key;
    result.d_ptr->_matchFlags = d_ptr->_matchFlags | other.d_ptr->_matchFlags; // matchFlags not supported
    return result;
}

const QMessageFilter& QMessageFilter::operator&=(const QMessageFilter& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    d_ptr->_matchFlags |= other.d_ptr->_matchFlags; // matchFlags not supported
    return *this;
}

const QMessageFilter& QMessageFilter::operator|=(const QMessageFilter& other)
{
    d_ptr->_key |= other.d_ptr->_key;
    d_ptr->_matchFlags |= other.d_ptr->_matchFlags; // matchFlags not supported
    return *this;
}

bool QMessageFilter::operator==(const QMessageFilter& other) const
{
    return ((d_ptr->_key == other.d_ptr->_key)
            && (d_ptr->_matchFlags == other.d_ptr->_matchFlags));
}

QMessageFilter QMessageFilter::byId(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::id(convert(id), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byId(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::id(convert(ids), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byId(const QMessageFilter &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::id(convert(key), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byType(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::messageType(convert(type), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byType(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::messageType(convert(type), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::sender(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::bySender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::sender(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byRecipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::recipients(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::subject(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::bySubject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::subject(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::timeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::timeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::receptionTimeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byReceptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::receptionTimeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byStatus(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::status(convert(value), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes));
    return result;
}

QMessageFilter QMessageFilter::byStatus(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    QMailMessageKey key;

    // This should be interpreted as specifying a match or otherwise on each of the flags set in the mask
    if (mask & QMessage::Read) {
        key &= QMailMessageKey::status(convert(QMessage::Read), convert(cmp));
    }
    if (mask & QMessage::HasAttachments) {
        key &= QMailMessageKey::status(convert(QMessage::HasAttachments), convert(cmp));
    }
    if (mask & QMessage::Incoming) {
        key &= QMailMessageKey::status(convert(QMessage::Incoming), convert(cmp));
    }
    if (mask & QMessage::Removed) {
        key &= QMailMessageKey::status(convert(QMessage::Removed), convert(cmp));
    }

    QMessageFilter result;
    result.d_ptr->_key = (key.isEmpty() ? QMailMessageKey::nonMatchingKey() : key);
    return result;
}

QMessageFilter QMessageFilter::byPriority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;

    switch (value)
    {
    case QMessage::HighPriority: 
        result.d_ptr->_key = QMailMessageKey::status(highPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes)); 
        break;

    case QMessage::LowPriority: 
        result.d_ptr->_key = QMailMessageKey::status(lowPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes)); 
        break;

    case QMessage::NormalPriority:
        result.d_ptr->_key = QMailMessageKey::status(lowPriorityMask() | highPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Excludes : QMailDataComparator::Includes)); 
        break;
    }

    return result;
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::size(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::bySize(int value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::size(value, convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::parentAccountId(convert(id), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::parentAccountId(convert(filter), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byStandardFolder(QMessage::StandardFolder folder, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    QMessageManager mgr; // Ensure standard folders are initialized
    Q_UNUSED(mgr)

    QMailDataComparator::InclusionComparator comparator(cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes); 

    switch (folder)
    {
    case QMessage::InboxFolder:
        result.d_ptr->_key = QMailMessageKey::status(QMailMessage::Incoming, comparator);

        // Exclude any trash messages
        result.d_ptr->_key &= ~QMailMessageKey::status(QMailMessage::Trash, comparator);
        break;

    case QMessage::OutboxFolder:
        result.d_ptr->_key = QMailMessageKey::status(QMailMessage::Outbox, comparator);
        break;

    case QMessage::DraftsFolder:
        result.d_ptr->_key = QMailMessageKey::status(QMailMessage::Draft, comparator);
        break;

    case QMessage::SentFolder:
        result.d_ptr->_key = QMailMessageKey::status(QMailMessage::Sent, comparator);
        break;

    case QMessage::TrashFolder:
        result.d_ptr->_key = QMailMessageKey::status(QMailMessage::Trash, comparator);
        break;
    }

    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::parentFolderId(convert(id), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::parentFolderId(convert(filter), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::ancestorFolderIds(convert(id), convert(cmp));
    return result;
}

QMessageFilter QMessageFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilter result;
    result.d_ptr->_key = QMailMessageKey::ancestorFolderIds(convert(filter), convert(cmp));
    return result;
}

QTM_END_NAMESPACE
