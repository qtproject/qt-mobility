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

#ifndef V4LCAMERASERVICE_H
#define V4LCAMERASERVICE_H

#include <QtCore/qobject.h>

#include <qmediaservice.h>
QTM_USE_NAMESPACE

class V4LCameraControl;
class V4LCameraSession;
class V4LVideoOutputControl;
class V4LVideoDeviceControl;
class V4LVideoRendererControl;
class V4LImageCaptureControl;
class V4LMediaContainerControl;
class V4LVideoEncode;
class V4LRecorderControl;

class V4LCameraService : public QMediaService
{
    Q_OBJECT

public:
    V4LCameraService(QObject *parent = 0);
    ~V4LCameraService();

    QMediaControl *control(const char *name) const;

private:
    V4LCameraControl        *m_control;
    V4LCameraSession        *m_session;
    V4LVideoOutputControl   *m_videoOutput;
    V4LVideoDeviceControl   *m_videoDevice;
    V4LVideoRendererControl *m_videoRenderer;
    V4LImageCaptureControl  *m_imageCapture;
    V4LMediaContainerControl   *m_formatControl;
    V4LVideoEncode          *m_videoEncode;
    V4LRecorderControl      *m_recorderControl;
    QByteArray m_device;
};

#endif
