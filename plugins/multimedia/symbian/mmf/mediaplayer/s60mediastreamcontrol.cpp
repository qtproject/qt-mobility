/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#include "s60mediastreamcontrol.h"
#include "s60mediaplayersession.h"
#include "s60mediaplayercontrol.h"
#include "s60mmtrace.h"

S60MediaStreamControl::S60MediaStreamControl(QObject *control, QObject *parent)
    : QMediaStreamsControl(parent)
    , m_control(0)
    , m_mediaType(S60MediaSettings::Unknown)
{
    TRACE("S60MediaStreamControl::S60MediaStreamControl" << qtThisPtr());
    m_control = qobject_cast<S60MediaPlayerControl*>(control);
    m_mediaType = m_control->mediaControlSettings().mediaType();
}

S60MediaStreamControl::~S60MediaStreamControl()
{
    TRACE("S60MediaStreamControl::~S60MediaStreamControl" << qtThisPtr());
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
    TRACE("S60MediaStreamControl::setActive" << qtThisPtr()
          << "streamNumber" << streamNumber << "state" << state);
    Q_UNUSED(streamNumber);
    Q_UNUSED(state);
    // Symbian MMF does not support enabling or disabling specific media streams
}

void S60MediaStreamControl::handleStreamsChanged()
{
    TRACE("S60MediaStreamControl::handleStreamsChanged" << qtThisPtr());
    emit streamsChanged();
}
