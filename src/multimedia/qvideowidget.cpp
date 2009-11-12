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

#include <qvideowidget_p.h>

#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qvideooutputcontrol.h>
#include <qvideowindowcontrol.h>
#include <qvideowidgetcontrol.h>

#ifndef QT_NO_MULTIMEDIA
#include <qpaintervideosurface_p.h>
#include <qvideorenderercontrol.h>
#include "qvideosurfaceformat.h"
#include <qpainter.h>
#endif

#include <qapplication.h>
#include <qevent.h>
#include <qdialog.h>
#include <qstackedlayout.h>

QVideoWidgetControlBackend::QVideoWidgetControlBackend(QVideoWidgetControl *control)
    : m_widgetControl(control)
{
}

void QVideoWidgetControlBackend::setBrightness(int brightness)
{
    m_widgetControl->setBrightness(brightness);
}

void QVideoWidgetControlBackend::setContrast(int contrast)
{
    m_widgetControl->setContrast(contrast);
}

void QVideoWidgetControlBackend::setHue(int hue)
{
    m_widgetControl->setHue(hue);
}

void QVideoWidgetControlBackend::setSaturation(int saturation)
{
    m_widgetControl->setSaturation(saturation);
}

void QVideoWidgetControlBackend::setFullScreen(bool fullScreen)
{
    m_widgetControl->setFullScreen(fullScreen);
}


QVideoWidget::AspectRatioMode QVideoWidgetControlBackend::aspectRatioMode() const
{
    return m_widgetControl->aspectRatioMode();
}

void QVideoWidgetControlBackend::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    m_widgetControl->setAspectRatioMode(mode);
}

QWidget *QVideoWidgetControlBackend::widget()
{
    return m_widgetControl->videoWidget();
}

#ifndef QT_NO_MULTIMEDIA

QVideoRendererWidget::QVideoRendererWidget(QVideoRendererControl *control, QWidget *parent)
#ifndef QT_NO_OPENGL
    : QGLWidget(parent)
#else
    : QWidget(parent)
#endif
    , m_rendererControl(control)
    , m_surface(new QPainterVideoSurface)
{
    connect(m_surface, SIGNAL(frameChanged()), SLOT(update()));
    connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)), SLOT(dimensionsChanged()));

    QPalette palette;
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    m_rendererControl->setSurface(m_surface);
}

QVideoRendererWidget::~QVideoRendererWidget()
{
    delete m_surface;
}

void QVideoRendererWidget::clearSurface()
{
    m_rendererControl->setSurface(0);
}

void QVideoRendererWidget::setBrightness(int brightness)
{
    m_surface->setBrightness(brightness);

    emit brightnessChanged(brightness);
}

void QVideoRendererWidget::setContrast(int contrast)
{
    m_surface->setContrast(contrast);

    emit contrastChanged(contrast);
}

void QVideoRendererWidget::setHue(int hue)
{
    m_surface->setHue(hue);

    emit hueChanged(hue);
}

void QVideoRendererWidget::setSaturation(int saturation)
{
    m_surface->setSaturation(saturation);

    emit saturationChanged(saturation);
}

QVideoWidget::AspectRatioMode QVideoRendererWidget::aspectRatioMode() const
{
    return m_aspectRatioMode;
}

void QVideoRendererWidget::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    m_aspectRatioMode = mode;

    updateGeometry();
}

QSize QVideoRendererWidget::sizeHint() const
{
    return m_surface->surfaceFormat().sizeHint();
}

#if !defined(QT_NO_OPENGL) && !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
void QVideoRendererWidget::initializeGL()
{
    makeCurrent();

    m_surface->setGLContext(const_cast<QGLContext *>(context()));
    if (m_surface->supportedShaderTypes() & QPainterVideoSurface::GlslShader)
        m_surface->setShaderType(QPainterVideoSurface::GlslShader);
    else
        m_surface->setShaderType(QPainterVideoSurface::FragmentProgramShader);
}
#endif

void QVideoRendererWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (m_surface->isActive()) {
        m_surface->paint(&painter, displayRect());

        m_surface->setReady(true);
    } else {
        painter.fillRect(event->rect(), palette().background());
    }
}

void QVideoRendererWidget::dimensionsChanged()
{
    updateGeometry();
}

