/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

//TESTED_COMPONENT=src/systeminfo

#include <QtTest/QtTest>
#include "qsysteminfo.h"
#include "qsystembatteryinfo.h"
#include "qsysteminfotestcommon.h"
#include <e32property.h>
#include <hwrmlight.h>
#include <avkondomainpskeys.h>
#include <hwrmdomainpskeys.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <e32std.h>
#include <QSignalSpy>
#include <qthread.h>
#include <QtDebug>
#include <f32file.h>
#include <usbman.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <hwrmpower.h>
#include <QSignalSpy>
#include <qthread.h>
#include <qnamespace.h>
#include <f32file.h>
#include <centralrepository.h>
#include <AvkonInternalCRKeys.h>
#include <AknFepInternalPSKeys.h>
#include <hwrmvibrasdkcrkeys.h>
#include <ProfileEngineSDKCRKeys.h>


QTM_USE_NAMESPACE
Q_DECLARE_METATYPE(QSystemBatteryInfo::BatteryStatus);
Q_DECLARE_METATYPE(QSystemBatteryInfo::EnergyUnit);
Q_DECLARE_METATYPE(QSystemBatteryInfo::ChargingState);
Q_DECLARE_METATYPE(QSystemBatteryInfo::ChargerType);
Q_DECLARE_METATYPE(QSystemDeviceInfo::InputMethodFlags);
Q_DECLARE_METATYPE(QSystemDeviceInfo::KeyboardTypeFlags);
Q_DECLARE_METATYPE(QSystemDeviceInfo::LockTypeFlags);
Q_DECLARE_METATYPE(QSystemDeviceInfo::Profile);
Q_DECLARE_METATYPE(QSystemDeviceInfo::LockType);
Q_DECLARE_METATYPE(QSystemDeviceInfo::KeypadType);
Q_DECLARE_METATYPE(QSystemInfo::Feature);
Q_DECLARE_METATYPE(QSystemStorageInfo::StorageState);



const TUint32 KAknKeyboardType = 0x0000000B;

class SleeperThread : public QThread
{
public:
 static void msleep(unsigned long msecs)
     {
     QThread::msleep(msecs);
     }
};


LOCAL_C TInt CallStop( TAny* aWait )
    {
    (static_cast<CActiveSchedulerWait*>(aWait))->AsyncStop();
     return 1;
    }

LOCAL_C void WaitActive( TInt aTime )
    {
     CActiveSchedulerWait*  wait = new (ELeave) CActiveSchedulerWait();
     CPeriodic*  periodic = CPeriodic::NewL( CActive::EPriorityIdle );
     periodic->Start(aTime, aTime, TCallBack( CallStop, wait ) );
     wait->Start();
     delete periodic;
     delete wait;
    }



class tst_QSystemDeviceInfo : public QObject
{
    Q_OBJECT
public :
    //tst_QSystemDeviceInfo();
private slots:
    void tst_lockStatusKeypadLocked();
    void tst_lockStatusDeviceLocked();
    void tst_lockStatusDeviceUnknownLock();

    void tst_isKeyboardFlippedOpen();
    void flipStatus(bool b);
    void updateLockStatus(QSystemDeviceInfo::LockType lock);

    void tst_keypadLightOn_keypadLocked();
    void tst_keypadLightOn_deviceLocked();
    void tst_keypadLightOn_deviceUnlockedFlipOpen();
    void tst_inputMethodType();
    void tst_keyboardType();
    void tst_isWirelessKeyboardConnected();
    void tst_vibrationVolumeSettings();
    void tst_uniqueDeviceID();
};
/*tst_QSystemDeviceInfo::tst_QSystemDeviceInfo()
{
    //qRegisterMetaType<QSystemDeviceInfo::LockType>("QSystemDeviceInfo::LockTypeFlags");
    //qRegisterMetaType<QSystemDeviceInfo::LockType>("QSystemDeviceInfo::LockType");
}*/

class tst_QSystemDisplayInfo : public QObject
{
    Q_OBJECT

private slots:

    void tst_getOrientation();
    void tst_contrast();
    void tst_getDPIWidth();
    void tst_getDPIHeight();
    void tst_physicalHeight();
    void tst_physicalWidth();
};

class tst_QSystemStorageInfo : public QObject
{
    Q_OBJECT

private slots:
    void tst_uriForDrive();
    void tst_getPhoneMemory();
    void tst_getMassMemory();
    void tst_PhoneMemoryThresholdNotifications();
    void DeleteFiles(QSystemStorageInfo* aSti, const TInt &aCount, QSystemStorageInfo::DriveType &aVolType, TBool &aMassDriveExist);
};

class tst_QSystemGeneralInfo : public QObject
{
    Q_OBJECT

private slots:
    void tst_getFmTransmitterFeature();
};

void tst_QSystemStorageInfo::tst_getPhoneMemory()
    {
    qDebug() << " tst_QSystemStorageInfo::tst_getPhoneMemory() - Begin ";
    QSystemStorageInfo *sti = new QSystemStorageInfo(this);
    QStringList vols = sti->logicalDrives();
    bool phoneMemory(false);
    foreach (QString volName, vols) {
        QSystemStorageInfo::DriveType volType;
        volType = sti->typeForDrive(volName);
        if (volType == QSystemStorageInfo::RamDrive) {
            phoneMemory = true;
            qDebug() << "Phone Memory Supported";
            break;
            }
        }
    if (!phoneMemory){
         qDebug() << "Phone Memory Not Supported";
         }
    QVERIFY(phoneMemory || !phoneMemory);
    qDebug() << " tst_QSystemStorageInfo::tst_getPhoneMemory() - End ";
    }

void tst_QSystemStorageInfo::tst_getMassMemory()
    {
    qDebug() << " tst_QSystemStorageInfo::tst_getMassMemory() - Begin ";
    QSystemStorageInfo *sti = new QSystemStorageInfo(this);
    QStringList vols = sti->logicalDrives();
    bool massMemory(false);
    foreach (QString volName, vols) {
        QSystemStorageInfo::DriveType volType;
        volType = sti->typeForDrive(volName);
        if (volType == QSystemStorageInfo::InternalFlashDrive) {
            massMemory = true;
            qDebug() << "Mass Memory Supported";
            break;
            }
        }
    if (!massMemory){
        qDebug() << "Mass Memory Not Supported";
        }
    QVERIFY(massMemory || !massMemory);
    qDebug() << " tst_QSystemStorageInfo::tst_getMassMemory() - End ";
    }

