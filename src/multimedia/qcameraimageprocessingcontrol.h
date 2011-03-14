/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCAMERAIMAGEPROCESSINGCONTROL_H
#define QCAMERAIMAGEPROCESSINGCONTROL_H

#include <qmediacontrol.h>
#include <qmediaobject.h>

#include <qcamera.h>

QT_BEGIN_NAMESPACE

class Q_MULTIMEDIA_EXPORT QCameraImageProcessingControl : public QMediaControl
{
    Q_OBJECT

public:
    ~QCameraImageProcessingControl();

    enum ProcessingParameter {
        Contrast = 0,
        Saturation = 1,
        Brightness = 2,
        Sharpening = 3,
        Denoising = 4,
        ColorTemperature = 5,
        ExtendedParameter = 1000
    };

    virtual QCameraImageProcessing::WhiteBalanceMode whiteBalanceMode() const = 0;
    virtual void setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode) = 0;
    virtual bool isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode) const = 0;

    virtual bool isProcessingParameterSupported(ProcessingParameter) const = 0;
    virtual QVariant processingParameter(ProcessingParameter parameter) const = 0;
    virtual void setProcessingParameter(ProcessingParameter parameter, QVariant value) = 0;

protected:
    QCameraImageProcessingControl(QObject* parent = 0);
};

#define QCameraImageProcessingControl_iid "com.nokia.Qt.QCameraImageProcessingControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QCameraImageProcessingControl, QCameraImageProcessingControl_iid)

QT_END_NAMESPACE

#endif