QRect QVideoRendererWidget::displayRect() const
{
    QRect displayRect = rect();

    if (m_aspectRatioMode != QVideoWidget::IgnoreAspectRatio) {
        QVideoSurfaceFormat format = m_surface->surfaceFormat();

        QSize aspectRatio = format.pixelAspectRatio();

        QSize size = format.viewport().size();
        size.rwidth() *= aspectRatio.width();
        size.rheight() *= aspectRatio.height();
        size.scale(displayRect.size(), Qt::KeepAspectRatio);

        QPoint center = displayRect.center();

        displayRect = QRect(QPoint(0, 0), size);
        displayRect.moveCenter(center);
    }

    return displayRect;
}

void QVideoRendererWidget::setFullScreen(bool fullScreen)
{
     Q_UNUSED(fullScreen);
}

QWidget *QVideoRendererWidget::widget()
{
    return this;
}

#endif

QVideoWindowWidget::QVideoWindowWidget(QVideoWindowControl *control, QWidget *parent)
    : QWidget(parent)
    , m_windowControl(control)
{
}

QVideoWindowWidget::~QVideoWindowWidget()
{
}

void QVideoWindowWidget::setBrightness(int brightness)
{
    m_windowControl->setBrightness(brightness);
}

void QVideoWindowWidget::setContrast(int contrast)
{
    m_windowControl->setContrast(contrast);
}

void QVideoWindowWidget::setHue(int hue)
{
    m_windowControl->setHue(hue);
}

void QVideoWindowWidget::setSaturation(int saturation)
{
    m_windowControl->setSaturation(saturation);
}

void QVideoWindowWidget::setFullScreen(bool fullScreen)
{
    m_windowControl->setFullScreen(fullScreen);
}

QVideoWidget::AspectRatioMode QVideoWindowWidget::aspectRatioMode() const
{
    return m_windowControl->aspectRatioMode();
}

void QVideoWindowWidget::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    m_windowControl->setAspectRatioMode(mode);
}

QWidget *QVideoWindowWidget::widget()
{
    return this;
}

QSize QVideoWindowWidget::sizeHint() const
{
    return m_windowControl->nativeSize();
}

void QVideoWindowWidget::setVisible(bool visible)
{
    if (visible)
        m_windowControl->setWinId(winId());

    QWidget::setVisible(visible);
}

void QVideoWindowWidget::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);

    QRect displayRect = rect();
    displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));

    m_windowControl->setDisplayRect(displayRect);
}

void QVideoWindowWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QRect displayRect = rect();
    displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));

    m_windowControl->setDisplayRect(displayRect);
}

void QVideoWindowWidget::paintEvent(QPaintEvent *event)
{
    m_windowControl->repaint();

    event->accept();
}

void QVideoWidgetPrivate::setCurrentBackend(QVideoWidgetBackendInterface *backend)
{
    if (currentBackend != backend) {
        currentBackend = backend;

        currentBackend->setBrightness(brightness);
        currentBackend->setContrast(contrast);
        currentBackend->setHue(hue);
        currentBackend->setSaturation(saturation);
        currentBackend->setAspectRatioMode(aspectRatioMode);
    }
    layout->setCurrentWidget(currentBackend->widget());
}

void QVideoWidgetPrivate::_q_serviceDestroyed()
{
    delete widgetBackend;
    widgetBackend = 0;

    delete windowBackend;
    windowBackend = 0;

#ifndef QT_NO_MULTIMEDIA
    delete rendererBackend;
    rendererBackend = 0;
#endif

    currentBackend = 0;
    outputControl = 0;
    service = 0;
}

void QVideoWidgetPrivate::_q_brightnessChanged(int b)
{
    if (b != brightness)
        emit q_func()->brightnessChanged(brightness = b);
}

void QVideoWidgetPrivate::_q_contrastChanged(int c)
{
    if (c != contrast)
        emit q_func()->contrastChanged(contrast = c);
}

void QVideoWidgetPrivate::_q_hueChanged(int h)
{
    if (h != hue)
        emit q_func()->hueChanged(hue = h);
}

void QVideoWidgetPrivate::_q_saturationChanged(int s)
{
    if (s != saturation)
        emit q_func()->saturationChanged(saturation = s);
}


