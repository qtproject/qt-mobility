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

#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include "qabstractmediaobject.h"

#include "qmediaserviceprovider.h"
#include "qmediasink.h"


class QMediaRecorderService;
extern Q_MEDIA_EXPORT QAbstractMediaService *createMediaCaptureService(QMediaServiceProvider *provider = defaultServiceProvider("audiorecorder"));

class QMediaRecorderPrivate;

class Q_MEDIA_EXPORT QMediaRecorder : public QAbstractMediaObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)

public:
    enum State
    {
        StoppedState,
        RecordingState,
        PausedState
    };

    enum Error
    {
        NoError,
        ResourceError,
        FormatError
    };

    QMediaRecorder(QAbstractMediaService *service = createMediaCaptureService(), QObject *parent = 0);
    QMediaRecorder(QAbstractMediaObject *mediaObject, QObject *parent = 0);
    virtual ~QMediaRecorder();

    bool isValid() const;

    QAbstractMediaService* service() const;

    QMediaSink sink() const;
    bool setSink(const QMediaSink &sink);

    State state() const;

    Error error() const;
    QString errorString() const;
    void unsetError();

    qint64 duration() const;
    void setPositionUpdatePeriod(int ms);

public slots:
    void record();
    void pause();
    void stop();

signals:
    void stateChanged(QMediaRecorder::State state);
    void durationChanged(qint64 duration);
    void error(QMediaRecorder::Error error);
    void errorStringChanged(const QString &error);


private:
    Q_DISABLE_COPY(QMediaRecorder)
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QMediaRecorder::State))
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &));
};

#endif  // QMEDIARECORDER_H
