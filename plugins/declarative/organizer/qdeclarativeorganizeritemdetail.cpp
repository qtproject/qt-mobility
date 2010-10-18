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
#include "qdeclarativeorganizeritemdetail_p.h"


Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerEventTime::DetailName, "eventTime");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemComment::DetailName, "comment");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemComment::DetailGroupName, "comments");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemDescription::DetailName, "description");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemDisplayLabel::DetailName, "label");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemGuid::DetailName, "guid");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemParent::DetailName, "parent");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemLocation::DetailName, "location");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemPriority::DetailName, "priority");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemRecurrence::DetailName, "recurrence");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemReminder::DetailName, "reminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemReminder::DetailGroupName, "reminders");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemAudibleReminder::DetailName, "audibleReminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemAudibleReminder::DetailGroupName, "audibleReminders");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemVisualReminder::DetailName, "visualReminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemVisualReminder::DetailGroupName, "visualReminders");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemEmailReminder::DetailName, "emailReminder");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemEmailReminder::DetailGroupName, "emailReminders");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemTimestamp::DetailName, "timestamp");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemType::DetailName, "type");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerJournalTime::DetailName, "journalTime");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoProgress::DetailName, "todoProgress");
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerTodoTime::DetailName, "todoTime");


QString QDeclarativeOrganizerItemDetail::definitionName(QDeclarativeOrganizerItemDetail::ItemDetailType type)
{
    switch (type) {
    case QDeclarativeOrganizerItemDetail::EventTime:
        return QDeclarativeOrganizerEventTime::DetailName;
    case QDeclarativeOrganizerItemDetail::JournalTime:
        return QDeclarativeOrganizerJournalTime::DetailName;
    case QDeclarativeOrganizerItemDetail::TodoTime:
        return QDeclarativeOrganizerTodoTime::DetailName;
    case QDeclarativeOrganizerItemDetail::TodoProgress:
        return QDeclarativeOrganizerTodoProgress::DetailName;
    case QDeclarativeOrganizerItemDetail::Reminder:
        return QDeclarativeOrganizerItemReminder::DetailName;
    case QDeclarativeOrganizerItemDetail::AudibleReminder:
        return QDeclarativeOrganizerItemAudibleReminder::DetailName;
    case QDeclarativeOrganizerItemDetail::VisualReminder:
        return QDeclarativeOrganizerItemVisualReminder::DetailName;
    case QDeclarativeOrganizerItemDetail::EmailReminder:
        return QDeclarativeOrganizerItemEmailReminder::DetailName;
    case QDeclarativeOrganizerItemDetail::Comment:
        return QDeclarativeOrganizerItemComment::DetailName;
    case QDeclarativeOrganizerItemDetail::Description:
        return QDeclarativeOrganizerItemDescription::DetailName;
    case QDeclarativeOrganizerItemDetail::DisplayLabel:
        return QDeclarativeOrganizerItemDisplayLabel::DetailName;
    case QDeclarativeOrganizerItemDetail::Guid:
        return QDeclarativeOrganizerItemGuid::DetailName;
    case QDeclarativeOrganizerItemDetail::Location:
        return QDeclarativeOrganizerItemLocation::DetailName;
    case QDeclarativeOrganizerItemDetail::Parent:
        return QDeclarativeOrganizerItemParent::DetailName;
    case QDeclarativeOrganizerItemDetail::Priority:
        return QDeclarativeOrganizerItemPriority::DetailName;
    case QDeclarativeOrganizerItemDetail::Recurrence:
        return QDeclarativeOrganizerItemRecurrence::DetailName;
    case QDeclarativeOrganizerItemDetail::Timestamp:
        return QDeclarativeOrganizerItemTimestamp::DetailName;
    case QDeclarativeOrganizerItemDetail::Type:
        return QDeclarativeOrganizerItemType::DetailName;
    default:
        break;
    }
    qWarning() << QString("Can't find the detail definition name for detail type '%1'").arg(type);
    return QString();
}

