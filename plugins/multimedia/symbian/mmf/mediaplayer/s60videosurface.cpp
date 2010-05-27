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

#include <qvideosurfaceformat.h>

#include "s60videosurface.h"

S60VideoSurface::S60VideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent)
    , m_winId(0)
{
}

S60VideoSurface::~S60VideoSurface()
{

}

WId S60VideoSurface::winId() const
{
    return m_winId;
}

void S60VideoSurface::setWinId(WId id)
{
    Q_UNUSED(id);
}

QRect S60VideoSurface::displayRect() const
{
    return m_displayRect;
}

void S60VideoSurface::setDisplayRect(const QRect &rect)
{
    m_displayRect = rect;
}

int S60VideoSurface::brightness() const
{
    return 0;
}

void S60VideoSurface::setBrightness(int brightness)
{
    Q_UNUSED(brightness);
}

int S60VideoSurface::contrast() const
{
    return 0;
}

void S60VideoSurface::setContrast(int contrast)
{
    Q_UNUSED(contrast);
}

int S60VideoSurface::hue() const
{
    return 0;
}

void S60VideoSurface::setHue(int hue)
{
    Q_UNUSED(hue);
}

int S60VideoSurface::saturation() const
{
    return 0;
}

void S60VideoSurface::setSaturation(int saturation)
{
    Q_UNUSED(saturation);
}

int S60VideoSurface::getAttribute(const char *attribute, int minimum, int maximum) const
{
    Q_UNUSED(attribute);
    Q_UNUSED(minimum);
    Q_UNUSED(maximum);
    return 0;
}

void S60VideoSurface::setAttribute(const char *attribute, int value, int minimum, int maximum)
{
    Q_UNUSED(attribute);
    Q_UNUSED(value);
    Q_UNUSED(minimum);
    Q_UNUSED(maximum);

}

int S60VideoSurface::redistribute(
        int value, int fromLower, int fromUpper, int toLower, int toUpper)
{
    Q_UNUSED(value);
    Q_UNUSED(fromLower);
    Q_UNUSED(fromUpper);
    Q_UNUSED(toLower);
    Q_UNUSED(toUpper);
    return 0;
}

QList<QVideoFrame::PixelFormat> S60VideoSurface::supportedPixelFormats(
        QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType);
    QList<QVideoFrame::PixelFormat> list;
    return list;
}

bool S60VideoSurface::start(const QVideoSurfaceFormat &format)
{
    Q_UNUSED(format);
    return false;
}

void S60VideoSurface::stop()
{
}

bool S60VideoSurface::present(const QVideoFrame &frame)
{
    Q_UNUSED(frame);
    return false;
}

bool S60VideoSurface::findPort()
{
    return false;
}

void S60VideoSurface::querySupportedFormats()
{
}

bool S60VideoSurface::isFormatSupported(const QVideoSurfaceFormat &format) const
{
    Q_UNUSED(format);
    return false;
}
