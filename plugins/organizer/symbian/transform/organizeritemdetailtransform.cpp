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
#include "organizeritemdetailtransform.h"

OrganizerItemDetailTransform::OrganizerItemDetailTransform()
{

}

OrganizerItemDetailTransform::~OrganizerItemDetailTransform()
{

}

QString OrganizerItemDetailTransform::toQString(const TDesC8 &des)
{
    return QString::fromUtf8((const char *)des.Ptr(), des.Length());
}

QString OrganizerItemDetailTransform::toQString(const TDesC16 &des)
{
    return QString::fromUtf16(des.Ptr(), des.Length());
}

TPtrC8 OrganizerItemDetailTransform::toPtrC8(const QString &string)
{
    return TPtrC8(reinterpret_cast<const TUint8*>(string.data()));
}

TPtrC16 OrganizerItemDetailTransform::toPtrC16(const QString &string)
{
    return TPtrC16(reinterpret_cast<const TUint16*>(string.utf16()));
}

TCalTime OrganizerItemDetailTransform::toTCalTimeL(QDateTime dateTime)
{
    TCalTime calTime;
    calTime.SetTimeUtcL(Time::NullTTime());

    if (dateTime.isValid()) {
        uint secondsFrom1970 = dateTime.toTime_t();
        quint64 usecondsFrom1970 = ((quint64) secondsFrom1970) * ((quint64) 1000000) + ((quint64) dateTime.time().msec() * (quint64)1000);
        TTime time1970(_L("19700000:000000.000000"));
        quint64 usecondsBCto1970 = time1970.MicroSecondsFrom(TTime(0)).Int64();
        quint64 useconds = usecondsBCto1970 + usecondsFrom1970;

        TTime time(useconds);
        calTime.SetTimeUtcL(time);
    }

    return calTime;
}

QDateTime OrganizerItemDetailTransform::toQDateTimeL(TCalTime calTime)
{
    const TTime time1970(_L("19700000:000000.000000"));
    quint64 usecondsBCto1970 = time1970.MicroSecondsFrom(TTime(0)).Int64();
    quint64 useconds = calTime.TimeUtcL().Int64() - usecondsBCto1970;
    quint64 seconds = useconds / (quint64)1000000;
    quint64 msecondscomponent = (useconds - seconds * (quint64)1000000) / (quint64)1000;
    QDateTime dateTime;
    dateTime.setTime_t(seconds);
    dateTime.setTime(dateTime.time().addMSecs(msecondscomponent));
    return dateTime;
}
