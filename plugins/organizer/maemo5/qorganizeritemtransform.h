/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QORGANIZERITEMTRANSFORM_H
#define QORGANIZERITEMTRANSFORM_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtorganizer.h"
#include "qorganizerrecurrencetransform.h"
//#include <CMulticalendar.h>
//#include <CRecurrenceRule.h>

QTM_USE_NAMESPACE

class OrganizerItemTransform
{
public:
    OrganizerItemTransform();
    ~OrganizerItemTransform();

    // manager uri
    void setManagerUri(QString managerUri);
    QString managerUri() const;

    // get recurrence transformer instance
    OrganizerRecurrenceTransform* recurrenceTransformer();

public: // transform functions
    // item conversion functions
    QOrganizerEvent convertCEventToQEvent(CEvent *cevent);

    QOrganizerEventOccurrence convertCEventToQEventOccurrence(CEvent *cevent);
    QOrganizerEventOccurrence convertCEventToQEventOccurrence(CEvent *cevent, const QDateTime &instanceStartDate, const QDateTime &instanceEndDate);
    QOrganizerEventOccurrence convertCEventToQEventOccurrence(CEvent *cevent, const QDateTime &instanceStartDate, const QDateTime &instanceEndDate, QOrganizerItemLocalId parentLocalId);

    QOrganizerTodo convertCTodoToQTodo(CTodo *ctodo);
    QOrganizerTodoOccurrence convertCTodoToQTodoOccurrence(CTodo *ctodo, const QString &calendarName);
    QOrganizerJournal convertCJournalToQJournal(CJournal *cjournal );

    // conversions between CComponent and QOrganizerItem (common details for all items)
    void fillInCommonCComponentDetails(QOrganizerItem *item, CComponent *component, bool setId = true);
    CComponent* createCComponent(CCalendar *cal, const QOrganizerItem *item);

    // recurrence information conversions
    CRecurrence* createCRecurrence(const QOrganizerItem *item);

    // error code conversion
    QOrganizerItemManager::Error calErrorToManagerError(int calError) const;

private:
    // random GUID generation
    QString randomGuid() const;

private:
    // manager uri
    QString m_managerUri;

    // recurrence rule converter instance
    OrganizerRecurrenceTransform m_recTransformer;
};

#endif // QORGANIZERITEMTRANSFORM_H
