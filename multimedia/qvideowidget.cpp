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

#include "qvideowidget_p.h"

#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"
#include "qvideooutputcontrol.h"
#include "qvideowindowcontrol.h"
#include "qvideowidgetcontrol.h"

#ifndef QT_NO_MULTIMEDIA
#include "qpaintervideosurface_p.h"
#include "qvideorenderercontrol.h"
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

void QVideoWidgetControlBackend::setDisplayMode(QVideoWidget::DisplayMode mode)
{
    m_widgetControl->setFullscreen(mode == QVideoWidget::FullscreenDisplay);
}

void QVideoWidgetControlBackend::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    m_widgetControl->setAspectRatio(ratio);
}

void QVideoWidgetControlBackend::setCustomPixelAspectRatio(const QSize &customRatio)
{
    m_widgetControl->setCustomAspectRatio(customRatio);
}

QWidget *QVideoWidgetControlBackend::widget()
{
    return m_widgetControl->videoWidget();
}

QFullScreenVideoWidget::QFullScreenVideoWidget(QWidget *parent)
    : QWidget(parent)
{
}

QFullScreenVideoWidget::~QFullScreenVideoWidget()
{
}

QVideoWidget::DisplayMode QFullScreenVideoWidget::displayMode() const
{
    return m_displayMode;
}

void QFullScreenVideoWidget::setDisplayMode(QVideoWidget::DisplayMode mode)
{
     if (mode == QVideoWidget::FullscreenDisplay) {
        setWindowFlags(windowFlags() | Qt::Window | Qt::WindowStaysOnTopHint);
        setWindowState(windowState() | Qt::WindowFullScreen);

        show();

        emit displayModeChanged(m_displayMode = mode);
    } else {
        setWindowFlags(windowFlags() & ~(Qt::Window | Qt::WindowStaysOnTopHint));
        setWindowState(windowState() & ~Qt::WindowFullScreen);

        show();

        emit displayModeChanged(m_displayMode = mode);
    }
}

QWidget *QFullScreenVideoWidget::widget()
{
    return this;
}

#ifndef QT_NO_MULTIMEDIA

QVideoRendererWidget::QVideoRendererWidget(QVideoRendererControl *control, QWidget *parent)
    : QFullScreenVideoWidget(parent)
    , m_rendererControl(control)
    , m_surface(new QPainterVideoSurface)
{
    connect(m_surface, SIGNAL(frameChanged()), SLOT(update()));
    connect(m_surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)), SLOT(dimensionsChanged()));

    m_rendererControl->setSurface(m_surface);
}

QVideoRendererWidget::~QVideoRendererWidget()
{
    m_rendererControl->setSurface(0);

    delete m_surface;
}

void QVideoRendererWidget::setAspectRatio(QVideoWidget::AspectRatio mode)
{
    m_aspectRatioMode = mode;

    updateGeometry();

    emit aspectRatioModeChanged(m_aspectRatioMode);
}

void QVideoRendererWidget::setCustomPixelAspectRatio(const QSize &ratio)
{
    m_aspectRatio = ratio;

    updateGeometry();

    emit customAspectRatioChanged(m_aspectRatio);
}

QSize QVideoRendererWidget::sizeHint() const
{
    return m_surface->surfaceFormat().sizeHint();
}

void QVideoRendererWidget::paintEvent(QPaintEvent *event)
{
    if (m_surface->isStarted()) {
        QPainter painter(this);

        m_surface->paint(&painter, displayRect());

        m_surface->setReady(true);
    } else {
        QWidget::paintEvent(event);
    }
}

void QVideoRendererWidget::dimensionsChanged()
{
    updateGeometry();
}

QRect QVideoRendererWidget::displayRect() const
{
    QRect displayRect = rect();

    return displayRect;
}

#endif

QVideoWindowWidget::QVideoWindowWidget(QVideoWindowControl *control, QWidget *parent)
    : QFullScreenVideoWidget(parent)
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

void QVideoWindowWidget::setDisplayMode(QVideoWidget::DisplayMode mode)
{
    m_windowControl->setFullscreen(mode == QVideoWidget::FullscreenDisplay);

    QFullScreenVideoWidget::setDisplayMode(mode);
}

