#include "v4lvideorenderer.h"

#include <QtCore/qdebug.h>

V4LVideoRendererControl::V4LVideoRendererControl(V4LCameraSession* session, QObject *parent)
    :QVideoRendererControl(parent),
    m_surface(0),
    m_session(session)
{
}

V4LVideoRendererControl::~V4LVideoRendererControl()
{
}

QAbstractVideoSurface* V4LVideoRendererControl::surface() const
{
    return m_surface;
}

void V4LVideoRendererControl::setSurface(QAbstractVideoSurface *surface)
{
    m_surface = surface;
    if(m_session)
        m_session->setSurface(m_surface);
}