void QVideoWidgetPrivate::_q_fullScreenChanged(bool fullScreen)
{
    if (!fullScreen && q_func()->isFullScreen())
        q_func()->showNormal();
}


/*!
    \class QVideoWidget
    \preliminary

    \brief The QVideoWidget class provides a widget which presents video produced by a media
    object.
    \ingroup multimedia

    Attaching a QVideoWidget to a QMediaObject allows it to display the video or image output
    of that media object.  A QVideoWidget is attached to media object by passing a pointer to
    the QMediaObject in its constructor, and detached by destroying the QVideoWidget.

    \code
        player = new QMediaPlayer;

        widget = new QVideoWidget(player);
        widget->show();

        player->setMedia(QUrl("http://example.com/movie.mp4"));
        player->play();
    \endcode

    \bold {Note}: Only a single display output can be attached to a media object at one time.

    \sa QMediaObject, QMediaPlayer, QGraphicsVideoItem
*/

/*!
    \enum QVideoWidget::AspectRatioMode

    Specfies how video is scaled with respect to its aspect ratio.

    \value IgnoreAspectRatio The video is scaled to fill the widget ignoring its aspect ratio.
    \value KeepAspectRatio The video is scaled to the largest rectangle that will fit within the
    widget's dimensions while still retaining its original aspect ratio.
*/

/*!
    Constructs a new widget which displays video produced by a media \a object.

    The \a parent is passed to QWidget.
*/
QVideoWidget::QVideoWidget(QMediaObject *object, QWidget *parent)
    : QWidget(parent, 0)
    , d_ptr(new QVideoWidgetPrivate)
{
    Q_D(QVideoWidget);

    d->q_ptr = this;

    QPalette palette = QWidget::palette();
    palette.setColor(QPalette::Window, Qt::black);
    setPalette(palette);

    if (object)
        d->service = object->service();

    if (!d->service)
        return;

    connect(d->service, SIGNAL(destroyed()), SLOT(_q_serviceDestroyed()));

    d->outputControl = qobject_cast<QVideoOutputControl *>(
            d->service->control(QVideoOutputControl_iid));

    d->layout = new QStackedLayout;

    QVideoWidgetControl *widgetControl = qobject_cast<QVideoWidgetControl *>(
            d->service->control(QVideoWidgetControl_iid));

    if (widgetControl != 0) {
        d->widgetBackend = new QVideoWidgetControlBackend(widgetControl);

        QWidget *widget = d->widgetBackend->widget();

        d->layout->addWidget(widget);

        connect(widgetControl, SIGNAL(brightnessChanged(int)), SLOT(_q_brightnessChanged(int)));
        connect(widgetControl, SIGNAL(contrastChanged(int)), SLOT(_q_contrastChanged(int)));
        connect(widgetControl, SIGNAL(hueChanged(int)), SLOT(_q_hueChanged(int)));
        connect(widgetControl, SIGNAL(saturationChanged(int)), SLOT(_q_saturationChanged(int)));
        connect(widgetControl, SIGNAL(fullScreenChanged(bool)), SLOT(_q_fullScreenChanged(bool)));
    } else {
        QVideoWindowControl *windowControl = qobject_cast<QVideoWindowControl *>(
                d->service->control(QVideoWindowControl_iid));

        if (windowControl != 0) {
            d->windowBackend = new QVideoWindowWidget(windowControl);

            d->layout->addWidget(d->windowBackend);

            connect(windowControl, SIGNAL(brightnessChanged(int)), SLOT(_q_brightnessChanged(int)));
            connect(windowControl, SIGNAL(contrastChanged(int)), SLOT(_q_contrastChanged(int)));
            connect(windowControl, SIGNAL(hueChanged(int)), SLOT(_q_hueChanged(int)));
            connect(windowControl, SIGNAL(saturationChanged(int)), SLOT(_q_saturationChanged(int)));
            connect(windowControl, SIGNAL(fullScreenChanged(bool)),
                    SLOT(_q_fullScreenChanged(bool)));
        }
#ifndef QT_NO_MULTIMEDIA
        QVideoRendererControl *rendererControl = qobject_cast<QVideoRendererControl *>(
                d->service->control(QVideoRendererControl_iid));

        if (rendererControl != 0) {
            d->rendererBackend = new QVideoRendererWidget(rendererControl);

            d->layout->addWidget(d->rendererBackend);

            connect(d->rendererBackend, SIGNAL(brightnessChanged(int)),
                    SLOT(_q_brightnessChanged(int)));
            connect(d->rendererBackend, SIGNAL(contrastChanged(int)),
                    SLOT(_q_contrastChanged(int)));
            connect(d->rendererBackend, SIGNAL(hueChanged(int)), SLOT(_q_hueChanged(int)));
            connect(d->rendererBackend, SIGNAL(saturationChanged(int)),
                    SLOT(_q_saturationChanged(int)));
        }
#endif
    }

    d->nonFullScreenFlags = windowFlags() & (Qt::SubWindow | Qt::Window);

    setLayout(d->layout);
}

