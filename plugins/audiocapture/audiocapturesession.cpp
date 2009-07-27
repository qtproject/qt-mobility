/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "audiocapturesession.h"
#include "audioencode.h"
#include "qmediastreams.h"
#include "qmediacapture.h"

#include <QDebug>
#include <QUrl>

#include <QtMultimedia/qaudiodeviceinfo.h>

AudioCaptureSession::AudioCaptureSession(QObject *parent)
    :QMediaCaptureControl(parent),
    m_state(QMediaCapture::StoppedState)
{
    m_audioInput = 0;
    m_position = 0;
    m_audioEncodeControl = new AudioEncode(this);
}

AudioCaptureSession::~AudioCaptureSession()
{
    stop();

    if(m_audioInput)
        delete m_audioInput;
}

QMediaSink AudioCaptureSession::sink() const
{
    return m_sink;
}

bool AudioCaptureSession::setSink(const QMediaSink& sink)
{
    m_sink = sink;
    file.setFileName(sink.dataLocation().toString());
    return true;
}

int AudioCaptureSession::state() const
{
    return int(m_state);
}

qint64 AudioCaptureSession::position() const
{
    return m_position;
}

void AudioCaptureSession::record()
{
    if(m_audioInput) {
        if(m_state == QMediaCapture::StoppedState)
            file.open(QIODevice::WriteOnly);

        m_audioInput->start(qobject_cast<QIODevice*>(&file));
    }

    m_state = QMediaCapture::RecordingState;
}

void AudioCaptureSession::pause()
{
    if(m_audioInput)
        m_audioInput->stop();

    m_state = QMediaCapture::PausedState;
}

void AudioCaptureSession::stop()
{
    if(m_audioInput) {
        m_audioInput->stop();
        file.close();
        m_position = 0;
    }
    m_state = QMediaCapture::StoppedState;
}

void AudioCaptureSession::setCaptureDevice(const QString &deviceName)
{
    QList<QAudioDeviceId> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    for(int i=0;i<devices.size();i++) {
        if(QAudioDeviceInfo(devices.at(i),this).deviceName().contains(deviceName)) {
            m_audioInput = new QAudioInput(devices.at(i),m_audioEncodeControl->format(),this);
            connect(m_audioInput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateChanged(QAudio::State)));
            connect(m_audioInput,SIGNAL(notify()),this,SLOT(notify()));
            return;
        }
    }
}

void AudioCaptureSession::stateChanged(QAudio::State state)
{
    switch(state) {
        case QAudio::ActiveState:
            emit stateChanged(QMediaCapture::RecordingState);
            break;
        default:
            if(!((m_state == QMediaCapture::PausedState)||(m_state == QMediaCapture::StoppedState)))
                m_state = QMediaCapture::StoppedState;

            emit stateChanged(m_state);
            break;
    }
}

void AudioCaptureSession::notify()
{
    m_position += m_audioInput->notifyInterval();
    emit positionChanged(m_position);
}
