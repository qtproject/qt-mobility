/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qmessageid.h"


QTM_BEGIN_NAMESPACE

class QMessageIdPrivate
{
public:
    QString _id;
};

QMessageId::QMessageId()
 : d_ptr(0)
{
}

QMessageId::QMessageId(const QMessageId& other)
 : d_ptr(0)
{
        this->operator=(other);
}

QMessageId::QMessageId(const QString& id)
{
    d_ptr = new QMessageIdPrivate;
    d_ptr->_id = id;
}

QMessageId::~QMessageId()
{
        delete d_ptr;
}

QMessageId& QMessageId::operator=(const QMessageId& other)
{
    if (!other.d_ptr) {
        delete d_ptr;
        return *this;
    }

    if (!d_ptr)
        d_ptr = new QMessageIdPrivate;

    d_ptr->_id = other.d_ptr->_id;

    return *this;
}

bool QMessageId::operator==(const QMessageId& other) const
{
    if (!other.d_ptr && !d_ptr)
        return true;

    if (!other.d_ptr || !d_ptr)
        return false;

    return (d_ptr->_id == other.d_ptr->_id);
}

bool QMessageId::operator<(const QMessageId& other) const
{
    long left = 0;
    long right = 0;
    if (d_ptr) {
        left = d_ptr->_id.toLong();
    }
    if (other.d_ptr) {
        right = other.d_ptr->_id.toLong();
    }

    return (left < right);
}

QString QMessageId::toString() const
{
    return d_ptr ? d_ptr->_id : QString();
}

bool QMessageId::isValid() const
{
    return d_ptr ? true : false;
}

uint qHash(const QMessageId &id)
{
    //TODO: return qHash(id.toString());
}

QTM_END_NAMESPACE