/*!
    Destroys a video widget.
*/
QVideoWidget::~QVideoWidget()
{
    if (d_ptr->service) {
        if (d_ptr->outputControl)
            d_ptr->outputControl->setOutput(QVideoOutputControl::NoOutput);

        if (d_ptr->widgetBackend) {
            QWidget *widget = d_ptr->widgetBackend->widget();

            d_ptr->layout->removeWidget(widget);
            widget->setParent(0);

            delete d_ptr->widgetBackend;
        }
        delete d_ptr->windowBackend;
#ifndef QT_NO_MULTIMEDIA
        if (d_ptr->rendererBackend) {
            d_ptr->rendererBackend->clearSurface();

            delete d_ptr->rendererBackend;
        }
#endif
    }
    delete d_ptr;
}

/*!
    \property QVideoWidget::aspectRatioMode
    \brief how video is scaled with respect to its aspect ratio.
*/

QVideoWidget::AspectRatioMode QVideoWidget::aspectRatioMode() const
{
    return d_func()->aspectRatioMode;
}

void QVideoWidget::setAspectRatioMode(QVideoWidget::AspectRatioMode mode)
{
    Q_D(QVideoWidget);

    if (d->currentBackend) {
        d->currentBackend->setAspectRatioMode(mode);
        d->aspectRatioMode = d->currentBackend->aspectRatioMode();
    } else {
        d->aspectRatioMode = mode;
    }
}

/*!
    \property QVideoWidget::fullScreen
    \brief whether video display is confined to a window or is fullScreen.
*/

void QVideoWidget::setFullScreen(bool fullScreen)
{
    Q_D(QVideoWidget);

    if (fullScreen) {
        Qt::WindowFlags flags = windowFlags();
        if (!(flags & Qt::Window)) {
            d->nonFullScreenFlags = flags & (Qt::Window | Qt::SubWindow);
            flags |= Qt::Window;
            flags ^= Qt::SubWindow;
            setWindowFlags(flags);
        }
        showFullScreen();
    }
    else
        showNormal();
}

/*!
    \fn QVideoWidget::fullScreenChanged(bool fullScreen)

    Signals that the \a fullScreen mode of a video widget has changed.

    \sa fullScreen
*/

/*!
    \property QVideoWidget::brightness
    \brief an adjustment to the brightness of displayed video.

    Valid brightness values range between -100 and 100, the default is 0.
*/

int QVideoWidget::brightness() const
{
    return d_func()->brightness;
}

void QVideoWidget::setBrightness(int brightness)
{
    Q_D(QVideoWidget);

    int boundedBrightness = qBound(-100, brightness, 100);

    if (d->currentBackend)
        d->currentBackend->setBrightness(boundedBrightness);
    else if (d->brightness != boundedBrightness)
        emit brightnessChanged(d->brightness = boundedBrightness);
}

/*!
    \fn QVideoWidget::brightnessChanged(int brightness)

    Signals that a video widgets's \a brightness adjustment has changed.

    \sa brightness
*/

/*!
    \property QVideoWidget::contrast
    \brief an adjustment to the contrast of displayed video.

    Valid contrast values range between -100 and 100, the default is 0.

*/

int QVideoWidget::contrast() const
{
    return d_func()->contrast;
}

