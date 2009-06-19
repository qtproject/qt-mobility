#include "qvideorendererwidget_p.h"
#include "qwidgetmediaoutput_p.h"

#ifndef QT_NO_VIDEOSURFACE

class QVideoRendererWidgetPrivate : public QWidgetMediaOutputPrivate
{
public:
    QVideoRendererWidgetPrivate()
        : renderer(0)
    {
    }

    QVideoRendererMediaOutput *renderer;
};

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
QVideoRendererMediaOutput *QVideoRendererWidget::renderer() const
{
    return d_func()->renderer;
}

/*!
    Sets the \a renderer that renders to a media output widget.
*/
void QVideoRendererWidget::setRenderer(QVideoRendererMediaOutput *renderer)
{
    d_func()->renderer = renderer;
}


/*!
    \reimp
*/
void QVideoRendererWidget::setFullscreen(bool fullscreen)
{
    Q_UNUSED(fullscreen);
}

#endif