void tst_QSystemStorageInfo::tst_PhoneMemoryThresholdNotifications()
    {
    qDebug() << "  - Begin ";
    QSystemStorageInfo *sti = new QSystemStorageInfo(this);
    _LIT(KSOURCEPATH,"C:\\Data\\file.mp3");
    _LIT(KDESTINATIONPATH,":\\Data\\");
    _LIT(KMP3,".mp3");
    RFs session;
    TInt count(0);
    session.Connect();
    TFileName source;
    source.Append(KSOURCEPATH);
    CFileMan* fileManager = CFileMan::NewL(session);
    QSystemStorageInfo::StorageState state = QSystemStorageInfo::UnknownStorageState;

    QSignalSpy DiskSpaceChangedSpy(sti, SIGNAL(storageStateChanged(const QString &, QSystemStorageInfo::StorageState)));
    QStringList vols = sti->logicalDrives();
    TBool massDriveExist(EFalse);
    foreach (QString volName, vols){
        QString type;
        QSystemStorageInfo::DriveType volType;
        volType = sti->typeForDrive(volName);
        qDebug() << "Testing volume type : " << volType;
        if (volType == QSystemStorageInfo::RamDrive || volType == QSystemStorageInfo::InternalFlashDrive || volType == QSystemStorageInfo::RemovableDrive ){
            DiskSpaceChangedSpy.clear();    // make sure signals count is zero for each valid drive
            while (1){
                TFileName temp;
                TChar driveLetter;
                if (volType == QSystemStorageInfo::RamDrive){
                    session.DriveToChar(EDriveC,driveLetter);
                    temp.Append(driveLetter);
                    temp.Append(KDESTINATIONPATH);
                    session.MkDir(temp);
                    }
                else if (volType == QSystemStorageInfo::InternalFlashDrive){
                    massDriveExist = ETrue;
                    session.DriveToChar(EDriveE,driveLetter);
                    temp.Append(driveLetter);
                    temp.Append(KDESTINATIONPATH);
                    session.MkDir(temp);
                    }
                else if (volType == QSystemStorageInfo::RemovableDrive){
                    if (massDriveExist){
                        session.DriveToChar(EDriveF,driveLetter);
                        }
                    else{
                        session.DriveToChar(EDriveE,driveLetter);
                        }
                    temp.Append(driveLetter);
                    temp.Append(KDESTINATIONPATH);
                    session.MkDir(temp);
                    }
                temp.Append(_L("tmp"));
                temp.AppendNum(count);
                temp.Append(KMP3);
                if (fileManager->Copy(source,temp,CFileMan::ERecurse) != KErrNone){
                    qDebug() << "Source file does not exist(KErrNotFound) : -1" ;
                    break;
                    }
                WaitActive(4000); //4 secs

                if (DiskSpaceChangedSpy.count() > 0){
                    QList<QVariant> arg = DiskSpaceChangedSpy.at(0);
                    const QVariant argVol = arg.at(0);
                    const QString volume = argVol.toString();
                    QVariant argState = arg.at(1);
                    QSystemStorageInfo::StorageState volState = argState.value<QSystemStorageInfo::StorageState>();
                    state = sti->getStorageState(volume);
                    QVERIFY(volState == state);
                    if (state == QSystemStorageInfo::NormalStorageState ){
                        qDebug() << "**********NORMAL state Begin**********";
                        }
                    if (state == QSystemStorageInfo::LowStorageState ){
                        qDebug() << "**********NORMAL state End**********";
                        qDebug() << "**********LOW state Begin**********";
                        }
                    if (state == QSystemStorageInfo::VeryLowStorageState ){
                        qDebug() << "**********LOW state End**********";
                        qDebug() << "**********VERYLOW state Begin**********";
                        }
                    if (state == QSystemStorageInfo::CriticalStorageState ){
                        qDebug() << "**********VERYLOW state End**********";
                        qDebug() << "**********CRICITAL state Begin & End**********";
                        DiskSpaceChangedSpy.clear();    // reset signal count
                        DeleteFiles(sti, count, volType, massDriveExist);
                        break;
                        }
                    DiskSpaceChangedSpy.clear();    // reset signal count
                    }
                count++;
                }
            }
        count = 0;   // reset counter
        state = QSystemStorageInfo::UnknownStorageState;     // reset storage state
        continue;
        }
    session.Close();
    qDebug() << " tst_QSystemStorageInfo::tst_PhoneMemoryThresholdNotifications() - End ";
    }

void tst_QSystemStorageInfo::DeleteFiles(QSystemStorageInfo* aSti, const TInt &aCount, QSystemStorageInfo::DriveType &aVolType, TBool &aMassDriveExist)
    {
    qDebug() << " tst_QSystemStorageInfo::DeleteFiles() - Begin ";
    qDebug() << " Number of files to be deleted : " << aCount;
    _LIT(KDESTINATIONPATH,":\\Data\\");
    _LIT(KMP3,".mp3");

    RFs session;
    session.Connect();
    CFileMan* fileManager = CFileMan::NewL(session);

    QSystemStorageInfo::StorageState state = QSystemStorageInfo::UnknownStorageState;
    QSignalSpy DiskSpaceChangedSpy(aSti, SIGNAL(storageStateChanged(const QString &, QSystemStorageInfo::StorageState)));

    for (TInt i=0; i<=aCount; i++){
        TFileName temp;
        TChar driveLetter;
        if (aVolType == QSystemStorageInfo::RamDrive){
            qDebug() << "Deleting Files from Phone Memory";
            session.DriveToChar(EDriveC,driveLetter);
            temp.Append(driveLetter);
            temp.Append(KDESTINATIONPATH);
            }
        else if (aVolType == QSystemStorageInfo::InternalFlashDrive){
            qDebug() << "Deleting Files from Mass Memory";
            session.DriveToChar(EDriveE,driveLetter);
            temp.Append(driveLetter);
            temp.Append(KDESTINATIONPATH);
            }
        else if (aVolType == QSystemStorageInfo::RemovableDrive){
            qDebug() << "Deleting Files from MMC Memory";
            if (aMassDriveExist){
                session.DriveToChar(EDriveF,driveLetter);
                }
            else{
                session.DriveToChar(EDriveE,driveLetter);
                }
            temp.Append(driveLetter);
            temp.Append(KDESTINATIONPATH);
            }
        temp.Append(_L("tmp"));
        temp.AppendNum(i);
        temp.Append(KMP3);
        TInt err = fileManager->Delete(temp);
        WaitActive(8000); //4 secs
        if (err != KErrNone){
            qDebug() << " error while deleting file : " << err;
            break;
            }

        if (DiskSpaceChangedSpy.count() > 0){
            QList<QVariant> arg = DiskSpaceChangedSpy.at(0);
            const QVariant argVol = arg.at(0);
            const QString volume = argVol.toString();
            QVariant argState = arg.at(1);
            QSystemStorageInfo::StorageState volState = argState.value<QSystemStorageInfo::StorageState>();
            state = aSti->getStorageState(volume);
            QVERIFY(volState == state);
            if (state == QSystemStorageInfo::NormalStorageState ){
                qDebug() << "**********Storage State Changed to NORMAL**********";
                }
            if (state == QSystemStorageInfo::LowStorageState ){
                qDebug() << "**********Storage State Changed to LOW**********";
                }
            if (state == QSystemStorageInfo::VeryLowStorageState ){
                qDebug() << "**********Storage State Changed to VERYLOW**********";
                }
            if (state == QSystemStorageInfo::CriticalStorageState ){
                qDebug() << "**********Storage State Changed to CRITICAL**********";
                }
            DiskSpaceChangedSpy.clear();    // reset signal count
            }
        state = QSystemStorageInfo::UnknownStorageState;     // reset storage state
        }
    session.Close();
    qDebug() << " tst_QSystemStorageInfo::DeleteFiles() - End ";
    }

