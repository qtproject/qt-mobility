/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#include "s60audioencodercontrol.h"
#include "s60videocapturesession.h"
#include "s60videocapturesettings.h"

S60AudioEncoderControl::S60AudioEncoderControl(QObject *parent) :
    QAudioEncoderControl(parent)
{
}

S60AudioEncoderControl::S60AudioEncoderControl(S60VideoCaptureSession *session, QObject *parent) :
    QAudioEncoderControl(parent)
{
    m_session = session;
}

S60AudioEncoderControl::~S60AudioEncoderControl()
{
}

QStringList S60AudioEncoderControl::supportedAudioCodecs() const
{
    return m_session->settings()->supportedAudioCodecs();
}

QString S60AudioEncoderControl::codecDescription(const QString &codecName) const
{
    // According to ForumNokia MMF camcorder plugin supports AAC, AMR and QCELP
    // QCELP is speech codec and can be discarded
    if (qstrcmp(codecName.toLocal8Bit().constData(), "audio/aac") == 0)
        return QLatin1String("Advanced Audio Coding");
    else if (qstrcmp(codecName.toLocal8Bit().constData(), "audio/amr") == 0)
        return QLatin1String("Adaptive Multi-Rate Audio Codec");

    return QString();
}

QStringList S60AudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec);

    QStringList supportedOptions;
    // Possible settings: EncodingMode, Codec, BitRate, ChannelCount, SampleRate, Quality
    // Possible (codec specific) Options: Audio Gain
    supportedOptions << QLatin1String("gain");

    return supportedOptions;
}

QVariant S60AudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    // Possible settings: EncodingMode, Codec, BitRate, ChannelCount, SampleRate, Quality
    // Possible (codec specific) Options: Audio Gain
    Q_UNUSED(codec);

    if (qstrcmp(name.toLocal8Bit().constData(), "gain") == 0)
        return QVariant(m_session->settings()->audioGain());

    return QVariant();
}

void S60AudioEncoderControl::setEncodingOption(
    const QString &codec, const QString &name, const QVariant &value)
{
    Q_UNUSED(codec)

    if (qstrcmp(name.toLocal8Bit().constData(), "gain") == 0) {
        bool convSuccess = false;
        int gain = value.toInt(&convSuccess);
        if (convSuccess) {
            m_session->settings()->setAudioGain(gain);
            return;
        }
    }

    m_session->setError(KErrNotSupported, tr("Audio encoding option is not supported"));
}

QList<int> S60AudioEncoderControl::supportedSampleRates(
    const QAudioEncoderSettings &settings, bool *continuous) const
{
    return m_session->settings()->supportedAudioSampleRates(settings, continuous);
}

QAudioEncoderSettings S60AudioEncoderControl::audioSettings() const
{
    return m_session->settings()->audioEncoderSettings();
}

void S60AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    if (!settings.isNull()) {
        // Notify that settings have been implicitly set and there's no need to
        // initialize them in case camera is changed
        m_session->settings()->notifySettingsSet();

        if (!settings.codec().isEmpty())
            m_session->settings()->setAudioCodec(settings.codec());
        if (settings.channelCount() != -1)
            m_session->settings()->setAudioChannelCount(settings.channelCount());

        // Set quality before BitRate and SampleRate (as quality defines unset settings)
        m_session->settings()->setAudioQuality(settings.quality());
        m_session->settings()->setAudioBitRate(settings.bitRate());
        m_session->settings()->setAudioSampleRate(settings.sampleRate());
        m_session->settings()->setAudioEncodingMode(settings.encodingMode());
    } else {
        m_session->setError(KErrNotSupported, tr("Unable to set undefined settings."));
    }
}

// End of file
