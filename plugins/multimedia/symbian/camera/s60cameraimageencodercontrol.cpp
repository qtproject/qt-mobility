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
#include "s60imagecapturesession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>

S60CameraImageEncoderControl::S60CameraImageEncoderControl(QObject *parent)
    :QImageEncoderControl(parent)
{
}

S60CameraImageEncoderControl::S60CameraImageEncoderControl(QObject *session, QObject *parent)
   :QImageEncoderControl(parent)
{
    m_session = qobject_cast<S60ImageCaptureSession*>(session);
}

S60CameraImageEncoderControl::~S60CameraImageEncoderControl()
{
}

QList<QSize> S60CameraImageEncoderControl::supportedResolutions(
        const QImageEncoderSettings &settings, bool *continuous) const
{
    // we support arbitary resolutions. they are converted to nearest real resolution
    //*continuous = true;
    QList<QSize> resolutions;
    if (m_session)
        resolutions = m_session->supportedCaptureSizesForCodec(settings.codec());
    return resolutions;
}
QStringList S60CameraImageEncoderControl::supportedImageCodecs() const
{
    QStringList codecs;
    if (m_session)
        codecs = m_session->supportedImageCaptureCodecs();
    return codecs;
}

QString S60CameraImageEncoderControl::imageCodecDescription(const QString &codecName) const
{
    QString codecDesc;
    if (m_session)
        codecDesc = m_session->imageCaptureCodecDescription(codecName);
    return codecDesc;
}

QImageEncoderSettings S60CameraImageEncoderControl::imageSettings() const
{
    return m_imageEncoderSettings;
}
void S60CameraImageEncoderControl::setImageSettings(const QImageEncoderSettings &settings)
{
    m_imageEncoderSettings = settings;
    if (m_session && !settings.isNull()) {
        // set codec first as optiomal capturesize is decided based on chosen codec 
        m_session->setImageCaptureCodec(m_imageEncoderSettings.codec());
        m_session->setCaptureSize(m_imageEncoderSettings.resolution());
        m_session->setCaptureQuality(m_imageEncoderSettings.quality());
        //update setting 
        m_session->updateImageCaptureCodecs();
    }
}