void tst_QSystemGeneralInfo::tst_getFmTransmitterFeature()
    {
    qDebug() << " tst_QSystemGeneralInfo::tst_getFmTransmitterFeature() - Begin ";
    QSystemInfo::Feature type(QSystemInfo::FmTransmitterFeature);
    QSystemInfo si;
    bool fmTransmitterSupport = si.hasFeatureSupported(type);
    QVERIFY(fmTransmitterSupport || !fmTransmitterSupport);
    qDebug() << " tst_QSystemGeneralInfo::tst_getFmTransmitterFeature() - End ";
    }

void tst_QSystemDeviceInfo::tst_uniqueDeviceID()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_uniqueDeviceID---START" ;
    QSystemDeviceInfo di;
    QUuid uniqueDeviceId = di.uniqueDeviceID();
    QVERIFY(uniqueDeviceId != 0);
}

//Lockstatus is set to KeypadLocked and then changed to DeviceLocked
void tst_QSystemDeviceInfo::tst_lockStatusKeypadLocked()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_lockStatusKeypadLocked ---START Line No" <<__LINE__;
    QSystemDeviceInfo *di = new QSystemDeviceInfo;
    connect(di,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),this,SLOT(updateLockStatus(QSystemDeviceInfo::LockType)));
    QSignalSpy spy(di, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)));
    TInt err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardLocked);
    WaitActive(1000000);

    //Testing lockStatus()
    QSystemDeviceInfo::LockTypeFlags lock = di->lockStatus();
    qDebug()<<"Lock is :"<<lock;
    QVERIFY2( (lock == QSystemDeviceInfo::TouchAndKeyboardLocked), " Not TouchAndKeyboardLocked");

    //Testing the signal lockStatusChanged()
    err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardAutolockEmulation);
    WaitActive(1000000);

    QCOMPARE(spy.count(), 2);

    delete di;
    qDebug() << "tst_QSystemDeviceInfo::tst_lockStatusKeypadLocked---END" ;
}

//Lockstatus is set to DeviceLocked and then changed to Device UnknownLock

void tst_QSystemDeviceInfo::tst_lockStatusDeviceLocked()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_lockStatusDeviceLocked---START" ;
    QSystemDeviceInfo *di = new QSystemDeviceInfo;
    connect(di,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),this,SLOT(updateLockStatus(QSystemDeviceInfo::LockType)));
    QSignalSpy spy(di, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)));
    TInt err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardAutolockEmulation);
    WaitActive(1000000);

    //Testing lockStatus()
    QSystemDeviceInfo::LockTypeFlags lock = di->lockStatus();
    qDebug()<<"Lock is :"<<lock;
    QVERIFY2( (lock == QSystemDeviceInfo::PinLocked),"Not PinLocked");

    //Testing the signal lockStatusChanged()
    TInt value = 5;
    err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus , value);
    WaitActive(1000000);
    QCOMPARE(spy.count(), 2);

    delete di;
    qDebug() << "tst_QSystemDeviceInfo::tst_lockStatusDeviceLocked---END" ;
}

//Lockstatus is set to DeviceUnknownLock and then changed to KeypadLocked
void tst_QSystemDeviceInfo::tst_lockStatusDeviceUnknownLock()
{
    TInt value = 5;
    qDebug() << "tst_QSystemDeviceInfo::tst_lockStatusDeviceUnknownLock---START" ;
    QSystemDeviceInfo *di = new QSystemDeviceInfo;
    connect(di,SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)),this,SLOT(updateLockStatus(QSystemDeviceInfo::LockType)));
    QSignalSpy spy(di, SIGNAL(lockStatusChanged(QSystemDeviceInfo::LockType)));
    TInt err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus ,value);
    WaitActive(1000000);

    //Testing lockStatus()
    QSystemDeviceInfo::LockTypeFlags lock = di->lockStatus();
    qDebug()<<"Lock is :"<<lock;
    QVERIFY2 (( lock == QSystemDeviceInfo::UnknownLock), "lock is Not Unknown");

    //Testing the signal lockStatusChanged()
    err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardLocked);
    WaitActive(1000000);
    QCOMPARE(spy.count(), 2);

    delete di;
    qDebug() << "tst_QSystemDeviceInfo::tst_lockStatusDeviceUnknownLock---END" ;
}

void tst_QSystemDeviceInfo::updateLockStatus(QSystemDeviceInfo::LockType lock)
{
 qDebug() <<"lock state:"<<lock;
 qDebug() <<"Signal received";
}

void tst_QSystemDeviceInfo::tst_isKeyboardFlippedOpen()
{

    qDebug() << "tst_QSystemDeviceInfo::tst_isKeyboardFlipOpen---START" <<endl ;
    QSystemDeviceInfo *di = new QSystemDeviceInfo;
    connect(di,SIGNAL(keyboardFlipped(bool)),this,SLOT(flipStatus(bool)));
    QSignalSpy spy(di, SIGNAL(keyboardFlipped(bool)));
    TInt flipState(EPSHWRMGripOpen);
    TInt flipKbType = 0;
    TRAP_IGNORE(
         CRepository* repository = CRepository::NewLC( KCRUidAvkon ) ;
         User::LeaveIfError(repository->Get( KAknKeyboardSlideOpen, flipKbType));
         CleanupStack::PopAndDestroy(repository);
    )
    if ( flipKbType )
    {
        //When the flip is open and keyboard is there
        TInt err = RProperty::Set( KPSUidHWRM, KHWRMGripStatus, flipState);
        WaitActive(1000000);
        qDebug() << "the value of err is " << err;

        //Testing isKeyboardFlipOpen()
        bool flip = di->isKeyboardFlippedOpen();
        QVERIFY( flip == true);

        //When the flip closed and keyboard is not there
        flipState = EPSHWRMGripClosed;
        flipKbType = 0;
        TRAP_IGNORE(
            CRepository* repository = CRepository::NewLC( KCRUidAvkon ) ;
            User::LeaveIfError(repository->Set( KAknKeyboardSlideOpen, flipKbType));
            CleanupStack::PopAndDestroy(repository);
        )
        err = RProperty::Set(KPSUidHWRM, KHWRMGripStatus, flipState);
        WaitActive(1000000);
        qDebug() << "the value of err is " << err;

        //Testing isKeyboardFlipOpen()
        flip = di->isKeyboardFlippedOpen();
        QVERIFY( flip == false);
        QCOMPARE(spy.count(), 2);
        QList<QVariant> args;
        args = spy.at(0);
        QVERIFY(args.at(0).toBool() == true); // verify the first argument

        args = spy.at(1);
        QVERIFY(args.at(0).toBool() == false); // verify the second argument
    }
    else {
        QVERIFY(true);
        qDebug()<<"Flip not supported for the device";
    }
    delete di;
    qDebug() << "tst_QSystemDeviceInfo::tst_isKeyboardFlipOpen---END" ;
}

