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

#include "qvideowidget.h"

#include "qabstractmediaobject.h"
#include "qabstractmediaservice.h"
#include "qvideooutputcontrol.h"
#include "qvideowindowcontrol.h"
#include "qvideowidgetcontrol.h"

#ifndef QT_NO_VIDEOSURFACE
#include "qpaintervideosurface_p.h"
#include "qvideorenderercontrol.h"
#include "qvideosurfaceformat.h"
#include <qpainter.h>
#endif

#include <qapplication.h>
#include <qevent.h>
#include <qdialog.h>

#include <private/qwidget_p.h>

class QVideoWidgetPrivate
{
    Q_DECLARE_PUBLIC(QVideoWidget)
public:
    QVideoWidgetPrivate()
        : service(0)
        , output(0)
        , overlay(0)
        , videoWidgetControl(0)
#ifndef QT_NO_VIDEOSURFACE
        , renderer(0)
        , surface(0)
#endif
        , displayMode(QVideoWidget::WindowedDisplay)
        , aspectRatio(QVideoWidget::AspectRatioAuto)
        , customPixelAspectRatio(1,1)
        , windowedFlags(0)
    {
    }

    QVideoWidget *q_ptr;
    QAbstractMediaService *service;
    QVideoOutputControl *output;
    QVideoWindowControl *overlay;
    QVideoWidgetControl *videoWidgetControl;
#ifndef QT_NO_VIDEOSURFACE
    QVideoRendererControl *renderer;
    QPainterVideoSurface *surface;
#endif
    QVideoWidget::DisplayMode displayMode;
    QVideoWidget::AspectRatio aspectRatio;
    QSize customPixelAspectRatio;

    Qt::WindowFlags windowedFlags;

    QRect displayRect() const;

    void _q_overlayFullscreenChanged(bool fullscreen);
    void _q_dimensionsChanged();
};

void QVideoWidgetPrivate::_q_overlayFullscreenChanged(bool fullscreen)
{
    if (fullscreen && displayMode != QVideoWidget::FullscreenDisplay)
        emit q_func()->displayModeChanged(displayMode = QVideoWidget::FullscreenDisplay);
    else if (!fullscreen && displayMode != QVideoWidget::WindowedDisplay)
        emit q_func()->displayModeChanged(displayMode = QVideoWidget::WindowedDisplay);
}

void QVideoWidgetPrivate::_q_dimensionsChanged()
{
    q_func()->updateGeometry();

    if (overlay)
        overlay->setDisplayRect(displayRect());

    q_func()->update();
}

QRect QVideoWidgetPrivate::displayRect() const
{
    QRect displayRect = q_func()->rect();
    displayRect.moveTo(q_func()->mapTo(q_func()->nativeParentWidget(), displayRect.topLeft()));

    if (overlay) {
        if (aspectRatio == QVideoWidget::AspectRatioAuto) {
            //TODO: take into account display aspect ratio, if not 1
            QSize videoSize = overlay->nativeSize();
            if (!videoSize.isEmpty()) {
                videoSize.scale(displayRect.size(), Qt::KeepAspectRatio);
                QRect videoRect(QPoint(0,0), videoSize);
                videoRect.moveCenter(displayRect.center());

                return videoRect;
            }
        }
    }

    return displayRect;
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

    qDebug(Q_FUNC_INFO);

    d->q_ptr = this;

    if (object)
        d->service = object->service();

    if (!d->service)
        return;

    d->output = qobject_cast<QVideoOutputControl*>(d->service->control(QVideoOutputControl_iid));
    d->overlay = qobject_cast<QVideoWindowControl*>(d->service->control(QVideoWindowControl_iid));

    if (d->overlay != 0) {
        qDebug("Use overlay");
        connect(d->overlay, SIGNAL(fullscreenChanged(bool)), SLOT(_q_overlayFullscreenChanged(bool)));
        connect(d->overlay, SIGNAL(nativeSizeChanged()), SLOT(_q_dimensionsChanged()));
        connect(d->overlay, SIGNAL(brightnessChanged(int)), SIGNAL(brightnessChanged(int)));
        connect(d->overlay, SIGNAL(contrastChanged(int)), SIGNAL(contrastChanged(int)));
        connect(d->overlay, SIGNAL(hueChanged(int)), SIGNAL(hueChanged(int)));
        connect(d->overlay, SIGNAL(saturationChanged(int)), SIGNAL(saturationChanged(int)));
    } else {
        qDebug("User video widget");
        d->videoWidgetControl = qobject_cast<QVideoWidgetControl*>(d->service->control(QVideoWidgetControl_iid));
        if (d->videoWidgetControl != 0) {
            d->videoWidgetControl->videoWidget()->setParent(this);
            d->videoWidgetControl->videoWidget()->installEventFilter(this);

            connect(d->videoWidgetControl, SIGNAL(fullscreenChanged(bool)), SLOT(_q_overlayFullscreenChanged(bool)));
            connect(d->videoWidgetControl, SIGNAL(brightnessChanged(int)), SIGNAL(brightnessChanged(int)));
            connect(d->videoWidgetControl, SIGNAL(contrastChanged(int)), SIGNAL(contrastChanged(int)));
            connect(d->videoWidgetControl, SIGNAL(hueChanged(int)), SIGNAL(hueChanged(int)));
            connect(d->videoWidgetControl, SIGNAL(saturationChanged(int)), SIGNAL(saturationChanged(int)));
        }
    }

#ifndef QT_NO_VIDEOSURFACE
    d->renderer = qobject_cast<QVideoRendererControl*>(d->service->control(QVideoRendererControl_iid));
    if (d->renderer != 0) {
        d->surface = new QPainterVideoSurface;

        d->renderer->setSurface(d->surface);

        connect(d->surface, SIGNAL(frameChanged()), SLOT(update()));
        connect(d->surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)), SLOT(_q_dimensionsChanged()));
    }
