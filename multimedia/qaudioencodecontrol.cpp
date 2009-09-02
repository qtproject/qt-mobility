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

#include "qaudioencodecontrol.h"
#include <QtCore/qstringlist.h>


/*!
    \class QAudioEncodeControl
    \ingroup multimedia

    \preliminary
    \brief The abstract class for controling audio encoding parameters, this is provided
    by a QAbstractMediaService object, and is used by QMediaRecorder.

    \sa QAbstractMediaService, QMediaRecorder
*/


/*!
  Create a new audio encode control object with the given \a parent.
*/
QAudioEncodeControl::QAudioEncodeControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}

/*!
  Destroys the audio encode control.
*/
QAudioEncodeControl::~QAudioEncodeControl()
{
}



/*!
  \fn QAudioEncodeControl::format() const

  Returns the audio format being used.
*/

/*!
  \fn QAudioEncodeControl::isFormatSupported(const QAudioFormat &format) const

  Returns true if the given \a format is supported.
*/

/*!
  \fn QAudioEncodeControl::setFormat(const QAudioFormat &format)

  Set the audio format. If format is null, the media engine will
  choose some resonable default one.
*/

/*!
  \fn QAudioEncodeControl::supportedAudioCodecs() const

  Returns the list of supported audio codec names.
*/

/*!
  \fn QAudioEncodeControl::audioCodec() const

  Returns the current audio codec name.
*/

/*!
  \fn QAudioEncodeControl::setAudioCodec(const QString &codecName)

  Sets the audio codec to \a codecName.
*/

/*!
  \fn QAudioEncodeControl::codecDescription(const QString &codec) const

  Returns description of audio \a codec.
*/

/*!
  \fn QAudioEncodeControl::bitrate() const

  Returns the bitrate of compressed audio stream.
*/

/*!
  \fn QAudioEncodeControl::setBitrate(int)

  Sets the bitrate of encoded audio stream to \a value.
*/

/*!
  \fn QAudioEncodeControl::quality() const

  Returns the normalized audio quality parameter.

  \sa QAudioEncodeControl::quality
*/

/*!
  \fn QAudioEncodeControl::setQuality(qreal value)

  Set the quality of audio stream to \a value in range [0..100].

  \sa QAudioEncodeControl::quality
*/


/*!
  Returns the list of codec specific audio encoding options.
*/
QStringList QAudioEncodeControl::supportedEncodingOptions() const
{
    return QStringList();
}

/*!
   Returns value of audio encoding \a option.
*/
QVariant QAudioEncodeControl::encodingOption(const QString &option) const
{
    Q_UNUSED(option);
    return QVariant();
}

/*!
  Set the codec specific \a option to \a value.
*/
void QAudioEncodeControl::setEncodingOption(const QString &option, const QVariant &value)
{
    Q_UNUSED(option);
    Q_UNUSED(value);
}