void tst_QSystemDeviceInfo::flipStatus(bool b)
{
 qDebug() <<"Flip state:"<<b;
 qDebug() <<"Signal received";
}

//////Testing keypadLightOn() when keypad is locked
void tst_QSystemDeviceInfo::tst_keypadLightOn_keypadLocked()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_keypadLightOn_keypadLocked---START" ;
    QSystemDeviceInfo di;
    TInt err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardLocked);
    WaitActive(1000000);
    if ( err == KErrNone ){
        TRAP_IGNORE(
            CHWRMLight* iLight = CHWRMLight::NewL();
            TInt supported = iLight->SupportedTargets();
            if (supported & CHWRMLight::EPrimaryKeyboard ){
                QVERIFY(false == di.keypadLightOn(QSystemDeviceInfo::PrimaryKeypad));
            }
            if (supported & CHWRMLight::ESecondaryKeyboard ){
                QVERIFY(false == di.keypadLightOn(QSystemDeviceInfo::SecondaryKeypad));
            }
        )
    }
    else {
    qDebug() << "Error in setting the property";
    }
    qDebug() << "tst_QSystemDeviceInfo::tst_keypadLightOn_keypadLocked---END" ;
}

//////Testing keypadLightOn() when Device is locked
void tst_QSystemDeviceInfo::tst_keypadLightOn_deviceLocked()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_keypadLightOn_deviceLocked---START" ;
    QSystemDeviceInfo di;
    TInt err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardAutolockEmulation);
    WaitActive(1000000);
    if ( err == KErrNone ){
        TRAP_IGNORE(
            CHWRMLight* iLight = CHWRMLight::NewL();
            TInt supported = iLight->SupportedTargets();
            if (supported & CHWRMLight::EPrimaryKeyboard ){
                QVERIFY(false == di.keypadLightOn(QSystemDeviceInfo::PrimaryKeypad));
            }
            if (supported & CHWRMLight::ESecondaryKeyboard ){
                QVERIFY(false == di.keypadLightOn(QSystemDeviceInfo::SecondaryKeypad));
            }
        )
    }
    else {
    qDebug() << "Error in setting the property";
    }
    qDebug() << "tst_QSystemDeviceInfo::tst_keypadLightOn_deviceLocked---END" ;
}

//////Testing keypadLightOn() when Device is unlocked and flip opened
void tst_QSystemDeviceInfo::tst_keypadLightOn_deviceUnlockedFlipOpen()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_keypadLightOn_deviceUnlockedFlipOpen---START" ;
    QSystemDeviceInfo di;
    int err = RProperty::Set(KPSUidAvkonDomain, KAknKeyguardStatus, EKeyguardNotActive);//unlocked
    int error = RProperty::Set( KPSUidHWRM, KHWRMGripStatus, EPSHWRMGripOpen );//flip open
    WaitActive(1000000);
    if ( err == KErrNone ){
        TRAP_IGNORE(
            CHWRMLight* iLight = CHWRMLight::NewL();
            TInt supported = iLight->SupportedTargets();
            qDebug()<<"Support : "<<supported;
            if (supported & CHWRMLight::EPrimaryKeyboard ){
                QVERIFY(true == di.keypadLightOn(QSystemDeviceInfo::PrimaryKeypad));
            }
            if (error == KErrNone){
                if (supported & CHWRMLight::ESecondaryKeyboard ){
                    QVERIFY(true == di.keypadLightOn(QSystemDeviceInfo::SecondaryKeypad));
                }
                else{
                    QVERIFY(false == di.keypadLightOn(QSystemDeviceInfo::SecondaryKeypad));
                }
            }
            else{
                qDebug() << "Error in setting the property for flip open";
            }
        )
    }
    else{
        qDebug() << "Error in setting the property for Device unlocked";
        }
    qDebug() << "tst_QSystemDeviceInfo::tst_keypadLightOn_deviceUnlockedFlipOpen---END" ;
}

/*QString tst_QSystemDeviceInfo::profileToString(QSystemDeviceInfo::Profile profile)
{
    switch (profile) {
    case QSystemDeviceInfo::SilentProfile : return QString("Silent"); //break;
    case QSystemDeviceInfo::NormalProfile : return QString("Normal"); //break;
    case QSystemDeviceInfo::LoudProfile : return QString("Loud"); //break;
    case QSystemDeviceInfo::OfflineProfile : return QString("Offline"); //break;
    case QSystemDeviceInfo::CustomProfile : return QString("Meet/Pager"); //break;
    default : return QString("Unknown Profile"); //break;
    }

}*/

void tst_QSystemDeviceInfo::tst_inputMethodType()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_inputMethodType---START" ;
     QSystemDeviceInfo testObj;
     QSystemDeviceInfo::InputMethodFlags imType = testObj.inputMethodType();
     if (imType & QSystemDeviceInfo::Keys)
        qDebug("imType is Keys");
     if (imType & QSystemDeviceInfo::Keypad)
        qDebug("imType is Keypad");
     if (imType & QSystemDeviceInfo::Keyboard)
        qDebug("imType is Keyboard");
     if (imType & QSystemDeviceInfo::SingleTouch)
        qDebug("imType is SingleTouch");
     if (imType & QSystemDeviceInfo::MultiTouch)
        qDebug("imType is MultiTouch");
     if (imType & QSystemDeviceInfo::Mouse)
        qDebug("imType is Mouse");
     bool pass= ( imType & QSystemDeviceInfo::Keys) ||
                (imType & QSystemDeviceInfo::Keypad) ||
                (imType & QSystemDeviceInfo::Keyboard) ||
                (imType & QSystemDeviceInfo::SingleTouch) ||
                (imType & QSystemDeviceInfo::MultiTouch) ||
                (imType & QSystemDeviceInfo::Mouse);

     QVERIFY2(pass,"InputMethod didn't return a Valid type... Aborting further Execution");
     qDebug() << "tst_QSystemDeviceInfo::tst_inputMethodType---END" ;
}

