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

#ifndef RECORDER_H
#define RECORDER_H

#include <experimental/qcamera.h>
#include <qmediarecorder.h>
#include <experimental/qstillimagecapture.h>
#include <qaudiocapturesource.h>

#include "mediakeysobserver.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class CameraCapture;
}
QT_END_NAMESPACE


#include <QMainWindow>
#include <QDir>

QTM_BEGIN_NAMESPACE
class QVideoWidget;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class CameraCapture : public QMainWindow
{
    Q_OBJECT
public:
    CameraCapture(QWidget *parent = 0);
    ~CameraCapture();

private slots:
    void setCamera(const QByteArray &cameraDevice);

    void toggleCamera();

    void record();
    void pause();
    void stop();

    void takeImage();

    void settings();

    void displayErrorMessage();

    void updateCameraDevice(QAction*);
    void updateAudioDevice(QAction*);

    void updateCameraState(QCamera::State);
    void updateRecorderState(QMediaRecorder::State state);

    void updateRecordTime();
    void updateAudioDevices();

    void processCapturedImage(const QString& fname, const QImage& img);
    void focusLocked();
    void zoomValueChanged(qreal value);
    
    void handleMediaKeyEvent(MediaKeysObserver::MediaKeys key);

private:
    Ui::CameraCapture *ui;

    QDir outputDir;
    QCamera *camera;
    QStillImageCapture *imageCapture;
    QMediaRecorder* mediaRecorder;
    QAudioCaptureSource *audioSource;
    QVideoWidget *videoWidget;
    
    MediaKeysObserver *mediaKeysObserver;
};

#endif
