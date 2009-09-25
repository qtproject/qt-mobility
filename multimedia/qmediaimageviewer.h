/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
**
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
** Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIAIMAGEVIEWER_H
#define QMEDIAIMAGEVIEWER_H

#include <multimedia/qabstractmediaobject.h>
#include <multimedia/qmediasource.h>

class QMediaImageViewerPrivate;

class Q_MEDIA_EXPORT QMediaImageViewer : public QAbstractMediaObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(MediaStatus mediaStatus READ mediaStatus NOTIFY mediaStatusChanged)
    Q_PROPERTY(QMediaSource media READ media WRITE setMedia NOTIFY mediaChanged)
    Q_PROPERTY(QMediaResource currentMedia READ currentMedia NOTIFY currentMediaChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_ENUMS(State MediaStatus)
public:
    enum State
    {
        StoppedState,
        PlayingState,
        PausedState
    };

    enum MediaStatus
    {
        NoMedia,
        LoadingMedia,
        LoadedMedia,
        InvalidMedia
    };

    explicit QMediaImageViewer(QObject *parent = 0);
    ~QMediaImageViewer();

    State state() const;
    MediaStatus mediaStatus() const;

    QMediaSource media() const;
    QMediaResource currentMedia() const;

    int timeout() const;

    void bind(QObject *);

public Q_SLOTS:
    void setMedia(const QMediaSource &media);

    void play();
    void pause();
    void stop();

    void setTimeout(int timeout);

Q_SIGNALS:
    void stateChanged(QMediaImageViewer::State state);
    void mediaStatusChanged(QMediaImageViewer::MediaStatus status);
    void mediaChanged(const QMediaSource &media);
    void currentMediaChanged(const QMediaResource &media);

protected:
    void timerEvent(QTimerEvent *event);

private:
    Q_DECLARE_PRIVATE(QMediaImageViewer)
    Q_PRIVATE_SLOT(d_func(), void _q_playlistMediaChanged(const QMediaSource &))
    Q_PRIVATE_SLOT(d_func(), void _q_playlistDestroyed(QObject *))
};

#endif
