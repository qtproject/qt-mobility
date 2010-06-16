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
#include "organizeritemguidtransform.h"

const TInt KGuidLength = 30;

OrganizerItemGuidTransform::OrganizerItemGuidTransform()
{
    // Set seed for qrand()
    TTime homeTime;
    homeTime.HomeTime();
    uint seed = (uint) homeTime.Int64();
    qsrand(seed);
}

void OrganizerItemGuidTransform::transformToDetailL(const CCalEntry& entry, QOrganizerItem *item)
{
    QString guid = toQString(entry.UidL());
    if (!guid.isEmpty())
        item->setGuid(guid);
}

void OrganizerItemGuidTransform::transformToEntryL(const QOrganizerItem& item, CCalEntry* entry)
{
    Q_UNUSED(item);
    Q_UNUSED(entry);
    // Not used. Guid is already set when CCalEntry was created.
}

HBufC8 *OrganizerItemGuidTransform::guidLC(const QOrganizerItem &item)
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
