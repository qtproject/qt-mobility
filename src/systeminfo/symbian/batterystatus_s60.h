/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include <usbman.h>
#endif
#include <e32debug.h>
#include <QList>
#include <QEventLoop>
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
    TInt GetAverageCurrent();
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
    QEventLoop* iEventLoop;
    };

#endif //BATTERYCOMMONINFO_H
