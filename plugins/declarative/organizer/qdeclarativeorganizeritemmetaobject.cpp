#include <QString>
#include <QDebug>

#include "qdeclarativeorganizeritem_p.h"
#include "qorganizeritemdetails.h"
#include "qdeclarativeorganizeritemdetail_p.h"
#include "qdeclarativeorganizeritemmetaobject_p.h"

QTM_USE_NAMESPACE
static OrganizerItemDetailNameMap qt_organizerItemDetailNameMap[] = {
   {"",               "",                false}
};

QDeclarativeOrganizerItemMetaObject::QDeclarativeOrganizerItemMetaObject(QObject* obj, const QOrganizerItem& item)
    :QDeclarativeOpenMetaObject(obj)
{
    setItem(item);
}

QDeclarativeOrganizerItemMetaObject::~QDeclarativeOrganizerItemMetaObject()
{

}

void QDeclarativeOrganizerItemMetaObject::getValue(int propId, void **a)
{
    qDebug() << "value for propId:" << propId;
    OrganizerItemDetailNameMap* detailMetaData = m_properties.value(propId);
    if (detailMetaData) {
        if (detailMetaData->group) {
            qDebug() << "name:" << detailMetaData->name;
            *reinterpret_cast< QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>* >(a[0]) =
                    QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>(object(), detailMetaData, detail_append, detail_count, detail_at, detail_clear);

        } else {
            foreach(QDeclarativeOrganizerItemDetail* cd, m_details) {
                if (cd->detail().definitionName() == detailMetaData->definitionName) {
                    *reinterpret_cast<QVariant *>(a[0]) = QVariant::fromValue(cd);
                }
            }
        }
    }

}

void QDeclarativeOrganizerItemMetaObject::setValue(int propId, void **a)
{
    OrganizerItemDetailNameMap* detailMetaData = m_properties.value(propId);
    if (detailMetaData) {
        if (!detailMetaData->group) {
            QVariant& v = *reinterpret_cast<QVariant *>(a[0]);
            QDeclarativeOrganizerItemDetail* detail = v.value<QDeclarativeOrganizerItemDetail*>();

            foreach(QDeclarativeOrganizerItemDetail* cd, m_details) {
                if (cd->detail().definitionName() == detailMetaData->definitionName) {
                    delete cd;
                    cd = detail;
                }
            }
        }
    }
}

int QDeclarativeOrganizerItemMetaObject::createProperty(const char * name,  const char *)
{

    const int detailCount = sizeof(qt_organizerItemDetailNameMap)/sizeof(OrganizerItemDetailNameMap);
    OrganizerItemDetailNameMap* detailMetaData = 0;

    for (int i = 0; i < detailCount; i++) {
        if (QString::fromLocal8Bit(qt_organizerItemDetailNameMap[i].name) == QString::fromLocal8Bit(name)) {
            detailMetaData = &qt_organizerItemDetailNameMap[i];
            break;
        }
    }

    if (detailMetaData) {
        int propId = -1;
        if (detailMetaData->group)
            propId = QDeclarativeOpenMetaObject::createProperty(name, "QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>");
        else
            propId = QDeclarativeOpenMetaObject::createProperty(name, "QVariant");
        m_properties.insert(propId, detailMetaData);
        qDebug() << "createProperty:" << name << "propId:" << propId;
        return propId;
    }
    return -1;
}


QVariant QDeclarativeOrganizerItemMetaObject::detail(const QString& name)
{
    int propId = indexOfProperty(name.toLatin1());

    if (propId > 0)
        return property(propId).read(object());

    //Assume it's a detail definition name
    foreach(QDeclarativeOrganizerItemDetail* cd, m_details) {
        if (cd->detail().definitionName() == name) {
            return QVariant::fromValue(cd);
        }
    }
    return QVariant();
}

QVariant QDeclarativeOrganizerItemMetaObject::details(const QString& name)
{
    if (name.isEmpty()) {
        //return all
        return QVariant::fromValue(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail>(object(), m_details));
    } else {
        int propId = indexOfProperty(name.toLatin1());
        if (propId > 0)
            return property(propId).read(object());

        //Assume it's a detail definition name
        //TODO::customized details
        for (QHash<int, OrganizerItemDetailNameMap*>::ConstIterator iter = m_properties.constBegin(); iter != m_properties.constEnd(); iter++) {
            if (iter.value()->group && iter.value()->definitionName == name)
                return property(iter.key()).read(object());
        }
    }
    return QVariant();
}

void QDeclarativeOrganizerItemMetaObject::setItem(const QOrganizerItem& contact)
{
    m_item = contact;
    QList<QOrganizerItemDetail> details = m_item.details();
    m_details.clear();
    foreach (const QOrganizerItemDetail& detail, details) {
      QDeclarativeOrganizerItemDetail* cd = new QDeclarativeOrganizerItemDetail(object());

      cd->connect(cd, SIGNAL(fieldsChanged()), object(), SIGNAL(detailsChanged()));

      cd->setDetail(detail);
      m_details.append(cd);
    }
}

QOrganizerItem QDeclarativeOrganizerItemMetaObject::item()
{
    m_item.clearDetails();
    foreach ( QDeclarativeOrganizerItemDetail* cd, m_details) {
       QOrganizerItemDetail detail = cd->detail();
       m_item.saveDetail(&detail);
    }
    return m_item;
}

uint QDeclarativeOrganizerItemMetaObject::localId() const
{
    return qHash(m_item.localId());
}

uint QDeclarativeOrganizerItemMetaObject::itemId() const
{
    return qHash(m_item.id());
}


void QDeclarativeOrganizerItemMetaObject::detail_append(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p, QDeclarativeOrganizerItemDetail *detail)
{
    OrganizerItemDetailNameMap* data = (OrganizerItemDetailNameMap*)(p->data);
    if (data) {
        QDeclarativeOrganizerItem* dc = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
        if (dc && detail->detail().definitionName() == data->definitionName) {
            detail->connect(detail, SIGNAL(fieldsChanged()), dc, SIGNAL(detailsChanged()));
            dc->d->m_details.append(detail);
        }
    }
}

int  QDeclarativeOrganizerItemMetaObject::detail_count(QDeclarativeListProperty<QDeclarativeOrganizerItemDetail> *p)
{
    OrganizerItemDetailNameMap* data = (OrganizerItemDetailNameMap*)(p->data);
    int count = 0;
    QDeclarativeOrganizerItem* dc = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
    if (dc && data) {
        foreach(QDeclarativeOrganizerItemDetail* detail, dc->d->m_details) {
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
    QDeclarativeOrganizerItem* dc = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
    if (dc && data) {
        int i = 0;
        foreach(QDeclarativeOrganizerItemDetail* cd,dc->d->m_details) {
            if (cd->detail().definitionName() == data->definitionName) {
                if (i == idx) {
                    detail = cd;
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
    QDeclarativeOrganizerItem* dc = qobject_cast<QDeclarativeOrganizerItem*>(p->object);
    if (dc && data) {
        foreach(QDeclarativeOrganizerItemDetail* cd, dc->d->m_details) {
            if (cd->detail().definitionName() == data->definitionName) {
                dc->d->m_details.removeAll(cd);
            }
        }
    }
}
