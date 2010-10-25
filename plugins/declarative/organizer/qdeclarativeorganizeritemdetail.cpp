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


/* ==================== QDeclarativeOrganizerItemDetail ======================= */

/*!
   \qmlclass ItemDetail QDeclarativeOrganizerItemDetail
   \brief The ItemDetail element represents a single, complete detail about a organizer item.
   \ingroup qml-organizer

   \sa QOrganizerItemDetail

    The ItemDetail element is part of the \bold{QtMobility.organizer 1.1} module.
 */


/*!
  \qmlproperty string ItemDetail::definitionName
  This property holds the string constant for the definition name of the detail.
  This property is read only.
  */

/*!
  \qmlproperty enumeration ItemDetail::type
  This property holds the type of the detail.
  This property is read only.

    \sa EventTime
    \sa JournalTime
    \sa TodoTime
    \sa TodoProgress
    \sa AudibleReminder
    \sa VisualReminder
    \sa EmailReminder
    \sa Comment
    \sa Description
    \sa DisplayLabel
    \sa Guid
    \sa Location
    \sa Parent
    \sa Priority
    \sa Recurrence
    \sa Timestamp
    \sa Type
    \sa Tag
  */


/*!
  \qmlproperty list<string> ItemDetail::fieldNames
  This property holds the list of all field names which this detail supports.

  This property is read only.
  */
/*!
  \qmlproperty bool ItemDetail::readOnly
  This property indicates whether or not this detail is writable.
  This property is read only.
  */
/*!
  \qmlproperty bool ItemDetail::removable
  This property indicates whether or not this detail is removale.
  This property is read only.
  */

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

////////////////QDeclarativeOrganizerEventTime////////////////////////
/*!
   \qmlclass EventTime QDeclarativeOrganizerEventTime
   \brief The EventTime element contains the start and end dates and times of a recurring event series, or occurrence of an event.
   \ingroup qml-organizer

   EventTime element contains the following field types:
   \list
   \o EventTime.StartDateTime
   \o EventTime.EndDateTime
   \o EventTime.AllDay
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty date EventTime::startDateTime

  This property holds the start date and time value of the event.
  */

/*!
  \qmlproperty date EventTime::endDateTime

  This property holds the end date and time value of the event.
  */
/*!
  \qmlproperty date EventTime::allDay

  This property holds whether the time is significant in the start datetime.
  */

////////////////QDeclarativeOrganizerTodoTime////////////////////////
/*!
   \qmlclass TodoTime QDeclarativeOrganizerTodoTime
   \brief The TodoTime element contains the start and due dates and times of a recurring todo series, or occurrence of an todo item.
   \ingroup qml-organizer

   TodoTime element contains the following field types:
   \list
   \o TodoTime.StartDateTime
   \o TodoTime.DueDateTime
   \o TodoTime.AllDay
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty date TodoTime::startDateTime

  This property holds the start date and time value of the todo item.
  */

/*!
  \qmlproperty date TodoTime::dueDateTime

  This property holds the end date and time value of the todo item.
  */
/*!
  \qmlproperty date TodoTime::allDay

  This property holds whether the time is significant in the start datetime.
  */

////////////////QDeclarativeOrganizerJournalTime////////////////////////
/*!
   \qmlclass JournalTime QDeclarativeOrganizerJournalTime
   \brief The JournalTime element contains the entry date and time of a journal item.
   \ingroup qml-organizer

   JournalTime element contains the following field types:
   \list
   \o JournalTime.EntryDateTime
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty date JournalTime::entryDateTime

  This property holds the entry date and time value of the journal.
  */

////////////////QDeclarativeOrganizerItemComment////////////////////////
/*!
   \qmlclass Comment QDeclarativeOrganizerItemComment
   \brief The Comment element contains the comment text of an organizer item.
   \ingroup qml-organizer

   Comment element contains the following field types:
   \list
   \o Comment.Comment
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string Comment::comment

  This property holds the text of the comment.
  */

////////////////QDeclarativeOrganizerItemDescription////////////////////////
/*!
   \qmlclass Description QDeclarativeOrganizerItemDescription
   \brief The Description element contains the description text of an organizer item.
   \ingroup qml-organizer

   Description element contains the following field types:
   \list
   \o Description.Description
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string Description::description

  This property holds the text of the description.
  */

////////////////QDeclarativeOrganizerItemDisplayLabel////////////////////////
/*!
   \qmlclass DisplayLabel QDeclarativeOrganizerItemDisplayLabel
   \brief The DisplayLabel element contains the display label of an organizer item.
   \ingroup qml-organizer

   DisplayLabel element contains the following field types:
   \list
   \o DisplayLabel.DisplayLabel
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string DisplayLabel::displayLabel

  This property holds the display label text.
  */

