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
    \reimp
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
    \reimp
*/
void QVideoRendererWidget::resizeEvent(QResizeEvent *event)
{
    QMediaWidgetEndpoint::resizeEvent(event);
}

#endif
