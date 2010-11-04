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

#ifndef NEARFIELDTAGTYPE2_H
#define NEARFIELDTAGTYPE2_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <nfcserver.h>
#include <nfctype1address.h>
#include "nearfieldtarget_symbian.h"

class CNfcType2Connection;
class MNfcTag;

class CNearFieldTagType2 : public CActive, public MNearFieldTarget
    {
    typedef enum TOperation
        {
        ENull,
        ERead,
        EWrite,
        ESelect
        };
        
public:
    // Cancel and destroy
    ~CNearFieldTagType2();

    // Two-phased constructor.
    static CNearFieldTagType2* NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

    // Two-phased constructor.
    static CNearFieldTagType2* NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

public: // New functions
    TInt ReadBlock(TUint8 aBlockAddress, TDes8& aResponse);
    TInt WriteBlock(TUint8 aBlockAddress, const TDesC8& aData);
    TInt SelectSector(TUint8 sector);

public:
    CNearFieldTagType2 * CastToTagType2();
    
    TInt OpenConnection();
    void CloseConnection();
    TBool IsConnectionOpened();

private:
    // C++ constructor
    CNearFieldTagType2(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

    // Second-phase constructor
    void ConstructL();

private: // From CActive
    // Handle completion
    void RunL();

    // How to cancel me
    void DoCancel();

    // Override to handle leaves from RunL(). Default implementation causes
    // the active scheduler to panic.
    TInt RunError( TInt aError );

private:
    // own
    CNfcType2Connection * iNfcType2Connection;
    CActiveSchedulerWait * iWait;
    MNfcTag * iNfcTag;
    
    RNfcServer& iNfcServer;

    TOperation iCurrentOperation;
    TInt iOperationError;
    };

#endif // NEARFIELDTAGTYPE2_H