#endif

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/*!
    Destroys a video widget.
*/
QVideoWidget::~QVideoWidget()
{
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
    d->aspectRatio = ratio;

    if (d->overlay)
        d->overlay->setDisplayRect(d->displayRect());

    if (d->videoWidgetControl)
        d->videoWidgetControl->setAspectRatio(ratio);
}

void QVideoWidget::setCustomPixelAspectRatio(const QSize &customRatio)
{
    Q_D(QVideoWidget);
    d->customPixelAspectRatio = customRatio;

    if (d->overlay)
        d->overlay->setDisplayRect(d->displayRect());

    if (d->videoWidgetControl)
        d->videoWidgetControl->setCustomAspectRatio(customRatio);
}

void QVideoWidget::setDisplayMode(DisplayMode mode)
{
    Q_D(QVideoWidget);

    if (mode == d->displayMode)
        return;

    if (mode == FullscreenDisplay) {
        d->windowedFlags = windowFlags();
        setWindowFlags((windowFlags() & ~Qt::SubWindow) | Qt::Window);
        setWindowState(windowState() | Qt::WindowFullScreen);

        show();

        if (d->overlay) {
            d->overlay->setFullscreen(true);
        } else if (d->videoWidgetControl) {
            d->videoWidgetControl->setFullscreen(true);
#ifndef QT_NO_VIDEOSURFACE
        } else if (d->surface) {
            emit displayModeChanged(d->displayMode = mode);
#endif
        }
    } else {
        setWindowFlags(d->windowedFlags);
        setWindowState(windowState() & ~Qt::WindowFullScreen);

        show();

        if (d->overlay) {
            d->overlay->setFullscreen(false);
        } else if (d->videoWidgetControl) {
            d->videoWidgetControl->setFullscreen(false);
#ifndef QT_NO_VIDEOSURFACE
        } else if (d->surface) {
            emit displayModeChanged(d->displayMode = mode);
#endif
        }
    }
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
    Q_D(const QVideoWidget);

    if (d->overlay)
        return d->overlay->brightness();
    else if (d->videoWidgetControl)
        return d->videoWidgetControl->brightness();
    else
        return 0;
}

void QVideoWidget::setBrightness(int brightness)
{
    Q_D(QVideoWidget);

    if (d->overlay)
        d->overlay->setBrightness(qBound(-100, brightness, 100));
    else if (d->videoWidgetControl)
        d->videoWidgetControl->setBrightness(qBound(-100, brightness, 100));
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
    Q_D(const QVideoWidget);

    if (d->overlay)
        return d->overlay->contrast();
    else if (d->videoWidgetControl)
        return d->videoWidgetControl->contrast();
    else
        return 0;
}

void QVideoWidget::setContrast(int contrast)
{
    Q_D(QVideoWidget);

    if (d->overlay)
        d->overlay->setContrast(qBound(-100, contrast, 100));
    else if (d->videoWidgetControl)
        d->videoWidgetControl->setContrast(qBound(-100, contrast, 100));
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
    Q_D(const QVideoWidget);

    if (d->overlay)
        return d->overlay->hue();
    else if (d->videoWidgetControl)
        return d->videoWidgetControl->hue();
    else
        return 0;
}

