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

#include <qvideoencodercontrol.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>

/*!
  Create a new video encode control object with the given \a parent.
*/
QVideoEncoderControl::QVideoEncoderControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}

/*!
  Destroys the video encode control.
*/
QVideoEncoderControl::~QVideoEncoderControl()
{
}


/*!
  \fn QVideoEncoderControl::resolution() const

  Returns the resolution of encoded video stream.
*/

/*!
  \fn QVideoEncoderControl::minimumResolution() const

  Returns the smallest resolution, video encoder supports.

  \sa resolution
*/

/*!
  \fn QVideoEncoderControl::maximumResolution() const

  Returns the largest resolution, video encoder supports.

  \sa resolution
*/

/*!
  \fn QVideoEncoderControl::setResolution(const QSize &size)

  Set the video \a resolution.
  If an empty size is passed,
  the media service should choose the default or content
  related resolution value.
*/

/*!
  \fn QVideoEncoderControl::frameRate() const
  Returns frame rate of video stream.
*/

/*!
  \fn QVideoEncoderControl::minimumFrameRate() const

  Returns the smallest frame rate, video encoder supports.

  \sa frameRate
*/

/*!
  \fn QVideoEncoderControl::maximumFrameRate() const

  Returns the largest frame rate, video encoder supports.

  \sa frameRate
*/

/*!
  \fn QVideoEncoderControl::setFrameRate(const QMediaRecorder::FrameRate &rate)

  Sets the video frame \a rate.
  If passed frame rate equals to -1/-1, the default value should be used.
*/

/*!
  \fn QVideoEncoderControl::supportedVideoCodecs() const

  Returns the list of supported video codec names.
*/

/*!
  \fn QVideoEncoderControl::videoCodec() const

  Returns the currently used video codec name.
*/

/*!
  \fn QVideoEncoderControl::setVideoCodec(const QString &codecName)

  Use the video codec with name equals to \a codecName.

*/

/*!
  \fn QVideoEncoderControl::videoCodecDescription(const QString &codec) const

  Returns description of video \a codec.
*/

/*!
  \fn QVideoEncoderControl::bitrate() const

  Returns bitrate of encoded video stream.
*/

/*!
  \fn QVideoEncoderControl::setBitrate(int value)

  Set the bitrate of encoded video stream to \a value.
*/

/*!
  \fn QVideoEncoderControl::quality() const

  Video quality value.

  \sa QMediaRecorder::videoQuality
*/

/*!
  \fn QVideoEncoderControl::setQuality(int value)

  Sets the video quality to \a value.

  \sa quality(), QMediaRecorder::videoQuality
*/


/*!
  Returns the list of supported video encoding options.

  The list and meaninng of options is codec and system depended.
*/
QStringList QVideoEncoderControl::supportedEncodingOptions() const
{
    return QStringList();
}

/*!
  Returns value of encoder \a option.
*/
QVariant QVideoEncoderControl::encodingOption(const QString &option) const
{
    Q_UNUSED(option);
    return QVariant();
}

/*!
  Set the codec specific \a option to \a value.
*/
void QVideoEncoderControl::setEncodingOption(const QString &option, const QVariant &value)
{
    Q_UNUSED(option);
    Q_UNUSED(value);
}

/*!
  Returns the list of resolutions if the video encoder supports only the limited set
  of video frame sizes, otherwise returns an empty list.

  \sa resolution(), minimumResolution(), maximumResolution()
*/
QList<QSize> QVideoEncoderControl::supportedResolutions() const
{
    return QList<QSize>();
}

/*!
  Returns the list of frame rates if the video encoder supports only the limited set
  of video frame rates, otherwise returns an empty list.

  \sa frameRate
*/
QList<QMediaRecorder::FrameRate> QVideoEncoderControl::supportedFrameRates() const
{
    return QList< QMediaRecorder::FrameRate >();
}