void tst_QSystemDeviceInfo::tst_keyboardType()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_keyboardType---START" ;
      QSystemDeviceInfo testObj;
      QSystemDeviceInfo::KeyboardTypeFlags kbType = QSystemDeviceInfo::UnknownKeyboard;

      // Mapping for KAknKeyboardType value<array index> to type <element string>
      const QString str[] =                                       { "No Keyboard"
                                                                  , "ITU-T keypad"
                                                                  , "4x12 QWERTY keyboard"
                                                                  , "4x10 QWERTY Keyboard"
                                                                  , "3x11 QWERTY Keyboard"
                                                                  , "Half QWERTY Keyboard"
                                                                  , "Custom QWERTY keyboard"
                                                                   };

      //mapping of Values in repository <array index here> to Keyflags <element>
      QSystemDeviceInfo::KeyboardTypeFlags keyboardFlagSet[] = { QSystemDeviceInfo::UnknownKeyboard
                                                             ,  QSystemDeviceInfo::ITUKeypad
                                                             , QSystemDeviceInfo::FullQwertyKeyboard
                                                             , QSystemDeviceInfo::FullQwertyKeyboard
                                                             , QSystemDeviceInfo::FullQwertyKeyboard
                                                             , QSystemDeviceInfo::HalfQwertyKeyboard
                                                             , QSystemDeviceInfo::FullQwertyKeyboard
                                                              };


      TInt iterator = -1;
      TInt virtualkeyiterator = -1;
      while ( virtualkeyiterator < 2 ) {
          if ( virtualkeyiterator == -1 )
            qDebug("Default virtual keytype");
          else {
            iterator = 0;
            if ( KErrNone == RProperty::Set(KPSUidAknFep, KAknFepVirtualKeyboardType,virtualkeyiterator) )
                qDebug()<<"Default virtual keytype set successfully to "<<virtualkeyiterator;
            else
                qDebug()<<"Default virtual keytype set failed to "<<virtualkeyiterator;
         }
        virtualkeyiterator++;
          while (iterator<7) {
                  if ( iterator == -1 )
                              qDebug("Original Kb Type");
                  else {
                        TRAP_IGNORE(
                         CRepository* repository = CRepository::NewLC( KCRUidAvkon ) ;
                         if ( iterator == 7 )
                             User::LeaveIfError(repository->Set( KAknKeyboardType, 0));
                         else
                             User::LeaveIfError(repository->Set( KAknKeyboardType, iterator));
                         qDebug()<< "Set Completed and kbType is " <<str[iterator];
                         CleanupStack::PopAndDestroy(repository);
                        )
                    }

                    kbType = testObj.keyboardTypes();
                    if ( kbType == QSystemDeviceInfo::UnknownKeyboard )
                       qDebug("kbType is UnknownKeyboard");
                    if ( kbType & QSystemDeviceInfo::SoftwareKeyboard )
                       qDebug("kbType is SoftwareKeyboard");
                    if ( kbType & QSystemDeviceInfo::ITUKeypad )
                       qDebug("kbType is ITUKeypad");
                    if ( kbType & QSystemDeviceInfo::HalfQwertyKeyboard )
                       qDebug("kbType is HalfQwertyKeyboard");
                    if ( kbType & QSystemDeviceInfo::FullQwertyKeyboard )
                       qDebug("kbType is FullQwertyKeyboard");
                    if ( kbType & QSystemDeviceInfo::WirelessKeyboard )
                       qDebug("kbType is WirelessKeyboard");
                    iterator++;
          }
        }
    qDebug() << "tst_QSystemDeviceInfo::tst_keyboardType---END" ;
}

void tst_QSystemDeviceInfo::tst_isWirelessKeyboardConnected()
{
    qDebug() << "tst_QSystemDeviceInfo::tst_isWirelessKeyboardConnected---START" ;
     QSystemDeviceInfo testObj;
     bool flag=testObj.isWirelessKeyboardConnected();
     if (flag)
             qDebug("isWirelessKeyboardConnected returned TRUE");
     else
             qDebug("isWirelessKeyboardConnected returned FALSE");
     QVERIFY2(!flag,"FLAG is supposed to return False.. But it didn't.. Aborting further Execution");
     qDebug() << "tst_QSystemDeviceInfo::tst_isWirelessKeyboardConnected---END" ;
}

QString boolToStr(bool val)
{
    QString result = "False";
    if (val)
        result = "True";
    return result;
}

void tst_QSystemDeviceInfo::tst_vibrationVolumeSettings()
{
    qDebug() << "tst_QSystemDisplayInfo::tst_vibrationVolumeSettings---START" ;
    QSystemDeviceInfo testObj;
    TInt restoreVolume;
    TInt restoreVibra;
    CRepository* repository = NULL;
    TRAP_IGNORE(
      repository  = CRepository::NewLC( KCRUidProfileEngine );
      User::LeaveIfError( repository->Get( KProEngActiveRingingVolume, restoreVolume ) );
      CleanupStack::PopAndDestroy(repository);
      repository = NULL;
    )
    TRAP_IGNORE(
      repository = CRepository::NewLC( KCRUidVibraCtrl );
      User::LeaveIfError( repository->Get( KVibraCtrlProfileVibraEnabled, restoreVibra ) );
      CleanupStack::PopAndDestroy(repository);
      repository = NULL;
    )
    qDebug()<<"Init Vals vibra"<<boolToStr(restoreVibra)<<"    Vol : "<<restoreVolume;
    bool vibraStates[] = { true, false };
    bool vibration = false;
    TInt msgVolume = -1, voiceVolume = -1;
    int loopiter = 0;

    for (;loopiter < 2;loopiter++)
    {
        TRAP_IGNORE(
            repository = CRepository::NewLC( KCRUidVibraCtrl );
            User::LeaveIfError( repository->Set( KVibraCtrlProfileVibraEnabled, vibraStates[loopiter] ) );
            WaitActive(1000000);
            qDebug()<<"Vibra Successfullyset  to "<<boolToStr(vibraStates[loopiter]);
            CleanupStack::PopAndDestroy(repository);
            repository = NULL;
        )
        vibration = testObj.getActiveProfileDetails().vibrationActive();
        qDebug()<<"Vibra Status returned is  "<<boolToStr(vibration);
        QVERIFY2(( vibration == vibraStates[loopiter] ) ," Failed to Set/Get vibra Status.. Aborting..");
    }

    for ( loopiter = 1; loopiter <11; loopiter++)
    {
        TRAP_IGNORE(
            repository  = CRepository::NewLC( KCRUidProfileEngine );
            User::LeaveIfError( repository->Set( KProEngActiveRingingVolume, loopiter ) );
            WaitActive(1000000);
            qDebug()<<"Volume Successfullyset  to "<<loopiter;
            CleanupStack::PopAndDestroy(repository);
            repository = NULL;
        )
        msgVolume = testObj.getActiveProfileDetails().messageRingtoneVolume();
        voiceVolume = testObj.getActiveProfileDetails().voiceRingtoneVolume();
        qDebug()<<"Msg Vol  : "<<msgVolume<<"   Voice vol : "<<voiceVolume;
        TInt calibVol = 100*(loopiter - 1)/9;
        QVERIFY2( ( msgVolume == calibVol), "Message Volume Get/Set Error... Aborting");
        QVERIFY2( ( voiceVolume == calibVol), "Voice Volume Get/Set Error... Aborting");
    }

    //Restore Earlier Settings
    qDebug ("Restoring Volume and Vibra Settings");
    TRAP_IGNORE(
        repository = CRepository::NewLC( KCRUidVibraCtrl );
        User::LeaveIfError( repository->Set( KVibraCtrlProfileVibraEnabled, restoreVibra ) );
        CleanupStack::PopAndDestroy(repository);
        repository = NULL;
    )
    TRAP_IGNORE(
        repository  = CRepository::NewLC( KCRUidProfileEngine );
        User::LeaveIfError( repository->Set( KProEngActiveRingingVolume, restoreVolume ) );
        CleanupStack::PopAndDestroy(repository);
        repository = NULL;
    )
    qDebug() << "tst_QSystemDisplayInfo::tst_vibrationVolumeSettings---END" ;
}

