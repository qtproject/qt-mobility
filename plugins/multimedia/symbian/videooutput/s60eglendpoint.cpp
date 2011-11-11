/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** qtThisPtr() file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60eglendpoint.h"
#include "s60eglextensions.h"
#include "s60mmtrace.h"
#include <e32base.h>
#include <egl/egl.h>
#include <graphics/surface.h>
#include <QtCore/QDebug>
#include <QtCore/QTime>

//-----------------------------------------------------------------------------
// S60EglEndpointEventHandler
//-----------------------------------------------------------------------------

class S60EglEndpointEventHandler : public CActive
{
public:
    static S60EglEndpointEventHandler *NewL(S60EglEndpoint *parent);
    ~S60EglEndpointEventHandler();
    void request();

private:
    S60EglEndpointEventHandler(S60EglEndpoint *parent);
    void RunL();
    void DoCancel();

private:
    S60EglEndpoint *const m_parent;
};

S60EglEndpointEventHandler *S60EglEndpointEventHandler::NewL(S60EglEndpoint *parent)
{
    S60EglEndpointEventHandler *self = new (ELeave) S60EglEndpointEventHandler(parent);
    CActiveScheduler::Add(self);
    return self;
}

S60EglEndpointEventHandler::S60EglEndpointEventHandler(S60EglEndpoint *parent)
    : CActive(EPriorityStandard)
    , m_parent(parent)
{

}

S60EglEndpointEventHandler::~S60EglEndpointEventHandler()
{
    Cancel();
}

void S60EglEndpointEventHandler::request()
{
    TRACE("S60EglEndpointEventHandler::request" << qtThisPtr());
    Q_ASSERT(!IsActive());
    iStatus = KRequestPending;
    EGLTRequestStatusNOK eglStatus = reinterpret_cast<EGLTRequestStatusNOK>(&iStatus);
    const EGLBoolean ret = m_parent->m_extensions->endpointRequestNotification(m_parent->m_display,
                                                                               m_parent->m_endpoint,
                                                                               eglStatus);
    if (ret) {
        VERBOSE_TRACE("S60EglEndpointEventHandler::request" << qtThisPtr() << "endpointRequestNotification OK");
        SetActive();
    } else {
        qWarning() << "S60EglEndpointEventHandler::request" << qtThisPtr() << "endpointRequestNotification failed";
    }
}

void S60EglEndpointEventHandler::RunL()
{
    VERBOSE_TRACE("S60EglEndpointEventHandler::RunL" << qtThisPtr());
    m_parent->endpointEvent();
}

void S60EglEndpointEventHandler::DoCancel()
{
    TRACE("S60EglEndpointEventHandler::DoCancel" << qtThisPtr());
    m_parent->m_extensions->endpointCancelNotification(m_parent->m_display,
                                                       m_parent->m_endpoint);
}

//-----------------------------------------------------------------------------
// S60EglEndpoint
//-----------------------------------------------------------------------------

