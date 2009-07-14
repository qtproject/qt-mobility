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
#include "qmessagecontentcontainerid.h"

#include <qmailmessage.h>

class QMessageContentContainerIdPrivate
{
public:
    QMailMessagePart::Location _location;
};

QMessageContentContainerId::QMessageContentContainerId()
    : d_ptr(0)
{
}

QMessageContentContainerId::QMessageContentContainerId(const QMessageContentContainerId& other)
    : d_ptr(0)
{
    this->operator=(other);
}

QMessageContentContainerId::QMessageContentContainerId(const QString& id)
    : d_ptr(0)
{
    QMailMessagePart::Location loc(id);
    if (loc.isValid(true)) {
        d_ptr = new QMessageContentContainerIdPrivate;
        d_ptr->_location = loc;
    }
}

QMessageContentContainerId::~QMessageContentContainerId()
{
    delete d_ptr;
}

bool QMessageContentContainerId::operator==(const QMessageContentContainerId& other) const
{
    if (isValid()) {
        return (other.isValid() ? (d_ptr->_location.toString(true) == other.d_ptr->_location.toString(true)) : false);
    } else {
        return !other.isValid();
    }
}

QMessageContentContainerId& QMessageContentContainerId::operator=(const QMessageContentContainerId& other)
{
    if (&other != this) {
        if (other.isValid()) {
            if (!d_ptr) {
                d_ptr = new QMessageContentContainerIdPrivate;
            }
            d_ptr->_location = other.d_ptr->_location;
        } else {
            delete d_ptr;
        }
    }

    return *this;
}

QString QMessageContentContainerId::toString() const
{
    return (isValid() ? d_ptr->_location.toString(true) : QString());
}

bool QMessageContentContainerId::isValid() const
{
    return (d_ptr && d_ptr->_location.isValid(true));
}

