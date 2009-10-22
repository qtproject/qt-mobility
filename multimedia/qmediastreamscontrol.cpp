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

#include "qmediastreamscontrol.h"
#include "qmediacontrol_p.h"

/*!
    \class QMediaStreamsControl
    \preliminary
    \brief The QMediaStreamsControl class provides an media streams selector control.

    The QMediaStreamsControl class provides descriptions of the media streams available
    and allows one to be selected as the currently played.

    The interface name of QMediaStreamsControl is \c com.nokia.Qt.MediaStreamsControl as
    defined in QMediaStreamsControl_iid.
*/

/*!
    \macro QMediaStreamsControl_iid

    \c com.nokia.Qt.MediaStreamsControl

    Defines the interface name of QMediaStreamsControl.

    \relates QMediaStreamsControl
*/

/*!
    Constructs a new media streams control with the given \a parent.
*/
QMediaStreamsControl::QMediaStreamsControl(QObject *parent)
    :QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destroys a media streams control.
*/
QMediaStreamsControl::~QMediaStreamsControl()
{
}

/*!
  \enum QMediaStreamControl::StreamType

  Media stream type.

  \value QMediaStreamControl::AudioStream Audio stream.
  \value QMediaStreamControl::VideoStream Video stream.
  \value QMediaStreamControl::SubPictureStream Subpicture or teletext stream.
  \value QMediaStreamControl::UnknownStream The stream type is unknown.
*/

/*!
\fn QMediaStreamsControl::streamCount()

Returns the number of media streams.
*/

/*!
\fn QMediaStreamsControl::streamType(int stream)

Return the type of media \a stream.
*/

/*!
\fn QMediaStreamsControl::metaData(int stream, QtMedia::MetaData key)

Returns the metadata value associated with media \a stream.

Useful metadata keya are QtMedia::Title, QtMedia::Description and QtMedia::Language.
*/

/*!
\fn QMediaStreamsControl::isActive(int stream)

Returns true if the media \a stream is active.
*/

/*!
\fn QMediaStreamsControl::setActive(int stream, bool state)

Calling setActive(stream, true) activates the media stream,
and since in most cases playback of only one stream of certain
type is supported, currently active stream of the same type
will be disactivated.

Calling setActive(stream, false) with a currently active
stream passed turns off playback of corresponding stream type.
*/

/*!
\fn QMediaStreamsControl::streamsChanged()

The signal is emited when the available streams list is changed.
*/

/*!
\fn QMediaStreamsControl::activeStreamsChanged()

The signal is emited when the active streams list is changed.
*/
