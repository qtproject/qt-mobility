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

#ifndef QORGANIZEREVENTATTENDEE_H
#define QORGANIZEREVENTATTENDEE_H

#include <QString>

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritemfilter.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerEventAttendee : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldName;
    static const QLatin1Constant FieldEmailAddress;
    static const QLatin1Constant FieldContactId;
    static const QLatin1Constant FieldParticipationStatus;
    static const QLatin1Constant FieldParticipationRole;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerEventAttendee, "Attendee")
    Q_DECLARE_LATIN1_CONSTANT(FieldName, "Name");
    Q_DECLARE_LATIN1_CONSTANT(FieldEmailAddress, "EmailAddress");
    Q_DECLARE_LATIN1_CONSTANT(FieldContactId, "ContactId");
    Q_DECLARE_LATIN1_CONSTANT(FieldParticipationStatus, "ParticipationStatus");
    Q_DECLARE_LATIN1_CONSTANT(FieldParticipationRole, "ParticipationRole");
#endif

    enum ParticipationStatus {
        StatusUnknown = 0,
        StatusAccepted = 1,
        StatusDeclined = 2,
        StatusTentative = 3,
        StatusDelegated = 4,
        StatusInProcess = 5,
        StatusCompleted = 6
    };

    enum ParticipationRole {
        RoleUnknown = 0,
        RoleOrganizer = 1,
        RoleChairperson = 2,
        RoleHost = 3,
        RoleRequiredParticipant = 4,
        RoleParticipant = 5,
        RoleNonParticipant = 6
    };

    void setName(const QString& name) {setValue(FieldName, name);}
    QString name() const {return value(FieldName);}

    void setEmailAddress(const QString& emailAddress) {setValue(FieldEmailAddress, emailAddress);}
    QString emailAddress() const {return value(FieldEmailAddress);}

    void setParticipationStatus(ParticipationStatus status) {setValue(FieldParticipationStatus, status);}
    ParticipationStatus participationStatus() const {return static_cast<ParticipationStatus>(value<int>(FieldParticipationStatus));}

    void setParticipationRole(ParticipationRole role) {setValue(FieldParticipationRole, static_cast<int>(role));}
    ParticipationRole participationRole() const {return static_cast<ParticipationRole>(value<int>(FieldParticipationRole));}

    void setContactId(const QString& contactId) {setValue(FieldContactId, contactId);}
    QString contactId() const {return value(FieldContactId);}

    // XXX TODO: Convenience filter
    //static QOrganizerItemFilter match(const QString& labelSubstring, Status status); // match person with status... intrafilter needed...
};

QTM_END_NAMESPACE

#endif


