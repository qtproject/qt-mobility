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

#ifndef NEARFIELDTAG_H
#define NEARFIELDTAG_H
#include <nfcserver.h>
#include <e32base.h>

#include "nearfieldtarget_symbian.h"

class MNfcTag;
class MNfcConnection;
class MNearFieldTagOperationCallback;

class CNearFieldTag : public CActive, public MNearFieldTarget
    {
public:
    // Cancel and destroy
    ~CNearFieldTag();

    // Two-phased constructor.
    static CNearFieldTag* NewL(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

    // Two-phased constructor.
    static CNearFieldTag* NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

public:
    CNearFieldTag * CastToTag();
    void SetConnection(MNfcConnection * aTagConnection) { iTagConnection = aTagConnection; }
    
    TInt OpenConnection();
    void CloseConnection();
    TBool IsConnectionOpened();

    TInt RawModeAccess(const TDesC8& aCommand, TDes8& aResponse,const TTimeIntervalMicroSeconds32& aTimeout);
    
    const TDesC8& Uid() const;

    MNfcConnection * TagConnection() { return iTagConnection;}

    void SetTagOperationCallback(MNearFieldTagOperationCallback * const aCallback);
    MNearFieldTagOperationCallback * TagOperationCallback();
private:
    // C++ constructor
    CNearFieldTag(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

private:
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:
    // own
    MNfcConnection * iTagConnection;
    MNfcTag * iNfcTag;
    RNfcServer& iNfcServer;
    // Not own
    MNearFieldTagOperationCallback * iCallback;
    };

#endif // NEARFIELDTAG_H
