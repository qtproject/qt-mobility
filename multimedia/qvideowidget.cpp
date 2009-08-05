/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qvideowidget.h"

#include "qabstractmediaservice.h"
#include "qvideooverlayendpoint.h"

#ifndef QT_NO_VIDEOSURFACE
#include "qpaintervideosurface_p.h"
#include "qvideorendererendpoint.h"
#include "qvideosurfaceformat.h"
#include <qpainter.h>
#endif

#include <qapplication.h>
#include <qevent.h>
#include <qdialog.h>

#include <private/qwidget_p.h>

class QVideoWidgetPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QVideoWidget)
public:
    QVideoWidgetPrivate()
        : service(0)
        , overlay(0)
#ifndef QT_NO_VIDEOSURFACE
        , renderer(0)
        , surface(0)
#endif
        , fullscreenWindow(0)
        , fullscreen(false)
    {
    }

    QAbstractMediaService *service;
    QVideoOverlayEndpoint *overlay;
#ifndef QT_NO_VIDEOSURFACE
    QVideoRendererEndpoint *renderer;
    QPainterVideoSurface *surface;
#endif
    QDialog *fullscreenWindow;
    bool fullscreen;

    void _q_overlayFullscreenChanged(bool fullscreen);
    void _q_dimensionsChanged();
    void _q_fullscreenWindowDone();
};

void QVideoWidgetPrivate::_q_overlayFullscreenChanged(bool fullscreen)
{
    emit q_func()->fullscreenChanged(this->fullscreen = fullscreen);
}

void QVideoWidgetPrivate::_q_dimensionsChanged()
{
    q_func()->updateGeometry();
}

void QVideoWidgetPrivate::_q_fullscreenWindowDone()
{
    if (fullscreen)
        q_func()->setFullscreen(false);
}

/*!
*/
QVideoWidget::QVideoWidget(QAbstractMediaService *service, QWidget *parent)
    : QWidget(*new QVideoWidgetPrivate, parent, 0)
{
    Q_D(QVideoWidget);

    d->service = service;

    if (!d->service)
        return;

    if ((d->overlay = service->createEndpoint<QVideoOverlayEndpoint *>())) {
        connect(d->overlay, SIGNAL(fullscreenChanged(bool)),
                this, SLOT(_q_overlayFullscreenChanged(bool)));
        connect(d->overlay, SIGNAL(nativeSizeChanged()),
                this, SLOT(_q_dimensionsChanged()));
        d->service->setVideoOutput(d->overlay);
#ifndef QT_NO_VIDEOSURFACE
    } else if ((d->renderer = service->createEndpoint<QVideoRendererEndpoint *>())) {
        d->surface = new QPainterVideoSurface;

        d->renderer->setSurface(d->surface);

        connect(d->surface, SIGNAL(frameChanged()), this, SLOT(update()));

        connect(d->surface, SIGNAL(surfaceFormatChanged(QVideoSurfaceFormat)),
                this, SLOT(_q_dimensionsChanged()));

        d->service->setVideoOutput(d->renderer);
#endif
    }

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

/*!
*/
QVideoWidget::~QVideoWidget()
{
    Q_D(QVideoWidget);

    delete d->fullscreenWindow;
}

/*!
*/
bool QVideoWidget::isFullscreen() const
{
    return d_func()->fullscreen;
}

/*!
*/
void QVideoWidget::setFullscreen(bool fullscreen)
{
    Q_D(QVideoWidget);

    if (fullscreen) {
        if (!d->fullscreenWindow) {
            d->fullscreenWindow = new QDialog;

            connect(d->fullscreenWindow, SIGNAL(finished(int)),
                    this, SLOT(_q_fullscreenWindowDone()));
        }

        d->fullscreenWindow->showFullScreen();

        if (d->overlay) {
            d->overlay->setWinId(d->fullscreenWindow->winId());
            d->overlay->setFullscreen(true);
            d->overlay->setDisplayRect(d->fullscreenWindow->rect());
        } else {
            emit fullscreenChanged(d->fullscreen = true);
        }
    } else if (d->fullscreenWindow) {
        if (d->overlay) {
            d->overlay->setFullscreen(false);
            d->overlay->setWinId(effectiveWinId());

            QRect displayRect = rect();
            displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));
            d->overlay->setDisplayRect(displayRect);
        } else {
            emit fullscreenChanged(d->fullscreen = false);
        }
        d->fullscreenWindow->hide();
    }
}

/*!
    \reimp
*/
QSize QVideoWidget::sizeHint() const
{
    Q_D(const QVideoWidget);

    if (d->overlay)
        return d->overlay->nativeSize();
#ifndef QT_NO_VIDEOSURFACE
    else if (d->surface)
        return d->surface->surfaceFormat().sizeHint();
#endif
    else
        return QSize();
}

/*!
    \fn QVideoWidget::fullscreenChanged(bool fullscreen)
*/

/*!
    \reimp
*/
void QVideoWidget::showEvent(QShowEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::showEvent(event);

    if (d->overlay) {
        d->overlay->setWinId(effectiveWinId());
        d->overlay->setEnabled(true);
    }
}

/*!
    \reimp
*/
void QVideoWidget::hideEvent(QHideEvent *event)
{
    Q_D(QVideoWidget);

    if (d->overlay)
        d->overlay->setEnabled(false);

    QWidget::hideEvent(event);
}

/*!
    \reimp
*/
void QVideoWidget::moveEvent(QMoveEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::moveEvent(event);

    if (d->overlay && !d->fullscreen) {
        QRect displayRect = rect();
        displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));
        d->overlay->setDisplayRect(displayRect);
    }
}

/*!
    \reimp
*/
void QVideoWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(QVideoWidget);

    QWidget::resizeEvent(event);

    if (d->overlay && !d->fullscreen) {
        QRect displayRect = rect();
        displayRect.moveTo(mapTo(nativeParentWidget(), displayRect.topLeft()));
        d->overlay->setDisplayRect(displayRect);
    }
}

/*!
    \reimp
*/
void QVideoWidget::paintEvent(QPaintEvent *event)
{
    Q_D(QVideoWidget);

    if (d->overlay && d->overlay->isEnabled()) {
        d->overlay->repaint();
#ifndef QT_NO_VIDEOSURFACE
    } else if (d->surface) {
        QWidget *widget;

        if (d->fullscreenWindow && d->fullscreenWindow->isVisible())
            widget = d->fullscreenWindow;
        else
            widget = this;

        QPainter painter(widget);

        if (d->surface->isStarted()) {
            d->surface->paint(&painter, widget->rect());

            d->surface->setReady(true);
        }
#endif
    } else {
        QWidget::paintEvent(event);
    }
}

#include "moc_qvideowidget.cpp"
