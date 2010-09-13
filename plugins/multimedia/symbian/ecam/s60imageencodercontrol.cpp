/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>

#include "s60imageencodercontrol.h"
#include "s60imagecapturesession.h"

S60ImageEncoderControl::S60ImageEncoderControl(QObject *parent) :
    QImageEncoderControl(parent)
{
}

S60ImageEncoderControl::S60ImageEncoderControl(S60ImageCaptureSession *session, QObject *parent) :
    QImageEncoderControl(parent)
{
    m_session = session;
}

S60ImageEncoderControl::~S60ImageEncoderControl()
{
}

QList<QSize> S60ImageEncoderControl::supportedResolutions(
        const QImageEncoderSettings &settings, bool *continuous) const
{
    QList<QSize> resolutions;
    if (m_session)
        resolutions = m_session->supportedCaptureSizesForCodec(settings.codec());

    // Discrete resolutions are returned
    *continuous = false;

    return resolutions;
}
QStringList S60ImageEncoderControl::supportedImageCodecs() const
{
    QStringList codecs;
    if (m_session)
        codecs = m_session->supportedImageCaptureCodecs();

    return codecs;
}

QString S60ImageEncoderControl::imageCodecDescription(const QString &codec) const
{
    QString formatDesc;
    if (m_session)
        formatDesc = m_session->imageCaptureCodecDescription(codec);

    return formatDesc;
}

QImageEncoderSettings S60ImageEncoderControl::imageSettings() const
{
    return m_imageEncoderSettings;
}
void S60ImageEncoderControl::setImageSettings(const QImageEncoderSettings &settings)
{
    m_imageEncoderSettings = settings;
    if (m_session && !settings.isNull()) {
        // Set codec first as optiomal capturesize is decided based on chosen codec
        m_session->setImageCaptureCodec(m_imageEncoderSettings.codec());
        m_session->setCaptureSize(m_imageEncoderSettings.resolution());
        m_session->setCaptureQuality(m_imageEncoderSettings.quality());
        // Update setting
        m_session->updateImageCaptureCodecs();
    }
}

// End of file
