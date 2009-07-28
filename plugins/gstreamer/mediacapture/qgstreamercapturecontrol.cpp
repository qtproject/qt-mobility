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

#include "qgstreamercapturecontrol.h"
#include <QtCore/QDebug>

QGstreamerCaptureControl::QGstreamerCaptureControl(QGstreamerCaptureSession *session)
    :QMediaCaptureControl(session), m_session(session)
{
    connect(m_session, SIGNAL(stateChanged(QGstreamerCaptureSession::State)), SLOT(updateState()));
    connect(m_session, SIGNAL(error(int,QString)), SIGNAL(error(int,QString)));
    connect(m_session, SIGNAL(positionChanged(qint64)), SIGNAL(positionChanged(qint64)));
}

QGstreamerCaptureControl::~QGstreamerCaptureControl()
{
}

QMediaSink QGstreamerCaptureControl::sink() const
{
    return m_session->sink();
}

bool QGstreamerCaptureControl::setSink(const QMediaSink &sink)
{
    m_session->setSink(sink);
    return true;
}


int QGstreamerCaptureControl::state() const
{
    switch ( m_session->state() ) {
        case QGstreamerCaptureSession::RecordingState:
            return int(QMediaCapture::RecordingState);
        case QGstreamerCaptureSession::PausedState:
            return int(QMediaCapture::PausedState);
        case QGstreamerCaptureSession::PreviewState:
        case QGstreamerCaptureSession::StoppedState:
            return int(QMediaCapture::StoppedState);
    }

    return QGstreamerCaptureSession::StoppedState;

}

void QGstreamerCaptureControl::updateState()
{
    emit stateChanged(state());
}

qint64 QGstreamerCaptureControl::position() const
{
    qDebug() << "position asked" <<m_session->position();
    return m_session->position();
}

void QGstreamerCaptureControl::record()
{
    m_session->dumpGraph("before-record");
    m_session->setState(QGstreamerCaptureSession::RecordingState);
    m_session->dumpGraph("after-record");
}

void QGstreamerCaptureControl::pause()
{
    m_session->dumpGraph("before-pause");
    m_session->setState(QGstreamerCaptureSession::PausedState);
}

void QGstreamerCaptureControl::stop()
{
    m_session->setState(QGstreamerCaptureSession::PreviewState);
}
