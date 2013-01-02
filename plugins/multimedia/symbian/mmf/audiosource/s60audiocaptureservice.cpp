/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60audiocaptureservice.h"
#include "s60audiocapturesession.h"
#include "s60audioendpointselector.h"
#include "s60audioencodercontrol.h"
#include "s60audiomediarecordercontrol.h"
#include "s60audiocontainercontrol.h"
#include "s60mmtrace.h"

S60AudioCaptureService::S60AudioCaptureService(QObject *parent)
    : QMediaService(parent)
{
    TRACE("S60AudioCaptureService::S60AudioCaptureService" << qtThisPtr());
    m_session = new S60AudioCaptureSession(this);
    m_encoderControl = new S60AudioEncoderControl(m_session, this);
    m_recorderControl = new S60AudioMediaRecorderControl(m_session, this);
    m_endpointSelector = new S60AudioEndpointSelector(m_session, this);
    m_containerControl = new S60AudioContainerControl(m_session, this);
}

S60AudioCaptureService::~S60AudioCaptureService()
{
    TRACE("S60AudioCaptureService::~S60AudioCaptureService" << qtThisPtr());
}

QMediaControl *S60AudioCaptureService::requestControl(const char *name)
{
    QMediaControl *result = 0;
    if (qstrcmp(name,QMediaRecorderControl_iid) == 0)
        result = m_recorderControl;
    if (qstrcmp(name,QAudioEncoderControl_iid) == 0)
        result =  m_encoderControl;
    if (qstrcmp(name,QAudioEndpointSelector_iid) == 0)
        result =  m_endpointSelector;
    if (qstrcmp(name,QMediaContainerControl_iid) == 0)
        result =  m_containerControl;
    TRACE("S60AudioCaptureService::requestControl" << qtThisPtr()
          << "name" << name << "result" << result);
    return result;
}

void S60AudioCaptureService::releaseControl(QMediaControl *control)
{
    TRACE("S60AudioCaptureService::releaseControl" << qtThisPtr()
          << "control" << control);
    Q_UNUSED(control)
}
