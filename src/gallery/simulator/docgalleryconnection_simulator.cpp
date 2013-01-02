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