QDeclarativeOrganizerItemDetail::ItemDetailType QDeclarativeOrganizerItemDetail::detailType(const QString& definitionName)
{
    if (definitionName == QDeclarativeOrganizerEventTime::DetailName)
        return QDeclarativeOrganizerItemDetail::EventTime;
    if (definitionName == QDeclarativeOrganizerJournalTime::DetailName)
            return QDeclarativeOrganizerItemDetail::JournalTime;
    if (definitionName == QDeclarativeOrganizerTodoTime::DetailName)
        return QDeclarativeOrganizerItemDetail::TodoTime;
    if (definitionName == QDeclarativeOrganizerTodoProgress::DetailName)
        return QDeclarativeOrganizerItemDetail::TodoProgress;
    if (definitionName == QDeclarativeOrganizerItemReminder::DetailName)
        return QDeclarativeOrganizerItemDetail::Reminder;
    if (definitionName == QDeclarativeOrganizerItemAudibleReminder::DetailName)
        return QDeclarativeOrganizerItemDetail::AudibleReminder;
    if (definitionName == QDeclarativeOrganizerItemVisualReminder::DetailName)
        return QDeclarativeOrganizerItemDetail::VisualReminder;
    if (definitionName == QDeclarativeOrganizerItemEmailReminder::DetailName)
        return QDeclarativeOrganizerItemDetail::EmailReminder;
    if (definitionName == QDeclarativeOrganizerItemComment::DetailName)
        return QDeclarativeOrganizerItemDetail::Comment;
    if (definitionName == QDeclarativeOrganizerItemDescription::DetailName)
        return QDeclarativeOrganizerItemDetail::Description;
    if (definitionName == QDeclarativeOrganizerItemDisplayLabel::DetailName)
        return QDeclarativeOrganizerItemDetail::DisplayLabel;
    if (definitionName == QDeclarativeOrganizerItemGuid::DetailName)
        return QDeclarativeOrganizerItemDetail::Guid;
    if (definitionName == QDeclarativeOrganizerItemLocation::DetailName)
        return QDeclarativeOrganizerItemDetail::Location;
    if (definitionName == QDeclarativeOrganizerItemParent::DetailName)
        return QDeclarativeOrganizerItemDetail::Parent;
    if (definitionName == QDeclarativeOrganizerItemPriority::DetailName)
        return QDeclarativeOrganizerItemDetail::Priority;
    if (definitionName == QDeclarativeOrganizerItemRecurrence::DetailName)
        return QDeclarativeOrganizerItemDetail::Recurrence;
    if (definitionName == QDeclarativeOrganizerItemTimestamp::DetailName)
        return QDeclarativeOrganizerItemDetail::Timestamp;
    if (definitionName == QDeclarativeOrganizerItemType::DetailName)
        return QDeclarativeOrganizerItemDetail::Type;
    qWarning() << QString("Can't find the detail type for detail name '%1'").arg(definitionName);
    return QDeclarativeOrganizerItemDetail::Customized;
}

QString QDeclarativeOrganizerItemDetail::fieldName(QDeclarativeOrganizerItemDetail::ItemDetailType detailType, int fieldType)
{
    switch (detailType) {
    case QDeclarativeOrganizerItemDetail::EventTime:
        return QDeclarativeOrganizerEventTime::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::JournalTime:
        return QDeclarativeOrganizerJournalTime::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::TodoTime:
        return QDeclarativeOrganizerTodoTime::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::TodoProgress:
        return QDeclarativeOrganizerTodoProgress::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Reminder:
        return QDeclarativeOrganizerItemReminder::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::AudibleReminder:
        return QDeclarativeOrganizerItemAudibleReminder::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::VisualReminder:
        return QDeclarativeOrganizerItemVisualReminder::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::EmailReminder:
        return QDeclarativeOrganizerItemEmailReminder::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Comment:
        return QDeclarativeOrganizerItemComment::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Description:
        return QDeclarativeOrganizerItemDescription::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::DisplayLabel:
        return QDeclarativeOrganizerItemDisplayLabel::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Guid:
        return QDeclarativeOrganizerItemGuid::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Location:
        return QDeclarativeOrganizerItemLocation::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Parent:
        return QDeclarativeOrganizerItemParent::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Priority:
        return QDeclarativeOrganizerItemPriority::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Recurrence:
        return QDeclarativeOrganizerItemRecurrence::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Timestamp:
        return QDeclarativeOrganizerItemTimestamp::fieldNameFromFieldType(fieldType);
    case QDeclarativeOrganizerItemDetail::Type:
        return QDeclarativeOrganizerItemType::fieldNameFromFieldType(fieldType);
    default:
        break;
    }
    qWarning() << QString("Can't find the field name for detail type '%1' and field type '%2'").arg(detailType).arg(fieldType);
    return QString();
}


QString QDeclarativeOrganizerItemComment::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemComment::Comment:
        return QOrganizerItemComment::FieldComment;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemDescription::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemDescription::Description:
        return QOrganizerItemDescription::FieldDescription;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemDisplayLabel::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemDisplayLabel::Label:
        return QOrganizerItemDisplayLabel::FieldLabel;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemGuid::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemGuid::Guid:
        return QOrganizerItemGuid::FieldGuid;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemParent::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemParent::ParentId:
        return QOrganizerItemParent::FieldParentId;
    case QDeclarativeOrganizerItemParent::OriginalDate:
        return QOrganizerItemParent::FieldOriginalDate;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}