//////getOrientation
 void tst_QSystemDisplayInfo::tst_getOrientation()
 {
 qDebug() << "tst_QSystemDisplayInfo::tst_getOrientation---START" ;
 QSystemDisplayInfo::DisplayOrientation orientation = QSystemDisplayInfo::Unknown;
 int screen = 0;
 QSystemDisplayInfo di;
 orientation = di.getOrientation(screen);
 qDebug() << "The Orientation is  " << orientation ;
 qDebug() << "tst_QSystemDisplayInfo::tst_getOrientation---END" ;
 }

 //////getOrientation
 void tst_QSystemDisplayInfo::tst_contrast()
 {
 qDebug() << "tst_QSystemDisplayInfo::tst_contrast---START" ;
 float result;
 int screen = 0;
 QSystemDisplayInfo di;
 result = di.contrast(screen);
 qDebug() << "The contrast is  " << result ;
 qDebug() << "tst_QSystemDisplayInfo::tst_contrast---END" ;
 }

 //////getDPIWidth
 void tst_QSystemDisplayInfo::tst_getDPIWidth()
 {
 qDebug() << "tst_QSystemDisplayInfo::tst_getDPIWidth---START" ;
 int screen = 0;
 QSystemDisplayInfo di;
 int dpi = di.getDPIWidth(screen);
 qDebug() << "The DPIWidth is  " << dpi ;
 qDebug() << "tst_QSystemDisplayInfo::tst_getDPIWidth---END" ;
 }

 //////getDPIHeight
 void tst_QSystemDisplayInfo::tst_getDPIHeight()
 {
 qDebug() << "tst_QSystemDisplayInfo::tst_getDPIHeight---START" ;
 int screen = 0;
 QSystemDisplayInfo di;
 int dpi = di.getDPIHeight(screen);
 qDebug() << "The DPIWidth is  " << dpi ;
 qDebug() << "tst_QSystemDisplayInfo::tst_getDPIHeight---END" ;
 }

 //////physicalHeight
 void tst_QSystemDisplayInfo::tst_physicalHeight()
 {
 qDebug() << "tst_QSystemDisplayInfo::tst_physicalHeight---START" ;
 int screen = 0;
 QSystemDisplayInfo di;
 int height = di.physicalHeight(screen);
 qDebug() << "PhysicalHeight is " << height <<"Milimeter";
 qDebug() << "tst_QSystemDisplayInfo::tst_physicalHeight---END" ;
 }

 /////physicalWidth
 void tst_QSystemDisplayInfo::tst_physicalWidth()
 {
 qDebug() << "tst_QSystemDisplayInfo::tst_physicalWidth---START" ;
 int screen = 0;
 QSystemDisplayInfo di;
 int width = di.physicalWidth(screen);
 qDebug() << "PhysicalWidth is " << width <<"Milimeter";
 qDebug() << "tst_QSystemDisplayInfo::tst_physicalWidth---END" ;
 }

  /////uriForDrive
void tst_QSystemStorageInfo::tst_uriForDrive()
 {
     qDebug() << "tst_QSystemStorageInfo::tst_uriForDrive---START " ;
     QSystemStorageInfo si;
     QStringList vols = si.logicalDrives();
     foreach (QString volName, vols) {
        QString res = si.uriForDrive(volName);
        QVERIFY( res.toInt() != -1 );
        qDebug() << "Drive name:"<<volName;
        qDebug() << "Uri Name:" << res;
        }
     qDebug() << "tst_QSystemStorageInfo::tst_uriForDrive---END" ;
 }


class tst_QSystemBatteryInfo : public QObject
{
    Q_OBJECT
private :
    static QString chargerTypeToString(const QSystemBatteryInfo::ChargerType&);
    static QString chargeringStateToString(const QSystemBatteryInfo::ChargingState&);
    static QString energyUnitToString(const QSystemBatteryInfo::EnergyUnit&);
    static QString batteryStatusToString(const QSystemBatteryInfo::BatteryStatus&);


private slots:

    void tst_chargerType();
    void tst_chargingState();
    void tst_nominalCapacity() ;
    void tst_remainingCapacityPercent();
    void tst_remainingCapacity() ;
    void tst_voltage();
    void tst_remainingCapacityBars() ;
    void tst_maxBars() ;
    void tst_batteryStatus() ;
    void tst_energyMeasurementUnit() ;
    void tst_ConnectAndDisconnectNotify();
    void tst_currentFlow();
    void updateCurrentFlow(int);
    //void tst_batteryStatusSignal();
    //void updateBatteryStatus(QSystemBatteryInfo::BatteryStatus);
};

QString tst_QSystemBatteryInfo::chargerTypeToString(const QSystemBatteryInfo::ChargerType& aCType)
{
    QString result;
    switch (aCType) {
        case QSystemBatteryInfo::NoCharger : result = "NoCharger"; break;
        case QSystemBatteryInfo::WallCharger : result = "WallCharger"; break;
        case QSystemBatteryInfo::USBCharger : result = "USBCharger"; break;
        default : result = "UnknownCharger"; break;
    }
    return result;
}