void QVideoWidget::setContrast(int contrast)
{
    Q_D(QVideoWidget);

    int boundedContrast = qBound(-100, contrast, 100);

    if (d->currentBackend)
        d->currentBackend->setContrast(boundedContrast);
    else if (d->contrast != boundedContrast)
        emit contrastChanged(d->contrast = boundedContrast);
}

/*!
    \fn QVideoWidget::contrastChanged(int contrast)

    Signals that a video widgets's \a contrast adjustment has changed.

    \sa contrast
*/

/*!
    \property QVideoWidget::hue
    \brief an adjustment to the hue of displayed video.

    Valid hue values range between -100 and 100, the default is 0.
*/

int QVideoWidget::hue() const
{
    return d_func()->hue;
}

void QVideoWidget::setHue(int hue)
{
    Q_D(QVideoWidget);

    int boundedHue = qBound(-100, hue, 100);

    if (d->currentBackend)
        d->currentBackend->setHue(boundedHue);
    else if (d->hue != boundedHue)
        emit hueChanged(d->hue = boundedHue);
}

/*!
    \fn QVideoWidget::hueChanged(int hue)

    Signals that a video widgets's \a hue has changed.

    \sa hue
*/

/*!
    \property QVideoWidget::saturation
    \brief an adjustment to the saturation of displayed video.

    Valid saturation values range between -100 and 100, the default is 0.
*/

int QVideoWidget::saturation() const
{
    return d_func()->saturation;
}

void QVideoWidget::setSaturation(int saturation)
{
    Q_D(QVideoWidget);

    int boundedSaturation = qBound(-100, saturation, 100);

    if (d->currentBackend)
        d->currentBackend->setSaturation(boundedSaturation);
    else if (d->saturation != boundedSaturation)
        emit saturationChanged(d->saturation = boundedSaturation);

}

/*!
    \fn QVideoWidget::saturationChanged(int saturation)

    Signals that a video widgets's \a saturation has changed.

    \sa saturation
*/

/*!
    \reimp
    \internal
*/

void QVideoWidget::setVisible(bool visible)
{
    Q_D(QVideoWidget);

    if (d->outputControl != 0) {
        if (visible) {
            if (d->widgetBackend != 0) {
                d->setCurrentBackend(d->widgetBackend);
                d->outputControl->setOutput(QVideoOutputControl::WidgetOutput);
            } else if (d->windowBackend != 0
                    && (window() == 0 || !window()->testAttribute(Qt::WA_DontShowOnScreen))) {
                d->setCurrentBackend(d->windowBackend);
                d->outputControl->setOutput(QVideoOutputControl::WindowOutput);
#ifndef QT_NO_MULTIMEDIA
            } else if (d->rendererBackend != 0) {
                d->setCurrentBackend(d->rendererBackend);
                d->outputControl->setOutput(QVideoOutputControl::RendererOutput);
#endif
            } else {
                d->outputControl->setOutput(QVideoOutputControl::NoOutput);
            }
        } else {
            d->outputControl->setOutput(QVideoOutputControl::NoOutput);
        }
    }

    QWidget::setVisible(visible);
}

/*!
    \reimp
    \internal
*/

bool QVideoWidget::event(QEvent *event)
{
    Q_D(QVideoWidget);

    if (event->type() == QEvent::WindowStateChange) {
        Qt::WindowFlags flags = windowFlags();

        if (windowState() & Qt::WindowFullScreen) {
            if (!(flags & Qt::Window)) {
                d->nonFullScreenFlags = flags & (Qt::Window | Qt::SubWindow);
                flags |= Qt::Window;
                flags ^= Qt::SubWindow;
                setWindowFlags(flags);
            }
            if (!d->wasFullScreen)
                emit fullScreenChanged(d->wasFullScreen = true);

            if (d->currentBackend)
                d->currentBackend->setFullScreen(true);
        } else {
            if (d->currentBackend)
                d->currentBackend->setFullScreen(false);

            if (isVisible()) {
                flags ^= (Qt::Window | Qt::SubWindow); //clear the flags...
                flags |= d->nonFullScreenFlags; //then we reset the flags (window and subwindow)
                setWindowFlags(flags);
            }

            if (d->wasFullScreen)
                emit fullScreenChanged(d->wasFullScreen = false);
        }
    }
    return QWidget::event(event);
}

#include "moc_qvideowidget.cpp"
