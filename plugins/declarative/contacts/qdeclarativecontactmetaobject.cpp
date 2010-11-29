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
    {QDeclarativeContactDetail::Timestamp,      "timestamp",      QContactTimestamp::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Timestamp,      "timestamps",     QContactTimestamp::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Url,            "url",            QContactUrl::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Url,            "urls",           QContactUrl::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Hobby,          "hobby",          QContactHobby::DefinitionName.latin1(), false},
    {QDeclarativeContactDetail::Hobby,          "hobbies",        QContactHobby::DefinitionName.latin1(), true},
    {QDeclarativeContactDetail::Customized,     "",               "",                false}
};

/*!
    \class QDeclarativeContactMetaObject
    \internal
    \brief open contact meta object for accessing dynamic contact detail properties in qml
*/
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
    foreach (const QDeclarativeContactDetail* cd, m_details) {
       QContactDetail detail = cd->detail();
       if (!detail.isEmpty())
          m_contact.saveDetail(&detail);
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
    case QDeclarativeContactDetail::Address:
        return new QDeclarativeContactAddress(parent);
    case QDeclarativeContactDetail::Anniversary:
        return new QDeclarativeContactAnniversary(parent);
    case QDeclarativeContactDetail::Avatar:
        return new QDeclarativeContactAvatar(parent);
    case QDeclarativeContactDetail::Birthday:
        return new QDeclarativeContactBirthday(parent);
    case QDeclarativeContactDetail::DisplayLabel:
        return new QDeclarativeContactDisplayLabel(parent);
    case QDeclarativeContactDetail::Email:
        return new QDeclarativeContactEmailAddress(parent);
    case QDeclarativeContactDetail::Family:
        return new QDeclarativeContactFamily(parent);
    case QDeclarativeContactDetail::Favorite:
        return new QDeclarativeContactFavorite(parent);
    case QDeclarativeContactDetail::Gender:
        return new QDeclarativeContactGender(parent);
    case QDeclarativeContactDetail::Geolocation:
        return new QDeclarativeContactGeoLocation(parent);
    case QDeclarativeContactDetail::GlobalPresence:
        return new QDeclarativeContactGlobalPresence(parent);
    case QDeclarativeContactDetail::Guid:
        return new QDeclarativeContactGuid(parent);
    case QDeclarativeContactDetail::Name:
        return new QDeclarativeContactName(parent);
    case QDeclarativeContactDetail::NickName:
        return new QDeclarativeContactNickname(parent);
    case QDeclarativeContactDetail::Note:
        return new QDeclarativeContactNote(parent);
    case QDeclarativeContactDetail::OnlineAccount:
        return new QDeclarativeContactOnlineAccount(parent);
    case QDeclarativeContactDetail::Organization:
        return new QDeclarativeContactOrganization(parent);
    case QDeclarativeContactDetail::PhoneNumber:
        return new QDeclarativeContactPhoneNumber(parent);
    case QDeclarativeContactDetail::Presence:
        return new QDeclarativeContactPresence(parent);
    case QDeclarativeContactDetail::Ringtone:
        return new QDeclarativeContactRingtone(parent);
    case QDeclarativeContactDetail::SyncTarget:
        return new QDeclarativeContactSyncTarget(parent);
    case QDeclarativeContactDetail::Tag:
        return new QDeclarativeContactTag(parent);
    case QDeclarativeContactDetail::Timestamp:
        return new QDeclarativeContactTimestamp(parent);
    case QDeclarativeContactDetail::Url:
        return new QDeclarativeContactUrl(parent);
    case QDeclarativeContactDetail::Customized:
    default:
        break;
    }
    return new QDeclarativeContactDetail(parent);
}
