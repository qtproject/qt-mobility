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

#include "qvideowidgetcontrol.h"
#include "qabstractmediacontrol_p.h"

/*!
    \class QVideoWidgetControl
    \preliminary
    \brief The QVideoWidgetControl class provides a media control for rendering video to QWidget
           provided by media service.
*/

/*!
    Constructs a new video widget control with the given \a parent.
*/
QVideoWidgetControl::QVideoWidgetControl(QObject *parent)
    :QAbstractMediaControl(parent)
{
}

/*!
    Destroys a video widget control.
*/
QVideoWidgetControl::~QVideoWidgetControl()
{
}

/*!
  \fn bool QVideoWidgetControl::isFullscreen() const
  Returns true if the video  is shown using the complete screen.
*/

/*!
  \fn void QVideoWidgetControl::setFullscreen(bool fullscreen)
    Sets whether a video widget is in \a fullscreen mode.
*/

/*!
    \fn QVideoWidgetControl::fullscreenChanged(bool fullscreen)

    Signals that the \a fullscreen state of a video widget has changed.
*/


/*!
  \fn int QVideoWidgetControl::brightness() const
    Returns the brightness adjustment applied to a video.

    Valid brightness values range between -100 and 100, the default is 0.
*/

/*!
  \fn void QVideoWidgetControl::setBrightness(int brightness)
    Sets a \a brightness adjustment for a video.

    Valid brightness values range between -100 and 100, the default is 0.
*/

/*!
    \fn QVideoWidgetControl::brightnessChanged(int brightness)

    Signals that a video widget's \a brightness adjustment has changed.
*/

/*!
  \fn int QVideoWidgetControl::contrast() const
    Returns the contrast adjustment applied to a video.

    Valid contrast values range between -100 and 100, the default is 0.
*/

/*!
  \fn void QVideoWidgetControl::setContrast(int contrast)
    Sets the contrast adjustment for a video widget.

    Valid contrast values range between -100 and 100, the default is 0.
*/


/*!
    \fn QVideoWidgetControl::contrastChanged(int contrast)

    Signals that a video widget's \a contrast adjustment has changed.
*/

/*!
  \fn int QVideoWidgetControl::hue() const
    Returns the hue adjustment applied to a video widget.

    Value hue values range between -100 and 100, the default is 0.
*/

/*!
  \fn void QVideoWidgetControl::setHue(int hue)
    Sets a \a hue adjustment for a video widget.

    Valid hue values range between -100 and 100, the default is 0.
*/


/*!
    \fn QVideoWidgetControl::hueChanged(int hue)

    Signals that a video widget's \a hue adjustment has changed.
*/

/*!
  \fn int QVideoWidgetControl::saturation() const
    Returns the saturation adjustment applied to a video widget.

    Value saturation values range between -100 and 100, the default is 0.
*/


/*!
  \fn void QVideoWidgetControl::setSaturation(int saturation)
    Sets a \a saturation adjustment for a video widget.

    Valid saturation values range between -100 and 100, the default is 0.
*/

/*!
    \fn QVideoWidgetControl::saturationChanged(int saturation)

    Signals that a video widget's \a saturation adjustment has changed.
*/
