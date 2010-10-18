#include <QString>
#include <QDebug>

#include "qdeclarativecontact_p.h"
#include "qcontactdetails.h"
#include "qdeclarativecontactdetail_p.h"
#include "qdeclarativecontactmetaobject_p.h"

QTM_USE_NAMESPACE
static ContactDetailNameMap qt_contactDetailNameMap[] = {
    {QDeclarativeContactDetail::Address,        "address",        QContactAddress::DefinitionName.latin1(),  false},
    {QDeclarativeContactDetail::Address,        "addresses",      QContactAddress::DefinitionName.latin1(),  true},
    {QDeclarativeContactDetail::Anniversary,    "anniversary",    QContactAnniversary::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Anniversary,    "anniversaries",  QContactAnniversary::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Avatar,         "avatar",         QContactAvatar::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Avatar,         "avatars",        QContactAvatar::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Birthday,       "birthday",       QContactBirthday::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Birthday,       "birthdays",      QContactBirthday::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::DisplayLabel,   "displayLabel",   QContactDisplayLabel::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::DisplayLabel,   "displayLabels",  QContactDisplayLabel::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Email,          "email",          QContactEmailAddress::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Email,          "emails",         QContactEmailAddress::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Family,         "family",         QContactFamily::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Family,         "families",       QContactFamily::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Favorite,       "favorite",       QContactFavorite::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Favorite,       "favorites",      QContactFavorite::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Gender,         "gender",         QContactGender::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Gender,         "genders",        QContactGender::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Geolocation,    "location",       QContactGeoLocation::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Geolocation,    "locations",      QContactGeoLocation::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::GlobalPresence, "globalPresence", QContactGlobalPresence::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::GlobalPresence, "globalPresences",QContactGlobalPresence::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Guid,           "guid",           QContactGuid::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Guid,           "guids",          QContactGuid::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Name,           "name",           QContactName::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Name,           "names",          QContactName::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::NickName,       "nickname",       QContactNickname::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::NickName,       "nicknames",      QContactNickname::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Note,           "note",           QContactNote::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Note,           "notes",          QContactNote::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::OnlineAccount,  "onlineAccount",  QContactOnlineAccount::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::OnlineAccount,  "onlineAccounts", QContactOnlineAccount::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Organization,   "organization",   QContactOrganization::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Organization,   "organizations",  QContactOrganization::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::PhoneNumber,    "phoneNumber",    QContactPhoneNumber::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::PhoneNumber,    "phoneNumbers",   QContactPhoneNumber::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Presence,       "presence",       QContactPresence::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Presence,       "presences",      QContactPresence::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Ringtone,       "ringtone",       QContactRingtone::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Ringtone,       "ringtones",      QContactRingtone::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::SyncTarget,     "syncTarget",     QContactSyncTarget::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::SyncTarget,     "syncTargets",    QContactSyncTarget::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Tag,            "tag",            QContactTag::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Tag,            "tags",           QContactTag::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Thumbnail,      "thumbnail",      QContactThumbnail::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Thumbnail,      "thumbnails",     QContactThumbnail::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Timestamp,      "timestamp",      QContactTimestamp::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Timestamp,      "timestamps",     QContactTimestamp::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Type,           "type",           QContactType::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Type,           "types",          QContactType::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Url,            "url",            QContactUrl::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Url,            "urls",           QContactUrl::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Customized,     "",               "",                false}
};

QDeclarativeContactMetaObject::QDeclarativeContactMetaObject(QObject* obj, const QContact& contact)
    :QDeclarativeOpenMetaObject(obj)
{
    setContact(contact);
}

QDeclarativeContactMetaObject::~QDeclarativeContactMetaObject()
{

}

void QDeclarativeContactMetaObject::getValue(int propId, void **a)
{
    qDebug() << "value for propId:" << propId;
    ContactDetailNameMap* detailMetaData = m_properties.value(propId);
    if (detailMetaData) {
        if (detailMetaData->group) {
            qDebug() << "name:" << detailMetaData->name;
            *reinterpret_cast< QDeclarativeListProperty<QDeclarativeContactDetail>* >(a[0]) =
                    QDeclarativeListProperty<QDeclarativeContactDetail>(object(), detailMetaData, detail_append, detail_count, detail_at, detail_clear);

        } else {
            foreach(QDeclarativeContactDetail* cd, m_details) {
                if (cd->detail().definitionName() == detailMetaData->definitionName) {
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
                if (cd->detail().definitionName() == detailMetaData->definitionName) {
                    delete cd;
                    cd = detail;
                }
            }
        }
    }
}

int QDeclarativeContactMetaObject::createProperty(const char * name,  const char *)
{

    const int detailCount = sizeof(qt_contactDetailNameMap)/sizeof(ContactDetailNameMap);
    ContactDetailNameMap* detailMetaData = 0;

    for (int i = 0; i < detailCount; i++) {
        if (QString::fromLocal8Bit(qt_contactDetailNameMap[i].name) == QString::fromLocal8Bit(name)) {
            detailMetaData = &qt_contactDetailNameMap[i];
            break;
        }
    }

    if (detailMetaData) {
        int propId = -1;
        if (detailMetaData->group)
            propId = QDeclarativeOpenMetaObject::createProperty(name, "QDeclarativeListProperty<QDeclarativeContactDetail>");
        else
            propId = QDeclarativeOpenMetaObject::createProperty(name, "QVariant");
        m_properties.insert(propId, detailMetaData);
        qDebug() << "createProperty:" << name << " propId:" << propId;
        return propId;
    }
    return -1;
}


QVariant QDeclarativeContactMetaObject::detail(const QString& name)
{
    int propId = indexOfProperty(name.toLatin1());

    if (propId > 0)
        return property(propId).read(object());

    //Assume it's a detail definition name
    foreach(QDeclarativeContactDetail* cd, m_details) {
        if (cd->detail().definitionName() == name) {
            return QVariant::fromValue(cd);
        }
    }
    return QVariant();
}

QVariant QDeclarativeContactMetaObject::details(const QString& name)
{
    if (name.isEmpty()) {
        //return all
        return QVariant::fromValue(QDeclarativeListProperty<QDeclarativeContactDetail>(object(), m_details));
    } else {
        int propId = indexOfProperty(name.toLatin1());
        if (propId > 0)
            return property(propId).read(object());

        //Assume it's a detail definition name
        //TODO::customized details
        for (QHash<int, ContactDetailNameMap*>::ConstIterator iter = m_properties.constBegin(); iter != m_properties.constEnd(); iter++) {
            if (iter.value()->group && iter.value()->definitionName == name)
                return property(iter.key()).read(object());
        }
    }
    return QVariant();
}

void QDeclarativeContactMetaObject::setContact(const QContact& contact)
{
    m_contact = contact;
    QList<QContactDetail> details = m_contact.details();
    m_details.clear();
    foreach (const QContactDetail& detail, details) {
      QDeclarativeContactDetail* cd = new QDeclarativeContactDetail(object());

      cd->connect(cd, SIGNAL(fieldsChanged()), object(), SIGNAL(detailsChanged()));

      cd->setDetail(detail);
      m_details.append(cd);
    }
}

QContact QDeclarativeContactMetaObject::contact()
{
    m_contact.clearDetails();
    foreach (const QDeclarativeContactDetail* cd, m_details) {
       QContactDetail detail = cd->detail();
       m_contact.saveDetail(&detail);
    }
    return m_contact;
}

int QDeclarativeContactMetaObject::localId() const
{
    return m_contact.localId();
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
