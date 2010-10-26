/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <QApplication>
#include <QtDeclarative>
#include <QDeclarativeExtensionPlugin>

#include "qdeclarativeorganizermodel_p.h"
#include "qdeclarativeorganizeritem_p.h"
#include "qdeclarativeorganizeritemdetail_p.h"
#include "qdeclarativeorganizeritemfilter_p.h"
#include "qdeclarativeorganizerrecurrencerule_p.h"
#include "qdeclarativeorganizeritemfetchhint_p.h"
#include "qdeclarativeorganizeritemsortorder_p.h"
#include "qdeclarativeorganizercollection_p.h"

QT_USE_NAMESPACE


class QOrganizerQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.organizer"));
        qmlRegisterType<QDeclarativeOrganizerModel>(uri, 1, 1, "OrganizerModel");
        qmlRegisterType<QDeclarativeOrganizerRecurrenceRule>(uri, 1, 1, "RecurrenceRule");
        qmlRegisterType<QDeclarativeOrganizerItemFetchHint>(uri, 1, 1, "FetchHint");
        qmlRegisterType<QDeclarativeOrganizerItemSortOrder>(uri, 1, 1, "SortOrder");
        qmlRegisterType<QDeclarativeOrganizerCollection>(uri, 1, 1, "Collection");

        //items
        qmlRegisterType<QDeclarativeOrganizerItem>(uri, 1, 1, "OrganizerItem");
        qmlRegisterType<QDeclarativeOrganizerEvent>(uri, 1, 1, "Event");
        qmlRegisterType<QDeclarativeOrganizerEventOccurrence>(uri, 1, 1, "EventOccurrence");
        qmlRegisterType<QDeclarativeOrganizerJournal>(uri, 1, 1, "Journal");
        qmlRegisterType<QDeclarativeOrganizerNote>(uri, 1, 1, "Note");
        qmlRegisterType<QDeclarativeOrganizerTodo>(uri, 1, 1, "Todo");
        qmlRegisterType<QDeclarativeOrganizerTodoOccurrence>(uri, 1, 1, "TodoOccurrence");

        //details
        qmlRegisterType<QDeclarativeOrganizerItemDetail>(uri, 1, 1, "Detail");
        qmlRegisterType<QDeclarativeOrganizerEventTime>(uri, 1, 1, "EventTime");
        qmlRegisterType<QDeclarativeOrganizerItemDescription>(uri, 1, 1, "Description");
        qmlRegisterType<QDeclarativeOrganizerItemDisplayLabel>(uri, 1, 1, "DisplayLabel");
        qmlRegisterType<QDeclarativeOrganizerItemGuid>(uri, 1, 1, "Guid");
        qmlRegisterType<QDeclarativeOrganizerItemParent>(uri, 1, 1, "ItemParent");
        qmlRegisterType<QDeclarativeOrganizerItemLocation>(uri, 1, 1, "Location");
        qmlRegisterType<QDeclarativeOrganizerItemPriority>(uri, 1, 1, "Priority");
        qmlRegisterType<QDeclarativeOrganizerItemRecurrence>(uri, 1, 1, "Recurrence");
        qmlRegisterType<QDeclarativeOrganizerItemReminder>(uri, 1, 1, "Reminder");
        qmlRegisterType<QDeclarativeOrganizerItemAudibleReminder>(uri, 1, 1, "AudibleReminder");
        qmlRegisterType<QDeclarativeOrganizerItemVisualReminder>(uri, 1, 1, "VisualReminder");
        qmlRegisterType<QDeclarativeOrganizerItemEmailReminder>(uri, 1, 1, "EmailReminder");
        qmlRegisterType<QDeclarativeOrganizerItemTimestamp>(uri, 1, 1, "Timestamp");
        qmlRegisterType<QDeclarativeOrganizerItemType>(uri, 1, 1, "ItemType");
        qmlRegisterType<QDeclarativeOrganizerJournalTime>(uri, 1, 1, "JournalTime");
        qmlRegisterType<QDeclarativeOrganizerTodoProgress>(uri, 1, 1, "TodoProgress");
        qmlRegisterType<QDeclarativeOrganizerTodoTime>(uri, 1, 1, "TodoTime");

        //filters
        qmlRegisterType<QDeclarativeOrganizerItemFilter>(uri, 1, 1, "Filter");
        qmlRegisterType<QDeclarativeOrganizerItemChangelogFilter>(uri, 1, 1, "ChangelogFilter");
        qmlRegisterType<QDeclarativeOrganizerItemCollectionFilter>(uri, 1, 1, "CollectionFilter");
        qmlRegisterType<QDeclarativeOrganizerItemDetailFilter>(uri, 1, 1, "DetailFilter");
        qmlRegisterType<QDeclarativeOrganizerItemDetailRangeFilter>(uri, 1, 1, "DetailRangeFilter");
        qmlRegisterType<QDeclarativeOrganizerItemIdFilter>(uri, 1, 1, "IdFilter");
        qmlRegisterType<QDeclarativeOrganizerItemIntersectionFilter>(uri, 1, 1, "IntersectionFilter");
        qmlRegisterType<QDeclarativeOrganizerItemUnionFilter>(uri, 1, 1, "UnionFilter");
        qmlRegisterType<QDeclarativeOrganizerItemInvalidFilter>(uri, 1, 1, "InvalidFilter");
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qorganizerqmlplugin, QOrganizerQmlPlugin);
