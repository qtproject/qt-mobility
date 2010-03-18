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

#ifndef S60MEDIACONTROL_H
#define S60MEDIACONTROL_H

#include <QtCore/qobject.h>
#include <QUrl>

#include "qmediarecorder.h"
#include "qmediarecordercontrol.h"
//#include "qcameracontrol.h"
#include "s60videocapturesession.h"

QTM_USE_NAMESPACE

class S60VideoCaptureSession;

class S60MediaControl : public QMediaRecorderControl
{
    Q_OBJECT
public:    
    S60MediaControl(QObject *session, QObject *parent = 0);
    ~S60MediaControl();

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl &sink);

    QMediaRecorder::State state() const;

    qint64 duration() const;

    void applySettings();        

private:
    QMediaRecorder::State convertState(S60VideoCaptureSession::TVideoCaptureState aState) const;

public slots:
    void record();
    void pause();
    void stop();

private slots:
    void updateState(S60VideoCaptureSession::TVideoCaptureState aState);
    
private:
    S60VideoCaptureSession* m_session;
    QMediaRecorder::State m_state;
};

#endif
