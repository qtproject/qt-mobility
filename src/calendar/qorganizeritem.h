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

#ifndef QORGANIZERITEM_H
#define QORGANIZERITEM_H

#include <QVariant>
#include <QString>
#include <QPair>
#include <QMultiMap>
#include <QList>
#include <QDateTime>
#include <QSharedDataPointer>

#include "qorganizeritemdetail.h"
#include "qorganizeritemdetaildefinition.h"

QTM_BEGIN_NAMESPACE

class QOrganizerItemManager;
class QOrganizerItemData;
class QOrganizerItemName;

class Q_CALENDAR_EXPORT QOrganizerItem
{
public:
    QOrganizerItem();

    ~QOrganizerItem();

    QOrganizerItem(const QOrganizerItem& other);
    QOrganizerItem& operator=(const QOrganizerItem& other);

    bool operator==(const QOrganizerItem &other) const;
    bool operator!=(const QOrganizerItem &other) const {return !(other == *this);}

    /* Unique ID */
    QOrganizerItemId id() const;
    void setId(const QOrganizerItemId& id);
    QOrganizerItemLocalId localId() const;

    /* Type - event, todo, journal, invitation, ... */
    QString type() const;
    void setType(const QString& type);
    void setType(const QOrganizerItemType& type);

    /* The (backend synthesized) display label of the item */
    QString displayLabel() const;

    /* Is this an empty item? */
    bool isEmpty() const;
    void clearDetails();

    /* Access details of particular type or which support a particular action */
    QOrganizerItemDetail detail(const QString& definitionId) const;
    QList<QOrganizerItemDetail> details(const QString& definitionId = QString()) const;
    QOrganizerItemDetail detailWithAction(const QString& actionName) const;
    QList<QOrganizerItemDetail> detailsWithAction(const QString& actionName) const;

    /* Templated (type-specific) detail retrieval */
    template<typename T> QList<T> details() const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName);
        QList<T> ret;
        foreach(QOrganizerItemDetail prop, props)
            ret.append(T(prop));
        return ret;
    }
    
    /* Templated (type-specific) detail retrieval base on given detail field name and field value */
    template<typename T> QList<T> details(const QString& fieldName, const QString& value) const
    {
        QList<QOrganizerItemDetail> props = details(T::DefinitionName, fieldName, value);
        QList<T> ret;
        foreach(QOrganizerItemDetail prop, props)
            ret.append(T(prop));
        return ret;
    }
    
    /* Detail retrieval base on given detail definition name, field name and field value */
    QList<QOrganizerItemDetail> details(const QString& definitionName, const QString& fieldName, const QString& value) const;

    template<typename T> T detail() const
    {
        return T(detail(T::DefinitionName));
    }

    /* generic detail addition/removal functions */
    bool saveDetail(QOrganizerItemDetail* detail);   // modifies the detail - sets its ID if detail already exists
    bool removeDetail(QOrganizerItemDetail* detail); // modifies the detail - unsets its ID

    /* Other items which are directly related to this item */
    QList<QOrganizerItemId> relatedItems(const QString& relationshipType = QString(), QOrganizerItemRelationshipFilter::Role role = QOrganizerItemRelationshipFilter::Either) const;

    /* Actions available to be performed on this item (publish details? set alarm? or..?) */
    QList<QOrganizerItemActionDescriptor> availableActions(const QString& vendorName = QString(), int implementationVersion = -1) const;

private:
    friend class QOrganizerItemManager;
    friend class QOrganizerItemManagerData;
    friend class QOrganizerItemManagerEngine;

    QSharedDataPointer<QOrganizerItemData> d;
};

QTM_END_NAMESPACE

#endif

