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


#ifndef QCONTACTGROUP_H
#define QCONTACTGROUP_H

#include "qtcontactsglobal.h"

#include "qcontactdetail.h"
#include "qcontact.h"

#include <QSharedData>

class QContactGroupData;
class QTCONTACTS_EXPORT QContactGroup
{
public:
    QContactGroup();
    QContactGroup(const QContactGroup &other);
    QContactGroup& operator=(const QContactGroup& other);
    virtual ~QContactGroup();

    bool operator==(const QContactGroup& other) const;
    bool operator!=(const QContactGroup& other) const {return !operator==(other);}

    enum Error {
        NoError = 0,
        OutOfMemoryError,
        ContactNotInGroupError,
        ContactAlreadyInGroupError,
        PermissionsError,
        UnspecifiedError
    };

    Error error() const;
    QUniqueId id() const;
    bool setId(const QUniqueId& id);

    bool isEmpty() const;

    QString name() const;
    bool setName(const QString& groupName);

    bool addMember(const QUniqueId& contactId);
    bool removeMember(const QUniqueId& contactId);
    bool setMembers(const QList<QUniqueId>& members);
    QList<QUniqueId> members() const;
    bool hasMember(const QUniqueId& contactId) const;

private:
    friend class QContactManager;
    friend class QContactManagerData;
    friend class QContactManagerEngine;
    QSharedDataPointer<QContactGroupData> d;
};

#endif