QString tst_QSystemBatteryInfo::chargeringStateToString(const QSystemBatteryInfo::ChargingState& aCState)
{
    QString result;
        switch (aCState) {
            case QSystemBatteryInfo::NotCharging : result = "NotCharging"; break;
            case QSystemBatteryInfo::Charging : result = "Charging"; break;
            default : result = "ChargingError"; break;
        }
        return result;
}

QString tst_QSystemBatteryInfo::energyUnitToString(const QSystemBatteryInfo::EnergyUnit& aEUnit)
{
    QString result;
    switch (aEUnit) {
        case QSystemBatteryInfo::UnitmAh : result = "UnitmAh"; break;
        case QSystemBatteryInfo::UnitmWh : result = "UnitmWh"; break;
        default : result = "UnitUnknown"; break;
    }
    return result;
}

QString tst_QSystemBatteryInfo::batteryStatusToString(const QSystemBatteryInfo::BatteryStatus& aBStatus)
{
    QString result;
    switch (aBStatus) {
        case QSystemBatteryInfo::BatteryEmpty : result = "BatteryEmpty"; break;
        case QSystemBatteryInfo::BatteryCritical : result = "BatteryCritical"; break;
        case QSystemBatteryInfo::BatteryVeryLow : result = "BatteryVeryLow"; break;
        case QSystemBatteryInfo::BatteryLow : result = "BatteryLow"; break;
        case QSystemBatteryInfo::BatteryOk : result = "BatteryOk"; break;
        case QSystemBatteryInfo::BatteryFull : result = "BatteryFull"; break;
        default : result = "UnitUnknown"; break;
    }
    return result;
}

void tst_QSystemBatteryInfo::tst_chargerType()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::ChargerType expectedOutput = QSystemBatteryInfo::UnknownCharger;
    int status = -1;
    RProperty::Get(KPSUidHWRMPowerState,KHWRMChargingStatus,status);
    if ( ( EChargingStatusCharging == status )
                || ( EChargingStatusAlmostComplete == status )
                || ( EChargingStatusChargingContinued == status ) ) {
        RUsb usbMan;
        if ( KErrNone == usbMan.Connect() ){
            TUsbDeviceState deviceState (EUsbDeviceStateUndefined );
            usbMan.GetDeviceState(deviceState);
            if ( EUsbDeviceStateConfigured == deviceState )
                expectedOutput = QSystemBatteryInfo::USBCharger;
            else
                expectedOutput = QSystemBatteryInfo::WallCharger;
        }
    }
        else if ( ( EChargingStatusNotConnected == status )
                || ( EChargingStatusChargingComplete == status )
                || ( EChargingStatusNotCharging  == status ) )
            expectedOutput = QSystemBatteryInfo::NoCharger;

    QSystemBatteryInfo::ChargerType actualOutput = bi.chargerType();
    qDebug()<<"ChargerType Actutal : "<<chargerTypeToString(actualOutput)<<"    Expected : "<<chargerTypeToString(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "ChargerType don't match.. Aborting Execution...");
}

void tst_QSystemBatteryInfo::tst_chargingState()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::ChargingState expectedOutput = QSystemBatteryInfo::ChargingError;
    QSystemBatteryInfo::ChargingState actualOutput = bi.chargingState();
    int status = -1;
    RProperty::Get(KPSUidHWRMPowerState,KHWRMChargingStatus,status);
    if ( ( EChargingStatusCharging == status )
            || ( EChargingStatusAlmostComplete == status )
            || ( EChargingStatusChargingContinued == status ) )
        expectedOutput = QSystemBatteryInfo::Charging;
    else
        expectedOutput = QSystemBatteryInfo::NotCharging;
    qDebug()<<"ChargeingState Actutal : "<<chargeringStateToString(actualOutput)<<"    Expected : "<<chargeringStateToString(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "ChargeingState don't match.. Aborting Execution...");
}

void tst_QSystemBatteryInfo::tst_nominalCapacity()
{
    QSystemBatteryInfo bi;
    CHWRMPower::TBatteryConsumptionData batteryConsumptionData;
    int expectedOutput = -1;
    int actualOutput = bi.nominalCapacity();
    CHWRMPower* hwrmPower = NULL;
    TRAPD( err, hwrmPower = CHWRMPower::NewL() );
    if ( KErrNone == err ) {
        TRequestStatus aStatus;
        hwrmPower->GetBatteryInfo( aStatus, batteryConsumptionData );
        User::WaitForRequest(aStatus);
        delete hwrmPower;
    }
    expectedOutput = batteryConsumptionData.iNominalCapacity;
    qDebug()<<"NominalCapacity Actutal : "<<(actualOutput)<<"    Expected : "<<(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "NominalCapacity don't match.. Aborting Execution...");
}

void tst_QSystemBatteryInfo::tst_remainingCapacityPercent()
{
    QSystemBatteryInfo bi;
    CHWRMPower::TBatteryConsumptionData batteryConsumptionData;
    int expectedOutput = -1;
    int actualOutput = bi.remainingCapacityPercent();
    CHWRMPower* hwrmPower = NULL;
    TRAPD( err, hwrmPower = CHWRMPower::NewL() );
    if ( KErrNone == err ) {
        TRequestStatus aStatus;
        hwrmPower->GetBatteryInfo( aStatus, batteryConsumptionData );
        User::WaitForRequest(aStatus);
        delete hwrmPower;
    }
    expectedOutput = batteryConsumptionData.iRemainingPercentageCapacity;
    qDebug()<<"RemainingPercentageCapacity Actutal : "<<(actualOutput)<<"    Expected : "<<(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "RemainingPercentageCapacity don't match.. Aborting Execution...");
}
void tst_QSystemBatteryInfo::tst_remainingCapacity()
{
    QSystemBatteryInfo bi;
    CHWRMPower::TBatteryConsumptionData batteryConsumptionData;
    int expectedOutput = -1;
    int actualOutput = bi.remainingCapacity();
    CHWRMPower* hwrmPower = NULL;
    TRAPD( err, hwrmPower = CHWRMPower::NewL() );
    if ( KErrNone == err ) {
        TRequestStatus aStatus;
        hwrmPower->GetBatteryInfo( aStatus, batteryConsumptionData );
        User::WaitForRequest(aStatus);
        delete hwrmPower;
    }
    expectedOutput = batteryConsumptionData.iRemainingCapacity;
    qDebug()<<"RemainingCapacity Actutal : "<<(actualOutput)<<"    Expected : "<<(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "RemainingCapacity don't match.. Aborting Execution...");
}
void tst_QSystemBatteryInfo::tst_voltage()
{
    QSystemBatteryInfo bi;
    CHWRMPower::TBatteryVoltageData batteryVoltageData;
    int expectedOutput = -1;
    int actualOutput = bi.voltage();
    CHWRMPower* hwrmPower = NULL;
    TRAPD( err, hwrmPower = CHWRMPower::NewL() );
    if ( KErrNone == err ) {
        TRequestStatus aStatus;
        hwrmPower->GetBatteryVoltageInfo( aStatus, batteryVoltageData );
        User::WaitForRequest(aStatus);
        delete hwrmPower;
    }
    expectedOutput = batteryVoltageData.iRemainingVoltage;
    qDebug()<<"RemainingVoltage  Actutal : "<<(actualOutput)<<"    Expected : "<<(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "RemainingVoltage don't match.. Aborting Execution...");
}

