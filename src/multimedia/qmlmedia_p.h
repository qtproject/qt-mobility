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

#ifndef QMLMEDIA_P_H
#define QMLMEDIA_P_H

#include <QtCore/qobject.h>
#include <QtDeclarative/qml.h>

#include <qmediaplayer.h>

QTM_BEGIN_NAMESPACE

class QmlMedia : public QObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(Status)
    Q_ENUMS(Error)
public:
    enum State
    {
        Stopped = QMediaPlayer::StoppedState,
        Playing = QMediaPlayer::PlayingState,
        Paused  = QMediaPlayer::PausedState
    };

    enum Status
    {
        UnknownStatus = QMediaPlayer::UnknownMediaStatus,
        NoMedia       = QMediaPlayer::NoMedia,
        Loading       = QMediaPlayer::LoadingMedia,
        Loaded        = QMediaPlayer::LoadedMedia,
        Stalled       = QMediaPlayer::StalledMedia,
        Buffering     = QMediaPlayer::BufferingMedia,
        Buffered      = QMediaPlayer::BufferedMedia,
        EndOfMedia    = QMediaPlayer::EndOfMedia,
        InvalidMedia  = QMediaPlayer::InvalidMedia
    };

    enum Error
    {
        NoError        = QMediaPlayer::NoError,
        ResourceError  = QMediaPlayer::ResourceError,
        FormatError    = QMediaPlayer::FormatError,
        NetworkError   = QMediaPlayer::NetworkError,
        AccessDenied   = QMediaPlayer::AccessDeniedError,
        ServiceMissing = QMediaPlayer::ServiceMissingError
    };
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QmlMedia))

#endif
