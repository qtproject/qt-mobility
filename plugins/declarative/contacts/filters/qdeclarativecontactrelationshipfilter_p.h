/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVECONTACTRELATIONSHIPFILTER_H
#define QDECLARATIVECONTACTRELATIONSHIPFILTER_H

#include "qdeclarativecontactfilter_p.h"
#include "qcontactrelationshipfilter.h"
#include "qdeclarativecontactrelationship_p.h"

class QDeclarativeContactRelationshipFilter : public QDeclarativeContactFilter
{
    Q_OBJECT
    Q_PROPERTY(QString relationshipType READ relationshipType WRITE setRelationshipType NOTIFY valueChanged)
    Q_PROPERTY(int relatedContactId READ relatedContactId WRITE setRelatedContactId NOTIFY valueChanged)
    Q_PROPERTY(QDeclarativeContactRelationship::Role relatedContactRole READ relatedContactRole WRITE setRelatedContactRole NOTIFY valueChanged)

public:

    QDeclarativeContactRelationshipFilter(QObject* parent = 0)
        :QDeclarativeContactFilter(parent)
    {
    }

    QString relationshipType() const
    {
        return d.relationshipType();
    }
    void setRelationshipType(const QString& relationshipType)
    {
        d.setRelationshipType(relationshipType);
    }

    int relatedContactId() const
    {
        return d.relatedContactId().localId();
    }

    void setRelatedContactId(const int& relatedContactId)
    {
        QContactId contactId;
        contactId.setLocalId(relatedContactId);
        d.setRelatedContactId(contactId);
    }

    QDeclarativeContactRelationship::Role relatedContactRole() const
    {
        switch (d.relatedContactRole()) {
        case QContactRelationship::First:
            return QDeclarativeContactRelationship::First;
        case QContactRelationship::Second:
            return QDeclarativeContactRelationship::Second;
        case QContactRelationship::Either:
            break;
        }
        return QDeclarativeContactRelationship::Either;
    }

    void setRelatedContactRole(QDeclarativeContactRelationship::Role relatedContactRole)
    {
        switch (relatedContactRole) {
        case QDeclarativeContactRelationship::First:
            d.setRelatedContactRole(QContactRelationship::First);
        case QDeclarativeContactRelationship::Second:
            d.setRelatedContactRole(QContactRelationship::Second);
        case QDeclarativeContactRelationship::Either:
            break;
        }
        d.setRelatedContactRole(QContactRelationship::Either);
    }
    QContactFilter filter() const
    {
        return d;
    }
signals:
    void valueChanged();


private:
    QContactRelationshipFilter d;
};

QML_DECLARE_TYPE(QDeclarativeContactRelationshipFilter)

#endif
