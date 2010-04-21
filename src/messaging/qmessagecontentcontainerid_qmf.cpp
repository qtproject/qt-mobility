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
#include "qmessagecontentcontainerid.h"
#include "qmfhelpers_p.h"
#include <qmailmessage.h>

QTM_BEGIN_NAMESPACE

class QMessageContentContainerIdPrivate
{
public:
    QMailMessagePart::Location _location;
    bool _body;

    QMessageContentContainerIdPrivate() : _body(false) {}
    
    static QMessageContentContainerId convert(const QMailMessagePart::Location &location);
    static QMailMessagePart::Location convert(const QMessageContentContainerId &id);

    static QMessageContentContainerId bodyId(const QMailMessageId &id);
};

QMessageContentContainerId QMessageContentContainerIdPrivate::convert(const QMailMessagePart::Location &location)
{
    QMessageContentContainerId result;
    result.d_ptr->_location = location;
    return result;
}

QMailMessagePart::Location QMessageContentContainerIdPrivate::convert(const QMessageContentContainerId &id)
{
    return id.d_ptr->_location;
}

QMessageContentContainerId QMessageContentContainerIdPrivate::bodyId(const QMailMessageId &id)
{
    QMessageContentContainerId result;
    result.d_ptr->_body = true;

    if (id.isValid()) {
        result.d_ptr->_location.setContainingMessageId(id);
    }

    return result;
}

namespace QmfHelpers {

QMessageContentContainerId convert(const QMailMessagePart::Location &location)
{
    return QMessageContentContainerIdPrivate::convert(location);
}

QMailMessagePart::Location convert(const QMessageContentContainerId &id)
{
    return QMessageContentContainerIdPrivate::convert(id);
}

QMessageContentContainerId bodyId(const QMailMessageId &id)
{
    return QMessageContentContainerIdPrivate::bodyId(id);
}

}

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
    QString input(QmfHelpers::stripIdentifierPrefix(id));

    if (input.startsWith("body:")) {
        d_ptr->_body = true;
        input = input.mid(5);
    }

    if (!input.isEmpty()) {
        QMailMessagePart::Location loc(input);
        if (loc.isValid(false) || loc.containingMessageId().isValid()) {
            d_ptr->_location = loc;
        }
    }
}

QMessageContentContainerId::~QMessageContentContainerId()
{
    delete d_ptr;
}

bool QMessageContentContainerId::operator==(const QMessageContentContainerId& other) const
{
    if (isValid()) {
        return (other.isValid() && (toString() == other.toString()));
    } else {
        return !other.isValid();
    }
}

QMessageContentContainerId& QMessageContentContainerId::operator=(const QMessageContentContainerId& other)
{
    if (&other != this) {
        d_ptr->_location = other.d_ptr->_location;
        d_ptr->_body = other.d_ptr->_body;
    }

    return *this;
}

QString QMessageContentContainerId::toString() const
{
    QString location(d_ptr->_location.toString(true));
    if (d_ptr->_body) {
        location.prepend("body:");
    }

    return QmfHelpers::prefixIdentifier(location);
}

bool QMessageContentContainerId::isValid() const
{
    // Either we have a valid part locator, or we indicate a message body
    return (d_ptr->_body || d_ptr->_location.isValid(false));
}

QTM_END_NAMESPACE
