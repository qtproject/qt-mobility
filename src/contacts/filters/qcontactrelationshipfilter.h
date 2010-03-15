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

#ifndef QCONTACTRELATIONSHIPFILTER_H
#define QCONTACTRELATIONSHIPFILTER_H

#include "qtcontactsglobal.h"
#include "qcontactfilter.h"
#include "qcontactid.h"
#include "qcontactrelationship.h"

#include <QSharedDataPointer>
#include <QStringList>
#include <QList>
#include <QString>

QTM_BEGIN_NAMESPACE

class QContactRelationshipFilterPrivate;
class Q_CONTACTS_EXPORT QContactRelationshipFilter : public QContactFilter
{
public:
    QContactRelationshipFilter();
    QContactRelationshipFilter(const QContactFilter& other);

    // this enum is DEPRECATED - use QContactRelationship::Role instead!
    // enum Q_DECL_VARIABLE_DEPRECATED Role {
    enum Q_DECL_VARIABLE_DEPRECATED Role {
        First = 0,
        Second,
        Either
    };

    void setRelationshipType(const QString& relationshipType);
    void setRelatedContactId(const QContactId& relatedContactId);
    void setRelatedContactRole(QContactRelationship::Role relatedContactRole);
    void Q_DECL_DEPRECATED setRelatedContactRole(QContactRelationshipFilter::Role relatedContactRole);

    QString relationshipType() const;
    QContactId relatedContactId() const;
    QContactRelationship::Role relatedContactRole() const;
    //QContactRelationshipFilter::Role Q_DECL_DEPRECATED relatedContactRole() const; // cannot deprecated as signature is the same.

    // deprecated and will be removed after transition period has elapsed.  replaced by setRelatedContactRole / setRelatedContactId.
    void Q_DECL_DEPRECATED setRole(QContactRelationshipFilter::Role roleInRelationship);
    void Q_DECL_DEPRECATED setOtherParticipantId(const QContactId& contactId);
    QContactRelationshipFilter::Role Q_DECL_DEPRECATED role() const;
    QContactId Q_DECL_DEPRECATED otherParticipantId() const;

private:
    Q_DECLARE_CONTACTFILTER_PRIVATE(QContactRelationshipFilter)
};

QTM_END_NAMESPACE

#endif
