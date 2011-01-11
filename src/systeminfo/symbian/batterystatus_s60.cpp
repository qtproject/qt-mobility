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

#include "batterystatus_s60.h"

CBatteryCommonInfo::CBatteryCommonInfo() : iBatteryStatus(NULL), iBatteryChargingStatus(NULL), iCapacityBars(NULL), iUsbConnected(EFalse),iChargingON(EFalse)
    {
    #ifdef SYMBIAN_3_PLATFORM
    iBatteryStatus = CPubSubKeyHandler::New(KPSUidHWRMPowerState, KHWRMBatteryStatus,*this );
    iBatteryChargingStatus = CPubSubKeyHandler::New(KPSUidHWRMPowerState, KHWRMChargingStatus,*this);
    iCapacityBars = CPubSubKeyHandler::New(KPSUidHWRMPowerState, KHWRMBatteryLevel,*this );
    iBatteryStatData=iRemainingCapacityBars=-1; //Assigning each to error code
    if (iBatteryChargingStatus)
        {
        TInt chargingState = iBatteryChargingStatus->GetValue();
        switch(chargingState)
            {
            case EChargingStatusCharging:
            case EChargingStatusAlmostComplete:
            case EChargingStatusChargingContinued: iChargingON = ETrue;
            break ;
            case EChargingStatusNotConnected:
            case EChargingStatusChargingComplete:
            case EChargingStatusNotCharging:
            default : iChargingON = EFalse;
            break;
            }

        if ( iChargingON )
            {
            RUsb usbMan;
            if ( KErrNone == usbMan.Connect() )
                {
                TUsbDeviceState deviceState (EUsbDeviceStateUndefined );
                usbMan.GetDeviceState(deviceState);
                if ( EUsbDeviceStateConfigured == deviceState )
                    iUsbConnected = ETrue;
                }
            }
        }
    if (iBatteryStatus)
        iBatteryStatData = iBatteryStatus->GetValue();
    if (iCapacityBars)
        iRemainingCapacityBars = iCapacityBars->GetValue();
    #else
    iRemainingCapacityBars = -1;
    iPreviousUsbConnected = EFalse;
    iBatteryStatData = -1;
    #endif
    }

CBatteryCommonInfo::~CBatteryCommonInfo()
    {
    if (iBatteryStatus)
        delete iBatteryStatus;
    if (iBatteryChargingStatus)
        delete iBatteryChargingStatus;
    if (iCapacityBars)
        delete iCapacityBars;
    iBatteryStatus = iBatteryChargingStatus = iCapacityBars =NULL;
    }

void CBatteryCommonInfo::AddObserver(MBatteryInfoObserver *aObserver)
    {
    if (aObserver)
        iObservers.append(aObserver);
    }

void CBatteryCommonInfo::RemoveObserver(MBatteryInfoObserver *aObserver)
    {
    if ( aObserver )
        iObservers.removeOne(aObserver);
    }

TBool CBatteryCommonInfo::ChargingState() const
    {
    return iChargingON;
    }

void CBatteryCommonInfo::ChargerType(bool& aChargerState, bool& aUsbState) const
    {
    aChargerState = iChargingON;
    aUsbState = iUsbConnected;
    }

#ifdef SYMBIAN_3_PLATFORM
TBool CBatteryCommonInfo::BatteryCapacity(CHWRMPower::TBatteryConsumptionData& aBatteryData) const
    {
    CHWRMPower* hwrmPower = NULL;
    TRAPD( err, hwrmPower = CHWRMPower::NewL() );
    if ( KErrNone == err )
        {
        TRequestStatus aStatus;
        hwrmPower->GetBatteryInfo( aStatus, aBatteryData );
        User::WaitForRequest(aStatus);
        delete hwrmPower;
        return ETrue;
        }
    return EFalse;
    }
#endif

TInt CBatteryCommonInfo::NominalCapacity() const
    {
    #ifdef SYMBIAN_3_PLATFORM
    CHWRMPower::TBatteryConsumptionData batterydata;
    if ( BatteryCapacity(batterydata) )
        return batterydata.iNominalCapacity;
    #endif
    return -1;
    }
TInt CBatteryCommonInfo::RemainingCapacityPercent() const
    {
    #ifdef SYMBIAN_3_PLATFORM
    CHWRMPower::TBatteryConsumptionData batterydata;
    if ( BatteryCapacity(batterydata) )
        return batterydata.iRemainingPercentageCapacity;
    #endif
    return -1;
    }
TInt CBatteryCommonInfo::RemainingCapacity() const
    {
    #ifdef SYMBIAN_3_PLATFORM
    CHWRMPower::TBatteryConsumptionData batterydata;
    if ( BatteryCapacity(batterydata) )
        return batterydata.iRemainingCapacity;
    #endif
    return -1;
    }
TInt CBatteryCommonInfo::Voltage() const
    {
    #ifdef SYMBIAN_3_PLATFORM
    CHWRMPower::TBatteryVoltageData batteryVoltageData;
    CHWRMPower* hwrmPower = NULL;
    TRAPD( err, hwrmPower = CHWRMPower::NewL() );
    if ( KErrNone == err )
        {
        TRequestStatus aStatus;
        hwrmPower->GetBatteryVoltageInfo( aStatus, batteryVoltageData );
        User::WaitForRequest(aStatus);
        delete hwrmPower;
        return batteryVoltageData.iRemainingVoltage;
        }
    #endif
    return -1;
    }