S60EglEndpoint::S60EglEndpoint(const TSurfaceId &surface,
                               S60EglExtensions *extensions,
                               QObject *parent)
    : QObject(parent)
    , m_extensions(extensions)
    , m_context(eglGetCurrentContext())
    , m_api(0)
    , m_display(eglGetCurrentDisplay())
    , m_endpoint(EGL_NO_ENDPOINT_NOK)
    , m_state(Null)
{
    TRACE("S60EglEndpoint::S60EglEndpoint" << qtThisPtr()
          << "surface" << (void*)surface.iInternal[3]
          << "context" << m_context << "display" << m_display);
    Q_ASSERT(m_extensions);
    bool ok = false;
    if (m_context) {
        m_api = eglQueryAPI();
        if (m_api) {
            m_display = eglGetCurrentDisplay();
            if (EGL_NO_DISPLAY != m_display) {
                ok = true;
            } else {
                qWarning() << "S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "failed to get valid EGL display";
            }
        } else {
            qWarning() << "S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "failed to get valid rendering API";
        }
    } else {
        qWarning() << "S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "failed to get valid EGL context";
    }
    if (ok) {
        static const EGLenum type = EGL_ENDPOINT_TYPE_CONSUMER_NOK;
        static const EGLenum sourceType = EGL_TSURFACEID_NOK;
        TSurfaceId localSurface = surface;
        EGLEndpointSourceNOK source = reinterpret_cast<EGLEndpointSourceNOK>(&localSurface);
        static const EGLint *attributes = 0;
        m_endpoint = m_extensions->createEndpoint(m_display, type, sourceType, source, attributes);
        const EGLint error = eglGetError();
        if (EGL_SUCCESS == error) {
            TRACE("S60EglEndpoint::S60EglEndpoint" << qtThisPtr()
                  << "endpoint" << m_endpoint);
            if (EGL_NO_ENDPOINT_NOK == m_endpoint) {
                qWarning() << "S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "failed to create EGL endpoint";
            } else {
                TRAPD(err, m_eventHandler.reset(S60EglEndpointEventHandler::NewL(this)));
                if (err) {
                    qWarning() << "S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "failed to create EGL endpoint event handler: error" << err;
                } else {
                    m_state = Created;
                    EGLBoolean ready = m_extensions->getEndpointAttrib(m_display, m_endpoint, EGL_ENDPOINT_READY_NOK);
                    TRACE("S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "ready" << ready);
                    if (ready) {
                        // Producer has already submitted content to the source surface
                        m_state = Active;
                        m_eventHandler->request();
                    } else {
                        // Request notification when producer has submitted content
                        m_eventHandler->request();
                    }
                }
            }
        } else {
            qWarning() << "S60EglEndpoint::S60EglEndpoint" << qtThisPtr() << "failed to create EGL endpoint: error" << error;
        }
    }
}

S60EglEndpoint::~S60EglEndpoint()
{
    TRACE("S60EglEndpoint::~S60EglEndpoint" << qtThisPtr());
    if (EGL_NO_ENDPOINT_NOK != m_endpoint)
        m_extensions->destroyEndpoint(m_display, m_endpoint);
}

void S60EglEndpoint::endpointEvent()
{
    const EGLBoolean ready = m_extensions->getEndpointAttrib(m_display, m_endpoint, EGL_ENDPOINT_READY_NOK);
    VERBOSE_TRACE("S60EglEndpoint::endpointEvent" << qtThisPtr() << "state" << m_state << "ready" << ready);
    switch (m_state) {
    case Created:
        m_state = Active;
    case Active:
        emit imageAvailable();
        break;
    case Null:
    default:
        Q_ASSERT(false);
        break;
    }
}

bool S60EglEndpoint::isValid() const
{
    return (m_state != Null);
}

void S60EglEndpoint::setDelay(qint64 delay)
{
    VERBOSE_TRACE("S60EglEndpoint::setDelay" << qtThisPtr() << "delay" << delay);
    m_extensions->setEndpointAttrib(m_display, m_endpoint, EGL_DELAY_NOK, delay);
}

EGLImageKHR S60EglEndpoint::acquireImage()
{
    Q_ASSERT(EGL_NO_IMAGE_KHR == m_image);
    m_image = m_extensions->acquireImage(m_display, m_endpoint);
    VERBOSE_TRACE("S60EglEndpoint::acquireImage" << qtThisPtr() << "image" << m_image);
    if (EGL_NO_IMAGE_KHR == m_image) {
        qWarning() << "S60EglEndpoint::endpointEvent" << qtThisPtr() << "null image";
        if (!m_eventHandler->IsActive())
            m_eventHandler->request();
    }
    return m_image;
}

void S60EglEndpoint::releaseImage()
{
    if (EGL_NO_IMAGE_KHR != m_image) {
        VERBOSE_TRACE("S60EglEndpoint::releaseImage" << qtThisPtr() << "image" << m_image);
        m_extensions->releaseImage(m_display, m_endpoint, m_image, m_api);
        m_image = EGL_NO_IMAGE_KHR;
        if (!m_eventHandler->IsActive())
            m_eventHandler->request();
    }
}

