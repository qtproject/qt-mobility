/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qstring.h>

#include "s60cameracapturedestinationcontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"

S60CameraCaptureDestinationControl::S60CameraCaptureDestinationControl(QObject *parent):
    QCameraCaptureDestinationControl(parent)
{
}

S60CameraCaptureDestinationControl::S60CameraCaptureDestinationControl(S60ImageCaptureSession *session, QObject *parent):
    QCameraCaptureDestinationControl(parent),
    m_session(0),
    m_service(0),
    m_captureDestination(QCameraImageCapture::CaptureToFile)
{
    m_session = session;
    connect(m_session, SIGNAL(destinationChanged(const QCameraImageCapture::CaptureDestinations)),
        this, SLOT(handleCaptureDestinationChange(const QCameraImageCapture::CaptureDestinations)));
}

S60CameraCaptureDestinationControl::~S60CameraCaptureDestinationControl()
{
}

bool S60CameraCaptureDestinationControl::isCaptureDestinationSupported(
    QCameraImageCapture::CaptureDestinations destination) const
{
    if (destination == 0 ||
        destination == QCameraImageCapture::CaptureToFile ||
        destination == QCameraImageCapture::CaptureToBuffer ||
        destination == (QCameraImageCapture::CaptureToFile | QCameraImageCapture::CaptureToBuffer))
        return true;
    else
        return false;
}

QCameraImageCapture::CaptureDestinations S60CameraCaptureDestinationControl::captureDestination() const
{
    return m_captureDestination;
}

void S60CameraCaptureDestinationControl::setCaptureDestination(
    const QCameraImageCapture::CaptureDestinations destination)
{
    if (isCaptureDestinationSupported(destination))
        m_session->setCaptureDestination(destination);
    else
        m_session->setError(KErrNotSupported, tr("Requested capture destination is not supported."), true);
}

void S60CameraCaptureDestinationControl::handleCaptureDestinationChange(
    const QCameraImageCapture::CaptureDestinations destination)
{
    if (m_captureDestination != destination) {
        m_captureDestination = destination;
        emit captureDestinationChanged(m_captureDestination);
    }
}

// End of file
