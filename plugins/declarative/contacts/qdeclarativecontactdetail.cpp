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

#include "qdeclarativecontactdetails_p.h"
#include "qdeclarativecontact_p.h"
#include "qcontactdetails.h"
#include <QDebug>
QDeclarativeContactDetail::QDeclarativeContactDetail(QObject* parent)
    :QObject(parent)
{
    QDeclarativeContact* c = qobject_cast<QDeclarativeContact*>(parent);
    if (c)
        connect(this, SIGNAL(valueChanged()), c, SLOT(detailsChanged()));
}

QContactDetail& QDeclarativeContactDetail::detail()
{
    return m_detail;
}

const QContactDetail& QDeclarativeContactDetail::detail() const
{
    return m_detail;
}

void QDeclarativeContactDetail::setDetail(const QContactDetail& detail)
{
    m_detail = detail;
}

QString QDeclarativeContactDetail::definitionName() const
{
    return m_detail.definitionName();
}

QStringList QDeclarativeContactDetail::contexts() const
{
    return m_detail.contexts();
}
void QDeclarativeContactDetail::setContexts(const QStringList& contexts)
{
    m_detail.setContexts(contexts);
}

bool QDeclarativeContactDetail::readOnly() const
{
    return m_detail.accessConstraints().testFlag(QContactDetail::ReadOnly);
}
bool QDeclarativeContactDetail::removable() const
{
    return !m_detail.accessConstraints().testFlag(QContactDetail::Irremovable);
}

QString QDeclarativeContactDetail::detailUri() const
{
    return m_detail.detailUri();
}
void QDeclarativeContactDetail::setDetailUri(const QString& detailUri)
{
    m_detail.setDetailUri(detailUri);
}

QStringList QDeclarativeContactDetail::linkedDetailUris() const
{
    return m_detail.linkedDetailUris();
}
void QDeclarativeContactDetail::setLinkedDetailUris(const QStringList& linkedDetailUris)
{
    m_detail.setLinkedDetailUris(linkedDetailUris);
}

QDeclarativeContactDetail::ContactDetailType QDeclarativeContactDetail::detailType() const
{
    return QDeclarativeContactDetail::ContactCustomizedDetail;
}

QStringList QDeclarativeContactDetail::fieldNames() const
{
    return m_detail.variantValues().keys();
}

QVariant QDeclarativeContactDetail::value(const QString& key) const
{
    return m_detail.variantValue(key);
}

bool QDeclarativeContactDetail::setValue(const QString& key, const QVariant& v)
{
    bool changed = false;

    if (value(key) != v)
         changed = m_detail.setValue(key, v);

    if (changed)
        emit valueChanged();

    return changed;
}

QString QDeclarativeContactDetail::definitionName(QDeclarativeContactDetail::ContactDetailType type)
{
    switch (type) {
    case QDeclarativeContactDetail::ContactAddress:
        return QContactAddress::DefinitionName;
    case QDeclarativeContactDetail::ContactAnniversary:
        return QContactAnniversary::DefinitionName;
    case QDeclarativeContactDetail::ContactAvatar:
        return QContactAvatar::DefinitionName;
    case QDeclarativeContactDetail::ContactBirthday:
        return QContactBirthday::DefinitionName;
    case QDeclarativeContactDetail::ContactDisplayLabel:
        return QContactDisplayLabel::DefinitionName;
    case QDeclarativeContactDetail::ContactEmail:
        return QContactEmailAddress::DefinitionName;
    case QDeclarativeContactDetail::ContactFamily:
        return QContactFamily::DefinitionName;
    case QDeclarativeContactDetail::ContactFavorite:
        return QContactFavorite::DefinitionName;
    case QDeclarativeContactDetail::ContactGender:
        return QContactGender::DefinitionName;
    case QDeclarativeContactDetail::ContactGeolocation:
        return QContactGeoLocation::DefinitionName;
    case QDeclarativeContactDetail::ContactGlobalPresence:
        return QContactGlobalPresence::DefinitionName;
    case QDeclarativeContactDetail::ContactGuid:
        return QContactGuid::DefinitionName;
    case QDeclarativeContactDetail::ContactName:
        return QContactName::DefinitionName;
    case QDeclarativeContactDetail::ContactNickName:
        return QContactNickname::DefinitionName;
    case QDeclarativeContactDetail::ContactNote:
        return QContactNote::DefinitionName;
    case QDeclarativeContactDetail::ContactOnlineAccount:
        return QContactOnlineAccount::DefinitionName;
    case QDeclarativeContactDetail::ContactOrganization:
        return QContactOrganization::DefinitionName;
    case QDeclarativeContactDetail::ContactPhoneNumber:
        return QContactPhoneNumber::DefinitionName;
    case QDeclarativeContactDetail::ContactPresence:
        return QContactPresence::DefinitionName;
    case QDeclarativeContactDetail::ContactRingtone:
        return QContactRingtone::DefinitionName;
    case QDeclarativeContactDetail::ContactSyncTarget:
        return QContactSyncTarget::DefinitionName;
    case QDeclarativeContactDetail::ContactTag:
        return QContactTag::DefinitionName;
    case QDeclarativeContactDetail::ContactTimestamp:
        return QContactTimestamp::DefinitionName;
    case QDeclarativeContactDetail::ContactUrl:
        return QContactUrl::DefinitionName;
    case QDeclarativeContactDetail::ContactCustomizedDetail:
    default:
        break;
    }
    return "";
}

