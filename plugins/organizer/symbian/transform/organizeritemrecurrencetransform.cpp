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
#include "organizeritemrecurrencetransform.h"
#include "qorganizeritemrecurrence.h"
#include <calrrule.h>

void OrganizerItemRecurrenceTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence;

    // TODO: do not set QOrganizerItemRecurrence if there are none

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
}

void OrganizerItemRecurrenceTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    // *** Repeat rules / RDate / ExDate Methods ***
    QOrganizerItemRecurrence recurrence = item.detail<QOrganizerItemRecurrence>();
    if (recurrence.isEmpty())
        return;

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

QString OrganizerItemRecurrenceTransform::detailDefinitionName()
{
    return QOrganizerItemRecurrence::DefinitionName;    
}

void OrganizerItemRecurrenceTransform::toTCalTimesL(const QList<QDate> &dates, RArray<TCalTime> &calDates) const
{
    foreach (QDate date, dates) {
        QDateTime dateTime(date);
        TCalTime calTime = toTCalTimeL(dateTime);
        calDates.Append(calTime);
    }
}

QList<QDate> OrganizerItemRecurrenceTransform::toQDatesL(const RArray<TCalTime> &calDates) const
{
    QList<QDate> dates;
    int count = calDates.Count();
    for (int i=0; i<count; i++) {
        QDateTime dateTime = toQDateTimeL(calDates[i]);
        dates << dateTime.date();
    }
    return dates;
}

QList<QOrganizerItemRecurrenceRule> OrganizerItemRecurrenceTransform::toItemRecurrenceRules(const TCalRRule &rrule) const
{
    // TODO: conversion
    return QList<QOrganizerItemRecurrenceRule>();
}

TCalRRule OrganizerItemRecurrenceTransform::toCalRRule(QList<QOrganizerItemRecurrenceRule> recrules) const
{
    // TODO: conversion
    return TCalRRule();
}
