#ifndef QGSTREAMERVIDEORENDERER_H
#define QGSTREAMERVIDEORENDERER_H

#ifndef QT_NO_VIDEOSURFACE

#include "qvideorenderercontrol.h"
#include "qvideosurfacegstsink.h"
#include "qgstreamerplayersession.h"

class QGstreamerVideoRenderer : public QVideoRendererControl, public QGstreamerVideoRendererInterface
{
    Q_OBJECT
    Q_INTERFACES(QGstreamerVideoRendererInterface)
public:
    QGstreamerVideoRenderer(QObject *parent = 0);
    virtual ~QGstreamerVideoRenderer();
    
    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    GstElement *videoSink();
    void precessNewStream() {}

private:    
    GstElement *m_videoSink;
    QAbstractVideoSurface *m_surface;
};

#endif

#endif // QGSTREAMERVIDEORENDRER_H
