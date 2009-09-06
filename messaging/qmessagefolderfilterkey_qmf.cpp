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
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefolderfilterkey.h"
#include "qmfhelpers_p.h"

#include <qmailaccountkey.h>
#include <qmailfolderkey.h>
#include <qmailmessagekey.h>

using namespace QmfHelpers;

class QMessageFolderFilterKeyPrivate
{
public:
    QMessageFolderFilterKeyPrivate() : _key(), _options(0) {}

    QMailFolderKey _key;
    QMessageDataComparator::Options _options;

    static QMessageFolderFilterKey convert(const QMailFolderKey &key);
    static QMailFolderKey convert(const QMessageFolderFilterKey &key);
};

QMessageFolderFilterKey QMessageFolderFilterKeyPrivate::convert(const QMailFolderKey &key)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = key;
    return result;
}

QMailFolderKey QMessageFolderFilterKeyPrivate::convert(const QMessageFolderFilterKey &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

QMessageFolderFilterKey convert(const QMailFolderKey &key)
{
    return QMessageFolderFilterKeyPrivate::convert(key);
}

QMailFolderKey convert(const QMessageFolderFilterKey &key)
{
    return QMessageFolderFilterKeyPrivate::convert(key);
}

}

QMessageFolderFilterKey::QMessageFolderFilterKey()
    : d_ptr(new QMessageFolderFilterKeyPrivate)
{
}

QMessageFolderFilterKey::QMessageFolderFilterKey(const QMessageFolderFilterKey &other)
    : d_ptr(new QMessageFolderFilterKeyPrivate)
{
    this->operator=(other);
}

QMessageFolderFilterKey::~QMessageFolderFilterKey()
{
    delete d_ptr;
    d_ptr = 0;
}

void QMessageFolderFilterKey::setOptions(QMessageDataComparator::Options options)
{
    d_ptr->_options |= options;
}

QMessageDataComparator::Options QMessageFolderFilterKey::options() const
{
    return d_ptr->_options;
}

bool QMessageFolderFilterKey::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageFolderFilterKey::isSupported() const
{
    return true; // TODO: Implement
}

QMessageFolderFilterKey QMessageFolderFilterKey::operator~() const
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = ~d_ptr->_key;
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::operator&(const QMessageFolderFilterKey& other) const
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::operator|(const QMessageFolderFilterKey& other) const
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = d_ptr->_key | other.d_ptr->_key;
    return result;
}

const QMessageFolderFilterKey& QMessageFolderFilterKey::operator&=(const QMessageFolderFilterKey& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

const QMessageFolderFilterKey& QMessageFolderFilterKey::operator|=(const QMessageFolderFilterKey& other)
{
    d_ptr->_key |= other.d_ptr->_key;
    return *this;
}

bool QMessageFolderFilterKey::operator==(const QMessageFolderFilterKey& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

const QMessageFolderFilterKey& QMessageFolderFilterKey::operator=(const QMessageFolderFilterKey& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::id(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::id(convert(ids), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::id(convert(key), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byDisplayName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::displayName(value, convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byDisplayName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::displayName(value, convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::path(value, convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::path(value, convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::parentAccountId(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::parentAccountId(convert(key), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::parentFolderId(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::parentFolderId(convert(key), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::ancestorFolderIds(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilterKey QMessageFolderFilterKey::byAncestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilterKey result;
    result.d_ptr->_key = QMailFolderKey::ancestorFolderIds(convert(key), convert(cmp));
    return result;
}
#endif
