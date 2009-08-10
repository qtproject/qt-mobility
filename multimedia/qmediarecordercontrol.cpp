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

#include "qmediarecordercontrol.h"


/*!
    \class QMediaRecorderControl
*/

/*!
    Construct a media recorder control.
*/

QMediaRecorderControl::QMediaRecorderControl(QObject* parent)
    : QAbstractMediaControl(parent)
{
}

/*!
    Destruct media recorder control object.
*/

QMediaRecorderControl::~QMediaRecorderControl()
{
}

/*!
    \fn QMediaSink QMediaRecorderControl::sink() const

    Return the current sink being used.
*/

/*!
    \fn bool QMediaRecorderControl::setSink(const QMediaSink &sink)

    Returns true if set of sink being used to \a sink is successful.
*/

/*!
    \fn int QMediaRecorderControl::state() const

    Return the current state.
*/

/*!
    \property position
    \brief Current position in milliseconds.
*/

/*!
    \fn qint64 QMediaRecorderControl::position() const

    Return the current position in milliseconds.
*/

/*!
    \fn void QMediaRecorderControl::record()

    Start recording.
*/

/*!
    \fn void QMediaRecorderControl::pause()

    Pause recording.
*/

/*!
    \fn void QMediaRecorderControl::stop()

    Stop recording.
*/

/*!
    \fn void QMediaRecorderControl::stateChanged(int state)

    Signal emitted when \a state changed.
*/

/*!
    \fn void QMediaRecorderControl::positionChanged(qint64 position)

    Signal emitted when \a position changed.
*/

/*!
    \fn void QMediaRecorderControl::error(int error, const QString &errorString)

    Signal emitted when \a error, \a errorString changed.
*/





