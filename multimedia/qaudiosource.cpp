/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <multimedia/qmediaobject_p.h>
#include <multimedia/qaudiosource.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qmediarecordercontrol.h>

/*!
    \class QAudioSource
    \brief The QAudioSource class provides an interface to query and select an audio format.
    \ingroup multimedia

    \preliminary

    QAudioSource provides access to the audio formats available from the audio devices available on your system.

    You can query formats available and select a format to use.

    A format in this context is a set consisting of a specific byte order,
    channel, codec, frequency, sample rate and sample type.
    A format is represented by the QAudioFormat class.

    The combinations supported are dependent on the platform,
    plugins installed and the audio device capabilities.

    A typical implementation example:
    \code
        QAudioSource* audiosource = new QAudioSource;
        QMediaRecorder* capture = new QMediaRecorder(audiosource);
    \endcode

    The audiosource interface is then used to:

    - Get and Set the audio format used (this is dependant on the audio device selected in the capture interface)

    The capture interface is then used to:

    - Set the destination source using setSink()

    - Get and Set the audio device using supportedEndpoints(), setActiveEndpoint()

    - Control the recording using record(),stop()

    \sa QMediaRecorder
*/

class QAudioSourcePrivate : public QMediaObjectPrivate
{
public:
    QAudioSourcePrivate():audioEncoderControl(0), mediaRecorderControl(0) {}
    QAudioEncoderControl  *audioEncoderControl;
    QMediaRecorderControl *mediaRecorderControl;
};

/*!
    Construct a QAudioSource using the QMediaService from \a provider, with \a parent.
*/

QAudioSource::QAudioSource(QObject *parent, QMediaServiceProvider *provider):
    QMediaObject(*new QAudioSourcePrivate, parent, provider->requestService("audiosource"))
{
    Q_D(QAudioSource);

    Q_ASSERT(d->service != 0);

    if (d->service != 0) {
        d->audioEncoderControl = qobject_cast<QAudioEncoderControl *>(d->service->control(QAudioEncoderControl_iid));
        d->mediaRecorderControl = qobject_cast<QMediaRecorderControl *>(d->service->control(QMediaRecorderControl_iid));
    }
}

/*!
    Destroys the audiosource object.
*/

QAudioSource::~QAudioSource()
{
}

/*!
    Returns true if audiosource device available.
*/

bool QAudioSource::isValid() const
{
    Q_D(const QAudioSource);

    return QMediaObject::isValid() &&
            (d->audioEncoderControl != NULL && d->mediaRecorderControl != NULL);
}

/*!
    Returns the audio format.

    \sa QAudioFormat
*/

QAudioFormat QAudioSource::format() const
{
    if(d_func()->audioEncoderControl) {
        QAudioFormat fmt;
        fmt.setFrequency(d_func()->audioEncoderControl->frequency());
        fmt.setChannels(d_func()->audioEncoderControl->channels());
        fmt.setSampleSize(d_func()->audioEncoderControl->sampleSize());
        fmt.setCodec(d_func()->audioEncoderControl->audioCodec());
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        return fmt;
    }

    return QAudioFormat();
}

/*!
    Sets the audio format to \a format.

    \sa QAudioFormat
*/

void QAudioSource::setFormat(const QAudioFormat &format)
{
    if(d_func()->audioEncoderControl) {
        d_func()->audioEncoderControl->setAudioCodec(format.codec());
        d_func()->audioEncoderControl->setFrequency(format.frequency());
        d_func()->audioEncoderControl->setChannels(format.channels());
        d_func()->audioEncoderControl->setSampleSize(format.sampleSize());
    }
}

/*!
    Returns true if able to set the audio format to \a format.
*/

bool QAudioSource::isFormatSupported(const QAudioFormat &format) const
{
    if(d_func()->audioEncoderControl) {
        if(!d_func()->audioEncoderControl->supportedAudioCodecs().contains(format.codec()))
            return false;
        if(!d_func()->audioEncoderControl->supportedFrequencies().contains(format.frequency()))
            return false;
        if(!d_func()->audioEncoderControl->supportedChannelCounts().contains(format.channels()))
            return false;
        if(!d_func()->audioEncoderControl->supportedSampleSizes().contains(format.sampleSize()))
            return false;

        return true;
    }

    return false;
}

/*!
    Returns a list of available codecs.

    For example "audio/pcm" for Linear PCM audio data.

    For additional codec values

    \sa QAudioDeviceInfo::supportedCodecs()
*/

QStringList QAudioSource::supportedCodecs() const
{
    QStringList list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedAudioCodecs();

    return list;
}

/*!
    Returns a list of available frequencies.

    Frequency is in Hertz.

    For example 8000,11025,22100,44100
*/

QList<int> QAudioSource::supportedFrequencies() const
{
    QList<int> list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedFrequencies();

    return list;
}

/*!
    Returns a list of available channels.

    This is typically 1 for mono, 2 for stereo.
*/

QList<int> QAudioSource::supportedChannels() const
{
    QList<int> list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedChannelCounts();

    return list;
}

/*!
    Returns a list of available sample sizes.

    Sample Size is the number of bits used for a sample.
    This is typically 8 or 16.
*/

QList<int> QAudioSource::supportedSampleSizes() const
{
    QList<int> list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedSampleSizes();

    return list;
}

