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

#include "s60cameraimagecapturecontrol.h"
#include "s60cameraservice.h"
#include "s60imagecapturesession.h"

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>


S60CameraImageCaptureControl::S60CameraImageCaptureControl(QObject *parent)
    :QImageCaptureControl(parent)
{
}

S60CameraImageCaptureControl::S60CameraImageCaptureControl(QObject *session, QObject *parent)
   :QImageCaptureControl(parent)
{
    // use cast if we want to change session class later on..
    m_session = qobject_cast<S60ImageCaptureSession*>(session);
    // chain these signals from session class
    connect(m_session, SIGNAL(imageCaptured(QString,QImage)),
            this, SIGNAL(imageCaptured(QString,QImage)));
    connect(m_session, SIGNAL(readyForCaptureChanged(bool)),
            this, SIGNAL(readyForCaptureChanged(bool)));
    connect(m_session, SIGNAL(imageSaved(const QString&)),
        this, SIGNAL(imageSaved(const QString&)));
    connect(m_session, SIGNAL(error(int, const QString &)),
        this, SIGNAL(error(int, const QString &)));
    
}

S60CameraImageCaptureControl::~S60CameraImageCaptureControl()
{
}

bool S60CameraImageCaptureControl::isReadyForCapture() const
{
    if (m_session)
        return m_session->deviceReady();
    else
        return false;
}

void S60CameraImageCaptureControl::capture(const QString &fileName)
{
    if (m_session)
        m_session->capture(fileName);
}

void S60CameraImageCaptureControl::cancelCapture()
{
}
