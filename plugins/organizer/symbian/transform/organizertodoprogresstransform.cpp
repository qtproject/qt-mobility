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
#include "organizertodoprogresstransform.h"
#include "qorganizertodoprogress.h"

void OrganizerTodoProgressTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    if (item->type() == QOrganizerItemType::TypeTodo)
    {
        QOrganizerTodoProgress progress;

        CCalEntry::TStatus entryStatus = entry.StatusL();
        if (entryStatus == CCalEntry::ETodoNeedsAction || entryStatus == CCalEntry::ENullStatus) {
            progress.setStatus(QOrganizerTodoProgress::StatusNotStarted);
        } else if (entryStatus == CCalEntry::ETodoInProcess) {
            progress.setStatus(QOrganizerTodoProgress::StatusInProgress);
        } else if (entryStatus == CCalEntry::ETodoCompleted) {
            progress.setStatus(QOrganizerTodoProgress::StatusComplete);
            progress.setFinishedDateTime(toQDateTimeL(entry.CompletedTimeL()));
        } else {
            User::Leave(KErrUnknown);
        }

        item->saveDetail(&progress);
    }
}

void OrganizerTodoProgressTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    if (item.type() == QOrganizerItemType::TypeTodo)
    {
        QOrganizerTodoProgress progress = item.detail<QOrganizerTodoProgress>();
        if (progress.isEmpty())
            return; // TODO: should we leave instead?

        if (progress.status() == QOrganizerTodoProgress::StatusNotStarted) {
            entry->SetStatusL(CCalEntry::ETodoNeedsAction);
        } else if (progress.status() == QOrganizerTodoProgress::StatusInProgress) {
            entry->SetStatusL(CCalEntry::ETodoInProcess);
        } else if (progress.status() == QOrganizerTodoProgress::StatusComplete) {
            entry->SetStatusL(CCalEntry::ETodoCompleted);
            // symbian calandar will panic if time is not valid
            if (!progress.finishedDateTime().isValid())
                User::Leave(KErrArgument);
            entry->SetCompletedL(true, toTCalTimeL(progress.finishedDateTime()));
        } else {
            User::Leave(KErrArgument);
        }
    }
}

QString OrganizerTodoProgressTransform::detailDefinitionName()
{
    return QOrganizerTodoProgress::DefinitionName;    
}

