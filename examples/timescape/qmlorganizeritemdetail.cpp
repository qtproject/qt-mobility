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

#include <qorganizeritemdetails.h>
#include "qmlorganizeritemdetail.h"


static QString normalizePropertyName(const QString& name)
{
   if (!name.isEmpty())
     return name.mid(1).prepend(name[0].toLower());
   return QString();
}

static QString defaultPropertyNameForOrganizerItemDetail(const QOrganizerItemDetail& detail)
{
    if (detail.definitionName() == QOrganizerItemAttendee::DefinitionName)
        return QOrganizerItemAttendee::FieldAttendeeDisplayLabel;

    if (detail.definitionName() == QOrganizerItemDescription::DefinitionName)
        return QOrganizerItemDescription::FieldDescription;

    if (detail.definitionName() == QOrganizerItemDisplayLabel::DefinitionName)
        return QOrganizerItemDisplayLabel::FieldLabel;

    if (detail.definitionName() == QOrganizerItemEventTimeRange::DefinitionName)
        return QOrganizerItemEventTimeRange::FieldStartDateTime;

    if (detail.definitionName() == QOrganizerItemGuid::DefinitionName)
        return QOrganizerItemGuid::FieldGuid;

    if (detail.definitionName() == QOrganizerItemInstanceOrigin::DefinitionName)
        return QOrganizerItemInstanceOrigin::FieldParentLocalId;

    if (detail.definitionName() == QOrganizerItemJournalTimeRange::DefinitionName)
        return QOrganizerItemJournalTimeRange::FieldEntryDateTime;

    if (detail.definitionName() == QOrganizerItemLocation::DefinitionName)
        return QOrganizerItemLocation::FieldLocationName;

    if (detail.definitionName() == QOrganizerItemNote::DefinitionName)
        return QOrganizerItemNote::FieldNote;

    if (detail.definitionName() == QOrganizerItemParticipation::DefinitionName)
        return QOrganizerItemParticipation::FieldParticipationRole;

    if (detail.definitionName() == QOrganizerItemPriority::DefinitionName)
        return QOrganizerItemPriority::FieldPriority;

    if (detail.definitionName() == QOrganizerItemProvenance::DefinitionName)
        return QOrganizerItemProvenance::FieldReceivedFrom;

    if (detail.definitionName() == QOrganizerItemRecurrence::DefinitionName)
        return QOrganizerItemRecurrence::FieldRecurrenceRules;

    if (detail.definitionName() == QOrganizerItemRsvpInfo::DefinitionName)
        return QOrganizerItemRsvpInfo::FieldContactUri;

    if (detail.definitionName() == QOrganizerItemTimestamp::DefinitionName)
        return QOrganizerItemTimestamp::FieldModificationTimestamp;

    if (detail.definitionName() == QOrganizerItemTodoProgress::DefinitionName)
        return QOrganizerItemTodoProgress::FieldStatus;

    if (detail.definitionName() == QOrganizerItemTodoTimeRange::DefinitionName)
        return QOrganizerItemTodoTimeRange::FieldDueDateTime;

    if (detail.definitionName() == QOrganizerItemType::DefinitionName)
        return QOrganizerItemType::FieldType;

    return QString();
}

QMLOrganizerItemDetail::QMLOrganizerItemDetail(QObject *parent)
    :QObject(parent)
{

}

QMLOrganizerItemDetail::QMLOrganizerItemDetail(const QOrganizerItemDetail& detail, QObject *parent)
    :QObject(parent)
{
    setItemDetail(detail);
}

void QMLOrganizerItemDetail::setItemDetail(const QOrganizerItemDetail& detail)
{
    QVariantMap values = detail.variantValues();
    foreach (const QString& key, values.keys()) {
        setProperty(normalizePropertyName(key).toLatin1().data(), values.value(key));
    }
    m_defaultPropertyName = normalizePropertyName(defaultPropertyNameForOrganizerItemDetail(detail));
}

QVariant QMLOrganizerItemDetail::defaultProperty() const
{
    if (m_defaultPropertyName.isEmpty())
        return QVariant();
    return property(m_defaultPropertyName.toLatin1().data());
}

void QMLOrganizerItemDetail::setDefaultProperty(const QVariant& value)
{
    if (!m_defaultPropertyName.isEmpty())
        setProperty(m_defaultPropertyName.toLatin1().data(), value);
}
