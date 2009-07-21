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
** contact Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifdef QMESSAGING_OPTIONAL_FOLDER
#include "qmessagefoldersortkey.h"

#include <qmailfoldersortkey.h>

class QMessageFolderSortKeyPrivate
{
public:
    QMailFolderSortKey _key;

    static QMessageFolderSortKey convert(const QMailFolderSortKey &key);
    static QMailFolderSortKey convert(const QMessageFolderSortKey &key);
};

QMessageFolderSortKey QMessageFolderSortKeyPrivate::convert(const QMailFolderSortKey &key)
{
    QMessageFolderSortKey result;
    result.d_ptr->_key = key;
    return result;
}

QMailFolderSortKey QMessageFolderSortKeyPrivate::convert(const QMessageFolderSortKey &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

QMessageFolderSortKey convert(const QMailFolderSortKey &key)
{
    return QMessageFolderSortKeyPrivate::convert(key);
}

QMailFolderSortKey convert(const QMessageFolderSortKey &key)
{
    return QMessageFolderSortKeyPrivate::convert(key);
}

}

QMessageFolderSortKey::QMessageFolderSortKey()
    : d_ptr(new QMessageFolderSortKeyPrivate)
{
}

QMessageFolderSortKey::QMessageFolderSortKey(const QMessageFolderSortKey &other)
    : d_ptr(new QMessageFolderSortKeyPrivate)
{
    this->operator=(other);
}

bool QMessageFolderSortKey::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

QMessageFolderSortKey QMessageFolderSortKey::operator+(const QMessageFolderSortKey& other) const
{
    QMessageFolderSortKey key;
    key.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    return key;
}

QMessageFolderSortKey& QMessageFolderSortKey::operator+=(const QMessageFolderSortKey& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    return *this;
}

bool QMessageFolderSortKey::operator==(const QMessageFolderSortKey& other) const
{
    return (d_ptr->_key == other.d_ptr->_key);
}

const QMessageFolderSortKey& QMessageFolderSortKey::operator=(const QMessageFolderSortKey& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageFolderSortKey QMessageFolderSortKey::displayName(Qt::SortOrder order)
{
    QMessageFolderSortKey key;
    key.d_ptr->_key = QMailFolderSortKey::displayName(order);
    return key;
}

QMessageFolderSortKey QMessageFolderSortKey::path(Qt::SortOrder order)
{
    QMessageFolderSortKey key;
    key.d_ptr->_key = QMailFolderSortKey::path(order);
    return key;
}
#endif
