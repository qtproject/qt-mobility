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

    GstElement *videoSink();
    void precessNewStream() {}

private:    
    GstElement *m_videoSink;
};

#endif

#endif // QGSTREAMERVIDEORENDRER_H
