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

#include <QDebug>
#include <calentry.h>
#include <calinstance.h>

#include "qorganizeritem.h"
#include "organizereventtimerangetransform.h"
#include "organizeritemdescriptiontransform.h"
#include "organizeritemdetailtransform.h"
#include "organizeritemdisplaylabeltransform.h"
#include "organizeritemguidtransform.h"
#include "organizeriteminstanceorigintransform.h"
#include "organizeritemlocationtransform.h"
#include "organizeritemprioritytransform.h"
#include "organizeritemrecurrencetransform.h"
#include "organizeritemtimestamptransform.h"
#include "organizeritemtypetransform.h"
#include "organizerjournaltimerangetransform.h"
#include "organizertodoprogresstransform.h"
#include "organizertodotimerangetransform.h"
#include "qorganizereventoccurrence.h"

QTM_USE_NAMESPACE

void debugEntryL(const CCalEntry &entry)
{
    qDebug() << QString("CCalEntry uid-%1 localUid-%2")
        .arg(OrganizerItemDetailTransform::toQString(entry.UidL()))
        .arg(entry.LocalUidL());
    qDebug() << "Type            :" << entry.EntryTypeL();
    qDebug() << "Summary         :" << OrganizerItemDetailTransform::toQString(entry.SummaryL());
    qDebug() << "Description     :" << OrganizerItemDetailTransform::toQString(entry.DescriptionL());
    qDebug() << "Method          :" << entry.MethodL();
    qDebug() << "SequenceNumber  :" << entry.SequenceNumberL();
    qDebug() << "RecurrenceId    :" << OrganizerItemDetailTransform::toQDateTimeL(entry.RecurrenceIdL());
    qDebug() << "RecurrenceRange :" << entry.RecurrenceRangeL();
    qDebug() << "StartTime       :" << OrganizerItemDetailTransform::toQDateTimeL(entry.StartTimeL());
    qDebug() << "EndTime         :" << OrganizerItemDetailTransform::toQDateTimeL(entry.EndTimeL());
}

void debugInstanceL(const CCalInstance &instance)
{
    qDebug() << QString("CCalInstance time:%1 starttime:%2 endtime:%3")
        .arg(OrganizerItemDetailTransform::toQDateTimeL(instance.Time()).toString())
        .arg(OrganizerItemDetailTransform::toQDateTimeL(instance.StartTimeL()).toString())
        .arg(OrganizerItemDetailTransform::toQDateTimeL(instance.EndTimeL()).toString());
    debugEntryL(instance.Entry());
}

void debugSchemaDefs(const QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs)
{
    foreach (QString itemTypeName, schemaDefs.keys()) {
        qDebug() << itemTypeName;
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        foreach (QString detailName, details.keys()) {
            qDebug() << "\t" << detailName;
            QOrganizerItemDetailDefinition detDef = details.value(detailName);
            foreach (QString detailField, detDef.fields().keys())
                qDebug() << "\t\t" << detailField;
        }
    }
}

OrganizerItemTransform::OrganizerItemTransform()
{
    m_detailTransforms.append(new OrganizerItemTypeTransform()); // this must be first always
    m_detailTransforms.append(new OrganizerEventTimeRangeTransform());
    m_detailTransforms.append(new OrganizerItemDescriptionTransform());
    m_detailTransforms.append(new OrganizerItemDisplayLabelTransform());
    m_detailTransforms.append(new OrganizerItemGuidTransform());
    m_detailTransforms.append(new OrganizerItemInstanceOriginTransform());
    m_detailTransforms.append(new OrganizerItemLocationTransform());
    m_detailTransforms.append(new OrganizerItemPriorityTransform());
    m_detailTransforms.append(new OrganizerItemRecurrenceTransform());
    m_detailTransforms.append(new OrganizerItemTimeStampTransform());
    m_detailTransforms.append(new OrganizerJournalTimeRangeTransform());
    m_detailTransforms.append(new OrganizerTodoProgressTransform());
    m_detailTransforms.append(new OrganizerTodoTimeRangeTransform());
}

OrganizerItemTransform::~OrganizerItemTransform()
{
    // Delete transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms)
        delete i;
}

void OrganizerItemTransform::modifyBaseSchemaDefinitions(QMap<QString, QMap<QString, QOrganizerItemDetailDefinition> > &schemaDefs) const
{
    //qDebug() << "Base schema definitions:";
    //debugSchemaDefs(schemaDefs);
    
    // Get all supported detail definition names
    QStringList supportedDetails;
    foreach (OrganizerItemDetailTransform *t, m_detailTransforms)
        supportedDetails << t->detailDefinitionName();
    
    // Remove all not supported details
    foreach (QString itemTypeName, schemaDefs.keys()) {
        QMap<QString, QOrganizerItemDetailDefinition> details = schemaDefs.value(itemTypeName);
        foreach (QString detailName, details.keys()) {
            if (!supportedDetails.contains(detailName))
                schemaDefs[itemTypeName].remove(detailName);
        }
    }
    
    // Make detail specific modifications
    foreach (OrganizerItemDetailTransform *t, m_detailTransforms)
        t->modifyBaseSchemaDefinitions(schemaDefs);
    
    //qDebug() << "Modified schema definitions:";
    //debugSchemaDefs(schemaDefs);
}

void OrganizerItemTransform::toEntryL(const QOrganizerItem &item, CCalEntry *entry)
{
    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) {
        // TODO: This is just for debugging. Remove before release.
        TRAPD(err, i->transformToEntryL(item, entry));
        if (err) {
            qDebug() << "transformToEntryL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }

    entry->SetLastModifiedDateL();
}

void OrganizerItemTransform::toItemL(const CCalEntry &entry, QOrganizerItem *item) const
{
    //debugEntryL(entry);
    
    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) {
        // TODO: This is just for debugging. Remove before release.
        TRAPD(err, i->transformToDetailL(entry, item));
        if (err) {
            qDebug() << "transformToDetailL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }
}

void OrganizerItemTransform::toItemPostSaveL(const CCalEntry &entry, QOrganizerItem *item, QString managerUri) const
{
    //debugEntryL(entry);
    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) {
        // TODO: This is just for debugging. Remove before release.
        TRAPD(err, i->transformToDetailPostSaveL(entry, item));
        if (err) {
            qDebug() << "transformToDetailPostSaveL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }

    // Update local id
    QOrganizerItemId itemId;
    TCalLocalUid localUid = entry.LocalUidL();
    itemId.setLocalId(localUid);
    itemId.setManagerUri(managerUri);
    item->setId(itemId);
}

void OrganizerItemTransform::toItemInstanceL(const CCalInstance &instance, QOrganizerItem *itemInstance) const
{
    //debugInstanceL(instance);

    // Loop through transform objects
    foreach (OrganizerItemDetailTransform *i, m_detailTransforms) 
    {
        TRAPD(err, i->transformToDetailL(instance, itemInstance));
        if (err) {
            // TODO: This is just for debugging. Remove before release.
            qDebug() << "toItemInstanceL failed! detail:" << i->detailDefinitionName() << "err:" << err;
            User::Leave(err);
        }
    }
}
