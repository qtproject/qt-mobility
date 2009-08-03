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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvideooverlayendpoint.h"

#include <private/qobject_p.h>

class QVideoOverlayEndpointPrivate : public QObjectPrivate
{
public:
    QVideoOverlayEndpointPrivate()
        : winId(0)
        , enabled(false)
        , fullscreen(false)
    {
    }

    QRect displayRect;
    WId winId;
    bool enabled;
    bool fullscreen;
};

/*!
    \class QVideoOverlayEndpoint
    \preliminary
    \brief The QVideoOverlayEndpoint class provides a media end point that renders video over a
    window.
*/

/*!
    Constructs a new video overlay end point with the given \a parent.
*/
QVideoOverlayEndpoint::QVideoOverlayEndpoint(QObject *parent)
    : QObject(*new QVideoOverlayEndpointPrivate, parent)
{
}

/*!
    Destroys a video overlay end point.
*/
QVideoOverlayEndpoint::~QVideoOverlayEndpoint()
{
}

QMediaEndpointInterface::Direction QVideoOverlayEndpoint::direction() const
{
    return Output;
}

/*!
    Identifies if a video overlay end point is enabled.

    Returns true if the end point is enabled, and false otherwise.
*/
bool QVideoOverlayEndpoint::isEnabled() const
{
    return d_func()->enabled;
}

/*!
    Sets the \a enabled state of a video overlay end point.
*/
void QVideoOverlayEndpoint::setEnabled(bool enabled)
{
    d_func()->enabled = enabled;
}

/*!
    Returns the ID of the window a video overlay end point renders to.
*/
WId QVideoOverlayEndpoint::winId() const
{
    return d_func()->winId;
}

/*!
    Sets the \a id of the window a video overlay end point renders to.
*/
void QVideoOverlayEndpoint::setWinId(WId id)
{
    d_func()->winId = id;
}

/*!
    Returns the sub-rect of a window where video is displayed.
*/
QRect QVideoOverlayEndpoint::displayRect() const
{
    return d_func()->displayRect;
}

/*!
    Sets the sub-\a rect of a window where video is displayed.
*/
void QVideoOverlayEndpoint::setDisplayRect(const QRect &rect)
{
    d_func()->displayRect = rect;
}

/*!
    Identifies if a video overlay is a fullscreen overlay.

    Returns true if the video overlay is fullscreen, and false otherwise.
*/
bool QVideoOverlayEndpoint::isFullscreen() const
{
    return d_func()->fullscreen;
}

/*!
    Sets whether a video overlay is a \a fullscreen overlay.
*/
void QVideoOverlayEndpoint::setFullscreen(bool fullscreen)
{
    Q_D(QVideoOverlayEndpoint);

    if (fullscreen != d->fullscreen)
        emit fullscreenChanged(d->fullscreen = fullscreen);
}

/*!
    \fn QVideoOverlayEndpoint::fullscreenChanged(bool fullscreen)

    Signals that the \a fullscreen state of a video overlay has changed.
*/

/*!
    Repaints the last frame.
*/
void QVideoOverlayEndpoint::repaint()
{
}

/*!
    \fn QVideoOverlayEndpoint::nativeSize() const

    Returns a suggested size for the video display based on the resolution and aspect ratio of the
    video.
*/

/*!
    \fn QVideoOverlayEndpoint::dimensionsChanged()

    Signals that the native dimensions of the video have changed.
*/
