#include "qvideorendererendpoint.h"

#include <private/qobject_p.h>

#ifndef QT_NO_VIDEOSURFACE

/*!
    \class QVideoRendererEndpointInterface
    \preliminary
    \internal
    \brief The QVideoRendererEndpointInterface class provides an interface for video renderer
    media end points.
*/

/*!
    Destroys a video renderer media output.
*/
QVideoRendererEndpointInterface::~QVideoRendererEndpointInterface()
{
}

class QVideoRendererEndpointPrivate : public QObjectPrivate
{
public:
    QVideoRendererEndpointPrivate()
        : surface(0)
    {
    }

    QAbstractVideoSurface *surface;
};

/*!
    \class QVideoRendererEndpoint
    \preliminary
    \brief The QVideoRendererEndpoint class provides a media end point that renders to a video
    surface.

    \note QVideoRendererEndpoint must be created by a media service and cannot be instantiated
    directly.

    \sa QAbstractVideoService::createEndpoint()
*/

/*!
    Constructs a new video renderer media end point.
*/
QVideoRendererEndpoint::QVideoRendererEndpoint(QObject *parent)
    : QObject(*new QVideoRendererEndpointPrivate, parent)
{
}

/*!
    Destroys a video renderer media end point.
*/
QVideoRendererEndpoint::~QVideoRendererEndpoint()
{
}

/*!
    Returns the video surface a renderer renders to.
*/
QAbstractVideoSurface *QVideoRendererEndpoint::surface() const
{
    return d_func()->surface;
}

/*!
    Sets the video \a surface a renderer renders to.
*/
void QVideoRendererEndpoint::setSurface(QAbstractVideoSurface *surface)
{
    d_func()->surface = surface;
}

#endif
