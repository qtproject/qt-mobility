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

#ifndef QVIDEOWIDGET_P_H
#define QVIDEOWIDGET_P_H

#include "qvideowidget.h"

#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

#ifndef QT_NO_MULTIMEDIA
#include "qpaintervideosurface_p.h"
#endif

class QVideoWidgetBackendInterface
{
public:
    virtual ~QVideoWidgetBackendInterface() {}

    virtual void setBrightness(int brightness) = 0;
    virtual void setContrast(int contrast) = 0;
    virtual void setHue(int hue) = 0;
    virtual void setSaturation(int saturation) = 0;

    virtual void setDisplayMode(QVideoWidget::DisplayMode mode) = 0;

    virtual void setAspectRatio(QVideoWidget::AspectRatio ratio) = 0;
    virtual void setCustomPixelAspectRatio(const QSize &customRatio) = 0;

    virtual QWidget *widget() = 0;
};

class QVideoWidgetControl;

class QVideoWidgetControlBackend : public QObject, public QVideoWidgetBackendInterface
{
    Q_OBJECT
public:
    QVideoWidgetControlBackend(QVideoWidgetControl *control);

    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);

    void setDisplayMode(QVideoWidget::DisplayMode mode);

    void setAspectRatio(QVideoWidget::AspectRatio ratio);
    void setCustomPixelAspectRatio(const QSize &customRatio);

    QWidget *widget();

Q_SIGNALS:
    void aspectRatioModeChanged(QVideoWidget::AspectRatio mode);
    void customAspectRatioChanged(const QSize &ratio);

private:
    QVideoWidgetControl *m_widgetControl;
};

class QFullScreenVideoWidget : public QWidget, public QVideoWidgetBackendInterface
{
    Q_OBJECT
public:
    QFullScreenVideoWidget(QWidget *parent = 0);
    ~QFullScreenVideoWidget();

    QVideoWidget::DisplayMode displayMode() const;
    void setDisplayMode(QVideoWidget::DisplayMode mode);

    QWidget *widget();

Q_SIGNALS:
    void displayModeChanged(QVideoWidget::DisplayMode mode);

private:
    QVideoWidget::DisplayMode m_displayMode;
};

#ifndef QT_NO_MULTIMEDIA

class QVideoRendererControl;

#ifndef QT_NO_OPENGL
class QGLWidgetVideoSurface : public QPainterVideoSurface
{
    Q_OBJECT
public:
    explicit QGLWidgetVideoSurface(QGLWidget *widget, QObject *parent = 0);

protected:
    void makeCurrent();
    void doneCurrent();

private:
    QGLWidget *m_widget;
};

#endif

class QVideoRendererWidget
#ifndef QT_NO_OPENGL
    : public QGLWidget
#else
    : public QWidget
#endif
    , public QVideoWidgetBackendInterface
{
    Q_OBJECT
public:
    QVideoRendererWidget(QVideoRendererControl *control, QWidget *parent = 0);
    ~QVideoRendererWidget();

    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);

    QVideoWidget::DisplayMode displayMode() const;
    void setDisplayMode(QVideoWidget::DisplayMode mode);

    void setAspectRatio(QVideoWidget::AspectRatio ratio);
    virtual void setCustomPixelAspectRatio(const QSize &customRatio);

    QWidget *widget();

    QSize sizeHint() const;

Q_SIGNALS:
    void brightnessChanged(int brightness);
    void contrastChanged(int contrast);
    void hueChanged(int hue);
    void saturationChanged(int saturation);
    void displayModeChanged(QVideoWidget::DisplayMode mode);
    void aspectRatioModeChanged(QVideoWidget::AspectRatio mode);
    void customAspectRatioChanged(const QSize &ratio);

protected:
    void paintEvent(QPaintEvent *event);

private Q_SLOTS:
    void dimensionsChanged();

private:
    QRect displayRect() const;

    QVideoRendererControl *m_rendererControl;
    QPainterVideoSurface *m_surface;
    QVideoWidget::DisplayMode m_displayMode;
    QVideoWidget::AspectRatio m_aspectRatioMode;
    QSize m_aspectRatio;
};
#endif

class QVideoWindowControl;

class QVideoWindowWidget : public QFullScreenVideoWidget
{
    Q_OBJECT
public:
    QVideoWindowWidget(QVideoWindowControl *control, QWidget *parent = 0);
    ~QVideoWindowWidget();

    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);

    void setDisplayMode(QVideoWidget::DisplayMode mode);

    void setAspectRatio(QVideoWidget::AspectRatio ratio);
    virtual void setCustomPixelAspectRatio(const QSize &customRatio);

    QSize sizeHint() const;

public Q_SLOTS:
    void setVisible(bool visible);

Q_SIGNALS:
    void aspectRatioModeChanged(QVideoWidget::AspectRatio mode);
    void customAspectRatioChanged(const QSize &ratio);

protected:
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QVideoWindowControl *m_windowControl;
    QVideoWidget::AspectRatio m_aspectRatioMode;
    QSize m_pixelAspectRatio;
};

#endif