////////////////QDeclarativeOrganizerItemGuid////////////////////////
/*!
   \qmlclass Guid QDeclarativeOrganizerItemGuid
   \brief The Guid element contains the GUID string of an organizer item.
   \ingroup qml-organizer

   Guid element contains the following field types:
   \list
   \o Guid.Guid
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string Guid::guid

  This property holds the GUID string.
  */

////////////////QDeclarativeOrganizerItemTag////////////////////////
/*!
   \qmlclass Tag QDeclarativeOrganizerItemTag
   \brief The Tag element contains the tag string of an organizer item.
   \ingroup qml-organizer

   Tag element contains the following field types:
   \list
   \o Tag.Tag
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string Tag::tag

  This property holds the tag string.
  */

////////////////QDeclarativeOrganizerItemType////////////////////////
/*!
   \qmlclass Type QDeclarativeOrganizerItemType
   \brief The Type element contains the type string of an organizer item.
   \ingroup qml-organizer

   Type element contains the following field types:
   \list
   \o Type.Type
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string Type::type

  This property holds the type string.
  */

////////////////QDeclarativeOrganizerItemTimestamp////////////////////////
/*!
   \qmlclass Timestamp QDeclarativeOrganizerItemTimestamp
   \brief The Timestamp element contains the created and last modified timestamp of an organizer item's creating date and time.
   \ingroup qml-organizer

   Timestamp element contains the following field types:
   \list
   \o Timestamp.LastModified
   \o Timestamp.Created
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty date Timestamp::lastModified

  This property holds the value of the item's last modified date and time.
  */

/*!
  \qmlproperty date Timestamp::created

  This property holds the value of the item's creation date and time.
  */

////////////////QDeclarativeOrganizerItemLocation////////////////////////
/*!
   \qmlclass Location QDeclarativeOrganizerItemLocation
   \brief The Location element contains information about a location which is related to the organizer item in some manner.
   \ingroup qml-organizer

   Location element contains the following field types:
   \list
   \o Location.Label
   \o Location.Longitude
   \o Location.Latitude
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string Location::label

  This property holds the location label value.
  */

/*!
  \qmlproperty double Location::longitude

  This property holds the location longitude value .
  */

/*!
  \qmlproperty double Location::latitude

  This property holds the location latitude value.
  */

////////////////QDeclarativeOrganizerTodoProgress////////////////////////
/*!
   \qmlclass TodoProgress QDeclarativeOrganizerTodoProgress
   \brief The TodoProgress element contains information about the progress of a todo item.
   \ingroup qml-organizer

   TodoProgress element contains the following field types:
   \list
   \o TodoProgress.Status
   \o TodoProgress.Percentage
   \o TodoProgress.FinishedDateTime
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty enumeration TodoProgress::status

  This property holds the value which describes the current completion status of the
   todo item. The value can be one of:
  \list
  \o TodoProgress.NotStarted
  \o TodoProgress.InProgress
  \o TodoProgress.Complete
  \endlist
  */

/*!
  \qmlproperty int TodoProgress::percentage

  This property holds the value which contains the current completion percentage of the
   todo item.
  */

/*!
  \qmlproperty date TodoProgress::finishedDateTime

  This property holds the date time value which contains the date and time at which the
   todo item was completed.
  */


////////////////QDeclarativeOrganizerItemParent////////////////////////
/*!
   \qmlclass Parent QDeclarativeOrganizerItemParent
   \brief The Parent element contains information about the event or todo that generated this item.
   \ingroup qml-organizer

   Parent element contains the following field types:
   \list
   \o Parent.ParentId
   \o Parent.OriginalDate
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty int Parent::parentId

  This property holds the id of the parent recurrent event or todo.
  */

/*!
  \qmlproperty date Parent::originalDate

  This property holds the original date of this instance origin item.
  */

////////////////QDeclarativeOrganizerItemRecurrence////////////////////////
/*!
   \qmlclass Recurrence QDeclarativeOrganizerItemRecurrence
   \brief The Recurrence element contains a list of rules and dates on which the recurrent item occurs,
          and a list of rules and dates on which exceptions occur.
   \ingroup qml-organizer

   Recurrence element contains the following field types:
   \list
   \o Recurrence.RecurrenceRules
   \o Recurrence.ExceptionRules
   \o Recurrence.RecurrenceDates
   \o Recurrence.ExceptionDates
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty QDeclarativeListProperty Recurrence::recurrenceRules

  This property holds the list of recurrence rules.

  \sa RecurrenceRule
  */

