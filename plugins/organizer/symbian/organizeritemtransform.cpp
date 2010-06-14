/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#include "organizeritemtransform.h"

#include <e32math.h>
#include <calinstance.h>
#include <caluser.h>
#include <calrrule.h>

#include <qorganizeritem.h>
#include <qorganizeritemdetails.h>

const TInt KGuidLength = 30;

QTM_USE_NAMESPACE

OrganizerItemTransform::OrganizerItemTransform()
{
    // Set seed for qrand()
    TTime homeTime;
    homeTime.HomeTime();
    uint seed = (uint) homeTime.Int64();
    qsrand(seed);
}

OrganizerItemTransform::~OrganizerItemTransform()
{

}

HBufC8 *OrganizerItemTransform::guidLC(const QOrganizerItem &item)
{
    // Read guid from organizer item
    QString globalUidString = item.guid();

    // Create a buffer for guid
    HBufC8* globalUidBuf = HBufC8::NewLC(KGuidLength);
    TPtr8 ptr = globalUidBuf->Des();

    if (item.guid().isEmpty()) {
        // Generate a new quid
        // TODO: is this long enough for us? We could add imei or second random number to it...
        ptr.Num(qrand());
    } else {
        // Use the old guid
        ptr = toPtrC8(globalUidString);
    }
    
    return globalUidBuf; // ownership passed
}

CCalEntry::TType OrganizerItemTransform::entryTypeL(const QOrganizerItem &item) const
{
    QString itemType = item.type();
    CCalEntry::TType entryType;

    if (itemType == QOrganizerItemType::TypeTodo)
        entryType = CCalEntry::ETodo;
    else if (itemType == QOrganizerItemType::TypeEvent)
        entryType = CCalEntry::EAppt;
    else
        User::Leave(KErrUnknown); // unknown type

    // TODO: CCalEntry::EEvent???
    // TODO: CCalEntry::EReminder
    // TODO: CCalEntry::EAnniv

    return entryType;
}

QString OrganizerItemTransform::itemTypeL(const CCalEntry &entry) const
{
    CCalEntry::TType entryType = entry.EntryTypeL();
    QString itemType;

    if (entryType == CCalEntry::ETodo)
        itemType = QLatin1String(QOrganizerItemType::TypeTodo);
    else if (entryType == CCalEntry::EEvent)
        itemType = QLatin1String(QOrganizerItemType::TypeEvent);
    else if (entryType == CCalEntry::EAppt)
        itemType = QLatin1String(QOrganizerItemType::TypeEvent);
    else
        User::Leave(KErrUnknown); // unknown type

    // TODO: CCalEntry::EAppt
    // TODO: CCalEntry::EReminder
    // TODO: CCalEntry::EAnniv

    return itemType;
}

CCalEntry *OrganizerItemTransform::toEntryLC(const QOrganizerItem &item)
{
    // Create entry
    CCalEntry::TType type = entryTypeL(item);
    HBufC8* globalUid = guidLC(item);
    CCalEntry::TMethod method = CCalEntry::EMethodAdd;
    TInt seqNum = 0;
    //TCalTime recurrenceId;
    //CalCommon::TRecurrenceRange recurrenceRange;
    CCalEntry *entry = CCalEntry::NewL(type, globalUid, method, seqNum);
    CleanupStack::Pop(globalUid); // ownership passed?
    CleanupStack::PushL(entry);

    // *** Date and Time ***
    if (item.type() == QOrganizerItemType::TypeEvent)
    {
        QOrganizerEventTimeRange range = item.detail<QOrganizerEventTimeRange>();
        if (!range.isEmpty())
            entry->SetStartAndEndTimeL(toTCalTimeL(range.startDateTime()), toTCalTimeL(range.endDateTime()));
    }
    else if(item.type() == QOrganizerItemType::TypeJournal)
    {
        QOrganizerJournalTimeRange range = item.detail<QOrganizerJournalTimeRange>();
        if (!range.isEmpty())
            entry->SetDTStampL(toTCalTimeL(range.entryDateTime()));
    }
    else if(item.type() == QOrganizerItemType::TypeTodo)
    {
        QOrganizerTodoTimeRange range = item.detail<QOrganizerTodoTimeRange>();
        if (!range.isEmpty())
            entry->SetStartAndEndTimeL(toTCalTimeL(range.notBeforeDateTime()), toTCalTimeL(range.dueDateTime()));

        QOrganizerTodoProgress progress = item.detail<QOrganizerTodoProgress>();
        if (!progress.isEmpty() && progress.status() == QOrganizerTodoProgress::StatusComplete)
            entry->SetCompletedL(true, toTCalTimeL(progress.finishedDateTime()));
    }

    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence = item.detail<QOrganizerItemRecurrence>();
    if (!recurrence.isEmpty())
    {
        // TODO: can we have recurrenceRules and recurrenceDates at the same time?

        if (recurrence.recurrenceRules().count())
            entry->SetRRuleL(toCalRRule(recurrence.recurrenceRules()));

        if (recurrence.recurrenceDates().count()) {
            RArray<TCalTime> calRDates;
            toTCalTimesL(recurrence.recurrenceDates(), calRDates);
            entry->SetRDatesL(calRDates);
        }

        if (recurrence.exceptionDates().count()) {
            RArray<TCalTime> calExDates;
            toTCalTimesL(recurrence.exceptionDates(), calExDates);
            entry->SetRDatesL(calExDates);
        }

        //  TODO: what about recurrence.exceptionRules()? there is no match in native API.
    }


    // *** Entry alarm ***
    //entry->SetAlarmL(CCalAlarm* aAlarm);

    // *** Text Fields ***
    if (!item.displayLabel().isEmpty())
        entry->SetSummaryL(toPtrC16(item.displayLabel()));

    if (!item.description().isEmpty())
        entry->SetDescriptionL(toPtrC16(item.description()));

    QOrganizerItemLocation loc = item.detail<QOrganizerItemLocation>();
    if (!loc.isEmpty()) {
        // NOTE: what about geoLocation & address?
        entry->SetLocationL(toPtrC16(loc.locationName()));
    }

    // *** Category list ***
    // TODO:
    //entry->AddCategoryL(CCalCategory* aCategory);
    //entry->DeleteCategoryL(TInt aIndex);

    // *** Attendee Methods ***
    // TODO:
    //entry->AddAttendeeL(CCalAttendee* aAttendee);
    //entry->DeleteAttendeeL(TInt aIndex);
    //entry->SetOrganizerL(CCalUser* aUser);
    //entry->SetPhoneOwnerL(const CCalUser* aOwner);

    // *** Other Attributes ***
    // TODO:
    //entry->SetStatusL(TStatus aStatus);
    //entry->SetReplicationStatusL(TReplicationStatus aReplicationStatus);

    QOrganizerItemPriority priority = item.detail<QOrganizerItemPriority>();
    if (!priority.isEmpty())
        entry->SetPriorityL(priority.priority()); // allowed values between 0 and 255.

    // TODO:
    //entry->SetMethodL(TMethod aMethod);
    //entry->SetSequenceNumberL(TInt aSeq);
    //entry->SequenceNumberL() const;
    //entry->SetTzRulesL(const CTzRules& aTzRule);
    //entry->SetTzRulesL();

    if (item.localId())
        entry->SetLocalUidL(item.localId());

    return entry;
}

