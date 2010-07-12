/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "camerabinimagecapture.h"
#include <QtCore/QDebug>

CameraBinImageCapture::CameraBinImageCapture(CameraBinSession *session)
    :QCameraImageCaptureControl(session), m_session(session), m_ready(false), requestId(0)
{
    connect(m_session, SIGNAL(stateChanged(CameraBinSession::State)), SLOT(updateState()));
    connect(m_session, SIGNAL(imageExposed(int)), this, SIGNAL(imageExposed(int)));
    connect(m_session, SIGNAL(imageCaptured(int,QImage)), this, SIGNAL(imageCaptured(int,QImage)));
    connect(m_session, SIGNAL(imageSaved(int,QString)), this, SIGNAL(imageSaved(int,QString)));
}

CameraBinImageCapture::~CameraBinImageCapture()
{
}

bool CameraBinImageCapture::isReadyForCapture() const
{
    return m_ready;
}

int CameraBinImageCapture::capture(const QString &fileName)
{
    requestId++;
    m_session->captureImage(requestId, fileName);

    return requestId;
}

void CameraBinImageCapture::cancelCapture()
{
}

void CameraBinImageCapture::updateState()
{
    bool ready = m_session->state() == QCamera::ActiveState;
    if (m_ready != ready) {
        emit readyForCaptureChanged(m_ready = ready);
    }
}
