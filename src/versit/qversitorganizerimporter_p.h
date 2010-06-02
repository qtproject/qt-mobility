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
#ifndef QVERSITORGANIZERIMPORTER_P_H
#define QVERSITORGANIZERIMPORTER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qmobilityglobal.h"
#include "qversitorganizerimporter.h"

QTM_BEGIN_NAMESPACE
class QOrganizerItem;
class QOrganizerItemDetail;
class QVersitProperty;
class QVersitDocument;

class Duration
{
    public:
        Duration() :
            mNegative(false), mWeeks(0), mDays(0), mHours(0), mMinutes(0), mSeconds(0), mValid(true)
            {}
        static Duration invalidDuration() { Duration d; d.setValid(false); return d; }
        static Duration parseDuration(QString str);
        static void parseDurationTime(QString* str, Duration* dur);
        static void parseDurationMinutes(QString* str, Duration* dur);
        static void parseDurationSeconds(QString* str, Duration* dur);
        static QString nextToken(QString* str);

        void setNegative(bool neg) { mNegative = neg; }
        void setWeeks(int weeks) { mWeeks = weeks; }
        void setDays(int days) { mDays = days; }
        void setHours(int hours) { mHours = hours; }
        void setMinutes(int minutes) { mMinutes = minutes; }
        void setSeconds(int seconds) { mSeconds = seconds; }
        void setValid(bool val) { mValid = val; }
        int weeks() { return mNegative ? -mWeeks : mWeeks; }
        int days() { return mNegative ? -mDays : mDays; }
        int hours() { return mNegative ? -mHours : mHours; }
        int minutes() { return mNegative ? -mMinutes : mMinutes; }
        int seconds() { return mNegative ? -mSeconds : mSeconds; }
        bool isValid() { return mValid; }

    private:
        bool mNegative;
        int mWeeks;
        int mDays;
        int mHours;
        int mMinutes;
        int mSeconds;
        bool mValid;
};

class Q_AUTOTEST_EXPORT QVersitOrganizerImporterPrivate
{
public:
    QVersitOrganizerImporterPrivate();
    ~QVersitOrganizerImporterPrivate();
    bool importDocument(const QVersitDocument& versitDocument,
                        QOrganizerItem* item,
                        QVersitOrganizerImporter::Error* error);
    void importProperty(const QVersitDocument& document,
                        const QVersitProperty& property,
                        QOrganizerItem* item);

    QList<QOrganizerItem> mItems;
    QMap<int, QVersitOrganizerImporter::Error> mErrors;
    QVersitOrganizerImporterPropertyHandler* mPropertyHandler;
    QVersitDefaultResourceHandler* mDefaultResourceHandler;
    QVersitResourceHandler* mResourceHandler;

private:
    bool createDisplayLabel(const QVersitProperty& property,
                            QOrganizerItem* item,
                            QList<QOrganizerItemDetail>* updatedDetails);
    bool createTimestampCreated(const QVersitProperty& property,
                                QOrganizerItem* item,
                                QList<QOrganizerItemDetail>* updatedDetails);
    bool createTimestampModified(const QVersitProperty& property,
                                 QOrganizerItem* item,
                                 QList<QOrganizerItemDetail>* updatedDetails);
    bool createStartDateTime(const QVersitProperty& property,
                             QOrganizerItem* item,
                             QList<QOrganizerItemDetail>* updatedDetails);
    bool createEndDateTime(const QVersitProperty& property,
                           QOrganizerItem* item,
                           QList<QOrganizerItemDetail>* updatedDetails);
    bool createDuration(const QVersitProperty& property,
                        QOrganizerItem* item,
                        QList<QOrganizerItemDetail>* updatedDetails);
    QDateTime parseDateTime(QString str);
};

QTM_END_NAMESPACE

#endif
