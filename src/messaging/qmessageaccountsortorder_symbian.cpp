/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qmessageaccountsortorder.h"
#include "qmessageaccountsortorder_p.h"

QTM_BEGIN_NAMESPACE

QMessageAccountSortOrderPrivate::QMessageAccountSortOrderPrivate(QMessageAccountSortOrder *sortOrder)
 : q_ptr(sortOrder)
{
}

bool QMessageAccountSortOrderPrivate::lessThan(const QMessageAccountSortOrder &sortOrder,
                                              const QMessageAccount &account1, const QMessageAccount &account2)
{
    QMessageAccountSortOrderPrivate *d(sortOrder.d_ptr);
    if (d->_order == Qt::AscendingOrder) {
        return (account1.name().compare(account2.name(), Qt::CaseInsensitive) < 0);
    }
    return (account1.name().compare(account2.name(), Qt::CaseInsensitive) > 0);
}

QMessageAccountSortOrder::QMessageAccountSortOrder()
 : d_ptr(0)
{
}

QMessageAccountSortOrder::QMessageAccountSortOrder(const QMessageAccountSortOrder &other)
 : d_ptr(new QMessageAccountSortOrderPrivate(this))
{
	this->operator=(other);
}

QMessageAccountSortOrder::~QMessageAccountSortOrder()
{
	delete d_ptr;
	d_ptr = 0;
}

bool QMessageAccountSortOrder::isEmpty() const
{
	return (d_ptr == 0);
}

bool QMessageAccountSortOrder::isSupported() const
{
    return true;
}

bool QMessageAccountSortOrder::operator==(const QMessageAccountSortOrder& other) const
{
    if (!d_ptr && !other.d_ptr) {
        return true;
    }
    if (!d_ptr || !other.d_ptr) {
        return false;
    }
    
	return (d_ptr->_order == other.d_ptr->_order);
}

QMessageAccountSortOrder& QMessageAccountSortOrder::operator=(const QMessageAccountSortOrder& other)
{
	if (&other != this) {
	    if (!d_ptr) {
			d_ptr = new QMessageAccountSortOrderPrivate(this);	    
	    }
		d_ptr->_order = other.d_ptr->_order;
	}
	
	return *this;
}

QMessageAccountSortOrder QMessageAccountSortOrder::byName(Qt::SortOrder order)
{
	QMessageAccountSortOrder sortOrder;
	sortOrder.d_ptr = new QMessageAccountSortOrderPrivate(&sortOrder);
	sortOrder.d_ptr->_order = order;
	return sortOrder;
}

QTM_END_NAMESPACE
