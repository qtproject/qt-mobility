/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/


#ifndef QORGANIZERITEMREMINDER_H
#define QORGANIZERITEMREMINDER_H

#include <QString>
#include <QUrl>
#include <QList>
#include <QVariantList>

#include "qtorganizerglobal.h"
#include "qorganizeritemdetail.h"
#include "qorganizeritem.h"

QTM_BEGIN_NAMESPACE

/* Leaf class */
class Q_ORGANIZER_EXPORT QOrganizerItemReminder : public QOrganizerItemDetail
{
public:
#ifdef Q_QDOC
    static const QLatin1Constant DefinitionName;
    static const QLatin1Constant FieldSecondsBeforeStart;
    static const QLatin1Constant FieldRepetitionCount;
    static const QLatin1Constant FieldRepetitionDelay;
#else
    Q_DECLARE_CUSTOM_ORGANIZER_DETAIL(QOrganizerItemReminder, "Reminder")
    Q_DECLARE_LATIN1_CONSTANT(FieldSecondsBeforeStart, "SecondsBeforeStart");
    Q_DECLARE_LATIN1_CONSTANT(FieldRepetitionCount, "RepetitionCount");
    Q_DECLARE_LATIN1_CONSTANT(FieldRepetitionDelay, "RepetitionDelay");
#endif

    enum ReminderType {
        NoReminder = 0,
        VisualReminder,
        AudibleReminder,
        EmailReminder

        // other types of reminders?
        //ProcedureReminder,
        //TactileReminder,
    };

    ReminderType reminderType() const;

    void setSecondsBeforeStart(int seconds) {setValue(FieldSecondsBeforeStart, seconds);}
    int secondsBeforeStart() const {return value<int>(FieldSecondsBeforeStart);}

    // duration/repetition
    void setRepetition(int count, int delaySeconds) {setValue(FieldRepetitionCount, count); setValue(FieldRepetitionDelay, delaySeconds);}
    int repetitionDelay() const {return value<int>(FieldRepetitionDelay);}
    int repetitionCount() const {return value<int>(FieldRepetitionCount);}

protected:
    QOrganizerItemReminder(const char* definitionName) : QOrganizerItemDetail(definitionName) {}
    QOrganizerItemReminder(const QOrganizerItemDetail& detail, const char* definitionName) : QOrganizerItemDetail(detail, definitionName) {}
};

#define Q_DECLARE_CUSTOM_ORGANIZER_REMINDER_DETAIL(className, definitionNameString) \
    className() : QOrganizerItemReminder(DefinitionName.latin1()) {} \
    className(const QOrganizerItemDetail& field) : QOrganizerItemReminder(field, DefinitionName.latin1()) {} \
    className& operator=(const QOrganizerItemDetail& other) {assign(other, DefinitionName.latin1()); return *this;} \
    \
    Q_DECLARE_LATIN1_CONSTANT(DefinitionName, definitionNameString);

#define Q_IMPLEMENT_CUSTOM_ORGANIZER_REMINDER_DETAIL(className, definitionNameString) \
    Q_DEFINE_LATIN1_CONSTANT(className::DefinitionName, definitionNameString)

QTM_END_NAMESPACE

#endif

