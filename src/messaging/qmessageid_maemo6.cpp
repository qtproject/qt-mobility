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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
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
    if (this == &other)
	return *this;

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
    if (!d_ptr)
	return false;

    if (!other.d_ptr)
	return true;

    bool isPrefixedQMF = d_ptr->_id.startsWith("QMF_");
    bool isPrefixedQMF2 = other.d_ptr->_id.startsWith("QMF_");

    bool isPrefixedSMS = d_ptr->_id.startsWith("SMS_");
    bool isPrefixedSMS2 = other.d_ptr->_id.startsWith("SMS_");

    // E-mail
    if (isPrefixedQMF && isPrefixedQMF2)  { // E-mail
	long left = d_ptr->_id.mid(4).toLong();
	long right = other.d_ptr->_id.mid(4).toLong();
	return left < right;
    } else if (isPrefixedSMS && isPrefixedSMS2)  { // SMS
        long left = d_ptr->_id.mid(4).toLong();
        long right = other.d_ptr->_id.mid(4).toLong();
        return left < right;
    } else if (!isPrefixedSMS && !isPrefixedSMS2 && !isPrefixedQMF && !isPrefixedQMF2) { // smth else
	long left = d_ptr->_id.toLong();
	long right = other.d_ptr->_id.toLong();
	return left < right;
    }

    return !isPrefixedQMF;
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

