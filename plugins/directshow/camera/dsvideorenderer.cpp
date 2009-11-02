#include "dsvideorenderer.h"

#include <QtCore/qdebug.h>

DSVideoRendererControl::DSVideoRendererControl(DSCameraSession* session, QObject *parent)
    :QVideoRendererControl(parent),
    m_surface(0),
    m_session(session)
{
}

DSVideoRendererControl::~DSVideoRendererControl()
{
}

QAbstractVideoSurface* DSVideoRendererControl::surface() const
{
    return m_surface;
}

void DSVideoRendererControl::setSurface(QAbstractVideoSurface *surface)
{
    m_surface = surface;
    if(m_session)
        m_session->setSurface(m_surface);
}