void OrganizerItemTransform::toItemL(const CCalEntry &entry, QOrganizerItem *item) const
{
    //debugEntryL(entry);
        
    item->setType(itemTypeL(entry));
    item->setGuid(toQString(entry.UidL()));

    // *** Date and Time ***
    TCalTime startTime = entry.StartTimeL();
    TCalTime endTime = entry.EndTimeL();
    TCalTime dtstamp = entry.DTStampL();

    if (item->type() == QOrganizerItemType::TypeEvent)
    {
        QOrganizerEventTimeRange range;
        if (startTime.TimeUtcL() != Time::NullTTime())
            range.setStartDateTime(toQDateTimeL(startTime));

        // Check if the end time is defined and if the end time is different to
        // start time. Effectively this means that if a QtMobility Organizer API
        // client defines an end time that is exactly the same as start time, the
        // end time is lost.
        if (endTime.TimeUtcL() != Time::NullTTime()
            && endTime.TimeUtcL() != startTime.TimeUtcL())
            range.setEndDateTime(toQDateTimeL(endTime));

        item->saveDetail(&range);
    }
    else if(item->type() == QOrganizerItemType::TypeJournal)
    {
        if (dtstamp.TimeUtcL() != Time::NullTTime()) {
            QOrganizerJournalTimeRange range;
            range.setEntryDateTime(toQDateTimeL(entry.DTStampL()));
            item->saveDetail(&range);
        }
    }
    else if(item->type() == QOrganizerItemType::TypeTodo)
    {
        QOrganizerTodoTimeRange range;
        if (startTime.TimeUtcL() != Time::NullTTime())
            range.setNotBeforeDateTime(toQDateTimeL(startTime));
        if (endTime.TimeUtcL() != Time::NullTTime())
            range.setDueDateTime(toQDateTimeL(endTime));
        item->saveDetail(&range);

        QOrganizerTodoProgress progress;

        CCalEntry::TStatus entryStatus = entry.StatusL();
           
        if (entryStatus == CCalEntry::ETodoInProcess)
            progress.setStatus(QOrganizerTodoProgress::StatusInProgress);
        else if (entryStatus == CCalEntry::ETodoCompleted) {
            progress.setStatus(QOrganizerTodoProgress::StatusComplete);
            progress.setFinishedDateTime(toQDateTimeL(entry.CompletedTimeL()));
        }
        else
            progress.setStatus(QOrganizerTodoProgress::StatusNotStarted);

        item->saveDetail(&progress);
    }
    
    QOrganizerItemTimestamp timeStamp;
    timeStamp.setLastModified(toQDateTimeL(entry.LastModifiedDateL()));
    item->saveDetail(&timeStamp);

    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence;

    TCalRRule calRRule;
    entry.GetRRuleL(calRRule);
    recurrence.setRecurrenceRules(toItemRecurrenceRules(calRRule));

    RArray<TCalTime> calRDateList;
    entry.GetRDatesL(calRDateList);
    recurrence.setRecurrenceDates(toQDatesL(calRDateList));

    RArray<TCalTime> calExDateList;
    entry.GetExceptionDatesL(calExDateList);
    recurrence.setExceptionDates(toQDatesL(calExDateList));

    // TODO: exceptionRules
    // There is no native support for this.

    item->saveDetail(&recurrence);

    // **** Text Fields ***
    item->setDisplayLabel(toQString(entry.SummaryL()));
    item->setDescription(toQString(entry.DescriptionL()));

    QOrganizerItemLocation location;
    location.setLocationName(toQString(entry.LocationL()));

    // **** Other Attributes ***
    QOrganizerItemPriority priority;
    // TODO: do we need some kind of conversion?
    // Item has range from 0-9 and entry has range from 0 - 255.
    priority.setPriority((QOrganizerItemPriority::Priority) entry.PriorityL());
    item->saveDetail(&priority);
}

