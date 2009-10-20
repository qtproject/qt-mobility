/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <multimedia/qmediaencodersettings.h>

class QAudioEncoderSettingsPrivate  : public QSharedData
{
public:
    QAudioEncoderSettingsPrivate() :
        isNull(true),
        encodingMode(QtMedia::ConstantQualityEncoding),
        bitrate(-1),
        sampleRate(-1),
        channels(-1),
        quality(QtMedia::NormalQuality)
    {
    }

    QAudioEncoderSettingsPrivate(const QAudioEncoderSettingsPrivate &other):
        QSharedData(other),
        isNull(other.isNull),
        encodingMode(other.encodingMode),
        codec(other.codec),
        bitrate(other.bitrate),
        sampleRate(other.sampleRate),
        channels(other.channels),
        quality(other.quality)
    {
    }

    bool isNull;
    QtMedia::EncodingMode encodingMode;
    QString codec;
    int bitrate;
    int sampleRate;
    int channels;
    QtMedia::EncodingQuality quality;

private:
    QAudioEncoderSettingsPrivate& operator=(const QAudioEncoderSettingsPrivate &other);
};

/*!
  \class QAudioEncoderSettings
  \ingroup multimedia

  \preliminary
  \brief The QAudioEncoderSettings class is use to pass around audio stream information.

*/

/*!
  Construct a QAudioEncoderSettings.
*/
QAudioEncoderSettings::QAudioEncoderSettings()
    :d(new QAudioEncoderSettingsPrivate)
{
}

/*!
  Construct a QAudioEncoderSettings from \a other.
*/

QAudioEncoderSettings::QAudioEncoderSettings(const QAudioEncoderSettings& other)
    :d(other.d)
{
}

/*!
  Destroy this object.
*/

QAudioEncoderSettings::~QAudioEncoderSettings()
{
}

/*!
    Construst a QAudioEncoderSettings from \a other.
*/

QAudioEncoderSettings& QAudioEncoderSettings::operator=(const QAudioEncoderSettings &other)
{
    d = other.d;
    return *this;
}

/*!
    Return true if equal to current settings.
*/

bool QAudioEncoderSettings::operator==(const QAudioEncoderSettings &other) const
{
    return (d == other.d) ||
           (d->isNull == other.d->isNull &&
            d->encodingMode == other.d->encodingMode &&
            d->bitrate == other.d->bitrate &&
            d->sampleRate == other.d->sampleRate &&
            d->channels == other.d->channels &&
            d->quality == other.d->quality &&
            d->codec == other.d->codec);
}

/*!
    Return true if equal to current settings.
*/

bool QAudioEncoderSettings::operator!=(const QAudioEncoderSettings &other) const
{
    return !(*this == other);
}

/*!
  Returns true if a valid object.
*/

bool QAudioEncoderSettings::isNull() const
{
    return d->isNull;
}

/*!
  Returns the audio encoding mode.

  \sa QtMedia::EncodingMode
*/
QtMedia::EncodingMode QAudioEncoderSettings::encodingMode() const
{
    return d->encodingMode;
}

/*!
  Sets the audio encoding \a mode.

  \sa encodingMode(), QtMedia::EncodingMode
*/
void QAudioEncoderSettings::setEncodingMode(QtMedia::EncodingMode mode)
{
    d->encodingMode = mode;
}

/*!
  Returns the audio codec.
*/
QString QAudioEncoderSettings::codec() const
{
    return d->codec;
}

/*!
  Sets the audio \a codec.
*/
void QAudioEncoderSettings::setCodec(const QString& codec)
{
    d->isNull = false;
    d->codec = codec;
}

/*!
  Returns the bitrate of compressed audio stream in bits per second.
*/
int QAudioEncoderSettings::bitrate() const
{
    return d->bitrate;
}

/*!
  Returns the number of audio channels.
*/
int QAudioEncoderSettings::channels() const
{
    return d->channels;
}

/*!
  Sets the number of audio \a channels.
  The default channels number of -1 means the service will
  choose the appropriate value.
*/
void QAudioEncoderSettings::setChannels(int channels)
{
    d->isNull = false;
    d->channels = channels;
}

/*!
  Sets the audio \a bitrate.
*/
void QAudioEncoderSettings::setBitrate(int bitrate)
{
    d->isNull = false;
    d->bitrate = bitrate;
}

/*!
  Returns the sample rate of audio stream.
*/
int QAudioEncoderSettings::sampleRate() const
{
    return d->sampleRate;
}

/*!
  Sets the sample \a rate of audio stream.

  Frequency value of -1 means the encoder will choose the
  suitable rate depending on source and codec capabilities.
  */
void QAudioEncoderSettings::setSampleRate(int rate)
{
    d->isNull = false;
    d->sampleRate = rate;
}

/*!
  Returns the audio encoding quality parameter.

  \sa setQuality()
*/

QtMedia::EncodingQuality QAudioEncoderSettings::quality() const
{
    return d->quality;
}

/*!
  Set the encoding quality of audio stream to \a value.

  \sa quality()
*/
void QAudioEncoderSettings::setQuality(QtMedia::EncodingQuality value)
{
    d->isNull = false;
    d->quality = value;
}