TInt CBatteryCommonInfo::RemainingCapacityBars() const
    {
    return iRemainingCapacityBars;
    }
TInt CBatteryCommonInfo::BatteryStatus() const
    {
    return iBatteryStatData;
    }

void CBatteryCommonInfo::CommanSignalHandler( const TUid aCategory, const TUint aKey )
    {
    #ifdef SYMBIAN_3_PLATFORM
    if ( KPSUidHWRMPowerState == aCategory )
        {
        switch ( aKey )
            {
            case KHWRMBatteryStatus :
                if (iBatteryStatus)
                    {
                    iBatteryStatData = iBatteryStatus->GetValue();
                    foreach (MBatteryInfoObserver *observer, iObservers)
                        {
                        observer->changedBatteryStatus();
                        }
                    }
                break;
            case KHWRMChargingStatus :
                //Get Charge Status
                if (iBatteryChargingStatus)
                    {
                    TInt chargeStatus = iBatteryChargingStatus->GetValue();
                    switch (chargeStatus)
                        {
                        case EChargingStatusCharging:
                        case EChargingStatusAlmostComplete:
                        case EChargingStatusChargingContinued: iChargingON = ETrue;
                        break ;
                        case EChargingStatusNotConnected:
                        case EChargingStatusChargingComplete:
                        case EChargingStatusNotCharging:
                        default : iChargingON = EFalse;
                        break;
                        }
                    //  Get Charger Type
                    RUsb usbMan;
                    if ( KErrNone == usbMan.Connect() )
                        {
                        TUsbDeviceState deviceState (EUsbDeviceStateUndefined );
                        usbMan.GetDeviceState(deviceState);
                        if ( EUsbDeviceStateConfigured == deviceState )
                            iUsbConnected = ETrue;
                        else
                            iUsbConnected =  EFalse;
                        }

                    foreach (MBatteryInfoObserver *observer, iObservers)
                        {
                        observer->changedChargingState();
                        observer->changedChargerType();
                        }
                    }
                break;
            case KHWRMBatteryLevel :
                if (iCapacityBars)
                    {
                    iRemainingCapacityBars = iCapacityBars->GetValue();
                    foreach (MBatteryInfoObserver *observer, iObservers)
                        {
                        observer->changedRemainingCapacityBars();
                        }
                    }
                break;
            default : break ; //do Nothing
            }
        }
    #endif
    }

CBatteryHWRM::CBatteryHWRM() : iObserver(NULL),iAverageCurrent(-1),iPowerReportingON(EFalse)
    {
    #ifdef SYMBIAN_3_PLATFORM
    iHwrmPower = NULL;
    TRAPD(err, iHwrmPower=CHWRMPower::NewL());
    if ( KErrNone == err )
        {
        iHwrmPower->SetPowerReportObserver(this);
        }
    #endif
    }

CBatteryHWRM* CBatteryHWRM::New( )
    {
    CBatteryHWRM *instance = NULL;
    instance = new CBatteryHWRM ();
    return instance;
    }

CBatteryHWRM::~CBatteryHWRM()
    {
    StopCurrentFlowMeasurement();
    #ifdef SYMBIAN_3_PLATFORM
    if ( iHwrmPower )
        {
        delete (iHwrmPower);
        iHwrmPower = NULL;
        }
    #endif
    }
#ifdef SYMBIAN_3_PLATFORM
void CBatteryHWRM::PowerMeasurement(TInt aErr, CHWRMPower::TBatteryPowerMeasurementData& aMeasurement)
    {
    if ( KErrNone == aErr)
        {
        if ( iAverageCurrent != aMeasurement.iAverageCurrent )
            {
            //currentFlowChanged SIGNAL
            iAverageCurrent = aMeasurement.iAverageCurrent;
            iObserver->changedCurrentFlow(iAverageCurrent);
            }
        }
    else if (iPowerReportingON)
        StartCurrentFlowMeasurement();
    }
#endif

TInt CBatteryHWRM::GetAvergaeCurrent() const
    {
    return iAverageCurrent;
    }

TInt CBatteryHWRM::StartCurrentFlowMeasurement()
    {
    TRequestStatus status;
    iPowerReportingON = ETrue;
    #ifdef SYMBIAN_3_PLATFORM
    if ( iHwrmPower )
        {
        iHwrmPower->StartAveragePowerReporting(status, 1);
        User::WaitForRequest(status);
        return (status.Int());
        }
    #endif
    return -1;
    }

TInt CBatteryHWRM::StartMeasurementAndSetObserver(MBatteryHWRMObserver* aObserver)
    {
    TInt result = StartCurrentFlowMeasurement ();
    if ( 0 == result )
        iObserver = aObserver;
    return result;
    }

void CBatteryHWRM::StopCurrentFlowMeasurement()
    {
    #ifdef SYMBIAN_3_PLATFORM
    if ( iHwrmPower )
        TRAP_IGNORE(iHwrmPower->StopAveragePowerReportingL() );
    #endif
    iPowerReportingON = false;
    }