void OrganizerItemTransform::toItemL(const CCalInstance &instance, QOrganizerItem *item) const
{
    toItemL(instance.Entry(), item);
    // TODO: strip recurrence stuff
    // TODO: set item instance origin
    // TODO: transform  CCalInstance::Time()
    // TODO: transform  CCalInstance::StartTimeL()
    // TODO: transform  CCalInstance::EndTimeL()
}

QString OrganizerItemTransform::toQString(const TDesC8 &des)
{
    return QString::fromUtf8((const char *)des.Ptr(), des.Length());
}

QString OrganizerItemTransform::toQString(const TDesC16 &des)
{
    return QString::fromUtf16(des.Ptr(), des.Length());
}

TPtrC8 OrganizerItemTransform::toPtrC8(const QString &string) const
{
    return TPtrC8(reinterpret_cast<const TUint8*>(string.data()));
}

TPtrC16 OrganizerItemTransform::toPtrC16(const QString &string) const
{
    return TPtrC16(reinterpret_cast<const TUint16*>(string.utf16()));
}

TCalTime OrganizerItemTransform::toTCalTimeL(QDateTime dateTime)
{
    TCalTime calTime;
    calTime.SetTimeUtcL(Time::NullTTime());

    if (dateTime.isValid()) {
        uint secondsFrom1970 = dateTime.toTime_t();
        quint64 usecondsFrom1970 = ((quint64) secondsFrom1970) * ((quint64) 1000000) + ((quint64) dateTime.time().msec() * (quint64)1000);
        TTime time1970(_L("19700000:000000.000000"));
        quint64 usecondsBCto1970 = time1970.MicroSecondsFrom(TTime(0)).Int64();
        quint64 useconds = usecondsBCto1970 + usecondsFrom1970;

        TTime time(useconds);
        calTime.SetTimeUtcL(time);
    }

    return calTime;
}

QDateTime OrganizerItemTransform::toQDateTimeL(TCalTime calTime)
{
    const TTime time1970(_L("19700000:000000.000000"));
    quint64 usecondsBCto1970 = time1970.MicroSecondsFrom(TTime(0)).Int64();
    quint64 useconds = calTime.TimeUtcL().Int64() - usecondsBCto1970;
    quint64 seconds = useconds / (quint64)1000000;
    quint64 msecondscomponent = (useconds - seconds * (quint64)1000000) / (quint64)1000;
    QDateTime dateTime;
    dateTime.setTime_t(seconds);
    dateTime.setTime(dateTime.time().addMSecs(msecondscomponent));
    return dateTime;
}

void OrganizerItemTransform::toTCalTimesL(const QList<QDate> &dates, RArray<TCalTime> &calDates) const
{
    foreach (QDate date, dates) {
        QDateTime dateTime(date);
        TCalTime calTime = toTCalTimeL(dateTime);
        calDates.Append(calTime);
    }
}

QList<QDate> OrganizerItemTransform::toQDatesL(const RArray<TCalTime> &calDates) const
{
    QList<QDate> dates;
    int count = calDates.Count();
    for (int i=0; i<count; i++) {
        QDateTime dateTime = toQDateTimeL(calDates[i]);
        dates << dateTime.date();
    }    
    return dates;
}

TCalRRule OrganizerItemTransform::toCalRRule(QList<QOrganizerItemRecurrenceRule> rules) const
{
    TCalRRule calRRule;
    // TODO: conversion
    return calRRule;
}

QList<QOrganizerItemRecurrenceRule> OrganizerItemTransform::toItemRecurrenceRules(TCalRRule calRule) const
{
    // TODO: conversion
    return QList<QOrganizerItemRecurrenceRule>();
}

void OrganizerItemTransform::debugEntryL(const CCalEntry &entry) const
{
    qDebug() << "CCalEntry";
    qDebug() << "LocalUid    :" << entry.LocalUidL();
    qDebug() << "Uid         :" << toQString(entry.UidL());
    qDebug() << "Type        :" << entry.EntryTypeL();
    qDebug() << "Summary     :" << toQString(entry.SummaryL());
    qDebug() << "Desription  :" << toQString(entry.DescriptionL());
}
