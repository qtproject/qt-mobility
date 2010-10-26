#include <QString>
#include <QDebug>

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
    return detailMetaDataByDetailType(QDeclarativeOrganizerItemDetail::detailType(name));
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
    if (m_defs.isEmpty() || !m_defs.value(QDeclarativeOrganizerItemDetail::definitionName(type)).isEmpty()) {
        QDeclarativeOrganizerItemDetail* itemDetail = createItemDetail(type, object());
        m_details.append(itemDetail);
        return QVariant::fromValue(itemDetail);
    }

    return QVariant();
}

QVariant QDeclarativeOrganizerItemMetaObject::detail(const QString& name)
{
    return detail(QDeclarativeOrganizerItemDetail::detailType(name));
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
    return details(QDeclarativeOrganizerItemDetail::definitionName(type));
}

void QDeclarativeOrganizerItemMetaObject::setItem(const QOrganizerItem& item)
{
    m_item = item;
    QList<QOrganizerItemDetail> details = m_item.details();
    m_details.clear();
    foreach (const QOrganizerItemDetail& detail, details) {
      QDeclarativeOrganizerItemDetail* itemDetail = new QDeclarativeOrganizerItemDetail(object());

      itemDetail->setDetail(detail);
      itemDetail->connect(itemDetail, SIGNAL(valueChanged()), object(), SIGNAL(detailsChanged()));

      m_details.append(itemDetail);
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

uint QDeclarativeOrganizerItemMetaObject::itemId() const
{
    return qHash(m_item.id());
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
    switch (type) {
    case QDeclarativeOrganizerItemDetail::EventTime:
        return new QDeclarativeOrganizerEventTime(parent);
    case QDeclarativeOrganizerItemDetail::JournalTime:
        return new QDeclarativeOrganizerJournalTime(parent);
    case QDeclarativeOrganizerItemDetail::TodoTime:
        return new QDeclarativeOrganizerTodoTime(parent);
    case QDeclarativeOrganizerItemDetail::TodoProgress:
        return new QDeclarativeOrganizerTodoProgress(parent);
    case QDeclarativeOrganizerItemDetail::Reminder:
        return new QDeclarativeOrganizerItemReminder(parent);
    case QDeclarativeOrganizerItemDetail::AudibleReminder:
        return new QDeclarativeOrganizerItemAudibleReminder(parent);
    case QDeclarativeOrganizerItemDetail::VisualReminder:
        return new QDeclarativeOrganizerItemVisualReminder(parent);
    case QDeclarativeOrganizerItemDetail::EmailReminder:
        return new QDeclarativeOrganizerItemEmailReminder(parent);
    case QDeclarativeOrganizerItemDetail::Comment:
        return new QDeclarativeOrganizerItemComment(parent);
    case QDeclarativeOrganizerItemDetail::Description:
        return new QDeclarativeOrganizerItemDescription(parent);
    case QDeclarativeOrganizerItemDetail::DisplayLabel:
        return new QDeclarativeOrganizerItemDisplayLabel(parent);
    case QDeclarativeOrganizerItemDetail::Guid:
        return new QDeclarativeOrganizerItemGuid(parent);
    case QDeclarativeOrganizerItemDetail::Location:
        return new QDeclarativeOrganizerItemLocation(parent);
    case QDeclarativeOrganizerItemDetail::Parent:
        return new QDeclarativeOrganizerItemParent(parent);
    case QDeclarativeOrganizerItemDetail::Priority:
        return new QDeclarativeOrganizerItemPriority(parent);
    case QDeclarativeOrganizerItemDetail::Recurrence:
        return new QDeclarativeOrganizerItemRecurrence(parent);
    case QDeclarativeOrganizerItemDetail::Timestamp:
        return new QDeclarativeOrganizerItemTimestamp(parent);
    case QDeclarativeOrganizerItemDetail::Type:
        return new QDeclarativeOrganizerItemType(parent);
    default:
        break;
    }
    //customized
    return new QDeclarativeOrganizerItemDetail(parent);
}
