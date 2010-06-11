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

#include <qcontactdetails.h>
#include "qmlcontactdetail.h"


static QString normalizePropertyName(const QString& name)
{
   if (!name.isEmpty())
     return name.mid(1).prepend(name[0].toLower());
   return QString();
}

static QString defaultPropertyNameForContactDetail(const QContactDetail& detail)
{
    if (detail.definitionName() == QContactAddress::DefinitionName)
        return QContactAddress::FieldLocality;
    if (detail.definitionName() == QContactAnniversary::DefinitionName)
        return QContactAnniversary::FieldOriginalDate;
    if (detail.definitionName() == QContactAvatar::DefinitionName)
        return QContactAvatar::FieldImageUrl;
    if (detail.definitionName() == QContactBirthday::DefinitionName)
        return QContactBirthday::FieldBirthday;
    if (detail.definitionName() == QContactDisplayLabel::DefinitionName)
        return QContactDisplayLabel::FieldLabel;
    if (detail.definitionName() == QContactEmailAddress::DefinitionName)
        return QContactEmailAddress::FieldEmailAddress;
    if (detail.definitionName() == QContactFamily::DefinitionName)
        return QContactFamily::FieldSpouse;
    if (detail.definitionName() == QContactGender::DefinitionName)
        return QContactGender::FieldGender;
    if (detail.definitionName() == QContactGeoLocation::DefinitionName)
        return QContactGeoLocation::FieldLabel;
    if (detail.definitionName() == QContactGlobalPresence::DefinitionName)
        return QContactGlobalPresence::FieldNickname;
    if (detail.definitionName() == QContactGuid::DefinitionName)
        return QContactGuid::FieldGuid;
    if (detail.definitionName() == QContactName::DefinitionName)
        return QContactName::FieldFirstName;
    if (detail.definitionName() == QContactNickname::DefinitionName)
        return QContactNickname::FieldNickname;
    if (detail.definitionName() == QContactNote::DefinitionName)
        return QContactNote::FieldNote;
    if (detail.definitionName() == QContactOnlineAccount::DefinitionName)
        return QContactOnlineAccount::FieldAccountUri;
    if (detail.definitionName() == QContactOrganization::DefinitionName)
        return QContactOrganization::FieldName;
    if (detail.definitionName() == QContactPhoneNumber::DefinitionName)
        return QContactPhoneNumber::FieldNumber;
    if (detail.definitionName() == QContactPresence::DefinitionName)
        return QContactPresence::FieldNickname;
    if (detail.definitionName() == QContactRingtone::DefinitionName)
        return QContactRingtone::FieldAudioRingtoneUrl;
    if (detail.definitionName() == QContactSyncTarget::DefinitionName)
        return QContactSyncTarget::FieldSyncTarget;
    if (detail.definitionName() == QContactTag::DefinitionName)
        return QContactTag::FieldTag;
    if (detail.definitionName() == QContactThumbnail::DefinitionName)
        return QContactThumbnail::FieldThumbnail;
    if (detail.definitionName() == QContactTimestamp::DefinitionName)
        return QContactTimestamp::FieldCreationTimestamp;
    if (detail.definitionName() == QContactType::DefinitionName)
        return QContactType::FieldType;
    if (detail.definitionName() == QContactUrl::DefinitionName)
        return QContactUrl::FieldUrl;
    return QString();
}

QMLContactDetail::QMLContactDetail(QObject *parent)
    :QObject(parent)
{

}

QMLContactDetail::QMLContactDetail(const QContactDetail& detail, QObject *parent)
    :QObject(parent)
{
    setContactDetail(detail);
}

void QMLContactDetail::setContactDetail(const QContactDetail& detail)
{
    QVariantMap values = detail.variantValues();
    foreach (const QString& key, values.keys()) {
        setProperty(normalizePropertyName(key).toLatin1().data(), values.value(key));
    }
    m_defaultPropertyName = normalizePropertyName(defaultPropertyNameForContactDetail(detail));
}

QVariant QMLContactDetail::defaultProperty() const
{
    if (m_defaultPropertyName.isEmpty())
        return QVariant();
    return property(m_defaultPropertyName.toLatin1().data());
}

void QMLContactDetail::setDefaultProperty(const QVariant& value)
{
    if (!m_defaultPropertyName.isEmpty())
        setProperty(m_defaultPropertyName.toLatin1().data(), value);
}
