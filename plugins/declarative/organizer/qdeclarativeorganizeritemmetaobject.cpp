/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

#include <QString>

#include "qdeclarativeorganizeritem_p.h"
#include "qorganizeritemdetails.h"
#include "qdeclarativeorganizeritemdetail_p.h"
#include "qdeclarativeorganizeritemmetaobject_p.h"

QTM_USE_NAMESPACE
#define Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(detailtype, classname) \
{QDeclarativeOrganizerItemDetail::detailtype, QDeclarativeOrganizer##classname::DetailName.latin1(), QOrganizer##classname::DefinitionName.latin1(), false}

#define Q_DECLARATIVE_ORGANIZER_ITEM_DETAILGROUPNAME(detailtype, classname) \
    {QDeclarativeOrganizerItemDetail::detailtype, QDeclarativeOrganizer##classname::DetailGroupName.latin1(), QOrganizer##classname::DefinitionName.latin1(), true}

static OrganizerItemDetailNameMap qt_organizerItemDetailNameMap[] = {
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(EventTime, EventTime),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Comment, ItemComment),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILGROUPNAME(Comment, ItemComment),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Description, ItemDescription),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(DisplayLabel, ItemDisplayLabel),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Guid, ItemGuid),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Parent, ItemParent),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Location, ItemLocation),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Priority, ItemPriority),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Recurrence, ItemRecurrence),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Reminder, ItemReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILGROUPNAME(Reminder, ItemReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(AudibleReminder, ItemAudibleReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILGROUPNAME(AudibleReminder, ItemAudibleReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(VisualReminder, ItemVisualReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILGROUPNAME(VisualReminder, ItemVisualReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(EmailReminder, ItemEmailReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILGROUPNAME(EmailReminder, ItemEmailReminder),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Timestamp, ItemTimestamp),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(Type, ItemType),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(JournalTime, JournalTime),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(TodoProgress, TodoProgress),
    Q_DECLARATIVE_ORGANIZER_ITEM_DETAILNAME(TodoTime, TodoTime)
};

/*!
    \class QDeclarativeOrganizerItemMetaObject
    \internal
    \brief open organizer item meta object for accessing organizer item detail dynamic properties in qml
*/

QDeclarativeOrganizerItemMetaObject::QDeclarativeOrganizerItemMetaObject(QObject* obj, const QOrganizerItem& item)
    :QDeclarativeOpenMetaObject(obj),
      m_modified(false)
{
    setItem(item);
}

QDeclarativeOrganizerItemMetaObject::~QDeclarativeOrganizerItemMetaObject()
{

}

void QDeclarativeOrganizerItemMetaObject::getValue(int propId, void **a)
{
    OrganizerItemDetailNameMap* detailMetaData = m_properties.value(propId);
    if (detailMetaData) {
        if (detailMetaData->group) {
            *reinterpret_cast< QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>* >(a[0]) =
                    QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>(object(), detailMetaData, detail_append, detail_count, detail_at, detail_clear);

        } else {
            foreach(QDeclarativeOrganizerItemDetail* itemDetail, m_details) {
                //TODO: compare by type
                if (itemDetail->detail().definitionName() == detailMetaData->definitionName) {
                    *reinterpret_cast<QVariant *>(a[0]) = QVariant::fromValue(itemDetail);
                }
            }
        }
    }

}

void QDeclarativeOrganizerItemMetaObject::setValue(int propId, void **a)
{
    Q_UNUSED(propId);
    Q_UNUSED(a);
}


OrganizerItemDetailNameMap* QDeclarativeOrganizerItemMetaObject::detailMetaDataByDetailName(const char * name)
{
    static const int detailCount = sizeof(qt_organizerItemDetailNameMap)/sizeof(OrganizerItemDetailNameMap);
    OrganizerItemDetailNameMap* detailMetaData = 0;

    for (int i = 0; i < detailCount; i++) {
        if (QString::fromLatin1(qt_organizerItemDetailNameMap[i].name) == QString::fromLatin1(name)) {
            detailMetaData = &qt_organizerItemDetailNameMap[i];
            break;
        }
    }
    return detailMetaData;
}

OrganizerItemDetailNameMap* QDeclarativeOrganizerItemMetaObject::detailMetaDataByDefinitionName(const char * name)
{
    return detailMetaDataByDetailType(QDeclarativeOrganizerItemDetail::detailTypeByDefinitionName(name));
}

OrganizerItemDetailNameMap* QDeclarativeOrganizerItemMetaObject::detailMetaDataByDetailType(QDeclarativeOrganizerItemDetail::ItemDetailType type)
{
    static const int detailCount = sizeof(qt_organizerItemDetailNameMap)/sizeof(OrganizerItemDetailNameMap);
    OrganizerItemDetailNameMap* detailMetaData = 0;

    for (int i = 0; i < detailCount; i++) {
        if (qt_organizerItemDetailNameMap[i].type == type && qt_organizerItemDetailNameMap[i].group) {
            detailMetaData = &qt_organizerItemDetailNameMap[i];
            break;
        }
    }
    return detailMetaData;
}



int QDeclarativeOrganizerItemMetaObject::createProperty(const char * name,  const char *)
{
    OrganizerItemDetailNameMap* detailMetaData = detailMetaDataByDetailName(name);;

    if (detailMetaData) {
        int propId = -1;
        if (detailMetaData->group) {
            QOrganizerItemDetailDefinition def = m_defs.value(detailMetaData->definitionName);

            //do not allow multiple details property for non unique details
            if (m_defs.isEmpty() || (!def.isEmpty() && !def.isUnique()))
                propId = QDeclarativeOpenMetaObject::createProperty(name, "QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>");
        }
        else {
            propId = QDeclarativeOpenMetaObject::createProperty(name, "QVariant");
        }
        m_properties.insert(propId, detailMetaData);
        return propId;
    }
    return -1;
}

QVariant QDeclarativeOrganizerItemMetaObject::detail(QDeclarativeOrganizerItemDetail::ItemDetailType type)
{
    foreach(QDeclarativeOrganizerItemDetail* itemDetail, m_details) {
        if (itemDetail->type() == type) {
            return QVariant::fromValue(itemDetail);
        }
    }

    //Check should we create a new detail for this type
    //XXX:TODO check mutable detail definition feature in manager?
    if (m_defs.isEmpty() || !m_defs.value(QDeclarativeOrganizerItemDetail::detailName(type)).isEmpty()) {
        QDeclarativeOrganizerItemDetail* itemDetail = createItemDetail(type, object());
        m_details.append(itemDetail);
        return QVariant::fromValue(itemDetail);
    }

    return QVariant();
}

QVariant QDeclarativeOrganizerItemMetaObject::detail(const QString& name)
{
    return detail(QDeclarativeOrganizerItemDetail::detailTypeByDetailName(name));
}

QVariant QDeclarativeOrganizerItemMetaObject::details(const QString& name)
{
    if (name.isEmpty()) {
        //return all
        return QVariant::fromValue(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>(object(), m_details));
    } else {
        int propId = indexOfProperty(name.toLatin1());
        if (propId <= 0) {
            OrganizerItemDetailNameMap* metaData  = detailMetaDataByDefinitionName(name.toLatin1());
            if (metaData) {
               propId = indexOfProperty(metaData->name);
            }
        }
        if (propId > 0)
            return property(propId).read(object());
    }
    return QVariant();
}

QVariant QDeclarativeOrganizerItemMetaObject::details(QDeclarativeOrganizerItemDetail::ItemDetailType type)
{
    return details(QDeclarativeOrganizerItemDetail::detailName(type));
}

void QDeclarativeOrganizerItemMetaObject::setItem(const QOrganizerItem& item)
{
    m_item = item;
    QList<QOrganizerItemDetail> details = m_item.details();
    m_details.clear();
    foreach (const QOrganizerItemDetail& detail, details) {
      QDeclarativeOrganizerItemDetail* itemDetail = createItemDetail(QDeclarativeOrganizerItemDetail::detailTypeByDefinitionName(detail.definitionName()), object());

      itemDetail->setDetail(detail);
      m_details.append(itemDetail);
    }
    if (m_item.type() == QOrganizerItemType::TypeEventOccurrence || m_item.type() == QOrganizerItemType::TypeTodoOccurrence ) {
        //create temporary id for occurrence items
        m_id = QString("qtorganizer:occurrence:%1").arg (QUuid::createUuid ().toString ());
    } else {
        m_id = m_item.id().toString();
    }
}

QOrganizerItem QDeclarativeOrganizerItemMetaObject::item()
{
    foreach ( QDeclarativeOrganizerItemDetail* cd, m_details) {
       QOrganizerItemDetail detail = cd->detail();
       m_item.saveDetail(&detail);
    }
    return m_item;
}

QString QDeclarativeOrganizerItemMetaObject::itemId() const
{
    return m_id;
}


void QDeclarativeOrganizerItemMetaObject::detail_append(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p, QDeclarativeOrganizerItemDetail *detail)
{
    OrganizerItemDetailNameMap* data = (OrganizerItemDetailNameMap*)(p->data);
    if (data) {
        QDeclarativeOrganizerItem* item = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
        if (item && detail->detail().definitionName() == data->definitionName) {
            detail->connect(detail, SIGNAL(valueChanged()), item, SIGNAL(valueChanged()));
            item->d->m_details.append(detail);
        }
    }
}

int  QDeclarativeOrganizerItemMetaObject::detail_count(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p)
{
    OrganizerItemDetailNameMap* data = (OrganizerItemDetailNameMap*)(p->data);
    int count = 0;
    QDeclarativeOrganizerItem* item = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
    if (item && data) {
        foreach(QDeclarativeOrganizerItemDetail* detail, item->d->m_details) {
            if (detail->detail().definitionName() == data->definitionName)
                count++;
        }
    }
    return count;
}

QDeclarativeOrganizerItemDetail * QDeclarativeOrganizerItemMetaObject::detail_at(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p, int idx)
{
    OrganizerItemDetailNameMap* data = (OrganizerItemDetailNameMap*)(p->data);
    QDeclarativeOrganizerItemDetail* detail = 0;
    QDeclarativeOrganizerItem* item = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
    if (item && data) {
        int i = 0;
        foreach(QDeclarativeOrganizerItemDetail* itemDetail,item->d->m_details) {
            if (itemDetail->detail().definitionName() == data->definitionName) {
                if (i == idx) {
                    detail = itemDetail;
                    break;
                } else {
                    i++;
                }
            }
        }
    }
    return detail;
}

void  QDeclarativeOrganizerItemMetaObject::detail_clear(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p)
{
    OrganizerItemDetailNameMap* data = (OrganizerItemDetailNameMap*)(p->data);
    QDeclarativeOrganizerItem* item = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
    if (item && data) {
        foreach(QDeclarativeOrganizerItemDetail* itemDetail, item->d->m_details) {
            if (itemDetail->detail().definitionName() == data->definitionName) {
                item->d->m_details.removeAll(itemDetail);
            }
        }
    }
}


QDeclarativeOrganizerItemDetail* QDeclarativeOrganizerItemMetaObject::createItemDetail(QDeclarativeOrganizerItemDetail::ItemDetailType type, QObject* parent)
{
    Q_ASSERT(qobject_cast<QDeclarativeOrganizerItem*>(parent));
    QDeclarativeOrganizerItemDetail* detail;
    switch (type) {
    case QDeclarativeOrganizerItemDetail::EventTime:
        detail = new QDeclarativeOrganizerEventTime(parent);
        break;
    case QDeclarativeOrganizerItemDetail::JournalTime:
        detail = new QDeclarativeOrganizerJournalTime(parent);
        break;
    case QDeclarativeOrganizerItemDetail::TodoTime:
        detail = new QDeclarativeOrganizerTodoTime(parent);
        break;
    case QDeclarativeOrganizerItemDetail::TodoProgress:
        detail = new QDeclarativeOrganizerTodoProgress(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Reminder:
        detail = new QDeclarativeOrganizerItemReminder(parent);
        break;
    case QDeclarativeOrganizerItemDetail::AudibleReminder:
        detail = new QDeclarativeOrganizerItemAudibleReminder(parent);
        break;
    case QDeclarativeOrganizerItemDetail::VisualReminder:
        detail = new QDeclarativeOrganizerItemVisualReminder(parent);
        break;
    case QDeclarativeOrganizerItemDetail::EmailReminder:
        detail = new QDeclarativeOrganizerItemEmailReminder(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Comment:
        detail = new QDeclarativeOrganizerItemComment(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Description:
        detail = new QDeclarativeOrganizerItemDescription(parent);
        break;
    case QDeclarativeOrganizerItemDetail::DisplayLabel:
        detail = new QDeclarativeOrganizerItemDisplayLabel(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Guid:
        detail = new QDeclarativeOrganizerItemGuid(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Location:
        detail = new QDeclarativeOrganizerItemLocation(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Parent:
        detail = new QDeclarativeOrganizerItemParent(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Priority:
        detail = new QDeclarativeOrganizerItemPriority(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Recurrence:
        detail = new QDeclarativeOrganizerItemRecurrence(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Timestamp:
        detail = new QDeclarativeOrganizerItemTimestamp(parent);
        break;
    case QDeclarativeOrganizerItemDetail::Type:
        detail = new QDeclarativeOrganizerItemType(parent);
        break;
    default:
        //customized
        detail = new QDeclarativeOrganizerItemDetail(parent);
        break;
    }
    QObject::connect(detail, SIGNAL(detailChanged()), parent, SIGNAL(itemChanged()));
    return detail;
}
