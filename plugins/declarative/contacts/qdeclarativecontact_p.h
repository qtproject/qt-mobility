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

#ifndef QDECLARATIVECONTACT_P_H
#define QDECLARATIVECONTACT_P_H
#include <qdeclarative.h>
#include <QDeclarativeListProperty>
#include "qcontact.h"
#include "qdeclarativecontactdetails_p.h"
QTM_USE_NAMESPACE;

class QDeclarativeContactMetaObject;
class QDeclarativeContactDetail;
class QDeclarativeContact : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QContactLocalId contactId READ contactId NOTIFY contactIdChanged);
    Q_PROPERTY (QString manager READ manager NOTIFY managerChanged);
    Q_PROPERTY (QDeclarativeListProperty<QDeclarativeContactDetail> details READ details NOTIFY detailsChanged);
    Q_PROPERTY (QDeclarativeContactAddress* address READ address NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactAnniversary* anniversary READ anniversary NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactAvatar* avatar READ avatar NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactBirthday*  birthday READ birthday NOTIFY detailsChanged)
    Q_PROPERTY (QString  displayLabel READ displayLabel NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactEmailAddress*  email READ email NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactFamily*  family READ family NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactFavorite*  favorite READ favorite NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactGender*  gender READ gender NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactGeoLocation*  geolocation READ geolocation NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactGlobalPresence*  globalPresence READ globalPresence NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactGuid*  guid READ guid NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactName*  name READ name NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactNickname*  nickname READ nickname  NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactNote*  note READ note NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactOnlineAccount*  onlineAccount READ onlineAccount NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactOrganization*  organization READ organization  NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactPhoneNumber*  phoneNumber READ phoneNumber NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactPresence*  presence READ presence NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactRingtone*  ringtone READ ringtone NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactSyncTarget*  syncTarget READ syncTarget NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactTag*  tag READ tag NOTIFY detailsChanged)
    Q_PROPERTY (QUrl thumbnail READ thumbnail WRITE setThumbnail NOTIFY detailsChanged)
    Q_PROPERTY (ContactType  type READ type  WRITE setType NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactUrl*  url READ url NOTIFY detailsChanged)
    Q_PROPERTY (QDeclarativeContactHobby*  hobby READ hobby NOTIFY detailsChanged)
    Q_PROPERTY (bool modified READ modified)
    Q_CLASSINFO("DefaultProperty", "details")
    Q_ENUMS(ContactType)
public:
    enum ContactType {
        Contact = 0,
        Group
    };
    explicit QDeclarativeContact(QObject *parent = 0);
    explicit QDeclarativeContact(const QContact& contact, const QMap<QString, QContactDetailDefinition>& defs, QObject *parent = 0);
    ~QDeclarativeContact();

    void setContact(const QContact& c);
    QContact contact() const;
    bool modified() const;

    void setType(ContactType type);
    ContactType type() const;

    void setDetailDefinitions(const QMap<QString, QContactDetailDefinition>& defs);
    QMap<QString, QContactDetailDefinition> detailDefinitions() const;

    QContactLocalId contactId() const;
    QString manager() const;
    QDeclarativeListProperty<QDeclarativeContactDetail> details();

    Q_INVOKABLE QDeclarativeContactDetail* detail(const QVariant& name);
    Q_INVOKABLE QDeclarativeListProperty<QDeclarativeContactDetail> details(const QVariant& name);

    Q_INVOKABLE bool removeDetail(QDeclarativeContactDetail* detail);
    QDeclarativeContactAddress* address();
    QDeclarativeContactAnniversary* anniversary();
    QDeclarativeContactAvatar* avatar();
    QDeclarativeContactBirthday*  birthday();
    QString  displayLabel();
    QDeclarativeContactEmailAddress*  email();
    QDeclarativeContactFamily*  family();
    QDeclarativeContactFavorite*  favorite();
    QDeclarativeContactGender*  gender();
    QDeclarativeContactGeoLocation*  geolocation();
    QDeclarativeContactGlobalPresence*  globalPresence();
    QDeclarativeContactGuid*  guid();
    QDeclarativeContactName*  name();
    QDeclarativeContactNickname*  nickname();
    QDeclarativeContactNote*  note();
    QDeclarativeContactOnlineAccount*  onlineAccount();
    QDeclarativeContactOrganization*  organization();
    QDeclarativeContactPhoneNumber*  phoneNumber();
    QDeclarativeContactPresence*  presence();
    QDeclarativeContactRingtone*  ringtone();
    QDeclarativeContactSyncTarget*  syncTarget();
    QDeclarativeContactTag*  tag();
    QUrl  thumbnail() const;
    void  setThumbnail(const QUrl& url);
    QDeclarativeContactUrl*  url();
    QDeclarativeContactHobby*  hobby();

public slots:
    void clearDetails();
    void save();
private slots:
    void setModified();
signals:
    void contactIdChanged();
    void managerChanged();
    void detailsChanged();
private:
    QDeclarativeContactMetaObject* d;
    friend class QDeclarativeContactMetaObject;
};

QML_DECLARE_TYPE(QDeclarativeContact)

#endif // QDECLARATIVECONTACT_P_H