QString QDeclarativeOrganizerItemLocation::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemLocation::Latitude:
        return QOrganizerItemLocation::FieldLatitude;
    case QDeclarativeOrganizerItemLocation::Longitude:
        return QOrganizerItemLocation::FieldLongitude;
    case QDeclarativeOrganizerItemLocation::Label:
        return QOrganizerItemLocation::FieldLabel;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}

QString QDeclarativeOrganizerItemPriority::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemPriority::Priority:
        return QOrganizerItemPriority::FieldPriority;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}

QString QDeclarativeOrganizerItemRecurrence::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemRecurrence::RecurrenceRules:
        return QOrganizerItemRecurrence::FieldRecurrenceRules;
    case QDeclarativeOrganizerItemRecurrence::ExceptionRules:
        return QOrganizerItemRecurrence::FieldExceptionRules;
    case QDeclarativeOrganizerItemRecurrence::RecurrenceDates:
        return QOrganizerItemRecurrence::FieldRecurrenceDates;
    case QDeclarativeOrganizerItemRecurrence::ExceptionDates:
        return QOrganizerItemRecurrence::FieldExceptionDates;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemReminder::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemReminder::Type:
        return QOrganizerItemReminder::FieldReminderType;
    case QDeclarativeOrganizerItemReminder::SecondsBeforeStart:
        return QOrganizerItemReminder::FieldSecondsBeforeStart;
    case QDeclarativeOrganizerItemReminder::RepetitionCount:
        return QOrganizerItemReminder::FieldRepetitionCount;
    case QDeclarativeOrganizerItemReminder::RepetitionDelay:
        return QOrganizerItemReminder::FieldRepetitionDelay;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemAudibleReminder::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemAudibleReminder::DataUrl:
        return QOrganizerItemAudibleReminder::FieldDataUrl;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}



QString QDeclarativeOrganizerItemVisualReminder::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemVisualReminder::DataUrl:
        return QOrganizerItemVisualReminder::FieldDataUrl;
    case QDeclarativeOrganizerItemVisualReminder::Message:
        return QOrganizerItemVisualReminder::FieldMessage;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerItemEmailReminder::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemEmailReminder::Subject:
        return QOrganizerItemEmailReminder::FieldSubject;
    case QDeclarativeOrganizerItemEmailReminder::Body:
        return QOrganizerItemEmailReminder::FieldBody;
    case QDeclarativeOrganizerItemEmailReminder::Recipients:
        return QOrganizerItemEmailReminder::FieldRecipients;
    case QDeclarativeOrganizerItemEmailReminder::Attachments:
        return QOrganizerItemEmailReminder::FieldAttachments;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}



QString QDeclarativeOrganizerItemTimestamp::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemTimestamp::LastModified:
        return QOrganizerItemTimestamp::FieldModificationTimestamp;
    case QDeclarativeOrganizerItemTimestamp::Created:
        return QOrganizerItemTimestamp::FieldCreationTimestamp;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}

QString QDeclarativeOrganizerEventTime::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerEventTime::StartDateTime:
        return QOrganizerEventTime::FieldStartDateTime;
    case QDeclarativeOrganizerEventTime::EndDateTime:
        return QOrganizerEventTime::FieldEndDateTime;
    case QDeclarativeOrganizerEventTime::AllDay:
        return QOrganizerEventTime::FieldAllDay;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}

QString QDeclarativeOrganizerItemType::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerItemType::ItemType:
        return QOrganizerItemType::FieldType;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerJournalTime::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerJournalTime::EntryDateTime:
        return QOrganizerJournalTime::FieldEntryDateTime;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}


QString QDeclarativeOrganizerTodoProgress::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerTodoProgress::Status:
        return QOrganizerTodoProgress::FieldStatus;
    case QDeclarativeOrganizerTodoProgress::Percentage:
        return QOrganizerTodoProgress::FieldPercentageComplete;
    case QDeclarativeOrganizerTodoProgress::FinishedDateTime:
        return QOrganizerTodoProgress::FieldFinishedDateTime;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}

QString QDeclarativeOrganizerTodoTime::fieldNameFromFieldType(int type)
{
    switch (type) {
    case QDeclarativeOrganizerTodoTime::AllDay:
        return QOrganizerTodoTime::FieldAllDay;
    case QDeclarativeOrganizerTodoTime::StartDateTime:
        return QOrganizerTodoTime::FieldStartDateTime;
    case QDeclarativeOrganizerTodoTime::DueDateTime:
        return QOrganizerTodoTime::FieldDueDateTime;
    }
    qWarning() << "invalid field type:" << type;
    return QString();
}
