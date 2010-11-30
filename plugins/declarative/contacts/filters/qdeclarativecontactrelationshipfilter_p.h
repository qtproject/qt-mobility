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

#include <QDebug>

class QDeclarativeContactRelationshipFilter : public QDeclarativeContactFilter
{
    Q_OBJECT
    Q_PROPERTY(QVariant relationshipType READ relationshipType WRITE setRelationshipType NOTIFY valueChanged())
    Q_PROPERTY(QContactLocalId relatedContactId READ relatedContactId WRITE setRelatedContactId NOTIFY valueChanged())
    Q_PROPERTY(QDeclarativeContactRelationship::RelationshipRole relatedContactRole READ relatedContactRole WRITE setRelatedContactRole NOTIFY valueChanged())

public:

    QDeclarativeContactRelationshipFilter(QObject* parent = 0)
        :QDeclarativeContactFilter(parent)
    {
        connect(this, SIGNAL(valueChanged()), SIGNAL(filterChanged()));
    }

    QVariant relationshipType() const
    {
        return d.relationshipType();
    }
    void setRelationshipType(const QVariant& v)
    {
        QString rt;
        if (v.type() != QVariant::String) {
            switch (v.toInt()) {
            case QDeclarativeContactRelationship::HasMember:
                rt = QLatin1String(QContactRelationship::HasMember);
                break;
            case QDeclarativeContactRelationship::Aggregates:
                rt = QLatin1String(QContactRelationship::Aggregates);
                break;
            case QDeclarativeContactRelationship::IsSameAs:
                rt = QLatin1String(QContactRelationship::IsSameAs);
                break;
            case QDeclarativeContactRelationship::HasAssistant:
                rt = QLatin1String(QContactRelationship::HasAssistant);
                break;
            case QDeclarativeContactRelationship::HasManager:
                rt = QLatin1String(QContactRelationship::HasManager);
                break;
            case QDeclarativeContactRelationship::HasSpouse:
                rt = QLatin1String(QContactRelationship::HasSpouse);
                break;
           default:
                break;
            }
        } else {
            rt = v.toString();
        }

        if (rt != relationshipType()) {
            d.setRelationshipType(rt);
            emit valueChanged();
        }
    }

    QContactLocalId relatedContactId() const
    {
        return d.relatedContactId().localId();
    }

    void setRelatedContactId(const QContactLocalId& v)
    {
        if (v != d.relatedContactId().localId()) {
            QContactId contactId;
            contactId.setLocalId(v);
            d.setRelatedContactId(contactId);
            emit valueChanged();
        }
    }

    QDeclarativeContactRelationship::RelationshipRole relatedContactRole() const
    {
        switch (d.relatedContactRole()) {
        case QContactRelationship::First:
            return QDeclarativeContactRelationship::First;
        case QContactRelationship::Second:
            return QDeclarativeContactRelationship::Second;
        default:
            break;
        }
        return QDeclarativeContactRelationship::Either;
    }

    void setRelatedContactRole(QDeclarativeContactRelationship::RelationshipRole v)
    {
        if (v != relatedContactRole()) {
            switch (v) {
            case QDeclarativeContactRelationship::First:
                d.setRelatedContactRole(QContactRelationship::First);
                break;
            case QDeclarativeContactRelationship::Second:
                d.setRelatedContactRole(QContactRelationship::Second);
                break;
            case QDeclarativeContactRelationship::Either:
                d.setRelatedContactRole(QContactRelationship::Either);
                break;
            }
            emit valueChanged();
        }
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