QDeclarativeContactDetail::ContactDetailType QDeclarativeContactDetail::detailType(const QString& definitionName)
{
    if (definitionName == QContactAddress::DefinitionName)
        return QDeclarativeContactDetail::ContactAddress;
    if (definitionName == QContactAnniversary::DefinitionName)
        return QDeclarativeContactDetail::ContactAnniversary;
    if (definitionName == QContactAvatar::DefinitionName)
        return QDeclarativeContactDetail::ContactAvatar;
    if (definitionName == QContactBirthday::DefinitionName)
        return QDeclarativeContactDetail::ContactBirthday;
    if (definitionName == QContactDisplayLabel::DefinitionName)
        return QDeclarativeContactDetail::ContactDisplayLabel;
    if (definitionName == QContactEmailAddress::DefinitionName)
        return QDeclarativeContactDetail::ContactEmail;
    if (definitionName == QContactFamily::DefinitionName)
        return QDeclarativeContactDetail::ContactFamily;
    if (definitionName == QContactFavorite::DefinitionName)
        return QDeclarativeContactDetail::ContactFavorite;
    if (definitionName == QContactGender::DefinitionName)
        return QDeclarativeContactDetail::ContactGender;
    if (definitionName == QContactGeoLocation::DefinitionName)
        return QDeclarativeContactDetail::ContactGeolocation;
    if (definitionName == QContactGlobalPresence::DefinitionName)
        return QDeclarativeContactDetail::ContactGlobalPresence;
    if (definitionName == QContactGuid::DefinitionName)
        return QDeclarativeContactDetail::ContactGuid;
    if (definitionName == QContactName::DefinitionName)
        return QDeclarativeContactDetail::ContactName;
    if (definitionName == QContactNickname::DefinitionName)
        return QDeclarativeContactDetail::ContactNickName;
    if (definitionName == QContactNote::DefinitionName)
        return QDeclarativeContactDetail::ContactNote;
    if (definitionName == QContactOnlineAccount::DefinitionName)
        return QDeclarativeContactDetail::ContactOnlineAccount;
    if (definitionName == QContactOrganization::DefinitionName)
        return QDeclarativeContactDetail::ContactOrganization;
    if (definitionName == QContactPhoneNumber::DefinitionName)
        return QDeclarativeContactDetail::ContactPhoneNumber;
    if (definitionName == QContactPresence::DefinitionName)
        return QDeclarativeContactDetail::ContactPresence;
    if (definitionName == QContactRingtone::DefinitionName)
        return QDeclarativeContactDetail::ContactRingtone;
    if (definitionName == QContactSyncTarget::DefinitionName)
        return QDeclarativeContactDetail::ContactSyncTarget;
    if (definitionName == QContactTag::DefinitionName)
        return QDeclarativeContactDetail::ContactTag;
    if (definitionName == QContactTimestamp::DefinitionName)
        return QDeclarativeContactDetail::ContactTimestamp;
    if (definitionName == QContactUrl::DefinitionName)
        return QDeclarativeContactDetail::ContactUrl;

    return QDeclarativeContactDetail::ContactCustomizedDetail;
}

QString QDeclarativeContactDetail::fieldName(ContactDetailType detailType, int fieldType)
{
    switch (detailType) {
    case QDeclarativeContactDetail::ContactAddress:
        return QDeclarativeContactAddress::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactAnniversary:
        return QDeclarativeContactAnniversary::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactAvatar:
        return QDeclarativeContactAvatar::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactBirthday:
        return QDeclarativeContactBirthday::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactDisplayLabel:
        return QDeclarativeContactDisplayLabel::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactEmail:
        return QDeclarativeContactEmailAddress::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactFamily:
        return QDeclarativeContactFamily::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactFavorite:
        return QDeclarativeContactFavorite::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactGender:
        return QDeclarativeContactGender::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactGeolocation:
        return QDeclarativeContactGeoLocation::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactGlobalPresence:
        return QDeclarativeContactGlobalPresence::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactGuid:
        return QDeclarativeContactGuid::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactName:
        return QDeclarativeContactName::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactNickName:
        return QDeclarativeContactNickname::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactNote:
        return QDeclarativeContactNote::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactOnlineAccount:
        return QDeclarativeContactOnlineAccount::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactOrganization:
        return QDeclarativeContactOrganization::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactPhoneNumber:
        return QDeclarativeContactPhoneNumber::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactPresence:
        return QDeclarativeContactPresence::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactRingtone:
        return QDeclarativeContactRingtone::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactSyncTarget:
        return QDeclarativeContactSyncTarget::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactTag:
        return QDeclarativeContactTag::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactTimestamp:
        return QDeclarativeContactTimestamp::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactUrl:
        return QDeclarativeContactUrl::fieldNameFromFieldType(fieldType);
    case QDeclarativeContactDetail::ContactCustomizedDetail:
    default:
        break;
    }
    return "";
}

