/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtMultimedia module of the Qt Toolkit.
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
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef RECORDER_H
#define RECORDER_H

#include <QtGui>

#ifdef AUDIOSERVICES
#include <QtMultimedia/qaudio.h>
#include <QtMultimedia/qaudioinput.h>
#endif

#include <qmediacapture.h>

class QComboBox;
class QLabel;
class QAudioDeviceEndpoint;
class QAudioCapturePropertiesControl;

class Recorder : public QMainWindow
{
    Q_OBJECT
public:
    Recorder();
    ~Recorder();

private slots:
    void status();
    void toggleRecord();
    void deviceChanged(int idx);
    void codecChanged(int idx);

private:
    QMediaCapture* audioCapture;
    QAudioDeviceEndpoint *audioDevice;
    QAudioCapturePropertiesControl *captureProperties;

    QComboBox*     deviceBox;
    QPushButton*   button;
    QLabel*        recTime;
    QFile*         file;
    bool           active;
    int            currentTime;
};

#endif
