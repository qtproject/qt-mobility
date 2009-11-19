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

#ifndef QVIDEOWIDGET_P_H
#define QVIDEOWIDGET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qvideowidget.h>

#ifndef QT_NO_OPENGL
#include <QGLWidget>
#endif

#ifndef QT_NO_MULTIMEDIA
#include <qpaintervideosurface_p.h>
#endif

QTM_BEGIN_NAMESPACE

class QVideoWidgetBackendInterface
{
public:
    virtual ~QVideoWidgetBackendInterface() {}

    virtual void setBrightness(int brightness) = 0;
    virtual void setContrast(int contrast) = 0;
    virtual void setHue(int hue) = 0;
    virtual void setSaturation(int saturation) = 0;

    virtual void setFullScreen(bool fullScreen) = 0;

    virtual QVideoWidget::AspectRatioMode aspectRatioMode() const = 0;
    virtual void setAspectRatioMode(QVideoWidget::AspectRatioMode mode) = 0;

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

    void setFullScreen(bool fullScreen);

    QVideoWidget::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);
    QWidget *widget();

private:
    QVideoWidgetControl *m_widgetControl;
};

#ifndef QT_NO_MULTIMEDIA

class QVideoRendererControl;

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

    void clearSurface();

    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);

    void setFullScreen(bool fullScreen);

    QVideoWidget::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    QWidget *widget();

    QSize sizeHint() const;

Q_SIGNALS:
    void brightnessChanged(int brightness);
    void contrastChanged(int contrast);
    void hueChanged(int hue);
    void saturationChanged(int saturation);

protected:
#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
	void initializeGL();
#endif

    void paintEvent(QPaintEvent *event);

private Q_SLOTS:
    void dimensionsChanged();

private:
    QRect displayRect() const;

    QVideoRendererControl *m_rendererControl;
    QPainterVideoSurface *m_surface;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    QSize m_aspectRatio;
};
#endif

class QVideoWindowControl;

class QVideoWindowWidget : public QWidget, public QVideoWidgetBackendInterface
{
    Q_OBJECT
public:
    QVideoWindowWidget(QVideoWindowControl *control, QWidget *parent = 0);
    ~QVideoWindowWidget();

    void setBrightness(int brightness);
    void setContrast(int contrast);
    void setHue(int hue);
    void setSaturation(int saturation);

   void setFullScreen(bool fullScreen);

    QVideoWidget::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(QVideoWidget::AspectRatioMode mode);

    QWidget *widget();

    QSize sizeHint() const;

public Q_SLOTS:
    void setVisible(bool visible);


protected:
    void moveEvent(QMoveEvent *event);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QVideoWindowControl *m_windowControl;
    QVideoWidget::AspectRatioMode m_aspectRatioMode;
    QSize m_pixelAspectRatio;
};

class QMediaService;
class QStackedLayout;
class QVideoOutputControl;

class QVideoWidgetPrivate
{
    Q_DECLARE_PUBLIC(QVideoWidget)
public:
    QVideoWidgetPrivate()
        : q_ptr(0)
        , layout(0)
        , service(0)
        , outputControl(0)
        , widgetBackend(0)
        , windowBackend(0)
#ifndef QT_NO_MULTIMEDIA
        , rendererBackend(0)
#endif
        , currentBackend(0)
        , brightness(0)
        , contrast(0)
        , hue(0)
        , saturation(0)
        , aspectRatioMode(QVideoWidget::KeepAspectRatio)
        , nonFullScreenFlags(0)
        , wasFullScreen(false)
    {
    }

    QVideoWidget *q_ptr;
    QStackedLayout *layout;
    QMediaService *service;
    QVideoOutputControl *outputControl;
    QVideoWidgetControlBackend *widgetBackend;
    QVideoWindowWidget *windowBackend;
#ifndef QT_NO_MULTIMEDIA
    QVideoRendererWidget *rendererBackend;
#endif
    QVideoWidgetBackendInterface *currentBackend;
    int brightness;
    int contrast;
    int hue;
    int saturation;
    QVideoWidget::AspectRatioMode aspectRatioMode;
    Qt::WindowFlags nonFullScreenFlags;
    bool wasFullScreen;

    void setCurrentBackend(QVideoWidgetBackendInterface *backend);

    void _q_serviceDestroyed();
    void _q_brightnessChanged(int brightness);
    void _q_contrastChanged(int contrast);
    void _q_hueChanged(int hue);
    void _q_saturationChanged(int saturation);
    void _q_fullScreenChanged(bool fullScreen);
};

QTM_END_NAMESPACE

#endif
