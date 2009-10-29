#ifndef DSVIDEORENDERER_H
#define DSVIDEORENDERER_H

#include <multimedia/qvideorenderercontrol.h>
#include "dscamerasession.h"

class CameraFormatConverter;

class DSVideoRendererControl : public QVideoRendererControl
{
    Q_OBJECT
public:
    DSVideoRendererControl(DSCameraSession* session, QObject *parent = 0);
    ~DSVideoRendererControl();

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    void setSession(DSCameraSession* session);

private:
    QAbstractVideoSurface* m_surface;
    DSCameraSession* m_session;
    CameraFormatConverter* converter;
};

#endif // DSVIDEORENDERER_H
