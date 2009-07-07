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

#include "qvideorendererwidget_p.h"
#include "qmediawidgetendpoint_p.h"

#ifndef QT_NO_VIDEOSURFACE

#include "qpaintervideosurface_p.h"
#include "qvideorendererendpoint.h"

#include <qpainter.h>

class QVideoRendererWidgetPrivate : public QMediaWidgetEndpointPrivate
{
    Q_DECLARE_PUBLIC(QVideoRendererWidget)
public:
    QVideoRendererWidgetPrivate()
        : rendererOutput(0)
    {
    }

    QVideoRendererEndpoint *rendererOutput;
    QPainterVideoSurface surface;
};

/*!
    \class QVideoRendererWidget
    \preliminary
    \internal
    \brief The QVideoRendererWidget class provides video renderer based media output widget.
*/

/*!
    Constructs a new media widget that a \a renderer outputs to.
*/
QVideoRendererWidget::QVideoRendererWidget(QVideoRendererEndpoint *renderer)
    : QMediaWidgetEndpoint(*new QVideoRendererWidgetPrivate, 0)
{
    Q_D(QVideoRendererWidget);

    d->rendererOutput = renderer;

    if (d->rendererOutput)
        d->rendererOutput->setSurface(&d->surface);

    connect(&d->surface, SIGNAL(frameChanged()), this, SLOT(update()));
}

/*!
    Destroys a media output widget.
*/
QVideoRendererWidget::~QVideoRendererWidget()
{
    Q_D(QVideoRendererWidget);

    delete d->rendererOutput;
}

/*!
    Returns the renderer that renders to a media output widget.
*/
QVideoRendererEndpoint *QVideoRendererWidget::rendererOutput() const
{
    return d_func()->rendererOutput;
}

/*!
    \reimp
*/
void QVideoRendererWidget::setFullscreen(bool fullscreen)
{
    Q_UNUSED(fullscreen);
}

/*!
    Handles a paint event.
*/
void QVideoRendererWidget::paintEvent(QPaintEvent *)
{
    Q_D(QVideoRendererWidget);

    QPainter painter(this);

    if (d->surface.isStarted()) {
        d->surface.paint(&painter, rect());

        d->surface.setReady(true);
    }
}

/*!
    Handles a resize \a event.
*/
void QVideoRendererWidget::resizeEvent(QResizeEvent *event)
{
    QMediaWidgetEndpoint::resizeEvent(event);
}

#endif
