#ifndef V4LVIDEORENDERER_H
#define V4LVIDEORENDERER_H

#include <qvideorenderercontrol.h>
#include "v4lcamerasession.h"

class CameraFormatConverter;

class V4LVideoRendererControl : public QVideoRendererControl
{
    Q_OBJECT
public:
    V4LVideoRendererControl(V4LCameraSession* session, QObject *parent = 0);
    ~V4LVideoRendererControl();

    QAbstractVideoSurface *surface() const;
    void setSurface(QAbstractVideoSurface *surface);

    void setSession(V4LCameraSession* session);

private:
    QAbstractVideoSurface* m_surface;
    V4LCameraSession* m_session;
    CameraFormatConverter* converter;
};

#endif // V4LVIDEORENDERER_H
