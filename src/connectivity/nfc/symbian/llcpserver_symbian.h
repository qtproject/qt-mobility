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

#ifndef LLCPSERVER_SYMBIAN_H_
#define LLCPSERVER_SYMBIAN_H_

#include <e32base.h>
#include <nfcserver.h>                      // RNfcServer
#include <llcpprovider.h>                   // CLlcpProvider
#include <llcpconnorientedlistener.h>       // MLlcpConnOrientedListener
#include <qmobilityglobal.h>
#include "../qllcpserver_symbian_p.h"


class CLlcpSocketType2;
class RTest;

class CLlcpServer : public CBase,
                    public MLlcpConnOrientedListener
   {
public:
   /*!
    * Creates a new CLlcpServer object.
    */
   static CLlcpServer* NewL(QtMobility::QLlcpServerPrivate&);

   /*!
    * Creates a new CLlcpServer object.
    */
   static CLlcpServer* NewLC(QtMobility::QLlcpServerPrivate&);

   /*!
    * Destructor
    */
   ~CLlcpServer();

public:
   TBool Listen( const TDesC8& aServiceName);
   void StopListening();
   TBool isListening() const;
   CLlcpSocketType2 *nextPendingConnection();
   TBool hasPendingConnections() const;
   const TDesC8& serviceUri() const;

private: // From MLlcpConnOrientedListener
    void RemoteConnectRequest( MLlcpConnOrientedTransporter* aConnection );

private:
    // Constructor
    CLlcpServer(QtMobility::QLlcpServerPrivate&);

    // Second phase constructor
    void ConstructL();

private:

    RPointerArray<CLlcpSocketType2>  iLlcpSocketArray;

   /*!
    * Handle to NFC-server.
    * Own.
    */
   RNfcServer iNfcServer;

   /*!
    * Pointer to CLlcpProvider object.
    * Own.
    */
   CLlcpProvider* iLlcp;

   TBool iSocketListening;

   RBuf8 iServiceName;

   QtMobility::QLlcpServerPrivate& iCallback;
   RTest *iLog;
   };

#endif /* LLCPSERVER_SYMBIAN_H_ */
