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

#include "pubandsubkey_s60.h"
#include <hwrmpowerstatesdkpskeys.h>
#ifdef SYMBIAN_3_PLATFORM
#include <hwrmpower.h>
#endif
#include <usbman.h>
#include <e32debug.h>
#include <QList>
//#include <QtDebug>

#ifndef BATTERYCOMMONINFO_H
#define BATTERYCOMMONINFO_H

class MBatteryInfoObserver
    {
public:
    virtual void changedBatteryStatus() = 0;
    virtual void changedChargingState() = 0;
    virtual void changedChargerType() = 0;
    virtual void changedRemainingCapacityBars() = 0;
    //virtual void currentFlowChanged( int ) = 0;
    };

class MBatteryHWRMObserver
    {
public :
    virtual void changedCurrentFlow( int ) = 0;
    };

class CBatteryCommonInfo : public CBase , public MCommonHandleObserver
    {
public:
    CBatteryCommonInfo();
    ~CBatteryCommonInfo();

    void AddObserver(MBatteryInfoObserver *observer);
    void RemoveObserver(MBatteryInfoObserver *observer);

private :
    #ifdef SYMBIAN_3_PLATFORM
    TBool BatteryCapacity(CHWRMPower::TBatteryConsumptionData& ) const;
    #endif

public : //Get Functions
    void ChargerType(bool&, bool&) const;
    TBool ChargingState() const;
    TInt NominalCapacity() const;
    TInt RemainingCapacityPercent() const;
    TInt RemainingCapacity() const;
    TInt Voltage() const;
    TInt RemainingCapacityBars() const;
    TInt BatteryStatus() const;

protected:
    void CommanSignalHandler( const TUid aCategory, const TUint aKey );//from MCommonHandleObserver

private:

    CPubSubKeyHandler *iBatteryStatus;
    CPubSubKeyHandler *iBatteryChargingStatus;
    CPubSubKeyHandler *iCapacityBars;



    TInt iRemainingCapacityBars;

    TBool iUsbConnected;
    TBool iPreviousUsbConnected;

    TBool iChargingON;

    TInt iBatteryStatData;

    QList<MBatteryInfoObserver *> iObservers;
    };
#ifdef SYMBIAN_3_PLATFORM
class CBatteryHWRM : public CBase , public MHWRMBatteryPowerObserver
#else
class CBatteryHWRM : public CBase
#endif
    {
private :
    CBatteryHWRM();
public:
    static CBatteryHWRM* New( );
    TInt GetAvergaeCurrent() const;
    ~CBatteryHWRM();
    TInt StartMeasurementAndSetObserver(MBatteryHWRMObserver* );
    void StopCurrentFlowMeasurement();

private :
    TInt StartCurrentFlowMeasurement();

protected:
    #ifdef SYMBIAN_3_PLATFORM
    void PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement);
    #endif
private:
    MBatteryHWRMObserver* iObserver;
    TInt iAverageCurrent;
    #ifdef SYMBIAN_3_PLATFORM
    CHWRMPower *iHwrmPower;
    #endif
    TBool iPowerReportingON;
    };

#endif //BATTERYCOMMONINFO_H
