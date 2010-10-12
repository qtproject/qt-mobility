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

#ifndef TIMEZONES_P_H
#define TIMEZONES_P_H

#include <QDateTime>
#include <QHash>
#include "qorganizerrecurrencerule.h"

QTM_BEGIN_NAMESPACE
class QOrganizerManager;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

class TimeZonePhase {
    public:
        TimeZonePhase() : mStandard(true), mUtcOffset(100000) {} // invalid offset
        void setStandard(bool standard) { mStandard = standard; }
        bool isStandard() const { return mStandard; }
        void setUtcOffset(int offset) { mUtcOffset = offset; }
        int utcOffset() const { return mUtcOffset; }
        void setStartDateTime(const QDateTime& dateTime) { mStartDateTime = dateTime; }
        QDateTime startDateTime() const { return mStartDateTime; }
        void setRecurrenceRule(const QOrganizerRecurrenceRule& rrule) { mRecurrenceRule = rrule; }
        QOrganizerRecurrenceRule recurrenceRule() const { return mRecurrenceRule; }
        void setRecurrenceDates(const QSet<QDate>& rdates) { mRecurrenceDates = rdates; }
        QSet<QDate> recurrenceDates() const { return mRecurrenceDates; }
        bool isValid() const {
            // offset must be within -24 hours to +24 hours
            return mStartDateTime.isValid() && mUtcOffset < 86400 && mUtcOffset > -86400;
        }
    private:
        bool mStandard; // true for STANDARD, false for DAYLIGHT
        int mUtcOffset; // in seconds, the offset to apply after mStartDateTime
        QDateTime mStartDateTime; // local time, when the phase comes into effect
        QOrganizerRecurrenceRule mRecurrenceRule;
        QSet<QDate> mRecurrenceDates;
};

class TimeZone {
    public:
        QDateTime convert(const QDateTime& dateTime) const;
        void setTzid(const QString& tzid) { mTzid = tzid; }
        QString tzid() const { return mTzid; }
        void addPhase(const TimeZonePhase& phase) { mPhases.append(phase); }
        bool isValid() const {
            foreach (const TimeZonePhase& phase, mPhases) {
                if (!phase.isValid()) return false;
            }
            return !mPhases.isEmpty();
        }

    private:
        static QOrganizerManager* getManager();
        QString mTzid;
        QList<TimeZonePhase> mPhases;
};

class TimeZones {
    public:
        QDateTime convert(const QDateTime& dateTime, const QString& tzid) const;
        void addTimeZone(const TimeZone& timezone) {
            if (!timezone.tzid().isEmpty())
                    mTimeZones.insert(timezone.tzid(), timezone);
        }
    private:
        QHash<QString, TimeZone> mTimeZones;
};

#endif
