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

#include <QtCore/qstring.h>

#include "s60cameracapturedestinationcontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60imagecapturesettings.h"

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
    connect(m_session->settings(), SIGNAL(destinationChanged(const QCameraImageCapture::CaptureDestinations)),
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
        m_session->settings()->setCaptureDestination(destination);
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
