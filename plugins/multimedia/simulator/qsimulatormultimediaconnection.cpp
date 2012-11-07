/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "../qsimulatormultimediaconnection_p.h"
#include "mobilitysimulatorglobal.h"
#include <mobilityconnection_p.h>

#include <private/qsimulatordata_p.h>

#include <QtNetwork/QLocalSocket>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

Q_GLOBAL_STATIC(QCameraData, qtCameraData)

namespace Simulator
{
    MultimediaConnection::MultimediaConnection(MobilityConnection *mobilityCon)
        : QObject(mobilityCon)
        , mConnection(mobilityCon)
        , mInitialDataReceived(false)
    {
        qt_registerCameraTypes();
        mobilityCon->addMessageHandler(this);
    }

    void MultimediaConnection::getInitialData()
    {
        RemoteMetacall<void>::call(mConnection->sendSocket(), NoSync, "setRequestsCameras");

        while (!mInitialDataReceived)
            mConnection->receiveSocket()->waitForReadyRead(100);
    }

    void MultimediaConnection::initialCameraDataSent()
    {
        mInitialDataReceived = true;
    }

    void MultimediaConnection::setCameraData(const QCameraData &data)
    {
        *qtCameraData() = data;
        emit cameraDataChanged(data);
    }

    void MultimediaConnection::addCamera(const QString &name, const QCameraData::QCameraDetails &details)
    {
        if (qtCameraData()->cameras.contains(name))
            return;

        qtCameraData()->cameras.insert(name, details);
        emit cameraAdded(name, details);
    }

    void MultimediaConnection::removeCamera(const QString &name)
    {
        if (!qtCameraData()->cameras.contains(name))
            return;

        qtCameraData()->cameras.remove(name);
        emit cameraRemoved(name);
    }

    void MultimediaConnection::changeCamera(const QString &name, const QCameraData::QCameraDetails &details)
    {
        if (!qtCameraData()->cameras.contains(name))
            return;

        qtCameraData()->cameras[name] = details;
        emit cameraChanged(name, details);
    }

} // namespace

QCameraData get_qtCameraData()
{
    return *qtCameraData();
}

#include "moc_qsimulatormultimediaconnection_p.cpp"

QTM_END_NAMESPACE
