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

#ifndef QIMAGEPROCESSINGCONTROL_H
#define QIMAGEPROCESSINGCONTROL_H

#include <qmediacontrol.h>
#include <qmediaobject.h>

#include <experimental/qcamera.h>

QTM_BEGIN_NAMESPACE

class Q_MEDIA_EXPORT QImageProcessingControl : public QMediaControl
{
    Q_OBJECT

public:
    ~QImageProcessingControl();

    virtual QCamera::WhiteBalanceMode whiteBalanceMode() const = 0;
    virtual void setWhiteBalanceMode(QCamera::WhiteBalanceMode mode) = 0;
    virtual QCamera::WhiteBalanceModes supportedWhiteBalanceModes() const = 0;
    virtual int manualWhiteBalance() const = 0;
    virtual void setManualWhiteBalance(int colorTemperature) = 0;

    virtual int contrast() const = 0;
    virtual void setContrast(int value) = 0;

    virtual int saturation() const = 0;
    virtual void setSaturation(int value) = 0;

    virtual bool isSharpeningSupported() const = 0;
    virtual int sharpeningLevel() const = 0;
    virtual void setSharpeningLevel(int value) = 0;

    virtual bool isDenoisingSupported() const = 0;
    virtual int denoisingLevel() const = 0;
    virtual void setDenoisingLevel(int value) = 0;

    virtual bool isWhiteBalanceLocked() const = 0;

public Q_SLOTS:
    virtual void lockWhiteBalance() = 0;
    virtual void unlockWhiteBalance() = 0;

Q_SIGNALS:
    void whiteBalanceLocked();

protected:
    QImageProcessingControl(QObject* parent = 0);
};

#define QImageProcessingControl_iid "com.nokia.Qt.QImageProcessingControl/1.0"
Q_MEDIA_DECLARE_CONTROL(QImageProcessingControl, QImageProcessingControl_iid)

QTM_END_NAMESPACE

#endif

