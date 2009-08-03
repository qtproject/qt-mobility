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
#include "qvideorendererendpoint.h"

#include <qevent.h>

#include <private/qwidget_p.h>

class QVideoWidgetPrivate : public QWidgetPrivate
{
    Q_DECLARE_PUBLIC(QVideoWidget)
public:
    QVideoWidgetPrivate()
        : service(0)
        , overlay(0)
        , fullscreen(false)
    {
    }

    QAbstractMediaService *service;
    QVideoOverlayEndpoint *overlay;
    bool fullscreen;

    void _q_overlayFullscreenChanged(bool fullscreen);
    void _q_dimensionsChanged();
};

void QVideoWidgetPrivate::_q_overlayFullscreenChanged(bool fullscreen)
{
    emit q_func()->fullscreenChanged(this->fullscreen = fullscreen);
}

void QVideoWidgetPrivate::_q_dimensionsChanged()
{
    q_func()->updateGeometry();
}

/*!
*/
QVideoWidget::QVideoWidget(QAbstractMediaService *service, QWidget *parent)
    : QWidget(*new QVideoWidgetPrivate, parent, 0)
{
    Q_D(QVideoWidget);

    d->service = service;

    if (d->service) {
        d->overlay = service->createEndpoint<QVideoOverlayEndpoint *>();

        if (d->overlay) {
            connect(d->overlay, SIGNAL(fullscreenChanged(bool)),
                    this, SLOT(_q_overlayFullscreenChanged(bool)));
            connect(d->overlay, SIGNAL(nativeSizeChanged()),
                    this, SLOT(_q_dimensionsChanged()));
            d->service->setVideoOutput(d->overlay);
        }
    }

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

/*!
*/
QVideoWidget::~QVideoWidget()
{
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

    if (d->overlay)
        d->overlay->setFullscreen(fullscreen);
}

/*!
    \reimp
*/
QSize QVideoWidget::sizeHint() const
{
    return d_func()->overlay
            ? d_func()->overlay->nativeSize()
            : QWidget::sizeHint();
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

    if (d->overlay) {
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

    if (d->overlay) {
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

    if (d->overlay && d->overlay->isEnabled())
        d->overlay->repaint();
    else
        QWidget::paintEvent(event);
}

#include "moc_qvideowidget.cpp"
