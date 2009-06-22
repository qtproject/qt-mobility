#include "qvideorendererwidget_p.h"
#include "qwidgetmediaoutput_p.h"

#ifndef QT_NO_VIDEOSURFACE

#include "qimagemediaoutput.h"
#include "qvideorenderermediaoutput.h"

#include <qpainter.h>

class QVideoRendererWidgetPrivate : public QWidgetMediaOutputPrivate
{
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
        painter.drawImage(d->imageOutput->image(), rect());
    }
}

/*!
    \reimp
*/
void QVideoRendererWidget::resizeEvent(QResizeEvent *event)
{
    QWidgetMediaOutput::resizeEvent(event);
}

#endif
