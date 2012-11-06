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

#include <QtCore/qstring.h>

#include "s60cameracapturebufferformatcontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"
#include "s60imagecapturesettings.h"
#include "s60cameraconstants.h"

using namespace S60CameraConstants;

S60CameraCaptureBufferFormatControl::S60CameraCaptureBufferFormatControl(QObject *parent):
QCameraCaptureBufferFormatControl(parent)
{
}

S60CameraCaptureBufferFormatControl::S60CameraCaptureBufferFormatControl(
    S60ImageCaptureSession *session, QObject *parent):
    QCameraCaptureBufferFormatControl(parent),
    m_session(0),
    m_service(0),
    m_bufferCaptureFormat(KDefaultBufferCaptureQtFormat)
{
    m_session = session;
    connect(m_session->settings(), SIGNAL(bufferCaptureFormatChanged(const QVideoFrame::PixelFormat)),
        this, SLOT(bufferFormatChanged(const QVideoFrame::PixelFormat)));
}

S60CameraCaptureBufferFormatControl::~S60CameraCaptureBufferFormatControl()
{
}

QList<QVideoFrame::PixelFormat> S60CameraCaptureBufferFormatControl::supportedBufferFormats() const
{
    return m_session->settings()->supportedBufferCaptureFormats();
}

QVideoFrame::PixelFormat S60CameraCaptureBufferFormatControl::bufferFormat() const
{
    return m_bufferCaptureFormat;
}

void S60CameraCaptureBufferFormatControl::setBufferFormat(const QVideoFrame::PixelFormat format)
{
    if (supportedBufferFormats().contains(format))
        m_session->settings()->setBufferCaptureFormat(format);
    else
        m_session->setError(KErrNotSupported, tr("Requested buffer capture format is not supported."), true);
}

void S60CameraCaptureBufferFormatControl::bufferFormatChanged(const QVideoFrame::PixelFormat format)
{
    if (m_bufferCaptureFormat != format) {
        m_bufferCaptureFormat = format;
        emit bufferFormatChanged(m_bufferCaptureFormat);
    }
}

// End of file
