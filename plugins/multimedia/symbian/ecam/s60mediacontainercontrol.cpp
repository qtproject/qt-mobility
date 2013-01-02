/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60mediacontainercontrol.h"
#include "s60videocapturesession.h"
#include "s60videocapturesettings.h"
#include "s60cameraconstants.h"

using namespace S60CameraConstants;

S60MediaContainerControl::S60MediaContainerControl(QObject *parent):
    QMediaContainerControl(parent)
{
}

S60MediaContainerControl::S60MediaContainerControl(S60VideoCaptureSession *session, QObject *parent):
    QMediaContainerControl(parent)
{
    m_session = session;

    // Set default video container
    m_supportedContainers = m_session->settings()->supportedVideoContainers();

    if (!m_supportedContainers.isEmpty()) {
        // Check if default container is supported
        if (m_supportedContainers.indexOf(KMimeTypeDefaultContainer) != -1)
            setContainerMimeType(KMimeTypeDefaultContainer);
        // Otherwise use first in the list
        else
            setContainerMimeType(m_supportedContainers[0]); // First as default
    } else {
        m_session->setError(KErrGeneral, tr("No supported video containers found."));
    }
}

S60MediaContainerControl::~S60MediaContainerControl()
{
}

QStringList S60MediaContainerControl::supportedContainers() const
{
    return m_session->settings()->supportedVideoContainers();
}

QString S60MediaContainerControl::containerMimeType() const
{
    return m_session->settings()->videoContainer();
}

void S60MediaContainerControl::setContainerMimeType(const QString &containerMimeType)
{
    m_session->settings()->setVideoContainer(containerMimeType);
}

QString S60MediaContainerControl::containerDescription(const QString &containerMimeType) const
{
    return m_session->settings()->videoContainerDescription(containerMimeType);
}

// End of file
