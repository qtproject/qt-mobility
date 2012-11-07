/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef S60VIDEOWIDGETCONTROL_H
#define S60VIDEOWIDGETCONTROL_H

#include <qvideowidgetcontrol.h>

QT_USE_NAMESPACE

class S60VideoWidgetDisplay;

class S60VideoWidgetControl : public QVideoWidgetControl
{
    Q_OBJECT

    /**
     * WId of the topmost window in the application, used to calculate the
     * absolute ordinal position of the video widget.
     * This is used by the "window" implementation of QGraphicsVideoItem.
     */
    Q_PROPERTY(WId topWinId READ topWinId WRITE setTopWinId)

    /**
     * Ordinal position of the video widget, relative to the topmost window
     * in the application.  If both the topWinId property and the ordinalPosition
     * property are set, the absolute ordinal position of the video widget is
     * the sum of the topWinId ordinal position and the value of the
     * ordinalPosition property.
     * This is used by the "window" implementation of QGraphicsVideoItem.
     */
    Q_PROPERTY(int ordinalPosition READ ordinalPosition WRITE setOrdinalPosition)

    /**
     * Extent of the video, relative to this video widget.
     * This is used by the "window" implementation of QGraphicsVideoItem.
     */
    Q_PROPERTY(QRect extentRect READ extentRect WRITE setExtentRect)

    /**
     * Native size of video.
     * This is used by the "window" implementation of QGraphicsVideoItem.
     */
    Q_PROPERTY(QSize nativeSize READ nativeSize)

    /**
     * Rotation to be applied to video.
     * Angle is measured in degrees, with positive values counter-clockwise.
     * Zero is at 12 o'clock.
     */
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

    /**
     * Specifies whether video content is currently being displayed on the
     * widget.
     */
    Q_PROPERTY(bool hasContent READ hasContent NOTIFY hasContentChanged)

public:
    S60VideoWidgetControl(QObject *parent);
    ~S60VideoWidgetControl();

public:
    // QVideoWidgetControl
    QWidget *videoWidget();
    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode ratio);
    bool isFullScreen() const;
    void setFullScreen(bool fullScreen);
    int brightness() const;
    void setBrightness(int brightness);
    int contrast() const;
    void setContrast(int contrast);
    int hue() const;
    void setHue(int hue);
    int saturation() const;
    void setSaturation(int saturation);

    S60VideoWidgetDisplay *display() const;

    WId topWinId() const;
    void setTopWinId(WId id);
    int ordinalPosition() const;
    void setOrdinalPosition(int ordinalPosition);
    const QRect &extentRect() const;
    void setExtentRect(const QRect &rect);
    QSize nativeSize() const;
    qreal rotation() const;
    void setRotation(qreal value);
    bool hasContent() const;

signals:
    void nativeSizeChanged();
    void hasContentChanged();

private:
    S60VideoWidgetDisplay *m_display;
};

#endif // S60VIDEOWIDGETCONTROL_H

