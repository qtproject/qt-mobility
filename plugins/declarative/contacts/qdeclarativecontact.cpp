/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qcontactdetails.h"
#include "qcontactmanager.h"
#include "qdeclarativecontact_p.h"
#include "qdeclarativecontactdetail_p.h"
#include "qdeclarativecontactmetaobject_p.h"
#include "qdeclarativecontactmodel_p.h"
#include <QImage>
#include <QUrl>
#include <QDeclarativeListProperty>

/*!
    \qmlclass Contact

    \brief The Contact element represents an addressbook contact, A Contact
     object has a collection of details (like a name, phone numbers and
     email addresses).  Each detail (which can have multiple fields) is stored
     in an appropriate subclass of ContactDetail, and the Contact allows
     retrieving these details in various ways.

    \inherits QObject

    \ingroup qml-contact

    The Contact element is part of the \bold{QtMobility.contacts 1.1} module.
*/

QDeclarativeContact::QDeclarativeContact(QObject *parent)
    :QObject(parent),
    d(new QDeclarativeContactMetaObject(this, QContact()))
{
    connect(this, SIGNAL(detailsChanged()), SLOT(setModified()));
}


QDeclarativeContact::QDeclarativeContact(const QContact& contact, const QMap<QString, QContactDetailDefinition>& defs, QObject *parent)
    :QObject(parent),
    d(new QDeclarativeContactMetaObject(this, contact))
{
    setDetailDefinitions(defs);
    connect(this, SIGNAL(detailsChanged()), SLOT(setModified()));
}

QDeclarativeContact::~QDeclarativeContact()
{
    delete d;
}

void QDeclarativeContact::setDetailDefinitions(const QMap<QString, QContactDetailDefinition>& defs)
{
    d->m_defs = defs;
}

QMap<QString, QContactDetailDefinition> QDeclarativeContact::detailDefinitions() const
{
    return d->m_defs;
}

void QDeclarativeContact::setContact(const QContact& contact)
{
   d->setContact(contact);
   d->m_modified = false;
}

QContact QDeclarativeContact::contact() const
{
    return d->contact();
}

/*!
    \qmlproperty bool Contact::modified

    This property holds the dirty flag of the Contact object.
    If the Contact has been changed, returns true, otherwise returns false.

*/
bool QDeclarativeContact::modified() const
{
     return d->m_modified;
}
void QDeclarativeContact::setModified()
{
     d->m_modified = true;
}

/*!
    \qmlproperty type Contact::type

    This property holds type of the Contact object.

    \sa Contact::ContactType
*/
void QDeclarativeContact::setType(QDeclarativeContact::ContactType newType)
{
    if (newType != type()) {
        if (newType == QDeclarativeContact::TypeContact)
            d->m_contact.setType(QContactType::TypeContact);
        else if (newType == QDeclarativeContact::TypeGroup)
            d->m_contact.setType(QContactType::TypeGroup);
        emit detailsChanged();
    }
}

QDeclarativeContact::ContactType QDeclarativeContact::type() const
{
    if (d->m_contact.type() == QContactType::TypeGroup)
        return QDeclarativeContact::TypeGroup;
    return QDeclarativeContact::TypeContact;
}

bool QDeclarativeContact::removeDetail(QDeclarativeContactDetail* detail)
{
    if (detail->removable()) {
        d->m_details.removeAll(detail);
        emit detailsChanged();
    }
    return false;

}

QDeclarativeListProperty<QDeclarativeContactDetail> QDeclarativeContact::details()
{
    return d->details(QString()).value< QDeclarativeListProperty<QDeclarativeContactDetail> >();
}

/*!
    \qmlproperty uint Contact::contactId

    This property holds the id of the Contact object, it's a read only property.
*/
QContactLocalId QDeclarativeContact::contactId() const
{
    return d->localId();
}

/*!
    \qmlproperty QString Contact::manager

    This property holds the manager name which the Contact object comes from.
*/
QString QDeclarativeContact::manager() const
{
    return d->contactId().managerUri();
}


QDeclarativeContactDetail* QDeclarativeContact::detail(const QVariant& name)
{
    if (name.type() == QVariant::Int)
        return d->detail(static_cast<QDeclarativeContactDetail::ContactDetailType>(name.value<int>())).value<QDeclarativeContactDetail*>();
    else
        return d->detail(name.toString()).value<QDeclarativeContactDetail*>();
}

QDeclarativeListProperty<QDeclarativeContactDetail> QDeclarativeContact::details(const QVariant& name)
{
    if (name.type() == QVariant::Int)
        return d->details(static_cast<QDeclarativeContactDetail::ContactDetailType>(name.value<int>())).value< QDeclarativeListProperty<QDeclarativeContactDetail> >();
    else
        return d->details(name.toString()).value< QDeclarativeListProperty<QDeclarativeContactDetail> >();;
}

void QDeclarativeContact::clearDetails()
{
    d->m_details.clear();
    emit detailsChanged();
}

void QDeclarativeContact::save()
{
    if (modified()) {
        QDeclarativeContactModel* model = qobject_cast<QDeclarativeContactModel*>(parent());
        if (model) {
            model->saveContact(this);
        }
    }
}

