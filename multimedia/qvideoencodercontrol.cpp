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

#include <multimedia/qvideoencodercontrol.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qvariant.h>

/*!
    \class QVideoEncoderControl
    \preliminary
    \brief The QVideoEncoderControl class provides access to the settings of a media service that
    performs video encoding.

    If a QMediaService supports encoding video data it will implement QVideoEncoderControl.  This
    control provides information about the limits of restricted video encoder options and allows
    the selection of a set of video encoder settings as specificed in a QVideoEncoderSettings
    object.

    The functionality provided by this control is exposed to application code through the
    QMediaRecorder class.

    The interface name of QVideoEncoderControl is \c com.nokia.Qt.QVideoEncoderControl/1.0 as
    defined in QVideoEncoderControl_iid.

    \sa QMediaService::control(), QMediaRecorder
*/

/*!
    \macro QVideoEncoderControl_iid

    \c com.nokia.Qt.QVideoEncoderControl/1.0

    Defines the interface name of the QVideoEncoderControl class.

    \relates QVideoEncoderControl
*/

/*!
    Create a new video encoder control object with the given \a parent.
*/
QVideoEncoderControl::QVideoEncoderControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys a video encoder control.
*/
QVideoEncoderControl::~QVideoEncoderControl()
{
}

/*!
    \fn QVideoEncoderControl::minimumResolution() const

    Returns the minium resolution a video encoder supports.

    \sa QVideoEncoderSettings::resolution()
*/

/*!
    \fn QVideoEncoderControl::maximumResolution() const

    Returns the maximum resolution a video encoder supports.

    \sa QVideoEncoderSettings::resolution()
*/

/*!
    \fn QVideoEncoderControl::minimumFrameRate() const

    Returns the minimum frame rate a video encoder supports.

    \sa QVideoEncoderSettings::frameRate()
*/

/*!
    \fn QVideoEncoderControl::maximumFrameRate() const

    Returns the maximum frame rate a video encoder supports.

    \sa QVideoEncoderSettings::frameRate()
*/

/*!
    \fn QVideoEncoderControl::supportedVideoCodecs() const

    Returns the list of supported video codecs.
*/

/*!
    \fn QVideoEncoderControl::videoCodecDescription(const QString &codec) const

    Returns a description of a video \a codec.
*/

/*!
    \fn QVideoEncoderControl::supportedEncodingOptions(const QString &codec) const

    Returns a list of supported encoding options for a video \a codec.

    The names and types of the options in the list is system dependent.
*/

/*!
    \fn QVideoEncoderControl::encodingOption(const QString &codec, const QString &option) const

    Returns the value of a video \a codec \a option.
*/

/*!
    \fn QVideoEncoderControl::setEncodingOption(const QString &codec, const QString &option, const QVariant &value)

    Sets the \a value of a \a codec specific \a option.
*/

/*!
    \fn QVideoEncoderControl::supportedResolutions() const

    Returns a list of supported resolutions.  An empty list is returned if the video encoder
    supports arbitrary resolutions within the minimum and maximum range.

    \sa QVideoEncoderSettings::resolution(), minimumResolution(), maximumResolution()
*/

/*!
    \fn QVideoEncoderControl::supportedFrameRates() const

    Returns a list of supported frame rates. An empty list is returned if the encoder supports
    arbitrary frame rates within the minimum and maximum range.

    \sa QVideoEncoderSettings::frameRate(), minimumFrameRate(), maximumFrameRate()
*/

/*!
    \fn QVideoEncoderControl::videoSettings() const

    Returns the selected video encoder settings.
*/

/*!
    \fn QVideoEncoderControl::setVideoSettings(const QVideoEncoderSettings &settings)

    Sets the selected video encoder \a settings.
*/
