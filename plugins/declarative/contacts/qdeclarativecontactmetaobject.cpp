#include <QString>
#include <QDebug>

#include "qdeclarativecontact_p.h"
#include "qcontactdetails.h"
#include "qdeclarativecontactdetail_p.h"
#include "qdeclarativecontactmetaobject_p.h"

QTM_USE_NAMESPACE
static ContactDetailNameMap qt_contactDetailNameMap[] = {
    {QDeclarativeContactDetail::ContactAddress,        "address",        QContactAddress::DefinitionName.latin1(),  false},
    {QDeclarativeContactDetail::ContactAddress,        "addresses",      QContactAddress::DefinitionName.latin1(),  true},
    {QDeclarativeContactDetail::ContactAnniversary,    "anniversary",    QContactAnniversary::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactAnniversary,    "anniversaries",  QContactAnniversary::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactAvatar,         "avatar",         QContactAvatar::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactAvatar,         "avatars",        QContactAvatar::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactBirthday,       "birthday",       QContactBirthday::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactBirthday,       "birthdays",      QContactBirthday::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactDisplayLabel,   "displayLabel",   QContactDisplayLabel::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactDisplayLabel,   "displayLabels",  QContactDisplayLabel::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactEmail,          "email",          QContactEmailAddress::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactEmail,          "emails",         QContactEmailAddress::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactFamily,         "family",         QContactFamily::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactFamily,         "families",       QContactFamily::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactFavorite,       "favorite",       QContactFavorite::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactFavorite,       "favorites",      QContactFavorite::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactGender,         "gender",         QContactGender::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactGender,         "genders",        QContactGender::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactGeolocation,    "location",       QContactGeoLocation::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactGeolocation,    "locations",      QContactGeoLocation::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactGlobalPresence, "globalPresence", QContactGlobalPresence::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactGlobalPresence, "globalPresences",QContactGlobalPresence::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactGuid,           "guid",           QContactGuid::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactGuid,           "guids",          QContactGuid::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactName,           "name",           QContactName::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactName,           "names",          QContactName::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactNickName,       "nickname",       QContactNickname::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactNickName,       "nicknames",      QContactNickname::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactNote,           "note",           QContactNote::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactNote,           "notes",          QContactNote::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactOnlineAccount,  "onlineAccount",  QContactOnlineAccount::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactOnlineAccount,  "onlineAccounts", QContactOnlineAccount::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactOrganization,   "organization",   QContactOrganization::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactOrganization,   "organizations",  QContactOrganization::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactPhoneNumber,    "phoneNumber",    QContactPhoneNumber::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactPhoneNumber,    "phoneNumbers",   QContactPhoneNumber::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactPresence,       "presence",       QContactPresence::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactPresence,       "presences",      QContactPresence::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactRingtone,       "ringtone",       QContactRingtone::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactRingtone,       "ringtones",      QContactRingtone::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactSyncTarget,     "syncTarget",     QContactSyncTarget::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactSyncTarget,     "syncTargets",    QContactSyncTarget::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactTag,            "tag",            QContactTag::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactTag,            "tags",           QContactTag::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactTimestamp,      "timestamp",      QContactTimestamp::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactTimestamp,      "timestamps",     QContactTimestamp::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactUrl,            "url",            QContactUrl::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::ContactUrl,            "urls",           QContactUrl::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::ContactCustomizedDetail,     "",               "",                false}
};

QDeclarativeContactMetaObject::QDeclarativeContactMetaObject(QObject* obj, const QContact& contact)
    :QDeclarativeOpenMetaObject(obj),
      m_modified(false)
{
    setContact(contact);
}

QDeclarativeContactMetaObject::~QDeclarativeContactMetaObject()
{

}

void QDeclarativeContactMetaObject::getValue(int propId, void **a)
{
    ContactDetailNameMap* detailMetaData = m_properties.value(propId);
    if (detailMetaData) {
        if (detailMetaData->group) {
            *reinterpret_cast< QDeclarativeListProperty<QDeclarativeContactDetail>* >(a[0]) =
                    QDeclarativeListProperty<QDeclarativeContactDetail>(object(), detailMetaData, detail_append, detail_count, detail_at, detail_clear);

        } else {
            foreach(QDeclarativeContactDetail* cd, m_details) {
                if (cd->detailType() == detailMetaData->type) {
                    *reinterpret_cast<QVariant *>(a[0]) = QVariant::fromValue(cd);
                }
            }
        }
    }

}

void QDeclarativeContactMetaObject::setValue(int propId, void **a)
{
    ContactDetailNameMap* detailMetaData = m_properties.value(propId);
    if (detailMetaData) {
        if (!detailMetaData->group) {
            QVariant& v = *reinterpret_cast<QVariant *>(a[0]);
            QDeclarativeContactDetail* detail = v.value<QDeclarativeContactDetail*>();

            foreach(const QDeclarativeContactDetail* cd, m_details) {
                if (cd->detailType() == detailMetaData->type) {
                    delete cd;
                    cd = detail;
                }
            }
        }
    }
}

