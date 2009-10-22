/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
**
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

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtMultimedia/qaudiodeviceinfo.h>

#include <qmediarecorder.h>

#include "audiocapturesession.h"

AudioCaptureSession::AudioCaptureSession(QObject *parent):
    QObject(parent)
{
    m_deviceInfo = new QAudioDeviceInfo(QAudioDeviceInfo::defaultInputDevice());
    m_audioInput = 0;
    m_position = 0;
    m_state = QMediaRecorder::StoppedState;

    m_format.setFrequency(8000);
    m_format.setChannels(1);
    m_format.setSampleSize(8);
    m_format.setSampleType(QAudioFormat::UnSignedInt);
    m_format.setCodec("audio/pcm");
}

AudioCaptureSession::~AudioCaptureSession()
{
    stop();

    if(m_audioInput)
        delete m_audioInput;
}

QAudioDeviceInfo* AudioCaptureSession::deviceInfo() const
{
    return m_deviceInfo;
}

QAudioFormat AudioCaptureSession::format() const
{
    return m_format;
}

bool AudioCaptureSession::isFormatSupported(const QAudioFormat &format) const
{
    if(m_deviceInfo) {
        return m_deviceInfo->isFormatSupported(format);
    }
    return false;
}

bool AudioCaptureSession::setFormat(const QAudioFormat &format)
{
    if(m_deviceInfo) {
        if(m_deviceInfo->isFormatSupported(format)) {
            m_format = format;
            if(m_audioInput) delete m_audioInput;
            m_audioInput = 0;
            QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
            for(int i=0;i<devices.size();i++) {
                if(qstrcmp(m_deviceInfo->deviceName().toLocal8Bit().constData(),
                            devices.at(i).deviceName().toLocal8Bit().constData()) == 0) {
                    m_audioInput = new QAudioInput(devices.at(i),m_format);
                    connect(m_audioInput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(stateChanged(QAudio::State)));
                    connect(m_audioInput,SIGNAL(notify()),this,SLOT(notify()));
                    break;
                }
            }
        }
    }
    return false;
}

QStringList AudioCaptureSession::supportedAudioCodecs() const
{
    QStringList list;
    if(m_deviceInfo) {
        list = m_deviceInfo->supportedCodecs();
    }
    return list;
}

QString AudioCaptureSession::codecDescription(const QString &codecName)
{
    if(m_deviceInfo) {
        if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/pcm") == 0)
            return QString(tr("Pulse Code Modulation"));
    }
    return QString();
}

bool AudioCaptureSession::setAudioCodec(const QString &codecName)
{
    if(m_deviceInfo) {
        QStringList codecs = m_deviceInfo->supportedCodecs();
        if(codecs.contains(codecName)) {
            m_format.setCodec(codecName);
            return true;
        }
    }
    return false;
}

QString AudioCaptureSession::audioCodec() const
{
    return m_format.codec();
}

QUrl AudioCaptureSession::sink() const
{
    return m_sink;
}

bool AudioCaptureSession::setSink(const QUrl& sink)
{
    m_sink = sink;
    file.setFileName(sink.toLocalFile());
    return true;
}

qint64 AudioCaptureSession::position() const
{
    return m_position;
}

int AudioCaptureSession::state() const
{
    return int(m_state);
}

void AudioCaptureSession::record()
{
    if(!m_audioInput) {
        setFormat(m_format);
    }

    if(m_audioInput) {
        if(m_state == QMediaRecorder::StoppedState) {
            if(file.open(QIODevice::WriteOnly)) {
                m_audioInput->start(qobject_cast<QIODevice*>(&file));
            } else {
                qWarning()<<"can't open source, failed";
                m_state = QMediaRecorder::StoppedState;
                emit stateChanged(m_state);
            }
        }
    }

    m_state = QMediaRecorder::RecordingState;
}

void AudioCaptureSession::pause()
{
    if(m_audioInput)
        m_audioInput->stop();

    m_state = QMediaRecorder::PausedState;
}

void AudioCaptureSession::stop()
{
    if(m_audioInput) {
        m_audioInput->stop();
        file.close();
        m_position = 0;
    }
    m_state = QMediaRecorder::StoppedState;
}

void AudioCaptureSession::stateChanged(QAudio::State state)
{
    switch(state) {
        case QAudio::ActiveState:
            emit stateChanged(QMediaRecorder::RecordingState);
            break;
        default:
            if(!((m_state == QMediaRecorder::PausedState)||(m_state == QMediaRecorder::StoppedState)))
                m_state = QMediaRecorder::StoppedState;

            emit stateChanged(m_state);
            break;
    }
}

void AudioCaptureSession::notify()
{
    m_position += m_audioInput->notifyInterval();
    emit positionChanged(m_position);
}

void AudioCaptureSession::setCaptureDevice(const QString &deviceName)
{
    m_captureDevice = deviceName;
    if(m_deviceInfo)
        delete m_deviceInfo;

    m_deviceInfo = 0;

    QList<QAudioDeviceInfo> devices = QAudioDeviceInfo::deviceList(QAudio::AudioInput);
    for(int i = 0; i < devices.size(); i++) {
        if(qstrcmp(m_captureDevice.toLocal8Bit().constData(),
                    devices.at(i).deviceName().toLocal8Bit().constData())==0){
            m_deviceInfo = new QAudioDeviceInfo(QAudioDeviceInfo::defaultInputDevice());
            return;
        }
    }
    m_deviceInfo = new QAudioDeviceInfo(QAudioDeviceInfo::defaultInputDevice());
}



