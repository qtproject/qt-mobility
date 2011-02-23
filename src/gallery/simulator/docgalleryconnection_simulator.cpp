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

#include "docgalleryconnection_simulator.h"

#include <mobilityconnection_p.h>
#include <private/qsimulatordata_p.h>

#include <QtNetwork/QLocalSocket>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

namespace Simulator {
DocGalleryConnection::DocGalleryConnection(MobilityConnection *mobilityCon)
    : QObject(mobilityCon)
    , mConnection(mobilityCon)
    , mRegisteredWithSimulator(false)
    , mInitialDataReceived(false)
{
    qt_registerDocGalleryTypes();
    mobilityCon->addMessageHandler(this);
    getInitialData();
}

DocGalleryConnection *DocGalleryConnection::instance()
{
    static DocGalleryConnection *docGalleryConnection = 0;

    if (!docGalleryConnection)
        docGalleryConnection = new DocGalleryConnection(MobilityConnection::instance());
    return docGalleryConnection;
}


QLocalSocket *DocGalleryConnection::sendSocket()
{
    return mConnection->sendSocket();
}

void DocGalleryConnection::getInitialData()
{
    mInitialDataReceived = false;
    RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsDocGallery");

    // blockingly handle all remote calls until the simulator signals the initial data
    // sending is done
    QByteArray readBuffer;
    QLocalSocket *readSocket = mConnection->receiveSocket();
    while (!mInitialDataReceived) {
        readSocket->waitForReadyRead(100);
        readBuffer += readSocket->readAll();
        forever {
            IncomingRemoteMetacall rpc;
            if (rpc.read(&readBuffer)) {
                if (rpc.call(readSocket, this))
                    continue;
                qWarning("Ignoring a call to %s,", rpc.signature().data());
            }
            break;
        }
    }
}

void DocGalleryConnection::initialDocGalleryDataSent()
{
    mInitialDataReceived = true;
}

void DocGalleryConnection::setDocGalleryData(const QtMobility::DocGallerySimulatorData &data)
{
    mGalleryData = data;
}

DocGallerySimulatorData DocGalleryConnection::galleryData() const
{
    return mGalleryData;
}

#include "moc_docgalleryconnection_simulator.cpp"
}

QTM_END_NAMESPACE
