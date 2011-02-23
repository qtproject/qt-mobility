/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qvideosurfaceformat.h"
#include "s60videooverlay.h"
#include "s60videosurface.h"
#include <coecntrl.h>
#include <w32std.h>
#include <QEvent>

S60VideoOverlay::S60VideoOverlay(QObject *parent)
    : QVideoWindowControl(parent)
    , m_winId(0)
    , m_aspectRatioMode(Qt::KeepAspectRatio)
    , m_fullScreen(false)
{
    setProperty("colorKey", Qt::transparent);
}

S60VideoOverlay::~S60VideoOverlay()
{
    m_winId = 0;
    emit windowHandleChanged();
}

WId S60VideoOverlay::winId() const
{
    return m_winId;
}

void S60VideoOverlay::setWinId(WId id)
{
    if (id != m_winId) {
        m_winId = id;
        emit windowHandleChanged();
    }
}

QRect S60VideoOverlay::displayRect() const
{
    return m_displayRect;
}

void S60VideoOverlay::setDisplayRect(const QRect &rect)
{
    if (rect != m_displayRect) {
        m_displayRect = rect;
        emit displayRectChanged();
    }
}

Qt::AspectRatioMode S60VideoOverlay::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void S60VideoOverlay::setAspectRatioMode(Qt::AspectRatioMode ratio)
{
    if (aspectRatioMode() != ratio) {
        m_aspectRatioMode = ratio;
        emit aspectRatioChanged();
    }
}

QSize S60VideoOverlay::customAspectRatio() const
{
    return m_aspectRatio;
}

void S60VideoOverlay::setCustomAspectRatio(const QSize &customRatio)
{
    if (customAspectRatio() != customRatio) {
        m_aspectRatio = customRatio;
        emit aspectRatioChanged();
    }
}

void S60VideoOverlay::repaint()
{

}

int S60VideoOverlay::brightness() const
{
    return 0;
}

void S60VideoOverlay::setBrightness(int brightness)
{
    Q_UNUSED(brightness)
}

int S60VideoOverlay::contrast() const
{
    return 0;
}

void S60VideoOverlay::setContrast(int contrast)
{
    Q_UNUSED(contrast)
}

int S60VideoOverlay::hue() const
{
    return 0;
}

void S60VideoOverlay::setHue(int hue)
{
    Q_UNUSED(hue)
}

int S60VideoOverlay::saturation() const
{
    return 0;
}

void S60VideoOverlay::setSaturation(int saturation)
{
    Q_UNUSED(saturation)
}

bool S60VideoOverlay::isFullScreen() const
{
    return m_fullScreen;
}

void S60VideoOverlay::setFullScreen(bool fullScreen)
{
    emit fullScreenChanged(m_fullScreen = fullScreen);
}

QSize S60VideoOverlay::nativeSize() const
{
    return QSize();
}

WId S60VideoOverlay::videoWinId() const
{
    return winId();
}

QRect S60VideoOverlay::videoDisplayRect() const
{
    QRect rect = displayRect();
#ifndef MMF_VIDEO_SURFACES_SUPPORTED
    if (m_winId) {
        // Translate rect into screen coordinates
        RWindow *window = static_cast<RWindow *>(m_winId->DrawableWindow());
        const TPoint windowPos = window->AbsPosition();
        rect.translate(windowPos.iX, windowPos.iY);
    }
#endif
    return rect;
}

Qt::AspectRatioMode S60VideoOverlay::videoAspectRatio() const
{
    return aspectRatioMode();
}

void S60VideoOverlay::videoStateChanged(QMediaPlayer::State state)
{
    Q_UNUSED(state)
}