/*!
  \qmlproperty QDeclarativeListProperty Recurrence::exceptionRules

  This property holds the list of exception rules.

  \sa RecurrenceRule
  */

/*!
  \qmlproperty list<date> Recurrence::recurrenceDates

  This property holds the list of recurrence dates.
  */

/*!
  \qmlproperty list<date> Recurrence::exceptionDates

  This property holds the list of exception dates.
  */

////////////////QDeclarativeOrganizerItemPriority////////////////////////
/*!
   \qmlclass Priority QDeclarativeOrganizerItemPriority
   \brief The Priority element contains the priority of the organizer item, which may be used to resolve scheduling conflicts.
   \ingroup qml-organizer

   Priority element contains the following field types:
   \list
   \o Priority.Priority
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty enumeration Priority::priority

  This property holds the priority associated with an organizer item. The value can be one of:
  \list
  \o Priority.Unknown
  \o Priority.Highest
  \o Priority.ExtremelyHigh
  \o Priority.VeryHigh
  \o Priority.High
  \o Priority.Medium
  \o Priority.Low
  \o Priority.VeryLow
  \o Priority.ExtremelyLow
  \o Priority.Lowest
  \endlist
  */

////////////////QDeclarativeOrganizerItemReminder////////////////////////
/*!
   \qmlclass Reminder QDeclarativeOrganizerItemReminder
   \brief The Reminder element contains information about when and how the user wants to reminded of the item.
   \ingroup qml-organizer

   Reminder element contains the following field types:
   \list
   \o Reminder.Type
   \o Reminder.SecondsBeforeStart
   \o Reminder.RepetitionCount
   \o Reminder.RepetitionDelay
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty enumeration Reminder::reminderType

  This property holds the reminder type of this reminder for an organizer item. The value can be one of:
  \list
  \o Reminder.NoReminder
  \o Reminder.VisualReminder
  \o Reminder.AudibleReminder
  \o Reminder.EmailReminder
  \endlist
  */

/*!
  \qmlproperty int Reminder::secondsBeforeStart

  This property holds the number of seconds prior to the activation of the item
   at which the user wants to be reminded of the item.
  */
/*!
  \qmlproperty int Reminder::repetitionCount

  This property holds the number of times the user should be reminded of the item.
  */
/*!
  \qmlproperty int Reminder::repetitionDelay

  This property holds the delay (in seconds) between each repetition of the reminder.
  */

////////////////QDeclarativeOrganizerItemAudibleReminder////////////////////////
/*!
   \qmlclass AudibleReminder QDeclarativeOrganizerItemAudibleReminder
   \brief The AudibleReminder element contains information about an audible reminder of an item.
   \ingroup qml-organizer

   \inherits Reminder

   AudibleReminder element contains the following field types:
   \list
   \o AudibleReminder.DataUrl
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty url AudibleReminder::dataUrl

  This property holds the url of the audible data to play.
  */

////////////////QDeclarativeOrganizerItemVisualReminder////////////////////////
/*!
   \qmlclass VisualReminder QDeclarativeOrganizerItemVisualReminder
   \brief The VisualReminder element contains information about a visual reminder of an item.
   \ingroup qml-organizer

   \inherits Reminder

   VisualReminder element contains the following field types:
   \list
   \o VisualReminder.DataUrl
   \o VisualReminder.Message
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty url VisualReminder::dataUrl

  This property holds the url of the visual data which the user wishes to be displayed as part of the reminder.
  */
/*!
  \qmlproperty string VisualReminder::message

  This property holds the message which the user wishes to be displayed as part of the reminder.
  */

////////////////QDeclarativeOrganizerItemEmailReminder////////////////////////
/*!
   \qmlclass EmailReminder QDeclarativeOrganizerItemEmailReminder
   \brief The EmailReminder element contains information about an email reminder of an item.
   \ingroup qml-organizer

   \inherits Reminder

   EmailReminder element contains the following field types:
   \list
   \o EmailReminder.Subject
   \o EmailReminder.Body
   \o EmailReminder.Recipients
   \o EmailReminder.Attachments
   \endlist

   This element is part of the \bold{QtMobility.organizer 1.1} module.
 */

/*!
  \qmlproperty string EmailReminder::subject

  This property holds the subject of the email.
  */
/*!
  \qmlproperty string EmailReminder::body

  This property holds the body of the email.
  */

/*!
  \qmlproperty list<string> EmailReminder::recipients

  This property holds the list of recipients that the user wishes to be sent an email as part of the reminder.
  */

/*!
  \qmlproperty list<variant> EmailReminder::attachments

  This property holds the attachments of the email.
  */
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