void QVideoWidget::setHue(int hue)
{
    Q_D(QVideoWidget);

    if (d->overlay)
        d->overlay->setHue(qBound(-100, hue, 100));
    else if (d->videoWidgetControl)
        d->videoWidgetControl->setHue(qBound(-100, hue, 100));
}

/*!
    \fn QVideoWidget::hueChanged(int hue)

    Signals that a video widgets's \a hue adjustment has changed.
*/

/*!
    \property QVideoWidget::saturation
    \brief an adjustment to the saturation of displayed video.

    Valid saturation values range between -100 and 100, the default is 0.
*/

int QVideoWidget::saturation() const
{
    Q_D(const QVideoWidget);

    if (d->overlay)
        return d->overlay->saturation();
    else if (d->videoWidgetControl)
        return d->videoWidgetControl->saturation();
    else
        return 0;
}

void QVideoWidget::setSaturation(int saturation)
{
    Q_D(QVideoWidget);

    if (d->overlay)
        d->overlay->setSaturation(qBound(-100, saturation, 100));
    else if (d->videoWidgetControl)
        d->videoWidgetControl->setSaturation(qBound(-100, saturation, 100));
}

/*!
    \fn QVideoWidget::saturationChanged(int saturation)

    Signals that a video widgets's \a saturation adjustment has changed.
*/

/*!
    \reimp
*/
QSize QVideoWidget::sizeHint() const
{
    Q_D(const QVideoWidget);

    if (d->overlay)
        return d->overlay->nativeSize();
    else if (d->videoWidgetControl)
        return d->videoWidgetControl->videoWidget()->sizeHint();
#ifndef QT_NO_VIDEOSURFACE
    else if (d->surface)
        return d->surface->surfaceFormat().sizeHint();
#endif
    else
        return QSize();
}

/*!
    \reimp
*/
void QVideoWidget::setVisible(bool visible)
{
    Q_D(QVideoWidget);

    if (isVisible() == visible || !d->output)
        return;

    if (visible) {
        if ((window() && window()->testAttribute(Qt::WA_DontShowOnScreen)) || (!d->overlay && !d->videoWidgetControl)) {
#ifndef QT_NO_VIDEOSURFACE
            if (d->renderer)
                d->output->setOutput(QVideoOutputControl::RendererOutput);
            else
                return;
#else
            return;
#endif
        } else if (d->overlay) {
            d->overlay->setWinId(winId());
            d->output->setOutput(QVideoOutputControl::WindowOutput);
        } else if (d->videoWidgetControl) {
            d->output->setOutput(QVideoOutputControl::WidgetOutput);
            d->videoWidgetControl->videoWidget()->show();
        }

    } else {
        d->output->setOutput(QVideoOutputControl::NoOutput);
    }
    QWidget::setVisible(visible);
}

/*!
    \reimp
*/
void QVideoWidget::showEvent(QShowEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::showEvent(event);

    if (d->videoWidgetControl) {
        d->videoWidgetControl->videoWidget()->show();
        d->videoWidgetControl->videoWidget()->setGeometry(0, 0, width(), height());
    }
}

/*!
    \reimp
*/
void QVideoWidget::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
}

/*!
    \reimp
*/
void QVideoWidget::moveEvent(QMoveEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::moveEvent(event);

    if (d->overlay) {
        d->overlay->setDisplayRect(d->displayRect());
    }
}

/*!
    \reimp
*/
void QVideoWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::resizeEvent(event);

    if (d->overlay) {
        d->overlay->setDisplayRect(d->displayRect());
    }

    if (d->videoWidgetControl) {
        d->videoWidgetControl->videoWidget()->setGeometry(0, 0, width(), height());
    }
}

/*!
    \reimp
*/
void QVideoWidget::paintEvent(QPaintEvent *event)
{
    Q_D(QVideoWidget);

    if (d->overlay) {
        d->overlay->repaint();
#ifndef QT_NO_VIDEOSURFACE
    } else if (d->surface) {
        QPainter painter(this);

        if (d->surface->isStarted()) {
            d->surface->paint(&painter, rect());

            d->surface->setReady(true);
        }
#endif
    } else {
        QWidget::paintEvent(event);
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

bool QVideoWidget::eventFilter(QObject *obj, QEvent *event)
{
    Q_D(QVideoWidget);

    if (d->videoWidgetControl &&
        obj == d->videoWidgetControl->videoWidget() &&
        d->displayMode == FullscreenDisplay &&
        event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = reinterpret_cast<QKeyEvent*>(event);
        if (ke->key() == Qt::Key_Escape) {
            setDisplayMode(WindowedDisplay);
            return true;
        }
    }
    return false;
}

#include "moc_qvideowidget.cpp"
