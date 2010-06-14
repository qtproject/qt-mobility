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

#ifndef ORGANIZERITEMTRANSFORM_H_
#define ORGANIZERITEMTRANSFORM_H_

#include <calentry.h>
#include <caltime.h>
#include <QDateTime>
#include <qmobilityglobal.h>
#include <qorganizeritemrecurrencerule.h>

QTM_BEGIN_NAMESPACE
class QOrganizerItem;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CCalInstance;

class OrganizerItemTransform
{
public:
    OrganizerItemTransform();
    ~OrganizerItemTransform();

    CCalEntry *toEntryLC(const QOrganizerItem &item);
    void toItemL(const CCalEntry &entry, QOrganizerItem *item) const;
    void toItemL(const CCalInstance &instance, QOrganizerItem *item) const;

    HBufC8 *guidLC(const QOrganizerItem &item);

    CCalEntry::TType entryTypeL(const QOrganizerItem &item) const;
    QString itemTypeL(const CCalEntry &entry) const;

    static QString toQString(const TDesC8 &des);
    static QString toQString(const TDesC16 &des);
    TPtrC8 toPtrC8(const QString &string) const;
    TPtrC16 toPtrC16(const QString &string) const;

    TCalTime toTCalTime(QDateTime dateTime) const;
    QDateTime toQDateTime(TCalTime calTime) const;

    void toTCalTimes(const QList<QDate> &dateTimes, RArray<TCalTime> &calDates) const;
    QList<QDate> toQDates(const RArray<TCalTime> &calDates) const;

    TCalRRule toCalRRule(QList<QOrganizerItemRecurrenceRule> rules) const;
    QList<QOrganizerItemRecurrenceRule> toItemRecurrenceRules(TCalRRule calRule) const;

private:
    void debugEntry(const CCalEntry &entry) const;
};

#endif /* ORGANIZERITEMTRANSFORM_H_ */
