/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef ORGANIZERSYMBIANUTILS_H_
#define ORGANIZERSYMBIANUTILS_H_

#include <e32base.h>
#include <caltime.h>
#include <QString>
#include <QDateTime>
#include <QVariantMap>
#include <calcommon.h>
#ifdef SYMBIAN_CALENDAR_V2
// This file (calenmulticaluids.hrh) no longer exists in S^4, so use a local copy for now
#include "local_calenmulticaluids.hrh"
#include <calcalendarinfo.h>
#endif
#include "qorganizersymbian_p.h"

const int KPropertyKeyLen = 32;

namespace OrganizerSymbianUtils
{
    QString toQString(const TDesC8 &des);
    QString toQString(const TDesC16 &des);
    TPtrC8 toPtrC8(const QByteArray &bytes);
    TPtrC16 toPtrC16(const QString &string);
    TCalTime toTCalTimeL(const QDateTime &dateTime);
    TCalTime toTCalTimeL(const QDate &date);
    QDateTime toQDateTimeL(const TCalTime &calTime);
    TTime toTTime(const QDateTime &dateTime, Qt::TimeSpec timeSpec);
    QDateTime toQDateTime(const TTime &time, Qt::TimeSpec timeSpec);
    QOrganizerCollectionId toCollectionId(quint64 collectionId);
    QOrganizerItemId toItemId(quint64 collectionId, quint32 itemId);
    TCalLocalUid toTCalLocalUid(const QOrganizerItemId& itemId);
    quint64 toTCalCollectionId(const QOrganizerItemId& itemId);
    QOrganizerCollectionId getCollectionId(const QOrganizerItemId& itemId);
#ifdef SYMBIAN_CALENDAR_V2
    
    template<typename T>
    T getCalInfoPropertyL(const CCalCalendarInfo &calInfo, TCalenPropertyUid propertyUid)
    {    
        TBuf8<KPropertyKeyLen> keyBuff;
        keyBuff.AppendNum(propertyUid);
        TPckgBuf<T> valuePckgBuf;
        valuePckgBuf.Copy(calInfo.PropertyValueL(keyBuff));
        return valuePckgBuf();
    }
    
    template<typename T> 
    void setCalInfoPropertyL(CCalCalendarInfo *calInfo, TCalenPropertyUid propertyUid, const T &value)
    {    
        TBuf8<KPropertyKeyLen> keyBuff;
        keyBuff.AppendNum(propertyUid);
        TPckgC<T> valuePckg(value);
        calInfo->SetPropertyL(keyBuff, valuePckg);
    }
    
    void setCalInfoPropertyL(CCalCalendarInfo *calInfo, TCalenPropertyUid propertyUid, const QString &value);
    
    QVariantMap toMetaDataL(const CCalCalendarInfo &calInfo);
    CCalCalendarInfo* toCalInfoLC(QVariantMap metaData);
#endif
}

#endif // ORGANIZERSYMBIANUTILS_H_
