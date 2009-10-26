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

#include <multimedia/qaudioencodercontrol.h>
#include <QtCore/qstringlist.h>


/*!
    \class QAudioEncoderControl
    \ingroup multimedia

    \preliminary
    \brief The abstract class for controling audio encoding parameters, this is provided
    by a QMediaService object, and is used by QMediaRecorder.

    The interface name of QAudioEncoderControl is \c com.nokia.Qt.QAudioEncoderControl/1.0 as
    defined in QAudioEncoderControl_iid.

    \sa QMediaService::control(), QMediaRecorder
*/

/*!
    \macro QAudioEncoderControl_iid

    \c com.nokia.Qt.AudioEncoderControl/1.0

    Defines the interface name of the QAudioEncoderControl class.

    \relates QAudioEncoderControl
*/

/*!
  Create a new audio encode control object with the given \a parent.
*/
QAudioEncoderControl::QAudioEncoderControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
  Destroys the audio encode control.
*/
QAudioEncoderControl::~QAudioEncoderControl()
{
}

/*!
  \fn QAudioEncoderControl::supportedAudioCodecs() const

  Returns the list of supported audio codec names.
*/

/*!
  \fn QString QAudioEncoderControl::audioCodec() const

  Returns the current audio codec being used.
*/

/*!
  \fn QAudioEncoderControl::codecDescription(const QString &codec) const

  Returns description of audio \a codec.
*/

/*!
  \fn QAudioEncoderControl::supportedSampleRates() const

  Returns the list of supported audio sample rates, if known.
*/

/*!
  \fn QAudioEncoderControl::channels() const

  Returns the number of audio channels or -1 if not specified.
*/


/*!
  \fn QAudioEncoderControl::setChannels(int channels)

  Sets the number of audio \a channels.
*/

/*!
  \fn QAudioEncoderControl::supportedChannelCounts() const

  Returns the list of supported audio channels counts, if known.
*/

/*!
  \fn QAudioEncoderControl::sampleSize() const

  Returns the sample size of uncompressed audio stream,
  or -1 if not specified.
*/

/*!
  \fn QAudioEncoderControl::setSampleSize(int sampleSize)

  Sets the sample size of uncompressed audio stream to \a sampleSize.
*/

/*!
  \fn QAudioEncoderControl::supportedSampleSizes() const

  Returns the list of supported audio sample sizes, if known.
*/

/*!
    \fn QAudioEncoderControl::supportedEncodingOptions(const QString &codec) const

    Returns the list of \a codec specific audio encoding options.
*/

/*!
    \fn QAudioEncoderControl::encodingOption(const QString &codec, const QString &option) const

    Returns the value of audio encoding \a option for \a codec.
*/

/*!
    \fn QAudioEncoderControl::setEncodingOption(const QString &codec, const QString &option, const QVariant &value)

    Set the \a codec specific \a option to \a value.
*/

/*!
    \fn QAudioEncoderControl::quality() const

    Returns the quality level being used.
*/

/*!
    \fn QAudioEncoderControl::setQuality(QtMedia::EncodingQuality quality)

    Sets the quality value to \a quality.
*/

/*!
    \fn bool QAudioEncoderControl::setAudioCodec(const QString &codecName)

    Returns true if the audio codec is changed to \a codecName.
*/

/*!
    \fn void QAudioEncoderControl::setBitrate(int rate)

    Changes the bitrate to \a rate.
*/

/*!
    \fn int QAudioEncoderControl::bitrate() const

    Returns the current bitrate being used.
*/

/*!
    \fn QAudioEncoderControl::setSampleRate(int sampleRate)

    Changes the sample rate to \a sampleRate.
*/

/*!
    \fn int QAudioEncoderControl::sampleRate() const

    Returns the current sample rate being used.
*/

