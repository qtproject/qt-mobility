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

#ifndef NEARFIELDTAGTYPE3_H
#define NEARFIELDTAGTYPE3_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <nfcserver.h>
#include "nearfieldtarget_symbian.h"

class CNfcType3Connection;
class MNfcTag;

class CNearFieldTagType3 : public CActive, public MNearFieldTarget
    {
public:
    // Cancel and destroy
    ~CNearFieldTagType3();

    // Two-phased constructor.
    static CNearFieldTagType3* NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

    // Two-phased constructor.
    static CNearFieldTagType3* NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

public:
    CNearFieldTagType3 * CastToTagType3();
    
    TInt OpenConnection();
    void CloseConnection();
    TBool IsConnectionOpened();

private:
    // C++ constructor
    CNearFieldTagType3(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

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
    CNfcType3Connection * iNfcType3Connection;
    CActiveSchedulerWait * iWait;
    MNfcTag * iNfcTag;
    
    RNfcServer& iNfcServer;
    };

#endif // NEARFIELDTAGTYPE3_H
