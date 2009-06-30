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
#include "qwidgetmediaoutput_p.h"

#ifndef QT_NO_VIDEOSURFACE

#include "qimagemediaoutput.h"
#include "qpaintervideosurface_p.h"
#include "qvideorenderermediaoutput.h"

#include <qpainter.h>

class QVideoRendererWidgetPrivate : public QWidgetMediaOutputPrivate
{
    Q_DECLARE_PUBLIC(QVideoRendererWidget)
public:
    QVideoRendererWidgetPrivate()
        : imageOutput(0)
        , rendererOutput(0)
    {
    }

    void _q_imageChanged(const QImage &image);

    QImageMediaOutput *imageOutput;
    QVideoRendererMediaOutput *rendererOutput;
    QPainterVideoSurface surface;
};

void QVideoRendererWidgetPrivate::_q_imageChanged(const QImage &)
{
    if (surface.isStarted())
        q_func()->update();
}

/*!
    \class QVideoRendererWidget
    \preliminary
    \internal
    \brief The QVideoRendererWidget class provides video renderer based media output widget.
*/

/*!
    Constructs a new media output widget with the given \a parent.
*/
QVideoRendererWidget::QVideoRendererWidget(QWidget *parent)
    : QWidgetMediaOutput(*new QVideoRendererWidgetPrivate, parent)
{
    Q_D(QVideoRendererWidget);

    connect(&d->surface, SIGNAL(frameChanged()), this, SLOT(update()));
}

/*!
    Destroys a media output widget.
*/
QVideoRendererWidget::~QVideoRendererWidget()
{
}

/*!
    Returns the renderer that renders to a media output widget.
*/
QVideoRendererMediaOutput *QVideoRendererWidget::rendererOutput() const
{
    return d_func()->rendererOutput;
}

/*!
    Sets the \a renderer that renders to a media output widget.
*/
void QVideoRendererWidget::setRendererOutput(QVideoRendererMediaOutput *output)
{
    Q_D(QVideoRendererWidget);

    if (d->rendererOutput)
        d->rendererOutput->setSurface(0);

    d->rendererOutput = output;

    if (d->rendererOutput)
        d->rendererOutput->setSurface(&d->surface);
}

/*!
    Returns the image output.
*/
QImageMediaOutput *QVideoRendererWidget::imageOutput() const
{
    return d_func()->imageOutput;
}

/*!
    Sets the image \a output.
*/
void QVideoRendererWidget::setImageOutput(QImageMediaOutput *output)
{
    Q_D(QVideoRendererWidget);

    if (d->imageOutput) {
        disconnect(d->imageOutput, SIGNAL(imageChanged(QImage)),
                this, SLOT(_q_imageChanged(QImage)));
    }

    d->imageOutput = output;

    if (d->imageOutput) {
        connect(d->imageOutput, SIGNAL(imageChanged(QImage)),
                this, SLOT(_q_imageChanged(QImage)));
    }
}

/*!
    \reimp
*/
void QVideoRendererWidget::setFullscreen(bool fullscreen)
{
    Q_UNUSED(fullscreen);
}

/*!
    \reimp
*/
void QVideoRendererWidget::paintEvent(QPaintEvent *)
{
    Q_D(QVideoRendererWidget);

    QPainter painter(this);

    if (d->surface.isStarted()) {
        d->surface.paint(&painter, rect());

        d->surface.setReady(true);
    } else if (d->imageOutput && d->imageOutput->hasImage()) {
        painter.drawImage(rect(), d->imageOutput->image());
    }
}

/*!
    \reimp
*/
void QVideoRendererWidget::resizeEvent(QResizeEvent *event)
{
    QWidgetMediaOutput::resizeEvent(event);
}

#include "moc_qvideorendererwidget_p.cpp"

#endif
