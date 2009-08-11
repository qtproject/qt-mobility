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

#include "qabstractmediacontrol_p.h"

class QVideoWindowControlPrivate : public QAbstractMediaControlPrivate
{
public:
    QVideoWindowControlPrivate()
        : winId(0)
        , brightness(0)
        , contrast(0)
        , hue(0)
        , saturation(0)
        , fullscreen(false)
    {
    }

    QRect displayRect;
    WId winId;
    int brightness;
    int contrast;
    int hue;
    int saturation;
    bool fullscreen;
};

/*!
    \class QVideoWindowControl
    \preliminary
    \brief The QVideoWindowControl class provides a media control for rendering video to a window.
*/

/*!
    Constructs a new video overlay end point with the given \a parent.
*/
QVideoWindowControl::QVideoWindowControl(QObject *parent)
    : QAbstractMediaControl(*new QVideoWindowControlPrivate, parent)
{
}

/*!
    Destroys a video overlay end point.
*/
QVideoWindowControl::~QVideoWindowControl()
{
}

/*!
    Returns the ID of the window a video overlay end point renders to.
*/
WId QVideoWindowControl::winId() const
{
    return d_func()->winId;
}

/*!
    Sets the \a id of the window a video overlay end point renders to.
*/
void QVideoWindowControl::setWinId(WId id)
{
    d_func()->winId = id;
}

/*!
    Returns the sub-rect of a window where video is displayed.
*/
QRect QVideoWindowControl::displayRect() const
{
    return d_func()->displayRect;
}

/*!
    Sets the sub-\a rect of a window where video is displayed.
*/
void QVideoWindowControl::setDisplayRect(const QRect &rect)
{
    d_func()->displayRect = rect;
}

/*!
    Identifies if a video overlay is a fullscreen overlay.

    Returns true if the video overlay is fullscreen, and false otherwise.
*/
bool QVideoWindowControl::isFullscreen() const
{
    return d_func()->fullscreen;
}

/*!
    Sets whether a video overlay is a \a fullscreen overlay.
*/
void QVideoWindowControl::setFullscreen(bool fullscreen)
{
    Q_D(QVideoWindowControl);

    if (fullscreen != d->fullscreen)
        emit fullscreenChanged(d->fullscreen = fullscreen);
}

/*!
    \fn QVideoWindowControl::fullscreenChanged(bool fullscreen)

    Signals that the \a fullscreen state of a video overlay has changed.
*/

/*!
    Repaints the last frame.
*/
void QVideoWindowControl::repaint()
{
}

/*!
    \fn QVideoWindowControl::nativeSize() const

    Returns a suggested size for the video display based on the resolution and aspect ratio of the
    video.
*/

/*!
    \fn QVideoWindowControl::nativeSizeChanged()

    Signals that the native dimensions of the video have changed.
*/

/*!
    Returns the brightness adjustment applied to a video overlay.

    Valid brightness values range between -100 and 100, the default is 0.
*/
int QVideoWindowControl::brightness() const
{
    return d_func()->brightness;
}

/*!
    Sets a \a brightness adjustment for a video overlay.

    Valid brightness values range between -100 and 100, the default is 0.
*/
void QVideoWindowControl::setBrightness(int brightness)
{
    emit brightnessChanged(d_func()->brightness = brightness);
}

/*!
    \fn QVideoWindowControl::brightnessChanged(int brightness)

    Signals that a video overlay's \a brightness adjustment has changed.
*/

/*!
    Returns the contrast adjustment applied to a video overlay.

    Valid contrast values range between -100 and 100, the default is 0.
*/
int QVideoWindowControl::contrast() const
{
    return d_func()->contrast;
}

/*!
    Sets the contrast adjustment for a video overlay.

    Valid contrast values range between -100 and 100, the default is 0.
*/
void QVideoWindowControl::setContrast(int contrast)
{
    emit contrastChanged(d_func()->contrast = contrast);
}

/*!
    \fn QVideoWindowControl::contrastChanged(int contrast)

    Signals that a video overlay's \a contrast adjustment has changed.
*/

/*!
    Returns the hue adjustment applied to a video overlay.

    Value hue values range between -100 and 100, the default is 0.
*/
int QVideoWindowControl::hue() const
{
    return d_func()->hue;
}

/*!
    Sets a \a hue adjustment for a video overlay.

    Valid hue values range between -100 and 100, the default is 0.
*/
void QVideoWindowControl::setHue(int hue)
{
    emit hueChanged(d_func()->hue = hue);
}

/*!
    \fn QVideoWindowControl::hueChanged(int hue)

    Signals that a video overlay's \a hue adjustment has changed.
*/

/*!
    Returns the saturation adjustment applied to a video overlay.

    Value saturation values range between -100 and 100, the default is 0.
*/
int QVideoWindowControl::saturation() const
{
    return d_func()->saturation;
}

/*!
    Sets a \a saturation adjustment for a video overlay.

    Valid saturation values range between -100 and 100, the default is 0.
*/
void QVideoWindowControl::setSaturation(int saturation)
{
    emit saturationChanged(d_func()->saturation = saturation);
}

/*!
    \fn QVideoWindowControl::saturationChanged(int saturation)

    Signals that a video overlay's \a saturation adjustment has changed.
*/
