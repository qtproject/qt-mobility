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
