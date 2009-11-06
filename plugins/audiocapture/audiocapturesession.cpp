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
    wavFile = false;
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
        if(format.codec().contains(QLatin1String("audio/x-wav"))) {
            QAudioFormat fmt = format;
            fmt.setCodec("audio/pcm");
            return m_deviceInfo->isFormatSupported(fmt);
        } else
            return m_deviceInfo->isFormatSupported(format);
    }
    return false;
}

bool AudioCaptureSession::setFormat(const QAudioFormat &format)
{
    if(m_deviceInfo) {

        QAudioFormat fmt = format;

        if(format.codec().contains(QLatin1String("audio/x-wav"))) {
            wavFile = true;
            fmt.setCodec("audio/pcm");
        }

        if(m_deviceInfo->isFormatSupported(fmt)) {
            m_format = fmt;
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
        list << "audio/x-wav";
    }
    return list;
}

QString AudioCaptureSession::codecDescription(const QString &codecName)
{
    if(m_deviceInfo) {
        if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/pcm") == 0)
            return QString(tr("Pulse Code Modulation"));
        if(qstrcmp(codecName.toLocal8Bit().constData(),"audio/x-wav") == 0)
            return QString(tr("WAV file format"));
    }
    return QString();
}

bool AudioCaptureSession::setAudioCodec(const QString &codecName)
{
    if(m_deviceInfo) {
        QStringList codecs = m_deviceInfo->supportedCodecs();
        codecs << "audio/x-wav";
        if(codecs.contains(codecName)) {
            if(codecName.contains(QLatin1String("audio/x-wav"))) {
                m_format.setCodec("audio/pcm");
                wavFile = true;
            } else {
                wavFile = false;
                m_format.setCodec(codecName);
            }

            return true;
        }
    }
    return false;
}

QString AudioCaptureSession::audioCodec() const
{
    if(wavFile)
        return QString("audio/x-wav");

    return m_format.codec();
}

QUrl AudioCaptureSession::outputLocation() const
{
    return m_sink;
}

bool AudioCaptureSession::setOutputLocation(const QUrl& sink)
{
    m_sink = sink;
    if(sink.toLocalFile().length() > 0)
        file.setFileName(sink.toLocalFile());
    else
        file.setFileName(sink.toString());

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
                //TODO: write wav header id wavFile true

                memset(&header,0,sizeof(CombinedHeader));
                memcpy(header.riff.descriptor.id,"RIFF",4);
                header.riff.descriptor.size = 0xFFFFFFFF; // This should be updated on stop() TODO! filesize-8
                memcpy(header.riff.type,"WAVE",4);
                memcpy(header.wave.descriptor.id,"fmt ",4);
                header.wave.descriptor.size = 16;
                header.wave.audioFormat = 1; // for PCM data
                header.wave.numChannels = m_format.channels();
                header.wave.sampleRate = m_format.frequency();
                header.wave.byteRate = m_format.frequency()*m_format.channels()*m_format.sampleSize()/8;
                header.wave.blockAlign = m_format.channels()*m_format.sampleSize()/8;
                header.wave.bitsPerSample = m_format.sampleSize();
                memcpy(header.data.descriptor.id,"data",4);
                header.data.descriptor.size = 0xFFFFFFFF; // This should be updated on stop() TODO! samples*channels*sampleSize/8
                file.write((char*)&header,sizeof(CombinedHeader));

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

        QFile tmpFile("record.tmp");
        tmpFile.open(QIODevice::WriteOnly);

        qint32 fileSize = file.size()-8;
        if(file.open(QIODevice::ReadOnly)) {
            file.read((char*)&header,sizeof(CombinedHeader));
            header.riff.descriptor.size = fileSize; // filesize-8
            header.data.descriptor.size = fileSize-44; // samples*channels*sampleSize/8
            tmpFile.write((char*)&header,sizeof(CombinedHeader));
            char buf[4096];
            while(!file.atEnd()) {
                int l = file.read(buf,4096);
                if(l > 0)
                    tmpFile.write(buf,l);
            }
            tmpFile.close();
            file.close();
            file.remove();
            tmpFile.rename(file.fileName());
        }
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