void QVideoWindowWidget::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    m_windowControl->setAspectRatio(ratio);

    emit aspectRatioModeChanged(m_windowControl->aspectRatio());
}

void QVideoWindowWidget::setCustomPixelAspectRatio(const QSize &customRatio)
{
    m_windowControl->setCustomAspectRatio(customRatio);

    emit customAspectRatioChanged(m_windowControl->customAspectRatio());
}

QSize QVideoWindowWidget::sizeHint() const
{
    return m_windowControl->nativeSize();
}

void QVideoWindowWidget::setVisible(bool visible)
{
    if (visible)
        m_windowControl->setWinId(winId());

    QFullScreenVideoWidget::setVisible(visible);
}

void QVideoWindowWidget::moveEvent(QMoveEvent *event)
{
    QFullScreenVideoWidget::moveEvent(event);

    QRect displayRect = rect();
    displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));

    m_windowControl->setDisplayRect(displayRect);
}

void QVideoWindowWidget::resizeEvent(QResizeEvent *event)
{
    QFullScreenVideoWidget::resizeEvent(event);

    QRect displayRect = rect();
    displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));

    m_windowControl->setDisplayRect(displayRect);
}

void QVideoWindowWidget::paintEvent(QPaintEvent *event)
{
    m_windowControl->repaint();

    event->accept();
}

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
        , displayMode(QVideoWidget::WindowedDisplay)
        , aspectRatio(QVideoWidget::AspectRatioAuto)
        , customPixelAspectRatio(1, 1)
    {
    }

    QVideoWidget *q_ptr;
    QStackedLayout *layout;
    QAbstractMediaService *service;
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
    QVideoWidget::DisplayMode displayMode;
    QVideoWidget::AspectRatio aspectRatio;
    QSize customPixelAspectRatio;

    void setCurrentBackend(QVideoWidgetBackendInterface *backend);

    void _q_serviceDestroyed();
    void _q_dimensionsChanged();
    void _q_brightnessChanged(int brightness);
    void _q_contrastChanged(int contrast);
    void _q_hueChanged(int hue);
    void _q_saturationChanged(int saturation);
    void _q_fullScreenChanged(bool fullscreen);
    void _q_displayModeChanged(QVideoWidget::DisplayMode mode);
    void _q_aspectRatioModeChanged(QVideoWidget::AspectRatio mode);
    void _q_customAspectRatioChanged(const QSize &ratio);
};

