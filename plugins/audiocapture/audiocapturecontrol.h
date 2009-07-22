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

#ifndef AUDIOCAPTURECONTROL_H
#define AUDIOCAPTURECONTROL_H

#include <QtCore/qobject.h>

#ifdef AUDIOSERVICES
#include <QtMultimedia/qaudioformat.h>
#endif

#include "qaudiocapturecontrol.h"

class AudioCaptureService;
class AudioCaptureSession;

class AudioCaptureControl : public QAudioCaptureControl
{
    Q_OBJECT
public:
    AudioCaptureControl(QObject *parent = 0);
    AudioCaptureControl(AudioCaptureService *service, QObject *parent = 0);
    ~AudioCaptureControl();

#ifdef AUDIOSERVICES
    QAudioFormat inputFormat() const;
    bool setInputFormat(const QAudioFormat &format);
    QAudioFormat outputFormat() const;
    bool setOutputFormat(const QAudioFormat &format);
#endif
    void setInputDevice(QIODevice *device);
    void setOutputDevice(QIODevice *device);

private:
    AudioCaptureService *m_service;
    AudioCaptureSession *m_session;

#ifdef AUDIOSERVICES
    QAudioFormat m_inputFormat;
    QAudioFormat m_outputFormat;;
#endif
};

#endif
