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
#ifndef ORGANIZERITEMDETAILTRANSFORM_H_
#define ORGANIZERITEMDETAILTRANSFORM_H_

#include <caltime.h>
#include <calentry.h>
#include "qorganizeritem.h"

QTM_USE_NAMESPACE

class OrganizerItemDetailTransform
{
public:
    OrganizerItemDetailTransform();
    virtual ~OrganizerItemDetailTransform();

    virtual void transformToDetailL(const CCalEntry& entry, QOrganizerItem *item) = 0;
    virtual void transformToDetailPostSaveL(const CCalEntry& entry, QOrganizerItem *item);
    virtual void transformToEntryL(const QOrganizerItem& item, CCalEntry* entry) = 0;
    virtual QString detailDefinitionName() = 0;

    static QString toQString(const TDesC8 &des);
    static QString toQString(const TDesC16 &des);
    static TPtrC8 toPtrC8(const QByteArray &bytes);
    static TPtrC16 toPtrC16(const QString &string);
    static TCalTime toTCalTimeL(QDateTime dateTime);
    static TCalTime toTCalTimeL(QDate date);
    static QDateTime toQDateTimeL(TCalTime calTime);
};

#endif /* ORGANIZERITEMDETAILTRANSFORM_H_ */
