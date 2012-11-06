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

#ifndef NETWORKSTATUS_S60_H
#define NETWORKSTATUS_S60_H

#include <e32base.h>
#include <e32debug.h>
#ifdef ETELMM_SUPPORTED
#include <etelmm.h>
#endif
#ifdef ETELPACKETSERVICE_SUPPORTED
#include <etelpckt.h>
#endif
#include <etel.h>
#include <QList>

//from etelbearers.h
const TUint KGprsBearer  = 20;
const TUint KEGprsBearer = 21;
const TUint KUmtsBearer  = 22;
const TUint KHsdpaBearer = 23;
const TUint KDefaultBearer = 24;

class MNetworkObserver
{
public:
    virtual void ChangedNetworkMode() = 0;
    virtual void ChangedNetworkStatus() = 0;
    virtual void ChangedCellDataTechnology() = 0;
};

class MNetworkInfoObserver
{
public:
    virtual void changedNetworkStatus() = 0;
    virtual void changedNetworkMode() = 0;
    virtual void changedCellDataTechnology() = 0;
};

#ifdef ETELMM_SUPPORTED
class CNetworkMode : public CActive
{
public:
    CNetworkMode(MNetworkObserver *aObserver, RMobilePhone &aMobilePhone);
    ~CNetworkMode();
    RMobilePhone::TMobilePhoneNetworkMode GetMode() const;

protected: //From CActive
    virtual void DoCancel();
    virtual void RunL() ;
    virtual void StartMonitoring();

private: //data
    RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;
    MNetworkObserver *iObserver;
    RMobilePhone &iMobilePhone;

    /*
    enum TMobilePhoneNetworkMode
        {
        ENetworkModeUnknown, // Network mode is unknown.    Modes: Common
        ENetworkModeUnregistered, // ME is not registered. Modes: Common
        ENetworkModeGsm,  // GSM/GPRS or DCS1800 network.
        ENetworkModeAmps, // AMPS network.  Modes: CDMA     @deprecated 9.5
        ENetworkModeCdma95, // CDMA (IS-95) network.    Modes: CDMA @deprecated 9.5
        ENetworkModeCdma2000, // CDMA (cdma2000) network.   Modes: CDMA @deprecated 9.5
        ENetworkModeWcdma, // WCDMA  (UTRA Frequency Division Duplex (FDD)) network.    Modes: WCDMA
        ENetworkModeTdcdma  // TD-CDMA (UTRA Time Division Duplex (TDD)) network.   Modes: WCDMA
        };
        */

};

class CNetworkStatus : public CActive
{
public:
    CNetworkStatus(MNetworkObserver *aObserver, RMobilePhone &aMobilePhone);
    ~CNetworkStatus();
    TUint32 GetCapability () const;
    RMobilePhone::TMobilePhoneRegistrationStatus GetStatus() const;

protected : //From CActive
    virtual void DoCancel();
    virtual void RunL() ;
    virtual void StartMonitoring();

private:
    RMobilePhone::TMobilePhoneRegistrationStatus iNetStatus;
    TUint32 iCapsPhone;
    MNetworkObserver *iObserver;
    RMobilePhone &iMobilePhone;
    /*
    BITWISE OR of
    enum TMobilePhoneModeCaps
                    {
                    KCapsGsmSupported=0x00000001, // Phone can operate in GSM mode on 900/1800/1900 MHz bands.
                    KCapsGprsSupported=0x00000002, // Phone can operate in GPRS mode on 900/1800/1900 MHz bands.
                    KCapsAmpsSupported=0x00000004, // Phone can operate in AMPS mode on 800MHz band.
                    KCapsCdma95Supported=0x00000008, // Phone can operate in CDMA (IS-95) mode on 800/1900 MHz bands. @deprecated 9.5
                    KCapsCdma2000Supported=0x00000010, // Phone can operate in CDMA (cdma2000) mode on 800/1900 MHz bands. @deprecated 9.5
                    KCapsWcdmaSupported=0x00000020, // Phone can operate in W-CDMA (UTRA Frequency Division Duplex (FDD)) mode.
                    KCapsTdcdmaSupported=0x00000040, // Phone can operate in TDMA/CDMA (UTRA Time Division Duplex (TDD)) mode.
                    KCapsEapSupported=0x00000080 //Phone can access Smart Card functionality required for an EAP. @deprecated 9.3
                    };
    */
};

#ifdef ETELPACKETSERVICE_SUPPORTED
class CPacketDataStatus : public CActive
{
public:
    CPacketDataStatus(MNetworkObserver *aObserver, RPacketService &aPacketService);
    ~CPacketDataStatus();
    TBool IsDynamicCapsSupported();
    TUint DynamicCaps();

protected: //From CActive
    virtual void DoCancel();
    virtual void RunL() ;
    virtual void StartMonitoring();

private: //data
    MNetworkObserver *iObserver;
    RPacketService &iPacketService;
    RPacketService::TStatus iPacketServiceStatus;
    RPacketService::TDynamicCapsFlags iDynCaps;
};
#endif //ETELPACKETSERVICE_SUPPORTED
#endif //ETELMM_SUPPORTED

class CNetworkInfo : public CBase, public MNetworkObserver
{
protected:
    virtual void ChangedNetworkMode() ;
    virtual void ChangedNetworkStatus();
    virtual void ChangedCellDataTechnology();

public:
    CNetworkInfo();
    ~CNetworkInfo();
    void Initialise();
    bool IsInitialised();
    void addObserver(MNetworkInfoObserver *aObserver);
    void removeObserver(MNetworkInfoObserver *aObserver);
#ifdef ETELMM_SUPPORTED
    RMobilePhone::TMobilePhoneNetworkMode GetMode() const;
    RMobilePhone::TMobilePhoneRegistrationStatus GetStatus() const;
#endif
    TUint32 GetCapability () const;
    TUint CellDataTechnology();

private:
    void InitialiseL();

private:
    QList<MNetworkInfoObserver *> iObservers;
    bool iConstructed;
#ifdef ETELMM_SUPPORTED
    CNetworkMode *iNetMode;
    CNetworkStatus *iNetStat;
    RTelServer iTelServer;
    RMobilePhone iMobilePhone;
    RTelServer::TPhoneInfo iPhoneInfo;
    MNetworkObserver *iObserver;
    //For CellData Technology
#ifdef ETELPACKETSERVICE_SUPPORTED
    CPacketDataStatus *iPacketDataStatus;
    RPacketService iPacketService;
#endif //ETELPACKETSERVICE_SUPPORTED
#endif //ETELMM_SUPPORTED
    TUint32 iCellDataTechnology;
    bool iPacketdataserviceCaps;
};

#endif // NETWORKSTATUS_S60_H
