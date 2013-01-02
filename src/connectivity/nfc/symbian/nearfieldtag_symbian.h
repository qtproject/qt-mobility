/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** Commercial License Usage
 ** Licensees holding valid commercial Qt licenses may use this file in
 ** accordance with the commercial license agreement provided with the
 ** Software or, alternatively, in accordance with the terms contained in
 ** a written agreement between you and Digia.  For licensing terms and
 ** conditions see http://qt.digia.com/licensing.  For further information
 ** use the contact form at http://qt.digia.com/contact-us.
 **
 ** GNU Lesser General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU Lesser
 ** General Public License version 2.1 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.LGPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU Lesser General Public License version 2.1 requirements
 ** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Digia gives you certain additional
 ** rights.  These rights are described in the Digia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU
 ** General Public License version 3.0 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.  Please review the following information to
 ** ensure the GNU General Public License version 3.0 requirements will be
 ** met: http://www.gnu.org/copyleft/gpl.html.
 **
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef NEARFIELDTAG_SYMBIAN_H
#define NEARFIELDTAG_SYMBIAN_H
#include <nfcserver.h>
#include <e32base.h>
#include <iso14443connection.h>

class MNfcTag;
class MNfcConnection;
class MNearFieldTagOperationCallback;


class CNearFieldTag : public CActive
    {
public:
    // Cancel and destroy
    ~CNearFieldTag();

    // Two-phased constructor.
    static CNearFieldTag* NewLC(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

public:
    CNearFieldTag * CastToTag();
    void SetConnection(MNfcConnection * aTagConnection) { iTagConnection = aTagConnection; }

    TInt OpenConnection();
    void CloseConnection();
    TBool IsConnectionOpened();

    TInt RawModeAccess(const TDesC8& aCommand, TDes8& aResponse, TTimeIntervalMicroSeconds32& aTimeout);

    MNfcConnection * TagConnection() { return iTagConnection;}

    void SetTagOperationCallback(MNearFieldTagOperationCallback * const aCallback);

private:
    // C++ constructor
    CNearFieldTag(MNfcTag * aNfcTag, RNfcServer& aNfcServer);

private:
    void RunL();
    void DoCancel();

private:
    // own
    MNfcConnection * iTagConnection;
    MNfcTag * iNfcTag;
    RNfcServer& iNfcServer;
    // Not own
    MNearFieldTagOperationCallback * iCallback;
    TBool iIsTag4;
    };

#endif // NEARFIELDTAG_SYMBIAN_H
