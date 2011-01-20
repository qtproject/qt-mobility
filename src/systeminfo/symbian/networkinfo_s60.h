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

#ifndef NETWORKSTATUS_S60_H
#define NETWORKSTATUS_S60_H

#include <e32base.h>
#include <e32debug.h>
#include <etelmm.h>
#include <etel.h>
#include <QList>

class MNetworkObserver
    {
public:
    virtual void ChangedNetworkMode() = 0;
    virtual void ChangedNetworkStatus() = 0;
    };

class MNetworkInfoObserver
    {
public:
    virtual void changedNetworkStatus() = 0;
    virtual void changedNetworkMode() = 0;
    };

class CNetworkBase : public CActive
    {
public:
    CNetworkBase();
    ~CNetworkBase();

    void AddObserver(MNetworkObserver *aObserver);
    void RemoveObserver();

protected:  //from CActive
    virtual void DoCancel() =0; //To Make the class abstract
    virtual void RunL() = 0;
    virtual void StartMonitoring() = 0;

protected:
    RTelServer iTelServer;
    RMobilePhone iMobilePhone;
    RTelServer::TPhoneInfo iPhoneInfo;
    TBool iConstructed;
    MNetworkObserver *iObserver;
    };

class CNetworkMode : private CNetworkBase
    {
public :
    CNetworkMode ();
    ~CNetworkMode ();
    RMobilePhone::TMobilePhoneNetworkMode GetMode() const;
    void Add(MNetworkObserver *aObserver) { AddObserver(aObserver);} ;
    void Remove() {RemoveObserver();};

private : //From CNetworkBase
    virtual void DoCancel();
    virtual void RunL() ;
    virtual void StartMonitoring();

private :
    RMobilePhone::TMobilePhoneNetworkMode iNetworkMode;
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

class CNetworkStatus : private CNetworkBase
    {
public:
    CNetworkStatus();
    ~CNetworkStatus();
    TUint32 GetCapability () const;
    RMobilePhone::TMobilePhoneRegistrationStatus GetStatus() const;
    void Add(MNetworkObserver *aObserver) { AddObserver(aObserver);} ;
    void Remove() {RemoveObserver();};

private : //From CNetworkBase
    virtual void DoCancel();
    virtual void RunL() ;
    virtual void StartMonitoring();


private:
    RMobilePhone::TMobilePhoneRegistrationStatus iNetStatus;
    TUint32 iCapsPhone;
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

class CNetworkInfo : public CBase, public MNetworkObserver
{
private :
        CNetworkMode iNetMode;
        CNetworkStatus iNetStat;
        QList<MNetworkInfoObserver *> iObservers;

protected :
    virtual void ChangedNetworkMode() ;
    virtual void ChangedNetworkStatus();

public :
        CNetworkInfo();
        //~CNetworkInfo();
        void addObserver(MNetworkInfoObserver *aObserver);
        void removeObserver(MNetworkInfoObserver *aObserver);
        RMobilePhone::TMobilePhoneNetworkMode GetMode() const;
        RMobilePhone::TMobilePhoneRegistrationStatus GetStatus() const;
        TUint32 GetCapability () const;
};


#endif // NETWORKSTATUS_S60_H
