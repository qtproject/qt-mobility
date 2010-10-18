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

#include "organizersymbianutils.h"
#include <badesca.h>
#include <QColor>

namespace OrganizerSymbianUtils
{

QString toQString(const TDesC8 &des)
{
    return QString::fromUtf8((const char *)des.Ptr(), des.Length());
}

QString toQString(const TDesC16 &des)
{
    return QString::fromUtf16(des.Ptr(), des.Length());
}

TPtrC8 toPtrC8(const QByteArray &bytes)
{
    return TPtrC8(reinterpret_cast<const TUint8*>(bytes.constData()), bytes.size());
}

TPtrC16 toPtrC16(const QString &string)
{
    return TPtrC16(reinterpret_cast<const TUint16*>(string.utf16()), string.size());
}

TCalTime toTCalTimeL(const QDateTime &dateTime)
{
    TCalTime calTime;
    calTime.SetTimeUtcL(Time::NullTTime());
    if (dateTime.isValid())      
        calTime.SetTimeLocalL(toTTime(dateTime, Qt::LocalTime));
    return calTime;
}

TCalTime toTCalTimeL(const QDate &date)
{
    TCalTime calTime;
    calTime.SetTimeUtcL(Time::NullTTime());
    if (date.isValid())
        calTime = toTCalTimeL(QDateTime(date));
    return calTime;
}

QDateTime toQDateTimeL(const TCalTime &calTime)
{
    // NOTE: 
    // The calendar/agenda server has a problem with UTC times. It will move
    // the UTC time with daylight saving time. UTC time should be fixed 
    // always and the actual local time for a calendar entry should be 
    // calculated from the UTC time. Fortunately local time's reported
    // from the server are correct and using it with TCalTime/QDateTime
    // conversions seems work as as workaround for this issue.
    return toQDateTime(calTime.TimeLocalL(), Qt::LocalTime);
}

TTime toTTime(const QDateTime &dateTime, Qt::TimeSpec timeSpec)
{
    TTime time = Time::NullTTime();
    if (dateTime.isValid()) {
        QDateTime qdt = dateTime.toTimeSpec(timeSpec);
        // NOTE: TDateTime month & day start from zero
        TDateTime tdt(qdt.date().year(), TMonth(qdt.date().month()-1), qdt.date().day()-1,
            qdt.time().hour(), qdt.time().minute(), qdt.time().second(), qdt.time().msec()*1000);
        time = TTime(tdt);
    }
    return time;
}

QDateTime toQDateTime(const TTime &time, Qt::TimeSpec timeSpec)
{
    QDateTime qdt;
    if (time != Time::NullTTime()) {
        TDateTime tdt = time.DateTime();
        qdt.setTimeSpec(timeSpec);
        // NOTE: TDateTime month & day start from zero
        qdt.setDate(QDate(tdt.Year(), tdt.Month()+1, tdt.Day()+1));
        qdt.setTime(QTime(tdt.Hour(), tdt.Minute(), tdt.Second(), tdt.MicroSecond()/1000));
    }
    return qdt.toTimeSpec(Qt::LocalTime); // return with default timespec set
}

QOrganizerCollectionLocalId toCollectionLocalId(quint64 collectionId)
{
    return QOrganizerCollectionLocalId(new QOrganizerCollectionSymbianEngineLocalId(collectionId));
}

QOrganizerItemLocalId toItemLocalId(quint64 collectionId, quint32 itemId)
{
    return QOrganizerItemLocalId(new QOrganizerItemSymbianEngineLocalId(collectionId, itemId));
}

TCalLocalUid toTCalLocalUid(const QOrganizerItemLocalId& itemLocalId)
{
    // TODO: should we have a check for engineLocalIdType here?
    return static_cast<QOrganizerItemSymbianEngineLocalId*>(QOrganizerItemManagerEngine::engineLocalItemId(itemLocalId))->calLocalUid();
}

quint64 toTCalCollectionId(const QOrganizerItemLocalId& itemLocalId)
{
    // TODO: should we have a check for engineLocalIdType here?
    return static_cast<QOrganizerItemSymbianEngineLocalId*>(QOrganizerItemManagerEngine::engineLocalItemId(itemLocalId))->calCollectionId();
}

QOrganizerCollectionLocalId getCollectionLocalId(const QOrganizerItemLocalId& itemLocalId)
{
    // TODO: should we have a check for engineLocalIdType here?
    quint64 calCollectionId  = static_cast<QOrganizerItemSymbianEngineLocalId*>(QOrganizerItemManagerEngine::engineLocalItemId(itemLocalId))->calCollectionId();
    return toCollectionLocalId(calCollectionId);
}

#ifdef SYMBIAN_CALENDAR_V2

void setCalInfoPropertyL(CCalCalendarInfo *calInfo, TCalenPropertyUid propertyUid, const QString &value)
{    
    TBuf8<KPropertyKeyLen> keyBuff;
    keyBuff.AppendNum(propertyUid);
    QByteArray bytes = value.toUtf8();
    TPtrC8 des = toPtrC8(bytes);
    calInfo->SetPropertyL(keyBuff, des);
}

QVariantMap toMetaDataL(const CCalCalendarInfo &calInfo)
{
    QVariantMap metaData;
    
    // TODO: we should define the keys somewhere
    metaData.insert("IsValid", (bool) calInfo.IsValid());
    metaData.insert("Name", toQString(calInfo.NameL()));
    metaData.insert("FileName", toQString(calInfo.FileNameL()));
    metaData.insert("Description", toQString(calInfo.DescriptionL()));
    TRgb color = calInfo.Color();
    QColor qcolor(color.Red(), color.Green(), color.Blue(), color.Alpha());
    metaData.insert("Color", qcolor);
    metaData.insert("Enabled", (bool) calInfo.Enabled());
    
    CDesC8Array* keys = calInfo.PropertyKeysL();
    CleanupStack::PushL(keys);
    for (int i=0; i<keys->Count(); i++) {
        
        // Get key and value 
        QString propKey = toQString(keys->MdcaPoint(i));
        TPtrC8 propValue = calInfo.PropertyValueL(keys->MdcaPoint(i));
           
        // Try converting the key to int
        bool ok = false;
        int calenPropertyUid = propKey.toInt(&ok);
        if (!ok) {
            // Default conversion to byte array
            QByteArray value((const char*) propValue.Ptr(), propValue.Size());
            metaData.insert(propKey, value);
            continue;
        }

        // Convert the predefined properties
        if (calenPropertyUid == EFolderLUID) {
            TPckgBuf<TUint> value;
            value.Copy(propValue);
            metaData.insert("FolderLUID", (uint) value());
        } else if (calenPropertyUid == ECreationTime) {
            TPckgBuf<TTime> value;
            value.Copy(propValue);
            metaData.insert("CreationTime", toQDateTime(value(), Qt::UTC));
        } else if (calenPropertyUid == EModificationTime) {
            TPckgBuf<TTime> value;
            value.Copy(propValue);
            metaData.insert("ModificationTime", toQDateTime(value(), Qt::UTC));
        } else if (calenPropertyUid == ESyncStatus) {
            TPckgBuf<TBool> value;
            value.Copy(propValue);
            metaData.insert("SyncStatus", (bool) value());
        } else if (calenPropertyUid == EIsSharedFolder) {
            TPckgBuf<TBool> value;
            value.Copy(propValue);
            metaData.insert("IsSharedFolder", (bool) value());
        } else if (calenPropertyUid == EGlobalUUID) {
            metaData.insert("GlobalUUID", toQString(propValue));
        } else if (calenPropertyUid == EDeviceSyncServiceOwner) {
            TPckgBuf<TUint> value;
            value.Copy(propValue);
            metaData.insert("DeviceSyncServiceOwner", (uint) value());
        } else if (calenPropertyUid == EOwnerName) {
            metaData.insert("OwnerName", toQString(propValue));
        } else if (calenPropertyUid == EMarkAsDelete) {
            TPckgBuf<TBool> value;
            value.Copy(propValue);            
            metaData.insert("MarkAsDelete", (bool) value());
        } else {
            // Default conversion for unknown property
            QByteArray value((const char*) propValue.Ptr(), propValue.Size());
            metaData.insert(propKey, value);
        }
        // TODO: EDeviceSyncProfileID can't find any reference of the type.. uint?
        // TODO: ESyncConfigEnabled can't find any reference of the type.. bool?
    }
    
    CleanupStack::PopAndDestroy(keys);     
    
    return metaData;
}

CCalCalendarInfo* toCalInfoLC(QVariantMap metaData)
{
    // Create a new calendar info
    CCalCalendarInfo* calInfo = CCalCalendarInfo::NewL();
    CleanupStack::PushL(calInfo);
    
    // TODO: we should define the keys somewhere
    
    // Filename
    QString fileName = metaData.value("FileName").toString();
    metaData.remove("FileName");
    if (fileName.isEmpty())
        User::Leave(KErrArgument); // mandatory parameter
    // NOTE: filename is set only when creating a new calendar...

    // Name
    QString name = metaData.value("Name").toString();
    metaData.remove("Name");
    if (!name.isEmpty())
        calInfo->SetNameL(toPtrC16(name));
    
    // Description
    QString description = metaData.value("Description").toString();
    metaData.remove("Description");
    if (!description.isEmpty())
        calInfo->SetDescriptionL(toPtrC16(description));
    
    // Color
    if (metaData.keys().contains("Color")) {
        QColor qcolor = metaData.value("Color").value<QColor>();
        TRgb color(qcolor.red(), qcolor.green(), qcolor.blue(), qcolor.alpha());
        calInfo->SetColor(color);
        metaData.remove("Color");
    }
    
    // Enabled
    if (metaData.keys().contains("Enabled")) {
        calInfo->SetEnabled(metaData.value("Enabled").toBool());
        metaData.remove("Enabled");
    }
        
    // Set remaining metadata as properties
    foreach (QString key, metaData.keys()) {
        
        QVariant value = metaData.value(key);
        
        // Set known properties by converting to correct type
        if (key == "FolderLUID") {
            setCalInfoPropertyL(calInfo, EFolderLUID, (TUint) value.toUInt());
        } else if (key == "CreationTime") {
            setCalInfoPropertyL(calInfo, ECreationTime, toTTime(value.toDateTime(), Qt::UTC));
        } else if (key == "ModificationTime") {
            setCalInfoPropertyL(calInfo, EModificationTime, toTTime(value.toDateTime(), Qt::UTC));
        } else if (key == "SyncStatus") {
            setCalInfoPropertyL(calInfo, ESyncStatus, (TBool) value.toBool());
        } else if (key == "IsSharedFolder") {
            setCalInfoPropertyL(calInfo, EIsSharedFolder, (TBool) value.toBool());
        } else if (key == "GlobalUUID") {
            setCalInfoPropertyL(calInfo, EGlobalUUID, value.toString());
        } else if (key == "DeviceSyncServiceOwner") {
            setCalInfoPropertyL(calInfo, EDeviceSyncServiceOwner, (TUint) value.toUInt());
        } else if (key == "OwnerName") {
            setCalInfoPropertyL(calInfo, EOwnerName, value.toString());
        } else if (key == "MarkAsDelete") {
            setCalInfoPropertyL(calInfo, EMarkAsDelete, (TBool) value.toBool());
        } else {
            // Default conversion for unknown property
            QByteArray keyBytes = key.toUtf8();
            TPtrC8 propName = toPtrC8(keyBytes);
            QByteArray valueBytes = metaData.value(key).toByteArray();
            TPtrC8 propValue = toPtrC8(valueBytes);
            calInfo->SetPropertyL(propName, propValue);
        }
        // TODO: EDeviceSyncProfileID can't find any reference of the type.. uint?
        // TODO: ESyncConfigEnabled can't find any reference of the type.. bool?
    }    
    return calInfo;
}
#endif // SYMBIAN_CALENDAR_V2

} // namespace OrganizerSymbianUtils