ContactDetailNameMap* QDeclarativeContactMetaObject::detailMetaDataByDetailName(const char * name)
{
    static const int detailCount = sizeof(qt_contactDetailNameMap)/sizeof(ContactDetailNameMap);
    ContactDetailNameMap* detailMetaData = 0;

    for (int i = 0; i < detailCount; i++) {
        if (QString::fromLocal8Bit(qt_contactDetailNameMap[i].name) == QString::fromLocal8Bit(name)) {
            detailMetaData = &qt_contactDetailNameMap[i];
            break;
        }
    }
    return detailMetaData;
}

ContactDetailNameMap* QDeclarativeContactMetaObject::detailMetaDataByDefinitionName(const char * name)
{
    return detailMetaDataByDetailType(QDeclarativeContactDetail::detailType(name));
}

ContactDetailNameMap* QDeclarativeContactMetaObject::detailMetaDataByDetailType(QDeclarativeContactDetail::ContactDetailType type)
{
    static const int detailCount = sizeof(qt_contactDetailNameMap)/sizeof(ContactDetailNameMap);
    ContactDetailNameMap* detailMetaData = 0;

    for (int i = 0; i < detailCount; i++) {
        if (qt_contactDetailNameMap[i].type == type && qt_contactDetailNameMap[i].group) {
            detailMetaData = &qt_contactDetailNameMap[i];
            break;
        }
    }
    return detailMetaData;
}


int QDeclarativeContactMetaObject::createProperty(const char * name,  const char *)
{
    ContactDetailNameMap* detailMetaData = detailMetaDataByDetailName(name);
    if (detailMetaData) {
        int propId = -1;
        if (detailMetaData->group) {
            QContactDetailDefinition def = m_defs.value(detailMetaData->definitionName);

            //do not allow multiple details property for non unique details
            if (m_defs.isEmpty() || (!def.isEmpty() && !def.isUnique()))
                propId = QDeclarativeOpenMetaObject::createProperty(name, "QDeclarativeListProperty<QDeclarativeContactDetail>");
        }
        else {
            propId = QDeclarativeOpenMetaObject::createProperty(name, "QVariant");
        }
        m_properties.insert(propId, detailMetaData);
        return propId;
    }
    return -1;
}


QVariant QDeclarativeContactMetaObject::detail(QDeclarativeContactDetail::ContactDetailType type)
{
    foreach(QDeclarativeContactDetail* cd, m_details) {
        if (cd->detailType() == type) {
            return QVariant::fromValue(cd);
        }
    }

    //Check should we create a new detail for this type
    //XXX:TODO check mutable detail definition feature in manager?
    if (m_defs.isEmpty() || !m_defs.value(QDeclarativeContactDetail::definitionName(type)).isEmpty()) {
        QDeclarativeContactDetail* cd = createContactDetail(type, object());
        m_details.append(cd);
        return QVariant::fromValue(cd);
    }

    return QVariant();
}

QVariant QDeclarativeContactMetaObject::detail(const QString& name)
{
    return detail(QDeclarativeContactDetail::detailType(name));
}

QVariant QDeclarativeContactMetaObject::details(const QString& name)
{
    if (name.isEmpty()) {
        //return all
        return QVariant::fromValue(QDeclarativeListProperty<QDeclarativeContactDetail>(object(), m_details));
    } else {
        int propId = indexOfProperty(name.toLatin1());
        if (propId <= 0) {
            ContactDetailNameMap* metaData  = detailMetaDataByDefinitionName(name.toLatin1());
            if (metaData) {
               propId = indexOfProperty(metaData->name);
            }
        }
        if (propId > 0)
            return property(propId).read(object());
    }
    return QVariant();
}

QVariant QDeclarativeContactMetaObject::details(QDeclarativeContactDetail::ContactDetailType type)
{
    return details(QDeclarativeContactDetail::definitionName(type));
}

void QDeclarativeContactMetaObject::setContact(const QContact& contact)
{
    m_contact = contact;

    QList<QContactDetail> details = m_contact.details();
    m_details.clear();
    foreach (const QContactDetail& detail, details) {
      QDeclarativeContactDetail* cd = createContactDetail(QDeclarativeContactDetail::detailType(detail.definitionName()), object());
      cd->setDetail(detail);
      cd->connect(cd, SIGNAL(valueChanged()), object(), SIGNAL(detailsChanged()));
      m_details.append(cd);
    }
}

QContact QDeclarativeContactMetaObject::contact()
{
    //m_contact.clearDetails();
    foreach (const QDeclarativeContactDetail* cd, m_details) {
       QContactDetail detail = cd->detail();
       if (!m_contact.saveDetail(&detail))
           qDebug() << "save detail error:" << detail.definitionName();
    }

    return m_contact;
}

int QDeclarativeContactMetaObject::localId() const
{
    return m_contact.localId();
}

