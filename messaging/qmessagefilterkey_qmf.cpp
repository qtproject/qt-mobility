/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessagefilterkey.h"
#include "qmfhelpers_p.h"

#include <qmailaccountkey.h>
#include <qmailfolderkey.h>
#include <qmailmessagekey.h>

using namespace QmfHelpers;

class QMessageFilterKeyPrivate
{
public:
    QMessageFilterKeyPrivate() : _key(), _options(0) {}

    QMailMessageKey _key;
    QMessageDataComparator::Options _options;

    static QMessageFilterKey convert(const QMailMessageKey &key);
    static QMailMessageKey convert(const QMessageFilterKey &key);
};

QMessageFilterKey QMessageFilterKeyPrivate::convert(const QMailMessageKey &key)
{
    QMessageFilterKey result;
    result.d_ptr->_key = key;
    return result;
}

QMailMessageKey QMessageFilterKeyPrivate::convert(const QMessageFilterKey &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

QMessageFilterKey convert(const QMailMessageKey &key)
{
    return QMessageFilterKeyPrivate::convert(key);
}

QMailMessageKey convert(const QMessageFilterKey &key)
{
    return QMessageFilterKeyPrivate::convert(key);
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

void QMessageFilterKey::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options |= options;
}

QMessageDataComparator::Options QMessageFilterKey::options() const
{
    return d_ptr->_options;
}

QMessageFilterKey::QMessageFilterKey()
    : d_ptr(new QMessageFilterKeyPrivate)
{
}

QMessageFilterKey::QMessageFilterKey(const QMessageFilterKey &other)
    : d_ptr(new QMessageFilterKeyPrivate)
{
    this->operator=(other);
}

QMessageFilterKey::~QMessageFilterKey()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageFilterKey::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

QMessageFilterKey QMessageFilterKey::operator~() const
{
    QMessageFilterKey result;
    result.d_ptr->_key = ~d_ptr->_key;
    return result;
}

QMessageFilterKey QMessageFilterKey::operator&(const QMessageFilterKey& other) const
{
    QMessageFilterKey result;
    result.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return result;
}

QMessageFilterKey QMessageFilterKey::operator|(const QMessageFilterKey& other) const
{
    QMessageFilterKey result;
    result.d_ptr->_key = d_ptr->_key | other.d_ptr->_key;
    return result;
}

const QMessageFilterKey& QMessageFilterKey::operator&=(const QMessageFilterKey& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

const QMessageFilterKey& QMessageFilterKey::operator|=(const QMessageFilterKey& other)
{
    d_ptr->_key |= other.d_ptr->_key;
    return *this;
}

bool QMessageFilterKey::operator==(const QMessageFilterKey& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

const QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::id(convert(id), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::id(convert(ids), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::id(convert(key), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::messageType(convert(type), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::messageType(convert(type), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::sender(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::sender(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::recipients(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::recipients(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::subject(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::subject(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::timeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::timeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::receptionTimeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::receptionTimeStamp(adjusted(value), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::status(convert(value), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes));
    return result;
}

QMessageFilterKey QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
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

    QMessageFilterKey result;
    result.d_ptr->_key = (key.isEmpty() ? QMailMessageKey::nonMatchingKey() : key);
    return result;
}

QMessageFilterKey QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;

    switch (value)
    {
    case QMessage::High: 
        result.d_ptr->_key = QMailMessageKey::status(highPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes)); 
        break;

    case QMessage::Low: 
        result.d_ptr->_key = QMailMessageKey::status(lowPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Includes : QMailDataComparator::Excludes)); 
        break;

    case QMessage::Normal:
        result.d_ptr->_key = QMailMessageKey::status(lowPriorityMask() | highPriorityMask(), (cmp == QMessageDataComparator::Equal ? QMailDataComparator::Excludes : QMailDataComparator::Includes)); 
        break;
    }

    return result;
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::size(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::RelationComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::size(value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::customField(name, value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::customField(name, value, convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::parentAccountId(convert(id), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::parentAccountId(convert(key), convert(cmp));
    return result;
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::parentFolderId(convert(id), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::parentFolderId(convert(key), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::ancestorFolderIds(convert(id), convert(cmp));
    return result;
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFilterKey result;
    result.d_ptr->_key = QMailMessageKey::ancestorFolderIds(convert(key), convert(cmp));
    return result;
}
#endif

