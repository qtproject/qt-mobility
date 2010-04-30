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

#ifndef QORGANIZERITEMATTENDEE_H
#define QORGANIZERITEMATTENDEE_H

#include <QString>

#include "qtorganizeritemsglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritem.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_CALENDAR_EXPORT QOrganizerItemAttendee : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldAttendeeName;
    const char* FieldAttendeeContactUri;
    const char* FieldParticipationStatus;
#else
    Q_DECLARE_CUSTOM_CALENDAR_DETAIL(QOrganizerItemAttendee, "Attendee")
    Q_DECLARE_LATIN1_CONSTANT(FieldAttendeeDisplayLabel, "AttendeeDisplayLabel");
    Q_DECLARE_LATIN1_CONSTANT(FieldAttendeeContactUri, "AttendeeContactUri");
    Q_DECLARE_LATIN1_CONSTANT(FieldAttendeeParticipationStatus, "AttendeeParticipationStatus");
#endif

    void setAttendeeDisplayLabel(const QString& attendeeDisplayLabel) {setValue(FieldAttendeeDisplayLabel, attendeeDisplayLabel);}
    QString attendeeDisplayLabel() const {return value(FieldAttendeeDisplayLabel);}

    void setAttendeeContactUri(const QString& contactUri) {setValue(FieldAttendeeContactUri, contactUri);}
    QString attendeeContactUri() const {return value(FieldAttendeeContactUri);}

    // TODO: participationstatus: enum comes from where?
};

QTM_END_NAMESPACE

#endif