void tst_QSystemBatteryInfo::tst_remainingCapacityBars()
{
    QSystemBatteryInfo bi;
    int expectedOutput = -1;
    RProperty::Get(KPSUidHWRMPowerState,KHWRMBatteryLevel,expectedOutput);
    int actualOutput = bi.remainingCapacityBars();
    qDebug()<<"CapacityBars  Actutal : "<<actualOutput<<"    Expected : "<<expectedOutput;
    QVERIFY2((actualOutput == expectedOutput), "CapacityBars don't match.. Aborting Execution...");
}
void tst_QSystemBatteryInfo::tst_maxBars()
{
    QSystemBatteryInfo bi;
    int expectedOutput = 7;
    int actualOutput = bi.maxBars();
    qDebug()<<"Maxbars  Actutal : "<<actualOutput<<"    Expected : "<<expectedOutput;
    QVERIFY2((actualOutput == expectedOutput), "Maxbars don't match.. Aborting Execution...");

}
void tst_QSystemBatteryInfo::tst_batteryStatus()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::BatteryStatus expectedOutput = QSystemBatteryInfo::BatteryUnknown;
    int batteryStatusVal = -1;
    RProperty::Get(KPSUidHWRMPowerState,KHWRMBatteryStatus,batteryStatusVal);
    switch (batteryStatusVal) {
        case EBatteryStatusOk : expectedOutput = QSystemBatteryInfo:: BatteryOk ;
                break;
        case EBatteryStatusLow : expectedOutput = QSystemBatteryInfo::BatteryLow ;
                break;
        case EBatteryStatusEmpty : expectedOutput = QSystemBatteryInfo::BatteryEmpty ;
                break;
        case EBatteryStatusUnknown :
        default : expectedOutput = QSystemBatteryInfo::BatteryUnknown;
                break;
    }

    QSystemBatteryInfo::BatteryStatus actualOutput = bi.batteryStatus();
    qDebug()<<"BatteryStatus  Actutal : "<<batteryStatusToString(actualOutput)<<"    Expected : "<<batteryStatusToString(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "BatteryStatus don't match.. Aborting Execution...");

}
void tst_QSystemBatteryInfo::tst_energyMeasurementUnit()
{
    QSystemBatteryInfo bi;
    QSystemBatteryInfo::EnergyUnit expectedOutput = QSystemBatteryInfo::UnitmAh;
    QSystemBatteryInfo::EnergyUnit actualOutput = bi.energyMeasurementUnit();
    qDebug()<<"EnergyUnit  Actutal : "<<energyUnitToString(actualOutput)<<"    Expected : "<<energyUnitToString(expectedOutput);
    QVERIFY2((actualOutput == expectedOutput), "EnergyUnit don't match.. Aborting Execution...");
}


void tst_QSystemBatteryInfo::tst_ConnectAndDisconnectNotify()
{
    qDebug() << "tst_QSystemBatteryInfo::tst_ConnectAndDisconnectNotify()---START" ;
    QSystemBatteryInfo bi;
    //connect(&bi,SIGNAL(batteryStatusChanged(QSystemBatteryInfo::BatteryStatus)),this,SLOT(updateBatteryStatus(QSystemBatteryInfo::BatteryStatus)),Qt::AutoConnection);
    connect(&bi,SIGNAL(currentFlowChanged(int)),this,SLOT(updateCurrentFlow(int)));
    qDebug()<<"Connect for currentFlowChanged Called : ";
    WaitActive(2000000);
    disconnect(&bi,SIGNAL(currentFlowChanged(int)));
    qDebug()<<"Dis-Connect for currentFlowChanged Called : ";
    qDebug() << "tst_QSystemBatteryInfo::tst_ConnectAndDisconnectNotify()---END";
    QVERIFY2(true, "");
}

void tst_QSystemBatteryInfo::updateCurrentFlow(int aValue)
{
    qDebug()<<"Recieved Signal for updateCurrentFlow "<<aValue;
}

void tst_QSystemBatteryInfo::tst_currentFlow()
{
     QSystemBatteryInfo bi;
     int current = bi.currentFlow();
     qDebug()<<"Current Flow rate is "<<current;
     QVERIFY2((current > 0), "current is zero or negative.. Aborting...");
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int ret;
    tst_QSystemGeneralInfo test_generalinfo;
    tst_QSystemStorageInfo test_storageinfo;
    tst_QSystemDeviceInfo  test_deviceinfo;
    tst_QSystemDisplayInfo test_displayinfo;
    tst_QSystemBatteryInfo test_batteryinfo;

    //code for writing to a log
    char *new_argv[3];
    char arg1[] = "-o";
    new_argv[0] = argv[0];
    new_argv[1] = arg1;
    QString str = "C:\\data\\tst_storageinfo.log";
    QByteArray   bytes  = str.toAscii();
    new_argv[2] = bytes.data();
    ret = QTest::qExec(&test_storageinfo,3,new_argv);

    str = "C:\\data\\tst_generalinfo.log";
    bytes  = str.toAscii();
    new_argv[2] = bytes.data();
    ret = QTest::qExec(&test_generalinfo,3,new_argv);

    str = "C:\\data\\tst_deviceinfo.log";
    bytes  = str.toAscii();
    new_argv[2] = bytes.data();
    ret = QTest::qExec(&test_deviceinfo,3,new_argv);

    str = "C:\\data\\tst_displayinfo.log";
    bytes  = str.toAscii();
    new_argv[2] = bytes.data();
    ret = QTest::qExec(&test_displayinfo,3,new_argv);

    str = "C:\\data\\tst_batteryinfo.log";
    bytes  = str.toAscii();
    new_argv[2] = bytes.data();
    ret = QTest::qExec(&test_batteryinfo,3,new_argv);
    return ret;
}



#include "tst_qsystemdeviceinfo.moc"