/*!
    \qmlproperty Address Contact::address

    This property holds the address detail of the Contact object.
*/
QDeclarativeContactAddress* QDeclarativeContact::address()
{
    return static_cast<QDeclarativeContactAddress*>(d->detail(QDeclarativeContactDetail::Address).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactAnniversary* QDeclarativeContact::anniversary()
{
    return static_cast<QDeclarativeContactAnniversary*>(d->detail(QDeclarativeContactDetail::Anniversary).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactAvatar* QDeclarativeContact::avatar()
{
    return static_cast<QDeclarativeContactAvatar*>(d->detail(QDeclarativeContactDetail::Avatar).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactBirthday*  QDeclarativeContact::birthday()
{
    return static_cast<QDeclarativeContactBirthday*>(d->detail(QDeclarativeContactDetail::Birthday).value<QDeclarativeContactDetail*>());
}
QString QDeclarativeContact::displayLabel()
{
    return d->m_contact.displayLabel();
}
QDeclarativeContactEmailAddress*  QDeclarativeContact::email()
{
    return static_cast<QDeclarativeContactEmailAddress*>(d->detail(QDeclarativeContactDetail::Email).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactFamily*  QDeclarativeContact::family()
{
    return static_cast<QDeclarativeContactFamily*>(d->detail(QDeclarativeContactDetail::Family).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactFavorite*  QDeclarativeContact::favorite()
{
    return static_cast<QDeclarativeContactFavorite*>(d->detail(QDeclarativeContactDetail::Favorite).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactGender*  QDeclarativeContact::gender()
{
    return static_cast<QDeclarativeContactGender*>(d->detail(QDeclarativeContactDetail::Gender).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactGeoLocation*  QDeclarativeContact::geolocation()
{
    return static_cast<QDeclarativeContactGeoLocation*>(d->detail(QDeclarativeContactDetail::Geolocation).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactGlobalPresence*  QDeclarativeContact::globalPresence()
{
    return static_cast<QDeclarativeContactGlobalPresence*>(d->detail(QDeclarativeContactDetail::GlobalPresence).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactGuid*  QDeclarativeContact::guid()
{
    return static_cast<QDeclarativeContactGuid*>(d->detail(QDeclarativeContactDetail::Guid).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactName*  QDeclarativeContact::name()
{
    return static_cast<QDeclarativeContactName*>(d->detail(QDeclarativeContactDetail::Name).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactNickname*  QDeclarativeContact::nickname()
{
    return static_cast<QDeclarativeContactNickname*>(d->detail(QDeclarativeContactDetail::NickName).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactNote*  QDeclarativeContact::note()
{
    return static_cast<QDeclarativeContactNote*>(d->detail(QDeclarativeContactDetail::Note).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactOnlineAccount*  QDeclarativeContact::onlineAccount()
{
    return static_cast<QDeclarativeContactOnlineAccount*>(d->detail(QDeclarativeContactDetail::OnlineAccount).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactOrganization*  QDeclarativeContact::organization()
{
    return static_cast<QDeclarativeContactOrganization*>(d->detail(QDeclarativeContactDetail::Organization).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactPhoneNumber*  QDeclarativeContact::phoneNumber()
{
    return static_cast<QDeclarativeContactPhoneNumber*>(d->detail(QDeclarativeContactDetail::PhoneNumber).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactPresence*  QDeclarativeContact::presence()
{
    return static_cast<QDeclarativeContactPresence*>(d->detail(QDeclarativeContactDetail::Presence).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactRingtone*  QDeclarativeContact::ringtone()
{
    return static_cast<QDeclarativeContactRingtone*>(d->detail(QDeclarativeContactDetail::Ringtone).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactSyncTarget*  QDeclarativeContact::syncTarget()
{
    return static_cast<QDeclarativeContactSyncTarget*>(d->detail(QDeclarativeContactDetail::SyncTarget).value<QDeclarativeContactDetail*>());
}
QDeclarativeContactTag*  QDeclarativeContact::tag()
{
    return static_cast<QDeclarativeContactTag*>(d->detail(QDeclarativeContactDetail::Tag).value<QDeclarativeContactDetail*>());
}
QUrl QDeclarativeContact::thumbnail() const
{
    //Just let the imager provider deal with it
    return QUrl(QString("image://thumbnail/%1.%2").arg(manager()).arg(contactId()).toLatin1());
}

//Only support local file
void QDeclarativeContact::setThumbnail(const QUrl& url)
{
    QImage image(100, 50, QImage::Format_RGB32);
    image.load(url.toLocalFile());
    QContactThumbnail detail;
    detail.setThumbnail(image);
    d->m_contact.saveDetail(&detail);
    emit detailsChanged();
}

QDeclarativeContactUrl*  QDeclarativeContact::url()
{
    return static_cast<QDeclarativeContactUrl*>(d->detail(QDeclarativeContactDetail::Url).value<QDeclarativeContactDetail*>());
}


QDeclarativeContactHobby*  QDeclarativeContact::hobby()
{
    return static_cast<QDeclarativeContactHobby*>(d->detail(QDeclarativeContactDetail::Hobby).value<QDeclarativeContactDetail*>());
}
