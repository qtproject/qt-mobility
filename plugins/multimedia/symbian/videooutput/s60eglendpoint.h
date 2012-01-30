/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
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

#ifndef S60EGLENDPOINT_H
#define S60EGLENDPOINT_H

#include <QtCore/QObject>
#include <egl/eglext.h>

QT_USE_NAMESPACE

class QTime;
class S60EglEndpointEventHandler;
class S60EglExtensions;
class TSurfaceId;

class S60EglEndpoint : public QObject
{
    Q_OBJECT

public:
    S60EglEndpoint(const TSurfaceId &surface, S60EglExtensions *extensions,
                   QObject *parent);
    ~S60EglEndpoint();

    bool isValid() const;
    void setDelay(qint64 delay);
    EGLImageKHR acquireImage();
    void releaseImage();

signals:
    void imageAvailable();

private:
    void requestImage();
    void endpointEvent();

private:
    friend class S60EglEndpointEventHandler;
    S60EglExtensions *m_extensions;
    EGLContext m_context;
    EGLenum m_api;
    EGLDisplay m_display;
    EGLEndpointNOK m_endpoint;
    QScopedPointer<S60EglEndpointEventHandler> m_eventHandler;
    enum State {
        Null,
        Created,
        Active
    } m_state;
    EGLImageKHR m_image;
};

#endif // S60EGLENDPOINT_H

