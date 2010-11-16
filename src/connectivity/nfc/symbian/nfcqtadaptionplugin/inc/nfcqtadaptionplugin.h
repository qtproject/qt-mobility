/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Content handler for data that is text type
*
*/


#ifndef C_NFCQTADAPTIONPLUGIN_H
#define C_NFCQTADAPTIONPLUGIN_H

#include <QServiceManager>
#include <QServiceInterfaceDescriptor>
#include <nfcch.h>

QTM_USE_NAMESPACE

class CNdefMessage;
class CNdefRecord;

/**
 *  NFC Content Handler to take care of MIME text/plain and
 *  urn:nfc:wkt:T messages.
 *
 *  @lib nfccontenthandler.lib
 */
class CNfcQtAdaptionPlugin : public CNfcCH
    {

public:

    static CNfcQtAdaptionPlugin* NewL();

    virtual ~CNfcQtAdaptionPlugin();

// from base class CNfcCH

    /**
     * From CNfcCH.
     * Method called by initiator when processing of a NDEF Message is
     * required.
     *
     * @param aRequestStatus Initiator's status to be completed when
     *        processing is done
     * @param aMessage NDEF Message, ownership is transferred
     */
    virtual void HandleNdefMessage(
        TRequestStatus& aRequestStatus,
        CNdefMessage* aMessage );

    /**
     * From CNfcCH.
     * Cancels NDEF Message processing
     */
    virtual void HandleNdefMessageCancel();
    
// from base class MNfcActiveListener
 
protected:

    CNfcQtAdaptionPlugin();

    void ConstructL();


private: // data

    /**
     * Not own.
     */
    TRequestStatus*         iServerStatus;

    };

#endif // C_NFCQTADAPTIONPLUGIN_H
