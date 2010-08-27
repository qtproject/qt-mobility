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

#ifndef QCONTACTANNIVERSARY_H
#define QCONTACTANNIVERSARY_H

#include <QString>

#include "qtcontactsglobal.h"
#include "qcontactdetail.h"
#include "qcontact.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CONTACTS_EXPORT QContactAnniversary : public QContactDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldCalendarId;
    static const QLatin1Constant FieldOriginalDate;
    static const QLatin1Constant FieldEvent;
    static const QLatin1Constant FieldSubType;
    static const QLatin1Constant SubTypeWedding;
    static const QLatin1Constant SubTypeEngagement;
    static const QLatin1Constant SubTypeHouse;
    static const QLatin1Constant SubTypeEmployment;
    static const QLatin1Constant SubTypeMemorial;
#else
    Q_DECLARE_CUSTOM_CONTACT_DETAIL(QContactAnniversary, "Anniversary")
    Q_DECLARE_LATIN1_CONSTANT(FieldCalendarId, "CalendarId");
    Q_DECLARE_LATIN1_CONSTANT(FieldOriginalDate, "OriginalDate");
    Q_DECLARE_LATIN1_CONSTANT(FieldEvent, "Event");
    Q_DECLARE_LATIN1_CONSTANT(FieldSubType, "SubType");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeWedding, "Wedding");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeEngagement, "Engagement");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeHouse, "House");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeEmployment, "Employment");
    Q_DECLARE_LATIN1_CONSTANT(SubTypeMemorial, "Memorial");
#endif

    void setOriginalDate(const QDate& date) {setValue(FieldOriginalDate, date);}
    QDate originalDate() const {return value<QDate>(FieldOriginalDate);}
    void setCalendarId(const QString& calendarId) {setValue(FieldCalendarId, calendarId);}
    QString calendarId() const {return value(FieldCalendarId);}
    void setEvent(const QString& event) {setValue(FieldEvent, event);}
    QString event() const {return value(FieldEvent);}

    void setSubType(const QString& subType) {setValue(FieldSubType, subType);}
    QString subType() const {return value(FieldSubType);}
};

QTM_END_NAMESPACE

#endif

