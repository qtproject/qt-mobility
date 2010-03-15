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

#ifndef QGSTREAMERIMAGEPROCESSINGCONTROL_H
#define QGSTREAMERIMAGEPROCESSINGCONTROL_H

#include <experimental/qcamera.h>
#include <experimental/qimageprocessingcontrol.h>

#include <gst/gst.h>
#include <glib.h>

class QGstreamerCaptureSession;

QTM_USE_NAMESPACE

class QGstreamerImageProcessingControl : public QImageProcessingControl
{
    Q_OBJECT

public:
    QGstreamerImageProcessingControl(GstElement &camerabin, QGstreamerCaptureSession *session);
    virtual ~QGstreamerImageProcessingControl();

    QCamera::WhiteBalanceMode whiteBalanceMode() const;
    void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode);
    QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const;
    int manualWhiteBalance() const;
    void setManualWhiteBalance(int colorTemperature);

    int contrast() const;
    void setContrast(int value);

    int saturation() const;
    void setSaturation(int value);

    bool isSharpeningSupported() const;
    int sharpeningLevel() const;
    void setSharpeningLevel(int value);

    bool isDenoisingSupported() const;
    int denoisingLevel() const;
    void setDenoisingLevel(int value);

    bool isWhiteBalanceLocked() const;

    void lockWhiteBalance();
    void unlockWhiteBalance();

private:
    QGstreamerCaptureSession *m_session;
    GstElement &m_camerabin;
/*Q_SIGNALS:
    void whiteBalanceLocked();
    */
};

#endif // QGSTREAMERIMAGEPROCESSINGCONTROL_H
