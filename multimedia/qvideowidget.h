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

#ifndef QVIDEOWIDGET_H
#define QVIDEOWIDGET_H

#include <QtGui/qwidget.h>

#include "qmultimediaglobal.h"

class QAbstractMediaObject;

class QVideoWidgetPrivate;

class Q_MEDIA_EXPORT QVideoWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode NOTIFY displayModeChanged)
    Q_PROPERTY(AspectRatio aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(QSize customPixelAspectRatio READ customPixelAspectRatio WRITE setCustomPixelAspectRatio NOTIFY customPixelAspectRatioChanged)
    Q_PROPERTY(int brightness READ brightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(int contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(int hue READ hue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(int saturation READ saturation WRITE setSaturation NOTIFY saturationChanged)
    Q_ENUMS(DisplayMode)
    Q_ENUMS(AspectRatio)

public:
    enum DisplayMode
    {
        WindowedDisplay,
        FullscreenDisplay
    };

    enum AspectRatio { AspectRatioAuto, AspectRatioWidget, AspectRatioCustom };

    QVideoWidget(QAbstractMediaObject *object, QWidget *parent = 0);
    ~QVideoWidget();

    DisplayMode displayMode() const;

    AspectRatio aspectRatio() const;
    QSize customPixelAspectRatio() const;

    int brightness() const;
    int contrast() const;
    int hue() const;
    int saturation() const;

    bool eventFilter(QObject *object, QEvent *event);

public Q_SLOTS:
    void setVisible(bool visible);
    void setDisplayMode(DisplayMode mode);
    void setAspectRatio(AspectRatio ratio);
    void setCustomPixelAspectRatio(const QSize &customRatio);
    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);

Q_SIGNALS:
    void displayModeChanged(QVideoWidget::DisplayMode mode);
    void aspectRatioChanged(QVideoWidget::AspectRatio);
    void customPixelAspectRatioChanged(const QSize&);
    void brightnessChanged(int brightness);
    void contrastChanged(int contrast);
    void hueChanged(int hue);
    void saturationChanged(int saturation);

protected:;
    void keyPressEvent(QKeyEvent *event);

protected:
    QVideoWidgetPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QVideoWidget)
    Q_PRIVATE_SLOT(d_func(), void _q_serviceDestroyed())
    Q_PRIVATE_SLOT(d_func(), void _q_dimensionsChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_brightnessChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_contrastChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_hueChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_saturationChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_fullScreenChanged(bool))
    Q_PRIVATE_SLOT(d_func(), void _q_displayModeChanged(QVideoWidget::DisplayMode))
    Q_PRIVATE_SLOT(d_func(), void _q_aspectRatioModeChanged(QVideoWidget::AspectRatio))
    Q_PRIVATE_SLOT(d_func(), void _q_customAspectRatioChanged(const QSize &))
};

#endif