void QVideoWidgetPrivate::setCurrentBackend(QVideoWidgetBackendInterface *backend)
{
    if (currentBackend != backend) {
        currentBackend = backend;

        currentBackend->setBrightness(brightness);
        currentBackend->setContrast(contrast);
        currentBackend->setHue(hue);
        currentBackend->setSaturation(saturation);
        currentBackend->setAspectRatio(aspectRatio);
        currentBackend->setCustomPixelAspectRatio(customPixelAspectRatio);
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

void QVideoWidgetPrivate::_q_dimensionsChanged()
{
    q_func()->updateGeometry();
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


void QVideoWidgetPrivate::_q_fullScreenChanged(bool fullscreen)
{
    _q_displayModeChanged(fullscreen
            ? QVideoWidget::FullscreenDisplay
            : QVideoWidget::WindowedDisplay);
}

void QVideoWidgetPrivate::_q_displayModeChanged(QVideoWidget::DisplayMode mode)
{       
    if (mode != displayMode) {
        if (mode == QVideoWidget::WindowedDisplay) {
            layout->activate();
        }

        emit q_func()->displayModeChanged(displayMode = mode);
    }
}


void QVideoWidgetPrivate::_q_aspectRatioModeChanged(QVideoWidget::AspectRatio mode)
{
    aspectRatio = mode;
}

void QVideoWidgetPrivate::_q_customAspectRatioChanged(const QSize &ratio)
{
    customPixelAspectRatio = ratio;
}

/*!
    \class QVideoWidget
    \brief The QVideoWidget class provides a widget which displays video produced by a media
    object.
    \preliminary
*/

/*!
    Constructs a new widget with the given \a parent which displays video produced by a media
    \a object.
*/
QVideoWidget::QVideoWidget(QAbstractMediaObject *object, QWidget *parent)
    : QWidget(parent, 0)
    , d_ptr(new QVideoWidgetPrivate)
{
    Q_D(QVideoWidget);

    d->q_ptr = this;

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

    widgetControl = 0;

    if (widgetControl != 0) {
        d->widgetBackend = new QVideoWidgetControlBackend(widgetControl);

        QWidget *widget = d->widgetBackend->widget();
        widget->installEventFilter(this);

        d->layout->addWidget(widget);

        connect(widgetControl, SIGNAL(brightnessChanged(int)), SLOT(_q_brightnessChanged(int)));
        connect(widgetControl, SIGNAL(contrastChanged(int)), SLOT(_q_contrastChanged(int)));
        connect(widgetControl, SIGNAL(hueChanged(int)), SLOT(_q_hueChanged(int)));
        connect(widgetControl, SIGNAL(saturationChanged(int)), SLOT(_q_saturationChanged(int)));
        connect(widgetControl, SIGNAL(fullscreenChanged(bool)), SLOT(_q_fullScreenChanged(bool)));

        connect(d->widgetBackend, SIGNAL(aspectRatioModeChanged(QVideoWidget::AspectRatio)),
                SLOT(_q_aspectRatioModeChanged(QVideoWidget::AspectRatio)));
        connect(d->widgetBackend, SIGNAL(customAspectRatioChanged(QSize)),
                SLOT(_q_customAspectRatioChanged(QSize)));

        d->brightness = widgetControl->brightness();
        d->contrast = widgetControl->contrast();
        d->hue = widgetControl->hue();
        d->saturation = widgetControl->saturation();

        d->currentBackend = d->widgetBackend;
    } else {
        QVideoWindowControl *windowControl = qobject_cast<QVideoWindowControl *>(
                d->service->control(QVideoWindowControl_iid));

        if (windowControl != 0) {
            d->windowBackend = new QVideoWindowWidget(windowControl);
            d->windowBackend->installEventFilter(this);

            d->layout->addWidget(d->windowBackend);

            connect(windowControl, SIGNAL(brightnessChanged(int)), SLOT(_q_brightnessChanged(int)));
            connect(windowControl, SIGNAL(contrastChanged(int)), SLOT(_q_contrastChanged(int)));
            connect(windowControl, SIGNAL(hueChanged(int)), SLOT(_q_hueChanged(int)));
            connect(windowControl, SIGNAL(saturationChanged(int)), SLOT(_q_saturationChanged(int)));
            connect(windowControl, SIGNAL(fullscreenChanged(bool)),
                    SLOT(_q_fullScreenChanged(bool)));

            connect(d->windowBackend, SIGNAL(displayModeChanged(QVideoWidget::DisplayMode)),
                    SLOT(_q_displayModeChanged(QVideoWidget::DisplayMode)));
            connect(d->windowBackend, SIGNAL(aspectRatioModeChanged(QVideoWidget::AspectRatio)),
                    SLOT(_q_aspectRatioModeChanged(QVideoWidget::AspectRatio)));
            connect(d->windowBackend, SIGNAL(customAspectRatioChanged(QSize)),
                    SLOT(_q_customAspectRatioChanged(QSize)));

            d->brightness = windowControl->brightness();
            d->contrast = windowControl->contrast();
            d->hue = windowControl->hue();
            d->saturation = windowControl->saturation();

            d->currentBackend = d->windowBackend;
        }
#ifndef QT_NO_MULTIMEDIA
        QVideoRendererControl *rendererControl = qobject_cast<QVideoRendererControl *>(
                d->service->control(QVideoRendererControl_iid));

        if (rendererControl != 0) {
            d->rendererBackend = new QVideoRendererWidget(rendererControl);
            d->rendererBackend->installEventFilter(this);

            d->layout->addWidget(d->rendererBackend);

            connect(d->rendererBackend, SIGNAL(brightnessChanged(int)),
                    SLOT(_q_brightnessChanged(int)));
            connect(d->rendererBackend, SIGNAL(contrastChanged(int)),
                    SLOT(_q_contrastChanged(int)));
            connect(d->rendererBackend, SIGNAL(hueChanged(int)), SLOT(_q_hueChanged(int)));
            connect(d->rendererBackend, SIGNAL(saturationChanged(int)),
                    SLOT(_q_saturationChanged(int)));
            connect(d->rendererBackend, SIGNAL(displayModeChanged(QVideoWidget::DisplayMode)),
                    SLOT(_q_displayModeChanged(QVideoWidget::DisplayMode)));

            connect(d->rendererBackend, SIGNAL(aspectRatioModeChanged(QVideoWidget::AspectRatio)),
                    SLOT(_q_aspectRatioModeChanged(QVideoWidget::AspectRatio)));
            connect(d->rendererBackend, SIGNAL(customAspectRatioChanged(QSize)),
                    SLOT(_q_customAspectRatioChanged(QSize)));
        }
#endif
    }

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
        delete d_ptr->rendererBackend;
#endif
    }
    delete d_ptr;
}

/*!
    \property QVideoWidget::displayMode
    \brief whether video display is confined to a window or is fullscreen.
*/

QVideoWidget::DisplayMode QVideoWidget::displayMode() const
{
    return d_func()->displayMode;
}

QVideoWidget::AspectRatio QVideoWidget::aspectRatio() const
{
    return d_func()->aspectRatio;
}

QSize QVideoWidget::customPixelAspectRatio() const
{
    return d_func()->customPixelAspectRatio;
}

void QVideoWidget::setAspectRatio(QVideoWidget::AspectRatio ratio)
{
    Q_D(QVideoWidget);

    if (d->currentBackend)
        d->currentBackend->setAspectRatio(ratio);
}

void QVideoWidget::setCustomPixelAspectRatio(const QSize &customRatio)
{
    Q_D(QVideoWidget);

    if (d->currentBackend)
        d->currentBackend->setCustomPixelAspectRatio(customRatio);
}

void QVideoWidget::setDisplayMode(DisplayMode mode)
{
    Q_D(QVideoWidget);

    if (d->currentBackend)
        d->currentBackend->setDisplayMode(mode);

    if (mode == d->displayMode)
        return;
}

/*!
    \fn QVideoWidget::displayModeChanged(QVideoWidget::DisplayMode mode)

    Signals that the display \a mode of a video widget has changed.
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

    if (d->currentBackend)
        d->currentBackend->setBrightness(qBound(-100, brightness, 100));
}

/*!
    \fn QVideoWidget::brightnessChanged(int brightness)

    Signals that a video widgets's \a brightness adjustment has changed.
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

    if (d->currentBackend)
        d->currentBackend->setContrast(qBound(-100, contrast, 100));
}

/*!
    \fn QVideoWidget::contrastChanged(int contrast)

    Signals that a video widgets's \a contrast adjustment has changed.
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

    if (d->currentBackend)
        d->currentBackend->setHue(qBound(-100, hue, 100));
}

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

    if (d->currentBackend)
        d->currentBackend->setSaturation(qBound(-100, saturation, 100));
}

void QVideoWidget::setVisible(bool visible)
{
    Q_D(QVideoWidget);

    if (d->service == 0 || d->outputControl == 0) {
        visible = false;
    } else if (visible) {
        if (d->widgetBackend != 0) {
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
            visible = false;
        }
    } else {
        d->outputControl->setOutput(QVideoOutputControl::NoOutput);
    }

    QWidget::setVisible(visible);
}

bool QVideoWidget::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonDblClick:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseMove:
        return event(e);
    default:
        return false;
    }
}

void QVideoWidget::keyPressEvent(QKeyEvent *event)
{
    Q_D(QVideoWidget);

    if (event->key() == Qt::Key_Escape && d->displayMode == FullscreenDisplay) {
        setDisplayMode(WindowedDisplay);

        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

#include "moc_qvideowidget.cpp"
