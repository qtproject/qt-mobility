/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "s60cameraimageencodercontrol.h"
#include "s60camerasession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>

S60CameraImageEncoderControl::S60CameraImageEncoderControl(QObject *parent)
    :QImageEncoderControl(parent)
{
}

S60CameraImageEncoderControl::S60CameraImageEncoderControl(QObject *session, QObject *parent)
   :QImageEncoderControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60CameraSession*>(session);
}

S60CameraImageEncoderControl::~S60CameraImageEncoderControl()
{
}

QSize S60CameraImageEncoderControl::resolution() const
{
    QSize resolution;
    if (m_session)
        resolution = m_session->captureSize();
    return resolution;
}
QSize S60CameraImageEncoderControl::minimumResolution() const
{
    QSize resolution;
    if (m_session)
        resolution = m_session->minimumCaptureSize();
    return resolution;
}
QSize S60CameraImageEncoderControl::maximumResolution() const
{
    QSize resolution;
    if (m_session)
        resolution = m_session->maximumCaptureSize();
    return resolution;
}
QList<QSize> S60CameraImageEncoderControl::supportedResolutions() const
{
    QList<QSize> resolutions;
    if (m_session)
        resolutions = m_session->supportedCaptureSizes();
    return resolutions;
}
void S60CameraImageEncoderControl::setResolution(const QSize &size)
{
    if (m_session)
        m_session->setCaptureSize(size);
}
QStringList S60CameraImageEncoderControl::supportedImageCodecs() const
{
    QStringList codecs;
    if (m_session)
        codecs = m_session->supportedImageCaptureCodecs();
    return codecs;
}
QString S60CameraImageEncoderControl::imageCodec() const
{
    QString codec;
    if (m_session)
        codec = m_session->imageCaptureCodec();
    return codec;
}
bool S60CameraImageEncoderControl::setImageCodec(const QString &codecName)
{
    bool success = false;
    if (m_session)
        success = m_session->setImageCaptureCodec(codecName);
    return success;
}

QString S60CameraImageEncoderControl::imageCodecDescription(const QString &codecName) const
{
    QString codecDesc;
    if (m_session)
        codecDesc = m_session->imageCaptureCodecDescription(codecName);
    return codecDesc;
}

QtMedia::EncodingQuality S60CameraImageEncoderControl::quality() const
{
    //TODO:  what is the quality if no session is available?
    int quality = KErrNotFound;
    if (m_session)
        quality = m_session->captureQuality();
    return (QtMedia::EncodingQuality) quality;
}
void S60CameraImageEncoderControl::setQuality(QtMedia::EncodingQuality quality)
{
    //TODO:
    if (m_session)
        m_session->setCaptureQuality(quality);
}
