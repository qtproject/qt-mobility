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

#include <qimageencodercontrol.h>
#include <QtCore/qstringlist.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QImageEncoderControl
    \preliminary
    \brief The QImageEncoderControl class provides access to the settings of a media service that
    performs image encoding.

    If a QMediaService supports encoding image data it will implement QImageEncoderControl.  This
    control provides functions for setting the image \l {setImageCodec()}{codec}, and the
    \l {setResolution()}{resolution} and \l {setQuality()}{quality} of the encoded image.

    The interface name of QImageEncoderControl is \c com.nokia.Qt.QImageEncoderControl/1.0 as
    defined in QImageEncoderControl_iid.

    \sa QMediaService::control()
*/

/*!
    \macro QImageEncoderControl_iid

    \c com.nokia.Qt.QImageEncoderControl/1.0

    Defines the interface name of the QImageEncoderControl class.

    \relates QImageEncoderControl
*/

/*!
    Constructs a new image encoder control object with the given \a parent
*/
QImageEncoderControl::QImageEncoderControl(QObject *parent)
    :QMediaControl(parent)
{
}

/*!
    Destroys the image encoder control.
*/
QImageEncoderControl::~QImageEncoderControl()
{
}

/*!
    \fn QImageEncoderControl::supportedResolutions() const

    Returns a list of supported resolutions.  This will return an empty list if the encoder supports
    arbitrary resolutions within the minimum and maximum range.

    \sa resolution(), minimumResolution(), maximumResolution()

*/

/*!
    \fn QImageEncoderControl::resolution() const

    Returns the resolution of the encoded image.
*/

/*!
    \fn QImageEncoderControl::minimumResolution() const

    Returns the minimum supported resolution.

    \sa resolution()
*/

/*!
    \fn QImageEncoderControl::maximumResolution() const

    Returns the maximum supported resolution.

    \sa resolution()
*/

/*!
    \fn QImageEncoderControl::setResolution(const QSize &resolution)

    Sets the \a resolution of the encoded image.

    An empty QSize indicates the encoder should make an optimal choice based on what is available
    from the video source and the limitations of the codec.
*/

/*!
    \fn QImageEncoderControl::supportedImageCodecs() const

    Returns a list of supported image codecs.
*/

/*!
    \fn QImageEncoderControl::imageCodec() const

    Returns the selected image codec.
*/

/*!
    \fn QImageEncoderControl::setImageCodec(const QString &codec)

    Sets the selected image \a codec.

    Returns true if the codec was accepted; and false otherwise.
*/

/*!
    \fn QImageEncoderControl::imageCodecDescription(const QString &codec) const

    Returns a description of an image \a codec.
*/

/*!
    \fn QImageEncoderControl::quality() const

    Returns the image encoding quality.
*/

/*!
    \fn QImageEncoderControl::setQuality(QtMedia::EncodingQuality quality)

    Sets the image encoding \a quality.

    \sa quality()
*/

#include "moc_qimageencodercontrol.cpp"
QTM_END_NAMESPACE

