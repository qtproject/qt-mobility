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
#include "qmessagefolderfilterkey_p.h"

QMessageFolderFilterKey::QMessageFolderFilterKey()
{
}

QMessageFolderFilterKey::QMessageFolderFilterKey(const QMessageFolderFilterKey &other)
{
    Q_UNUSED(other)
}

QMessageFolderFilterKey::~QMessageFolderFilterKey()
{
}

void QMessageFolderFilterKey::setOptions(QMessageDataComparator::Options options)
{
    Q_UNUSED(options)
}

QMessageDataComparator::Options QMessageFolderFilterKey::options() const
{
    return 0;
}

bool QMessageFolderFilterKey::isEmpty() const
{
    return false; // stub
}

bool QMessageFolderFilterKey::isSupported() const
{
    return true; // TODO: Implement
}

QMessageFolderFilterKey QMessageFolderFilterKey::operator~() const
{
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::operator&(const QMessageFolderFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::operator|(const QMessageFolderFilterKey& other) const
{
    Q_UNUSED(other)
    return QMessageFolderFilterKey(); // stub
}

const QMessageFolderFilterKey& QMessageFolderFilterKey::operator&=(const QMessageFolderFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

const QMessageFolderFilterKey& QMessageFolderFilterKey::operator|=(const QMessageFolderFilterKey& other)
{
    Q_UNUSED(other)
    return *this;
}

bool QMessageFolderFilterKey::operator==(const QMessageFolderFilterKey& other) const
{
    Q_UNUSED(other)
    return false; // stub
}

const QMessageFolderFilterKey& QMessageFolderFilterKey::operator=(const QMessageFolderFilterKey& other)
{
    Q_UNUSED(other)
    return *this; // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(ids)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byDisplayName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byDisplayName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(value)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentAccountId(const QMessageAccountFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byParentFolderId(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(id)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}

QMessageFolderFilterKey QMessageFolderFilterKey::byAncestorFolderIds(const QMessageFolderFilterKey &key, QMessageDataComparator::InclusionComparator cmp)
{
    Q_UNUSED(key)
    Q_UNUSED(cmp)
    return QMessageFolderFilterKey(); // stub
}
#endif
