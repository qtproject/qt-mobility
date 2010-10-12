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
#include "qdeclarativeorganizeritemrecurrencerule_p.h"
#include "qdeclarativeorganizeritemfetchhint_p.h"
#include "qdeclarativeorganizeritemsortorder_p.h"
#include "qdeclarativeorganizeritemcollection_p.h"

QT_USE_NAMESPACE


class QOrganizerQmlPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri)
    {
        Q_ASSERT(uri == QLatin1String("QtMobility.organizer"));
        qmlRegisterType<QDeclarativeOrganizerModel>(uri, 1, 1, "OrganizerModel");
        qmlRegisterType<QDeclarativeOrganizerItem>(uri, 1, 1, "OrganizerItem");
        qmlRegisterType<QDeclarativeOrganizerItemRecurrenceRule>(uri, 1, 1, "OrganizerItemRecurrenceRule");
        qmlRegisterType<QDeclarativeOrganizerItemFetchHint>(uri, 1, 1, "OrganizerItemFetchHint");
        qmlRegisterType<QDeclarativeOrganizerItemSortOrder>(uri, 1, 1, "OrganizerItemSortOrder");
        qmlRegisterType<QDeclarativeOrganizerCollection>(uri, 1, 1, "OrganizerCollection");

        //items
        qmlRegisterType<QDeclarativeOrganizerItem>(uri, 1, 1, "OrganizerItem");
        qmlRegisterType<QDeclarativeOrganizerEvent>(uri, 1, 1, "OrganizerEvent");
        qmlRegisterType<QDeclarativeOrganizerEventOccurrence>(uri, 1, 1, "OrganizerEventOccurrence");
        qmlRegisterType<QDeclarativeOrganizerJournal>(uri, 1, 1, "OrganizerJournal");
        qmlRegisterType<QDeclarativeOrganizerNote>(uri, 1, 1, "OrganizerNote");
        qmlRegisterType<QDeclarativeOrganizerTodo>(uri, 1, 1, "OrganizerTodo");
        qmlRegisterType<QDeclarativeOrganizerTodoOccurrence>(uri, 1, 1, "OrganizerTodoOccurrence");

        //details
        qmlRegisterType<QDeclarativeOrganizerItemDetail>(uri, 1, 1, "OrganizerItemDetail");
        qmlRegisterType<QDeclarativeOrganizerEventTime>(uri, 1, 1, "OrganizerEventTime");
        qmlRegisterType<QDeclarativeOrganizerItemDescription>(uri, 1, 1, "OrganizerItemDescription");
        qmlRegisterType<QDeclarativeOrganizerItemDisplayLabel>(uri, 1, 1, "OrganizerItemDisplayLabel");
        qmlRegisterType<QDeclarativeOrganizerItemGuid>(uri, 1, 1, "OrganizerItemGuid");
        qmlRegisterType<QDeclarativeOrganizerItemParent>(uri, 1, 1, "OrganizerItemParent");
        qmlRegisterType<QDeclarativeOrganizerItemLocation>(uri, 1, 1, "OrganizerItemLocation");
        qmlRegisterType<QDeclarativeOrganizerItemPriority>(uri, 1, 1, "OrganizerItemPriority");
        qmlRegisterType<QDeclarativeOrganizerItemRecurrence>(uri, 1, 1, "OrganizerItemRecurrence");
        qmlRegisterType<QDeclarativeOrganizerItemReminder>(uri, 1, 1, "OrganizerItemReminder");
        qmlRegisterType<QDeclarativeOrganizerItemAudibleReminder>(uri, 1, 1, "OrganizerItemAudibleReminder");
        qmlRegisterType<QDeclarativeOrganizerItemVisualReminder>(uri, 1, 1, "OrganizerItemVisualReminder");
        qmlRegisterType<QDeclarativeOrganizerItemEmailReminder>(uri, 1, 1, "OrganizerItemEmailReminder");
        qmlRegisterType<QDeclarativeOrganizerItemTimestamp>(uri, 1, 1, "OrganizerItemTimestamp");
        qmlRegisterType<QDeclarativeOrganizerItemType>(uri, 1, 1, "OrganizerItemType");
        qmlRegisterType<QDeclarativeOrganizerJournalTime>(uri, 1, 1, "OrganizerJournalTime");
        qmlRegisterType<QDeclarativeOrganizerTodoProgress>(uri, 1, 1, "OrganizerTodoProgress");
        qmlRegisterType<QDeclarativeOrganizerTodoTime>(uri, 1, 1, "OrganizerTodoTime");

        //filters
        qmlRegisterType<QDeclarativeOrganizerItemFilter>(uri, 1, 1, "OrganizerItemFilter");
        qmlRegisterType<QDeclarativeOrganizerItemChangelogFilter>(uri, 1, 1, "OrganizerItemChangelogFilter");
        qmlRegisterType<QDeclarativeOrganizerItemCollectionFilter>(uri, 1, 1, "OrganizerItemCollectionFilter");
        qmlRegisterType<QDeclarativeOrganizerItemDetailFilter>(uri, 1, 1, "OrganizerItemDetailFilter");
        qmlRegisterType<QDeclarativeOrganizerItemDetailRangeFilter>(uri, 1, 1, "OrganizerItemDetailRangeFilter");
        qmlRegisterType<QDeclarativeOrganizerItemLocalIdFilter>(uri, 1, 1, "OrganizerItemLocalIdFilter");
        qmlRegisterType<QDeclarativeOrganizerItemIntersectionFilter>(uri, 1, 1, "OrganizerItemIntersectionFilter");
        qmlRegisterType<QDeclarativeOrganizerItemUnionFilter>(uri, 1, 1, "OrganizerItemUnionFilter");
        qmlRegisterType<QDeclarativeOrganizerItemInvalidFilter>(uri, 1, 1, "OrganizerItemInvalidFilter");
    }
};

#include "plugin.moc"

Q_EXPORT_PLUGIN2(qorganizerqmlplugin, QOrganizerQmlPlugin);