class QVideoEncoderSettingsPrivate  : public QSharedData
{
public:
    QVideoEncoderSettingsPrivate() :
        isNull(true),
        encodingMode(QtMedia::ConstantQualityEncoding),
        bitrate(-1),
        frameRate(0),
        quality(QtMedia::NormalQuality)
    {
    }

    QVideoEncoderSettingsPrivate(const QVideoEncoderSettingsPrivate &other):
        QSharedData(other),
        isNull(other.isNull),
        encodingMode(other.encodingMode),
        codec(other.codec),
        bitrate(other.bitrate),
        resolution(other.resolution),
        frameRate(other.frameRate),
        quality(other.quality)
    {
    }

    bool isNull;
    QtMedia::EncodingMode encodingMode;
    QString codec;
    int bitrate;
    QSize resolution;
    qreal frameRate;
    QtMedia::EncodingQuality quality;

private:
    QVideoEncoderSettingsPrivate& operator=(const QVideoEncoderSettingsPrivate &other);
};

/*!
  \class QVideoEncoderSettings
  \ingroup multimedia

  \preliminary
  \brief The QVideoEncoderSettings class is use to pass around video stream information.

*/

/*!
  Construct a QVideoEncoderSettings.
*/

QVideoEncoderSettings::QVideoEncoderSettings()
    :d(new QVideoEncoderSettingsPrivate)
{
}

/*!
  Construct a QVideoEncoderSettings from \a other
*/

QVideoEncoderSettings::QVideoEncoderSettings(const QVideoEncoderSettings& other)
    :d(other.d)
{
}

/*!
  Destroy this object.
*/

QVideoEncoderSettings::~QVideoEncoderSettings()
{
}

QVideoEncoderSettings& QVideoEncoderSettings::operator=(const QVideoEncoderSettings &other)
{
    d = other.d;
    return *this;
}

bool QVideoEncoderSettings::operator==(const QVideoEncoderSettings &other) const
{
    return (d == other.d) ||
           (d->isNull == other.d->isNull &&
            d->encodingMode == other.d->encodingMode &&
            d->bitrate == other.d->bitrate &&
            d->quality == other.d->quality &&
            d->codec == other.d->codec &&
            d->resolution == other.d->resolution &&
            qFuzzyCompare(d->frameRate, other.d->frameRate));
}

bool QVideoEncoderSettings::operator!=(const QVideoEncoderSettings &other) const
{
    return !(*this == other);
}

bool QVideoEncoderSettings::isNull() const
{
    return d->isNull;
}

/*!
  Returns the video encoding mode.

  \sa QtMedia::EncodingMode
*/
QtMedia::EncodingMode QVideoEncoderSettings::encodingMode() const
{
    return d->encodingMode;
}

/*!
  Sets the video encoding \a mode.

  \sa encodingMode(), QtMedia::EncodingMode
*/
void QVideoEncoderSettings::setEncodingMode(QtMedia::EncodingMode mode)
{
    d->isNull = false;
    d->encodingMode = mode;
}

/*!
  Returns the currently used video codec name.
*/

QString QVideoEncoderSettings::codec() const
{
    return d->codec;
}

/*!
  Sets the video \a codec.
*/
void QVideoEncoderSettings::setCodec(const QString& codec)
{
    d->isNull = false;
    d->codec = codec;
}

/*!
  Returns bitrate of encoded video stream.
*/
int QVideoEncoderSettings::bitrate() const
{
    return d->bitrate;
}

/*!
  Set the bitrate of encoded video stream to \a value.
*/

void QVideoEncoderSettings::setBitrate(int value)
{
    d->isNull = false;
    d->bitrate = value;
}

/*!
  Returns frame rate of video stream.
*/
qreal QVideoEncoderSettings::frameRate() const
{
    return d->frameRate;
}

/*!
  \fn QVideoEncoderSettings::setFrameRate(qreal rate)

  Sets the video frame \a rate.
  If a null frame rate is passed, the default value should be used.
*/

void QVideoEncoderSettings::setFrameRate(qreal rate)
{
    d->isNull = false;
    d->frameRate = rate;
}

/*!
  Returns the resolution of encoded video stream.
*/

QSize QVideoEncoderSettings::resolution() const
{
    return d->resolution;
}

/*!
  Set the video \a resolution.
  If an empty size is passed,
  the media service should choose the default or content
  related resolution value.
*/
void QVideoEncoderSettings::setResolution(const QSize &resolution)
{
    d->isNull = false;
    d->resolution = resolution;
}

void QVideoEncoderSettings::setResolution(int width, int height)
{
    d->isNull = false;
    d->resolution = QSize(width, height);
}

/*!
  Video encoding quality value.
*/
QtMedia::EncodingQuality QVideoEncoderSettings::quality() const
{
    return d->quality;
}

/*!
  Sets the video encoding \a quality.
  \sa quality()
*/
void QVideoEncoderSettings::setQuality(QtMedia::EncodingQuality quality)
{
    d->isNull = false;
    d->quality = quality;
}

