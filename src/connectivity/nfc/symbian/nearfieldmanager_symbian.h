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
#ifndef QNEARFIELDMANAGER_SYMBIAN_H_
#define QNEARFIELDMANAGER_SYMBIAN_H_

#include <nfcserver.h>
#include <nfctag.h>
#include <nfctagsubscription.h>
#include <nfcconnection.h>
#include <nfctype2connection.h>

#include <nfctagdiscovery.h>
#include <nfctagconnectionlistener.h>
#include <nfcconnectioninfo.h>
#include <llcpprovider.h>                   // CLlcpProvider
#include <llcplinklistener.h>               // MLlcpLinkListener

#include <ndefmessagelistener.h>
#include <ndefdiscovery.h>

#include "../qndefrecord.h"

namespace QtMobility{
class QNearFieldManagerPrivateImpl;
}

class CNearFieldManager : public CBase,
						  public MNfcTagConnectionListener,
						  public MLlcpLinkListener,
						  public MNdefMessageListener
  {
public:
    
    static CNearFieldManager* NewL( QtMobility::QNearFieldManagerPrivateImpl& aCallback);
    static CNearFieldManager* NewLC( QtMobility::QNearFieldManagerPrivateImpl& aCallback);
    virtual ~CNearFieldManager();
    
    void StartTagDetectionL();
    //for registerTargetDetectedHandler ... api
    TInt AddNdefSubscription( const QtMobility::QNdefRecord::TypeNameFormat aTnf, 
                                       const QByteArray& aType );
    void RemoveNdefSubscription( const QtMobility::QNdefRecord::TypeNameFormat aTnf, 
                                              const QByteArray& aType );
    
public: // From MNfcTagConnectionListener
    
    void TagDetected( MNfcTag* aNfcTag );  
    void TagLost();
    
public: // From MLlcpLinkListener
   
    void LlcpRemoteFound();
    void LlcpRemoteLost();

public: // From MNdefMessageListener
    
    void MessageDetected( CNdefMessage* aMessage );
    
private:
    
    CNearFieldManager( QtMobility::QNearFieldManagerPrivateImpl& aCallback);
    void ConstructL();
    //own
    RNfcServer iServer;
    //for Tag discovery
    CNfcTagDiscovery* iNfcTagDiscovery;    
    CNfcTagSubscription* iTagSubscription;
    //for LLCP discovery
    CLlcpProvider* iLlcpProvider;
    //for NDEF discovery
    CNdefDiscovery* iNdefDiscovery;
    //not own
    QtMobility::QNearFieldManagerPrivateImpl& iCallback;
    };
#endif /* QNEARFIELDMANAGER_SYMBIAN_H_ */
