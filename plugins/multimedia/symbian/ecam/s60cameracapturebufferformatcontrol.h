/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef S60CAMERACAPTUREBUFFERFORMATCONTROL_H
#define S60CAMERACAPTUREBUFFERFORMATCONTROL_H

#include <qcameracapturebufferformatcontrol.h>

QT_USE_NAMESPACE

QT_FORWARD_DECLARE_CLASS(S60CameraService)
QT_FORWARD_DECLARE_CLASS(S60ImageCaptureSession)

/*
 * Control to setup buffer image capture format.
 */
class S60CameraCaptureBufferFormatControl : public QCameraCaptureBufferFormatControl
{
    Q_OBJECT

public: // Constructors & Destructor

    S60CameraCaptureBufferFormatControl(QObject *parent = 0);
    S60CameraCaptureBufferFormatControl(S60ImageCaptureSession *session, QObject *parent = 0);
    ~S60CameraCaptureBufferFormatControl();

public: // QCameraCaptureBufferFormatControl

    // Image Buffer Format
    QList<QVideoFrame::PixelFormat> supportedBufferFormats() const;
    QVideoFrame::PixelFormat bufferFormat() const;
    void setBufferFormat(const QVideoFrame::PixelFormat format);

/*
Q_SIGNALS: // QCameraCaptureBufferFormatControl

    void bufferFormatChanged(QVideoFrame::PixelFormat);
*/

private slots: // Internal

    void bufferFormatChanged(const QVideoFrame::PixelFormat format);

private: // Data

    S60ImageCaptureSession          *m_session;
    S60CameraService                *m_service;
    QVideoFrame::PixelFormat        m_bufferCaptureFormat;
};

#endif // S60CAMERACAPTUREBUFFERFORMATCONTROL_H
