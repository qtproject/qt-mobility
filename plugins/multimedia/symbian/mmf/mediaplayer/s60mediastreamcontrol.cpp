/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "s60mediastreamcontrol.h"
#include "s60mediaplayersession.h"
#include "s60mediaplayercontrol.h"
#include <qmediastreamscontrol.h>

#include <QtCore/qdir.h>
#include <QtCore/qurl.h>
#include <QtCore/qdebug.h>

S60MediaStreamControl::S60MediaStreamControl(QObject *control, QObject *parent)
    : QMediaStreamsControl(parent)
    , m_control(NULL)
    , m_mediaType(S60MediaSettings::Unknown)
{
    m_control = qobject_cast<S60MediaPlayerControl*>(control);
    m_mediaType = m_control->mediaControlSettings().mediaType();
}

S60MediaStreamControl::~S60MediaStreamControl()
{
}

int S60MediaStreamControl::streamCount()
{
    int streamCount = 0;
    if (m_control->isAudioAvailable())
        streamCount++;
    if (m_control->isVideoAvailable())
        streamCount++;

    return streamCount;
}

QMediaStreamsControl::StreamType S60MediaStreamControl::streamType(int streamNumber)
{
    Q_UNUSED(streamNumber);

    QMediaStreamsControl::StreamType type = QMediaStreamsControl::UnknownStream;

    if (m_control->mediaControlSettings().mediaType() ==  S60MediaSettings::Video)
        type = QMediaStreamsControl::VideoStream;
    else
        type = QMediaStreamsControl::AudioStream;

    return type;
}

QVariant S60MediaStreamControl::metaData(int streamNumber, QtMultimediaKit::MetaData key)
{
    Q_UNUSED(streamNumber);

    if (m_control->session()) {
        if (m_control->session()->isMetadataAvailable())
            return m_control->session()->metaData(key);
    }
    return QVariant();
}

bool S60MediaStreamControl::isActive(int streamNumber)
{
    if (m_control->mediaControlSettings().mediaType() ==  S60MediaSettings::Video) {
        switch (streamNumber) {
        case 1:
            return m_control->isVideoAvailable();
        case 2:
            return m_control->isAudioAvailable();
        default:
            break;
        }
    }
    return m_control->isAudioAvailable();
}

void S60MediaStreamControl::setActive(int streamNumber, bool state)
{
    Q_UNUSED(streamNumber);
    Q_UNUSED(state);
    // Symbian MMF does not support enabling or disabling specific media streams
}

void S60MediaStreamControl::handleStreamsChanged()
{
    emit streamsChanged();
}