QContactId QDeclarativeContactMetaObject::contactId() const
{
    return m_contact.id();
}
void QDeclarativeContactMetaObject::detail_append(QDeclarativeListProperty<QDeclarativeContactDetail> *p, QDeclarativeContactDetail *detail)
{
    ContactDetailNameMap* data = (ContactDetailNameMap*)(p->data);
    if (data) {
        QDeclarativeContact* dc = qobject_cast<QDeclarativeContact*>(p->object);
        if (dc && detail->detail().definitionName() == data->definitionName) {
            detail->connect(detail, SIGNAL(fieldsChanged()), dc, SIGNAL(detailsChanged()));
            dc->d->m_details.append(detail);
        }
    }
}

int  QDeclarativeContactMetaObject::detail_count(QDeclarativeListProperty<QDeclarativeContactDetail> *p)
{
    ContactDetailNameMap* data = (ContactDetailNameMap*)(p->data);
    int count = 0;
    QDeclarativeContact* dc = qobject_cast<QDeclarativeContact*>(p->object);
    if (dc && data) {
        foreach(const QDeclarativeContactDetail* detail, dc->d->m_details) {
            if (detail->detail().definitionName() == data->definitionName)
                count++;
        }
    }
    return count;
}

QDeclarativeContactDetail * QDeclarativeContactMetaObject::detail_at(QDeclarativeListProperty<QDeclarativeContactDetail> *p, int idx)
{
    ContactDetailNameMap* data = (ContactDetailNameMap*)(p->data);
    QDeclarativeContactDetail* detail = 0;
    QDeclarativeContact* dc = qobject_cast<QDeclarativeContact*>(p->object);
    if (dc && data) {
        int i = 0;
        foreach(QDeclarativeContactDetail* cd,dc->d->m_details) {
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

void  QDeclarativeContactMetaObject::detail_clear(QDeclarativeListProperty<QDeclarativeContactDetail> *p)
{
    ContactDetailNameMap* data = (ContactDetailNameMap*)(p->data);
    QDeclarativeContact* dc = qobject_cast<QDeclarativeContact*>(p->object);
    if (dc && data) {
        foreach(QDeclarativeContactDetail* cd, dc->d->m_details) {
            if (cd->detail().definitionName() == data->definitionName) {
                dc->d->m_details.removeAll(cd);
            }
        }
    }
}


QDeclarativeContactDetail* QDeclarativeContactMetaObject::createContactDetail(QDeclarativeContactDetail::ContactDetailType type, QObject* parent)
{
    switch (type) {
    case QDeclarativeContactDetail::ContactAddress:
        return new QDeclarativeContactAddress(parent);
    case QDeclarativeContactDetail::ContactAnniversary:
        return new QDeclarativeContactAnniversary(parent);
    case QDeclarativeContactDetail::ContactAvatar:
        return new QDeclarativeContactAvatar(parent);
    case QDeclarativeContactDetail::ContactBirthday:
        return new QDeclarativeContactBirthday(parent);
    case QDeclarativeContactDetail::ContactDisplayLabel:
        return new QDeclarativeContactDisplayLabel(parent);
    case QDeclarativeContactDetail::ContactEmail:
        return new QDeclarativeContactEmailAddress(parent);
    case QDeclarativeContactDetail::ContactFamily:
        return new QDeclarativeContactFamily(parent);
    case QDeclarativeContactDetail::ContactFavorite:
        return new QDeclarativeContactFavorite(parent);
    case QDeclarativeContactDetail::ContactGender:
        return new QDeclarativeContactGender(parent);
    case QDeclarativeContactDetail::ContactGeolocation:
        return new QDeclarativeContactGeoLocation(parent);
    case QDeclarativeContactDetail::ContactGlobalPresence:
        return new QDeclarativeContactGlobalPresence(parent);
    case QDeclarativeContactDetail::ContactGuid:
        return new QDeclarativeContactGuid(parent);
    case QDeclarativeContactDetail::ContactName:
        return new QDeclarativeContactName(parent);
    case QDeclarativeContactDetail::ContactNickName:
        return new QDeclarativeContactNickname(parent);
    case QDeclarativeContactDetail::ContactNote:
        return new QDeclarativeContactNote(parent);
    case QDeclarativeContactDetail::ContactOnlineAccount:
        return new QDeclarativeContactOnlineAccount(parent);
    case QDeclarativeContactDetail::ContactOrganization:
        return new QDeclarativeContactOrganization(parent);
    case QDeclarativeContactDetail::ContactPhoneNumber:
        return new QDeclarativeContactPhoneNumber(parent);
    case QDeclarativeContactDetail::ContactPresence:
        return new QDeclarativeContactPresence(parent);
    case QDeclarativeContactDetail::ContactRingtone:
        return new QDeclarativeContactRingtone(parent);
    case QDeclarativeContactDetail::ContactSyncTarget:
        return new QDeclarativeContactSyncTarget(parent);
    case QDeclarativeContactDetail::ContactTag:
        return new QDeclarativeContactTag(parent);
    case QDeclarativeContactDetail::ContactTimestamp:
        return new QDeclarativeContactTimestamp(parent);
    case QDeclarativeContactDetail::ContactUrl:
        return new QDeclarativeContactUrl(parent);
    case QDeclarativeContactDetail::ContactCustomizedDetail:
    default:
        break;
    }
    return new QDeclarativeContactDetail(parent);
}
