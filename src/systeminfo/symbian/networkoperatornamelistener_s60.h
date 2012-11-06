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



#ifndef NETWORKOPERATORNAMELISTENER_S60_H
#define NETWORKOPERATORNAMELISTENER_S60_H

class MNetworkOperatorNameObserver
    {
public:
    virtual void OperatorNameChanged() = 0;
    };

#include <nwhandlingengine.h>


class CNWSession;


class CNetworkOperatorNameListener : public CBase, public MNWMessageObserver
    {

public:

    static CNetworkOperatorNameListener* NewL();

    virtual ~CNetworkOperatorNameListener();
    void addObserver(MNetworkOperatorNameObserver *observer);
    void removeObserver(MNetworkOperatorNameObserver *observer);
    QString networkName();

    /**
     * Pass the current operator name.
     *
     * @param aOperatorName Placeholder for the operator name.
     */
    //void GetOperatorName( TDes& aOperatorName );

// from base class MNWMessageObserver

    /**
     * From MNWMessageObserver.
     * Offers message interface to the client.
     *
     * @param aMessage
     * This methods execute time must be short,since code
     * starting to run from RunL.
     */
    virtual void HandleNetworkMessage( const TNWMessages aMessage );

    /**
     * From MNWMessageObserver.
     * Offers error message interface to the client.
     *
     * @param aOperation operation which failed
     * @param aErrorCode returned Symbian OS error code
     */
    virtual void HandleNetworkError( const TNWOperation aOperation,
        TInt aErrorCode );

private:

    CNetworkOperatorNameListener();

    void ConstructL();

    /**
     * Check SPN status from PLMNField. Also check if current country and
     * network codes are in PLMNField.
     */
    void CheckServiceProviderDisplayListStatus(
        TInt aMCC,
        TInt aMNC,
        TBool& aShowSPN,
        TBool& aIsInSPDI );

    /**
     * Check if current network is ready. Returns ETrue if all needed messages
     * are received from network info listener.
     */
    TBool CurrentNetworkOk();

    /**
     * Returns ETrue if phone is currently roaming.
     */
    TBool IsRoaming();

    /**
     * Client can use this method to check if certain network message has been
     * already received.
     *
     * @since S60 3.2
     * @param aMessage is type of the message.
     * @return ETrue if the message has been received.
     */
    TBool MessageReceived( MNWMessageObserver::TNWMessages aMessage );

    /**
     * Show network indicator.
     * @param aTryToPublish
     * ETrue: Sets operator name, priority and tries to publish operator name.
     * EFalse: Sets operator name, priority and not to try to publish it.
     */
    void ShowNetworkIdentityNameL( TBool aTryToPublish );

    /**
     * Check priority of operator name.
     */
    TBool OperatorNamePriority( TInt& aPriority );

    /**
     * Check if network info has been changed.
     * @param aMessage Network message
     * @return ETrue if network info has changed since last check.
     */
    TBool HasNetworkInfoChanged( const TNWMessages aMessage );

    /**
     * Check if it's allowed to show operator indicator.
     * Number of rules control operator indicator visibility.
     * Parameter aMessage is used for fulfilling these rules - it can have
     * various values so this method is called many times. The sum of the
     * calls (and thus sum of different aMessage values) defines whether
     * rules are fulfilled and indicator is shown.
     * @param aMessage Network message
     * @return ETrue if rules are not fulfilled and indicator should
     *                  not be shown.
     */
    TBool NotAllowedToDisplayOperatorIndicator( const TNWMessages aMessage );

    /**
     * Check status of keylock.
     */
    TBool IsKeyLockEnabled();

    /**
     * Set current operator name string based on the set priority.
     *
     * @param aOperatorName New operator name string.
     * @param aPriority Operator name priority.
     */
    void SetOperatorName( const TPtrC& aOperatorName, TInt aPriority );

    /**
     * Used for check if SPN should be skipped and PLMN need to be displayed instead.
     */
    TBool DoesOperatorPreferPlmnName( TInt aMCC, TInt aMNC ) const;

private: // Data

    /**
     * Session to network handling engine.
     * Own.
     */
    CNWSession* iNWSession;

    /**
     * Cached network info structure. Will be updated by iNWSession in a
     * callback.
     */
    TNWInfo iNWInfo;

    /**
     * Previous network information.
     */
    TNWInfo iOldInfo;

    /**
     * Message cache. There is only one instance of one type of message.
     * Own.
     */
    CArrayFixFlat<MNWMessageObserver::TNWMessages>* iMessageCache;

    /**
     * Sorting rule.
     */
    TKeyArrayFix iKeyProperties;

    /**
     * Holds the operator name.
     * Own.
     */
    HBufC* iOperatorName;

    /**
     * Network identity name
     */
    TPtrC iNetworkIdentityName;

    /**
     * Network related message flags
     */
    enum TNetInfoFlags
        {
        ENetworkProviderNameReceived      = 0x00000001,
        ENetworkProviderNameOk            = 0x00000002,
        EServiceProviderNameReceived      = 0x00000004,
        EServiceProviderNameOk            = 0x00000008,
        ERegistrationStatusReceived       = 0x00000010,
        ENetworkInfoChangeReceived        = 0x00000020,
        EProgrammableOperatorInfoReceived = 0x00000040,
        EProgrammableOperatorInfoReceivedOk = 0x00000080,
        ECSRegistrationNotOk              = 0x00000100
        };

    /**
     * Subset of sum of TNetInfoFlags.
     */
    TUint iReceivedMessageFlags;

    /**
     * Subset of sum of old TNetInfoFlags.
     */
    TUint iOldReceivedMessageFlags;

    /**
     * Operator name priority.
     */
    TInt iOperatorPriority;

    //MNetworkOperatorNameObserver& iObserver; //TBD : Make this as a list

    /** flag for KFeatureIdFfEePnnSpn feature */
    TBool   iFfEePnnSpnFeatSupported;

    /** flag to track the capabilities of the client app*/
    bool iNWSesssionstatus;

    //Observers list
    QList<MNetworkOperatorNameObserver *> m_observers;

    //QString Operator name
    QString m_operatorName;

    };


#endif //NETWORKOPERATORNAMELISTENER_S60_H
