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

#include <qimageencodercontrol.h>
#include <QtCore/qstringlist.h>

/*!
  Create a new image encode control object with the given \a parent
*/
QImageEncoderControl::QImageEncoderControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}

/*!
  Destroys the image encode control.
*/
QImageEncoderControl::~QImageEncoderControl()
{
}

/*!
  Returns the list of resolutions if the image encoder supports only the limited set
  of image sizes, otherwise returns an empty list.

  \sa resolution(), minimumResolution(), maximumResolution()

*/
QList<QSize> QImageEncoderControl::supportedResolutions() const
{
    return QList<QSize>();
}


/*!
  \fn QImageEncoderControl::resolution() const

   Returns the resolution of encoded image.
*/

/*!
  \fn QImageEncoderControl::minimumResolution() const

  Returns the smallest supported resolution.

  \sa resolution
*/

/*!
  \fn QImageEncoderControl::maximumResolution() const

  Returns the largest supported resolution.

  \sa resolution
*/

/*!
  \fn QImageEncoderControl::setResolution(const QSize &)

  Set the image \a resolution.
  If an empty size is passed,
  the media service should choose the default or content
  related resolution value.
*/

/*!
  \fn QImageEncoderControl::supportedImageCodecs() const

  Returns the list of supported image codec names.
*/

/*!
  \fn QImageEncoderControl::imageCodec() const

  Returns the currently used image codec name.
*/

/*!
  \fn QImageEncoderControl::setImageCodec(const QString &codecName)

  Use the image codec with name equals to \a codecName.

*/

/*!
  \fn QImageEncoderControl::imageCodecDescription(const QString &codecName) const

  Returns description of image \a codec.
*/

/*!
  \fn QImageEncoderControl::quality() const

  Image quality value, in 0..100 range.
*/

/*!
  \fn QImageEncoderControl::setQuality(qreal)

  Sets the image quality to \a value.
  Value has the same meaning as QAudioEncoderControl::quality.

  \sa quality()
*/
