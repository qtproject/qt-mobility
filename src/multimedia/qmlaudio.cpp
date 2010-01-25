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

#include "qmlaudio_p.h"

#include "qmediaplayercontrol.h"

QML_DEFINE_TYPE(Qt,4,6,Audio,QTM_PREPEND_NAMESPACE(QmlAudio));

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Audio QmlAudio
    \brief The Video element allows you to add audio to a scene.
    \inherits Item
*/

/*!
    \internal
    \class QmlAudio
    \brief The QmlAudio class provides a audio item that you can add to a QmlView.
*/


QmlAudio::QmlAudio(QObject *parent)
    : QObject(parent)
{
    setObject(this);
}

QmlAudio::~QmlAudio()
{
    shutdown();
}

/*!
    \qmlmethod Audio::play()
*/

void QmlAudio::play()
{
    m_playerControl->play();
}

/*!
    \qmlmethod Audio::pause()
*/

void QmlAudio::pause()
{
    m_playerControl->pause();
}

/*!
    \qmlmethod Audio::stop()
*/

void QmlAudio::stop()
{
    m_playerControl->stop();
}

/*!
    \qmlproperty url Audio::source
*/

/*!
    \qmlproperty enum Audio::state
*/

/*!
    \qmlproperty enum Audio::status
*/

/*!
    \qmlproperty int Audio::duration
*/

/*!
    \qmlproperty int Audio::position
*/

/*!
    \qmlproperty real Audio::volume
*/

/*!
    \qmlproperty bool Audio::muted
*/

/*!
    \qmlproperty int Audio::bufferStatus
*/

/*!
    \qmlproperty bool Audio::seekable
*/

/*!
    \qmlproperty qreal playbackRate
*/

/*!
    \qmlproperty enum Audio::error
*/

/*!
    \qmlproperty enum Audio::errorString
*/

#include "moc_qmlaudio_p.cpp"

QTM_END_NAMESPACE

