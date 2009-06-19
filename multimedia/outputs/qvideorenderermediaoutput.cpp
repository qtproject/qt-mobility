#include "qvideorenderermediaoutput.h"

#include <private/qobject_p.h>

#ifndef QT_NO_VIDEOSURFACE

/*!
    \class QVideoRendererMediaOutputInterface
    \preliminary
    \internal
    \brief The QVideoRendererMediaOutputInterface class provides an interface for video renderer
    media outputs.
*/

/*!
    Destroys a video renderer media output.
*/
QVideoRendererMediaOutputInterface::~QVideoRendererMediaOutputInterface()
{
}

class QVideoRendererMediaOutputPrivate : public QObjectPrivate
{
public:
    QVideoRendererMediaOutputPrivate()
        : surface(0)
    {
    }

    QAbstractVideoSurface *surface;
};

/*!
    \class QVideoRendererMediaOutput
    \preliminary
    \brief The QVideoRendererMediaOutput class provides a media output that renders to a video
    surface.

    \note QVideoRendererMediaOutput must be created by a media object and cannot be instantiated
    directly.
*/

/*!
    Constructs a new video renderer media output.
*/
QVideoRendererMediaOutput::QVideoRendererMediaOutput(QObject *parent)
    : QObject(*new QVideoRendererMediaOutputPrivate, parent)
{
}

/*!
    Destroys a video renderer media output.
*/
QVideoRendererMediaOutput::~QVideoRendererMediaOutput()
{
}

/*!
    Returns the video surface a renderer renders to.
*/
QAbstractVideoSurface *QVideoRendererMediaOutput::surface() const
{
    return d_func()->surface;
}

/*!
    Sets the video \a surface a renderer renders to.
*/
void QVideoRendererMediaOutput::setSurface(QAbstractVideoSurface *surface)
{
    d_func()->surface = surface;
}

#endif
