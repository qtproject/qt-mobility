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

#ifndef QORGANIZEREVENTRSVP_H
#define QORGANIZEREVENTRSVP_H

#include <QString>

#include "qorganizereventattendee.h"

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritemfilter.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerEventRsvp : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldParticipationStatus;
    static const QLatin1Constant FieldParticipationRole;
    static const QLatin1Constant FieldResponseRequirement;
    static const QLatin1Constant FieldResponseDeadline;
    static const QLatin1Constant FieldResponseDate;
    static const QLatin1Constant FieldOrganizerName;
    static const QLatin1Constant FieldOrganizerEmail;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerEventRsvp, "Rsvp")
    Q_DECLARE_LATIN1_CONSTANT(FieldParticipationStatus, "ParticipationStatus");
    Q_DECLARE_LATIN1_CONSTANT(FieldParticipationRole, "ParticipationRole");
    Q_DECLARE_LATIN1_CONSTANT(FieldResponseRequirement, "ResponseRequirement");
    Q_DECLARE_LATIN1_CONSTANT(FieldResponseDeadline, "ResponseDeadline");
    Q_DECLARE_LATIN1_CONSTANT(FieldResponseDate, "ResponseDate");
    Q_DECLARE_LATIN1_CONSTANT(FieldOrganizerName, "OrganizerName");
    Q_DECLARE_LATIN1_CONSTANT(FieldOrganizerEmail, "OrganizerEmail");
#endif

    enum ResponseRequirement {
        ResponseNotRequired = 0,
        ResponseRequired = 1
        // ResponsePreferred = 2?
    };

    void setParticipationStatus(QOrganizerEventAttendee::ParticipationStatus status) {setValue(FieldParticipationStatus, static_cast<int>(status));}
    QOrganizerEventAttendee::ParticipationStatus participationStatus() const {return static_cast<QOrganizerEventAttendee::ParticipationStatus>(value<int>(FieldParticipationStatus));}

    void setParticipationRole(QOrganizerEventAttendee::ParticipationRole role) {setValue(FieldParticipationRole, static_cast<int>(role));}
    QOrganizerEventAttendee::ParticipationRole participationRole() const {return static_cast<QOrganizerEventAttendee::ParticipationRole>(value<int>(FieldParticipationRole));}

    void setResponseRequirement(ResponseRequirement responseRequirement) {setValue(FieldResponseRequirement, static_cast<int>(responseRequirement));}
    ResponseRequirement responseRequirement() const {return static_cast<ResponseRequirement>(value<int>(FieldResponseRequirement));}

    void setResponseDeadline(const QDate& date) {setValue(FieldResponseDeadline, date);}
    QDate responseDeadline() const {return value<QDate>(FieldResponseDeadline);}

    void setResponseDate(const QDate& date) {setValue(FieldResponseDate, date);}
    QDate responseDate() const {return value<QDate>(FieldResponseDate);}

    void setOrganizerName(const QString& name) {setValue(FieldOrganizerName, name);}
    QString organizerName() const {return value(FieldOrganizerName);}

    void setOrganizerEmail(const QString& email) {setValue(FieldOrganizerEmail, email);}
    QString organizerEmail() const {return value(FieldOrganizerEmail);}

    // Convenience filter - eg "match any event which I'm definitely attending (StatusAccepted)"
    static QOrganizerItemFilter match(QOrganizerEventAttendee::ParticipationStatus status);
};

QTM_END_NAMESPACE

#endif

