/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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

#include "qmediawidgetendpoint_p.h"

/*!
    \class QMediaWidgetEndpoint
    \preliminary
    \brief The QMediaWidgetEndpoint class provides a QWidget media output.

    The interface name of QMediaWidgetEndpoint is \c com.nokia.Qt.QMediaWidgetEndpoint/1.0 as
    defined in QMediaWidgetEndpoint_iid.

    \sa QAbstractMediaService::createEndpoint(), QAbstractMediaService::setVideoOutput()
*/

/*!
    \macro QMediaWidgetEndpoint_iid

    \c com.nokia.Qt.QMediaWidgetEndpoint/1.0

    Defines the interface name of QIODeviceEndpoint.

    \relates QMediaWidgetEndpoint
*/

/*!
    Constructs a new media output widget with the given \a parent.
*/
QMediaWidgetEndpoint::QMediaWidgetEndpoint(QWidget *parent)
    : QWidget(parent, 0)
    , d_ptr(new QMediaWidgetEndpointPrivate)
{
}

/*!
    \internal
*/
QMediaWidgetEndpoint::QMediaWidgetEndpoint(QMediaWidgetEndpointPrivate &dd, QWidget *parent)
    : QWidget(parent, 0)
    , d_ptr(&dd)
{
}

/*!
    Destroys a media output widget.
*/
QMediaWidgetEndpoint::~QMediaWidgetEndpoint()
{
    delete d_ptr;
}

/*!
    \reimp
*/
QMediaEndpointInterface::Direction QMediaWidgetEndpoint::direction() const
{
    return Output;
}

/*!
    \property QMediaWidgetEndpoint::fullscreen
    \brief the full screen state of a media output widget.
*/

bool QMediaWidgetEndpoint::isFullscreen() const
{
    return d_func()->fullscreen;
}

void QMediaWidgetEndpoint::setFullscreen(bool fullscreen)
{
    d_func()->fullscreen = fullscreen;
}

/*!
  \enum QMediaWidgetEndpoint::AspectRatio

  Defines the width:height ratio to be used for the video.

  \value AspectRatioAuto
            Let the decoder find the aspect ratio automatically from
            the media file (this is the default).

  \value AspectRatioWidget
            Fits the video into the widget making the aspect ratio depend solely on the size of the widget.
            This way the aspect ratio is freely resizeable by the user.

  \value AspectRatioCustom
            The aspect ratio is defined by QMediaWidgetEndpoint::customAspectRatio()
*/


/*!
    \property QMediaWidgetEndpoint::aspectRatio

    The aspect ratio mode of a media output widget.

    \sa QMediaWidgetEndpoint::AspectRatio, QMediaWidgetEndpoint::customAspectRatio
*/

QMediaWidgetEndpoint::AspectRatio QMediaWidgetEndpoint::aspectRatio() const
{
    return d_func()->aspectRatio;
}

void QMediaWidgetEndpoint::setAspectRatio(QMediaWidgetEndpoint::AspectRatio ratio)
{
    d_func()->aspectRatio = ratio;
}

/*!
    \property QMediaWidgetEndpoint::customAspectRatio

    The aspect ratio value used when QMediaWidgetEndpoint::aspectRatio == AspectRatioCustom.

    \sa QMediaWidgetEndpoint::aspectRatio
*/

QSize QMediaWidgetEndpoint::customAspectRatio() const
{
    return d_func()->customAspectRatio;
}

void QMediaWidgetEndpoint::setCustomAspectRatio(const QSize &customRatio)
{
    d_func()->customAspectRatio = customRatio;
}
