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
#include "qmessagecontentcontainerid.h"
#include <symbianhelpers_p.h>


QTM_BEGIN_NAMESPACE

using namespace SymbianHelpers;

class QMessageContentContainerIdPrivate
{
public:
    enum {
		Invalid = -1,
		Body = 0
	};

    int _number;

    QMessageContentContainerIdPrivate() : _number(Invalid) {}
};

QMessageContentContainerId::QMessageContentContainerId()
: d_ptr(new QMessageContentContainerIdPrivate)
{
}

QMessageContentContainerId::QMessageContentContainerId(const QMessageContentContainerId& other)
: d_ptr(new QMessageContentContainerIdPrivate)
{
	this->operator=(other);
}

QMessageContentContainerId::QMessageContentContainerId(const QString& id)
: d_ptr(new QMessageContentContainerIdPrivate)
{
	if (!id.isEmpty()) {
		d_ptr->_number = stripIdPrefix(id).toUInt();
	}
}

QMessageContentContainerId::~QMessageContentContainerId()
{
	delete d_ptr;
}

bool QMessageContentContainerId::operator==(const QMessageContentContainerId& other) const
{
	return (d_ptr->_number == other.d_ptr->_number);
}

QMessageContentContainerId& QMessageContentContainerId::operator=(const QMessageContentContainerId& other)
{
	if (&other != this) {
		d_ptr->_number = other.d_ptr->_number;
	}

    return *this;
}

QString QMessageContentContainerId::toString() const
{
	return addIdPrefix(QString::number(d_ptr->_number));
}

bool QMessageContentContainerId::isValid() const
{
	return (d_ptr->_number != QMessageContentContainerIdPrivate::Invalid);
}

QTM_END_NAMESPACE
