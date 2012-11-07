/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageid.h"

#include <qhash.h>

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
        d_ptr = 0;
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
    return qHash(id.toString());
}

QTM_END_NAMESPACE

