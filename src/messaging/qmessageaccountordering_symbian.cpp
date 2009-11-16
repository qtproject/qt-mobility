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
#include "qmessageaccountordering.h"
#include "qmessageaccountordering_p.h"

QMessageAccountOrderingPrivate::QMessageAccountOrderingPrivate(QMessageAccountOrdering *ordering)
 : q_ptr(ordering)
{
}

bool QMessageAccountOrderingPrivate::lessThan(const QMessageAccountOrdering &ordering,
                                              const QMessageAccount &account1, const QMessageAccount &account2)
{
    QMessageAccountOrderingPrivate *d(ordering.d_ptr);
    if (d->_order == Qt::AscendingOrder) {
        return (account1.name().compare(account2.name(), Qt::CaseInsensitive) < 0);
    }
    return (account1.name().compare(account2.name(), Qt::CaseInsensitive) > 0);
}

QMessageAccountOrdering::QMessageAccountOrdering()
 : d_ptr(0)
{
}

QMessageAccountOrdering::QMessageAccountOrdering(const QMessageAccountOrdering &other)
 : d_ptr(new QMessageAccountOrderingPrivate(this))
{
	this->operator=(other);
}

QMessageAccountOrdering::~QMessageAccountOrdering()
{
	delete d_ptr;
	d_ptr = 0;
}

bool QMessageAccountOrdering::isEmpty() const
{
	return (d_ptr == 0);
}

bool QMessageAccountOrdering::isSupported() const
{
    return true;
}

bool QMessageAccountOrdering::operator==(const QMessageAccountOrdering& other) const
{
    if (!d_ptr && !other.d_ptr) {
        return true;
    }
    if (!d_ptr || !other.d_ptr) {
        return false;
    }
    
	return (d_ptr->_order == other.d_ptr->_order);
}

QMessageAccountOrdering& QMessageAccountOrdering::operator=(const QMessageAccountOrdering& other)
{
	if (&other != this) {
	    if (!d_ptr) {
			d_ptr = new QMessageAccountOrderingPrivate(this);	    
	    }
		d_ptr->_order = other.d_ptr->_order;
	}
	
	return *this;
}

QMessageAccountOrdering QMessageAccountOrdering::byName(Qt::SortOrder order)
{
	QMessageAccountOrdering ordering;
	ordering.d_ptr = new QMessageAccountOrderingPrivate(&ordering);
	ordering.d_ptr->_order = order;
	return ordering;
}
