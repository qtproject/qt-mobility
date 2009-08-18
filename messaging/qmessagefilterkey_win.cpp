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
#include "qmessagefilterkey_p.h"

void QMessageFilterKey::setOptions(QMessageDataComparator::Options options)
{
    Q_UNUSED(options)
}

QMessageDataComparator::Options QMessageFilterKey::options() const
{
    return 0;
}

QMessageFilterKey::QMessageFilterKey()
{
}

QMessageFilterKey::QMessageFilterKey(const QMessageFilterKey &other)
{
    Q_UNUSED(other)
}

QMessageFilterKey::~QMessageFilterKey()
{
}

bool QMessageFilterKey::isEmpty() const
{
    return false; // stub
}

QMessageFilterKey QMessageFilterKey::operator~() const
{
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::operator&(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::operator|(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFilterKey(); // stub
}

const QMessageFilterKey& QMessageFilterKey::operator&=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

const QMessageFilterKey& QMessageFilterKey::operator|=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

bool QMessageFilterKey::operator==(const QMessageFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

const QMessageFilterKey& QMessageFilterKey::operator=(const QMessageFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessageFilterKey QMessageFilterKey::id(const QMessageId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::id(const QMessageIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::id(const QMessageFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::type(QMessage::Type type, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::type(QMessage::TypeFlags type, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(type)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::sender(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::recipients(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::subject(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::timeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::receptionTimeStamp(const QDateTime &value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::status(QMessage::Status value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::status(QMessage::StatusFlags mask, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(mask)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::priority(QMessage::Priority value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::size(int value, QMessageDataComparator::RelationComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::customField(const QString &name, const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(name)
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::parentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

#ifdef QMESSAGING_OPTIONAL_FOLDER
QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::parentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}

QMessageFilterKey QMessageFilterKey::ancestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFilterKey(); // stub
}
#endif
