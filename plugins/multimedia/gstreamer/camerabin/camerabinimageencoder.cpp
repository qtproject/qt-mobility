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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "camerabinimageencoder.h"
#include "camerabinsession.h"

#include <QtCore/qdebug.h>

CameraBinImageEncoder::CameraBinImageEncoder(CameraBinSession *session)
    :QImageEncoderControl(session), m_session(session)
{
}

CameraBinImageEncoder::~CameraBinImageEncoder()
{
}

QList<QSize> CameraBinImageEncoder::supportedResolutions(const QImageEncoderSettings &, bool *continuous) const
{
    qDebug() << "CameraBinImageEncoder::supportedResolutions()";
    if (continuous)
        *continuous = false;

    return m_session->supportedResolutions(qMakePair<int,int>(0,0), continuous, QCamera::CaptureStillImage);
}

QStringList CameraBinImageEncoder::supportedImageCodecs() const
{
    return QStringList() << "jpeg";
}

QString CameraBinImageEncoder::imageCodecDescription(const QString &codecName) const
{
    if (codecName == "jpeg")
        return tr("JPEG image");

    return QString();
}

QImageEncoderSettings CameraBinImageEncoder::imageSettings() const
{
    return m_settings;
}

void CameraBinImageEncoder::setImageSettings(const QImageEncoderSettings &settings)
{
    m_settings = settings;
    emit settingsChanged();
}
