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
#include "qmessageaccountsortkey.h"

#include <qmailaccountsortkey.h>

class QMessageAccountSortKeyPrivate
{
public:
    QMailAccountSortKey _key;

    static QMessageAccountSortKey convert(const QMailAccountSortKey &key);
    static QMailAccountSortKey convert(const QMessageAccountSortKey &key);
};

QMessageAccountSortKey QMessageAccountSortKeyPrivate::convert(const QMailAccountSortKey &key)
{
    QMessageAccountSortKey result;
    result.d_ptr->_key = key;
    return result;
}

QMailAccountSortKey QMessageAccountSortKeyPrivate::convert(const QMessageAccountSortKey &key)
{
    return key.d_ptr->_key;
}

namespace QmfHelpers {

QMessageAccountSortKey convert(const QMailAccountSortKey &key)
{
    return QMessageAccountSortKeyPrivate::convert(key);
}

QMailAccountSortKey convert(const QMessageAccountSortKey &key)
{
    return QMessageAccountSortKeyPrivate::convert(key);
}

}

QMessageAccountSortKey::QMessageAccountSortKey()
    : d_ptr(new QMessageAccountSortKeyPrivate)
{
}

QMessageAccountSortKey::QMessageAccountSortKey(const QMessageAccountSortKey &other)
    : d_ptr(new QMessageAccountSortKeyPrivate)
{
    this->operator=(other);
}

QMessageAccountSortKey::~QMessageAccountSortKey()
{
    delete d_ptr;
    d_ptr = 0;
}

bool QMessageAccountSortKey::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageAccountSortKey::operator==(const QMessageAccountSortKey& other) const
{
    return d_ptr->_key == other.d_ptr->_key;
}

const QMessageAccountSortKey& QMessageAccountSortKey::operator=(const QMessageAccountSortKey& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
    }

    return *this;
}

QMessageAccountSortKey QMessageAccountSortKey::name(Qt::SortOrder order)
{
    QMessageAccountSortKey key;
    key.d_ptr->_key = QMailAccountSortKey::name(order);
    return key;
}

