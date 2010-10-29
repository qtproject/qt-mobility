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

#ifndef NEARFIELDNDEFTARGET_H
#define NEARFIELDNDEFTARGET_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <nfcserver.h>
#include <ndefconnection.h>
#include <e32cmn.h> 
#include <ndefhandler.h>

#include "nearfieldtarget_symbian.h"

class CNearFieldTagType1;
class CNearFieldTagType2;
class CNearFieldTagType3;
class CNearFieldTagType4;
class CNdefMessage;

class CNdefConnection;

class CNearFieldNdefTarget : public MNearFieldTarget, 
                             public MNdefHandler
    {
public:
    // Cancel and destroy
    ~CNearFieldNdefTarget();

    // Two-phased constructor.
    static CNearFieldNdefTarget* NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

    // Two-phased constructor.
    static CNearFieldNdefTarget* NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

public: // New functions
    void SetRealTarget(MNearFieldTarget * aRealTarget);

    // NdefAccess
    bool hasNdefMessage(){};
    void ndefMessages(RPointerArray<CNdefMessage>& aMessages){};
    void setNdefMessages(const RPointerArray<CNdefMessage>& aMessages){};

public:
    CNearFieldTagType1 * CastToTagType1();
#if 0
    CNearFieldTagType2 * CastToTagType2();
    CNearFieldTagType3 * CastToTagType3();
    CNearFieldTagType4 * CastToTagType4();
#endif
    CNearFieldNdefTarget * CastToNdefTarget();
    
    TInt OpenConnection();
    void CloseConnection();
    TBool IsConnectionOpened();

private:
    // C++ constructor
    CNearFieldNdefTarget(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

    // Second-phase constructor
    void ConstructL();

private: // From MNdefHandler
    void ReadComplete( CNdefRecord* aRecord, CNdefRecord::TNdefMessagePart aPart );    
    void ReadComplete( CNdefMessage* aMessage );
    void WriteComplete();
    void HandleError( TInt aError );
    
private:
    // own
    MNearFieldTarget * iTagConnection;
    CActiveSchedulerWait * iWait;
    CNdefConnection * iNdefConnection;
    // own by real target if real target is created
    // otherwise, own by this.
    MNfcTag * iNfcTag;
    
    RNfcServer& iNfcServer;
    };

#endif // NEARFIELDNDEFTARGET_H
