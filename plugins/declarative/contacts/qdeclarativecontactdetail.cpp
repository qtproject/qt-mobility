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

#include "qdeclarativecontactdetail_p.h"
#include "qcontactdetails.h"
#include <QDebug>
QDeclarativeContactDetail::QDeclarativeContactDetail(QObject* parent)
    :QObject(parent)
{
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
    return QDeclarativeContactDetail::Customized;
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
        emit fieldsChanged();

    return changed;
}

QString QDeclarativeContactDetail::definitionName(QDeclarativeContactDetail::ContactDetailType type)
{
    switch (type) {
    case QDeclarativeContactDetail::Address:
        return QContactAddress::DefinitionName;
    case QDeclarativeContactDetail::Anniversary:
        return QContactAnniversary::DefinitionName;
    case QDeclarativeContactDetail::Avatar:
        return QContactAvatar::DefinitionName;
    case QDeclarativeContactDetail::Birthday:
        return QContactBirthday::DefinitionName;
    case QDeclarativeContactDetail::DisplayLabel:
        return QContactDisplayLabel::DefinitionName;
    case QDeclarativeContactDetail::Email:
        return QContactEmailAddress::DefinitionName;
    case QDeclarativeContactDetail::Family:
        return QContactFamily::DefinitionName;
    case QDeclarativeContactDetail::Favorite:
        return QContactFavorite::DefinitionName;
    case QDeclarativeContactDetail::Gender:
        return QContactGender::DefinitionName;
    case QDeclarativeContactDetail::Geolocation:
        return QContactGeoLocation::DefinitionName;
    case QDeclarativeContactDetail::GlobalPresence:
        return QContactGlobalPresence::DefinitionName;
    case QDeclarativeContactDetail::Guid:
        return QContactGuid::DefinitionName;
    case QDeclarativeContactDetail::Name:
        return QContactName::DefinitionName;
    case QDeclarativeContactDetail::NickName:
        return QContactNickname::DefinitionName;
    case QDeclarativeContactDetail::Note:
        return QContactNote::DefinitionName;
    case QDeclarativeContactDetail::OnlineAccount:
        return QContactOnlineAccount::DefinitionName;
    case QDeclarativeContactDetail::Organization:
        return QContactOrganization::DefinitionName;
    case QDeclarativeContactDetail::PhoneNumber:
        return QContactPhoneNumber::DefinitionName;
    case QDeclarativeContactDetail::Presence:
        return QContactPresence::DefinitionName;
    case QDeclarativeContactDetail::Ringtone:
        return QContactRingtone::DefinitionName;
    case QDeclarativeContactDetail::SyncTarget:
        return QContactSyncTarget::DefinitionName;
    case QDeclarativeContactDetail::Tag:
        return QContactTag::DefinitionName;
    case QDeclarativeContactDetail::Thumbnail:
        return QContactThumbnail::DefinitionName;
    case QDeclarativeContactDetail::Timestamp:
        return QContactTimestamp::DefinitionName;
    case QDeclarativeContactDetail::Type:
        return QContactType::DefinitionName;
    case QDeclarativeContactDetail::Url:
        return QContactUrl::DefinitionName;
    case QDeclarativeContactDetail::Customized:
    default:
        break;
    }
    return "";
}

QDeclarativeContactDetail::ContactDetailType QDeclarativeContactDetail::detailType(const QString& definitionName)
{
    if (definitionName == QContactAddress::DefinitionName)
        return QDeclarativeContactDetail::Address;
    if (definitionName == QContactAnniversary::DefinitionName)
        return QDeclarativeContactDetail::Anniversary;
    if (definitionName == QContactAvatar::DefinitionName)
        return QDeclarativeContactDetail::Avatar;
    if (definitionName == QContactBirthday::DefinitionName)
        return QDeclarativeContactDetail::Birthday;
    if (definitionName == QContactDisplayLabel::DefinitionName)
        return QDeclarativeContactDetail::DisplayLabel;
    if (definitionName == QContactEmailAddress::DefinitionName)
        return QDeclarativeContactDetail::Email;
    if (definitionName == QContactFamily::DefinitionName)
        return QDeclarativeContactDetail::Family;
    if (definitionName == QContactFavorite::DefinitionName)
        return QDeclarativeContactDetail::Favorite;
    if (definitionName == QContactGender::DefinitionName)
        return QDeclarativeContactDetail::Gender;
    if (definitionName == QContactGeoLocation::DefinitionName)
        return QDeclarativeContactDetail::Geolocation;
    if (definitionName == QContactGlobalPresence::DefinitionName)
        return QDeclarativeContactDetail::GlobalPresence;
    if (definitionName == QContactGuid::DefinitionName)
        return QDeclarativeContactDetail::Guid;
    if (definitionName == QContactName::DefinitionName)
        return QDeclarativeContactDetail::Name;
    if (definitionName == QContactNickname::DefinitionName)
        return QDeclarativeContactDetail::NickName;
    if (definitionName == QContactNote::DefinitionName)
        return QDeclarativeContactDetail::Note;
    if (definitionName == QContactOnlineAccount::DefinitionName)
        return QDeclarativeContactDetail::OnlineAccount;
    if (definitionName == QContactOrganization::DefinitionName)
        return QDeclarativeContactDetail::Organization;
    if (definitionName == QContactPhoneNumber::DefinitionName)
        return QDeclarativeContactDetail::PhoneNumber;
    if (definitionName == QContactPresence::DefinitionName)
        return QDeclarativeContactDetail::Presence;
    if (definitionName == QContactRingtone::DefinitionName)
        return QDeclarativeContactDetail::Ringtone;
    if (definitionName == QContactSyncTarget::DefinitionName)
        return QDeclarativeContactDetail::SyncTarget;
    if (definitionName == QContactTag::DefinitionName)
        return QDeclarativeContactDetail::Tag;
    if (definitionName == QContactThumbnail::DefinitionName)
        return QDeclarativeContactDetail::Thumbnail;
    if (definitionName == QContactTimestamp::DefinitionName)
        return QDeclarativeContactDetail::Timestamp;
    if (definitionName == QContactType::DefinitionName)
        return QDeclarativeContactDetail::Type;
    if (definitionName == QContactUrl::DefinitionName)
        return QDeclarativeContactDetail::Url;

    return QDeclarativeContactDetail::Customized;
}

