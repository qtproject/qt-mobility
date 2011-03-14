/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include "organizertodoprogresstransform.h"
#include "qorganizertodoprogress.h"

void OrganizerTodoProgressTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    // Find Todo progress details
    foreach (QString itemTypeName, schemaDefs.keys()) {
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        if (details.contains(QOrganizerTodoProgress::DefinitionName)) {
            // Remove not supported details and replace the original detail definition
            QOrganizerItemDetailDefinition d = details.value(QOrganizerTodoProgress::DefinitionName);
            d.removeField(QOrganizerTodoProgress::FieldPercentageComplete);
            schemaDefs[itemTypeName].insert(d.name(), d);
        }
    }
}

void OrganizerTodoProgressTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    if (item->type() == QOrganizerItemType::TypeTodo || item->type() == QOrganizerItemType::TypeTodoOccurrence)
    {
        QOrganizerTodoProgress progress;

        CCalEntry::TStatus entryStatus = entry.StatusL();
        if (entryStatus == CCalEntry::ENullStatus) {
            return; // don't save the detail if there is no status
        } else if (entryStatus == CCalEntry::ETodoNeedsAction) {
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
    if (item.type() == QOrganizerItemType::TypeTodo || item.type() == QOrganizerItemType::TypeTodoOccurrence)
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

