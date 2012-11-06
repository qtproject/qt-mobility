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

#include "s60imageencodercontrol.h"
#include "s60imagecapturesession.h"
#include "s60imagecapturesettings.h"

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
    // Discrete resolutions are returned
    if (continuous)
        *continuous = false;

    return m_session->settings()->supportedImageResolutionsForCodec(settings.codec());
}

QStringList S60ImageEncoderControl::supportedImageCodecs() const
{
    return m_session->settings()->supportedImageCodecs();
}

QString S60ImageEncoderControl::imageCodecDescription(const QString &codec) const
{
    return m_session->settings()->imageCodecDescription(codec);
}

QImageEncoderSettings S60ImageEncoderControl::imageSettings() const
{
    // Update setting values from session
    QImageEncoderSettings settings;
    settings.setCodec(m_session->settings()->imageCodec());
    settings.setResolution(m_session->settings()->imageResolution());
    settings.setQuality(m_session->settings()->imageQuality());

    return settings;
}

void S60ImageEncoderControl::setImageSettings(const QImageEncoderSettings &settings)
{
    if (!settings.isNull()) {
        // Notify that settings have been implicitly set and there's no need to
        // initialize them in case camera is changed
        m_session->settings()->notifySettingsSet();

        if (!settings.codec().isEmpty())
            m_session->settings()->setImageCodec(settings.codec());

        // Set quality before resolution as quality defines the
        // resolution used in case it's not explicitly set
        m_session->settings()->setImageQuality(settings.quality());
        m_session->settings()->setImageResolution(settings.resolution());

        // Prepare ImageCapture with the settings and set error if needed
        int prepareSuccess = m_session->prepareImageCapture();

        // Preparation fails with KErrNotReady if camera has not been started.
        // That can be ignored since settings are set internally in that case.
        if (prepareSuccess != KErrNotReady && prepareSuccess != KErrNone)
            m_session->setError(prepareSuccess, tr("Failed to apply image settings."), true);
    } else {
        m_session->setError(KErrNotSupported, tr("Unable to set undefined settings."), true);
    }
}

// End of file
