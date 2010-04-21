/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include "qmediaobject.h"
#include "qmediaserviceprovider.h"
#include "qmediaencodersettings.h"

#include <QtCore/qpair.h>

QT_BEGIN_NAMESPACE
class QUrl;
class QSize;
class QAudioFormat;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QMediaRecorderService;
class QAudioEncoderSettings;
class QVideoEncoderSettings;

class QMediaRecorderPrivate;
class Q_MEDIA_EXPORT QMediaRecorder : public QMediaObject
{
    Q_OBJECT
    Q_ENUMS(State)
    Q_ENUMS(Error)    
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QUrl outputLocation READ outputLocation WRITE setOutputLocation)
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

    QMediaRecorder(QMediaObject *mediaObject, QObject *parent = 0);
    ~QMediaRecorder();

    bool isAvailable() const;
    QtMediaServices::AvailabilityError availabilityError() const;

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl &location);

    State state() const;

    Error error() const;
    QString errorString() const;

    qint64 duration() const;

    QStringList supportedContainers() const;
    QString containerDescription(const QString &containerMimeType) const;

    QStringList supportedAudioCodecs() const;
    QString audioCodecDescription(const QString &codecName) const;

    QList<int> supportedAudioSampleRates(const QAudioEncoderSettings &settings = QAudioEncoderSettings(),
                                         bool *continuous = 0) const;

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;

    QList<QSize> supportedResolutions(const QVideoEncoderSettings &settings = QVideoEncoderSettings(),
                                      bool *continuous = 0) const;

    QList<qreal> supportedFrameRates(const QVideoEncoderSettings &settings = QVideoEncoderSettings(),
                                     bool *continuous = 0) const;

    QAudioEncoderSettings audioSettings() const;
    QVideoEncoderSettings videoSettings() const;
    QString containerMimeType() const;

    void setEncodingSettings(const QAudioEncoderSettings &audioSettings,
                             const QVideoEncoderSettings &videoSettings = QVideoEncoderSettings(),
                             const QString &containerMimeType = QString());

public Q_SLOTS:
    void record();
    void pause();
    void stop();

Q_SIGNALS:
    void stateChanged(QMediaRecorder::State state);
    void durationChanged(qint64 duration);

    void error(QMediaRecorder::Error error);

private:
    Q_DISABLE_COPY(QMediaRecorder)
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QMediaRecorder::State))
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &))
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QMediaRecorder::State)
Q_DECLARE_METATYPE(QMediaRecorder::Error)

#endif  // QMEDIARECORDER_H
