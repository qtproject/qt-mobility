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


#ifndef QORGANIZERITEMREMINDER_H
#define QORGANIZERITEMREMINDER_H

#include <QtDebug>
#include <QString>

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritem.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerItemReminder : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    const char* DefinitionName;
    const char* FieldReminderTypes;
    const char* FieldTimeDelta;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerItemReminder, "Reminder")
    Q_DECLARE_LATIN1_CONSTANT(FieldReminderTypes, "ReminderTypes");
    Q_DECLARE_LATIN1_CONSTANT(FieldTimeDelta, "TimeDelta");
#endif

    enum ReminderType {
        NoReminder = 0,
        VisualReminder = 0x01,
        AudibleReminder = 0x02,
        TactileReminder = 0x04
    };
    Q_DECLARE_FLAGS(ReminderTypes, ReminderType)

    void setReminderTypes(ReminderTypes reminderTypes) {setValue(FieldReminderTypes, static_cast<int>(reminderTypes));}
    ReminderTypes reminderTypes() const {return static_cast<ReminderTypes>(value<int>(FieldReminderTypes));}

    void setTimeDelta(int secondsBefore) {setValue(FieldTimeDelta, secondsBefore);}
    int timeDelta() const {return value<int>(FieldTimeDelta);}

    // XXX TODO: do we need a reminder priority?
    // highest priority = will cancel phone calls to remind you, lowest = only if the device is idle?
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QOrganizerItemReminder::ReminderTypes);

QTM_END_NAMESPACE

#endif

