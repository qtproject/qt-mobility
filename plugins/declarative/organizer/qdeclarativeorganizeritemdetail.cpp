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
Q_DEFINE_LATIN1_CONSTANT(QDeclarativeOrganizerItemParent::DetailName, "parentId");
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
