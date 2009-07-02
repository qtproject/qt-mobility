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

#include "qmediarecorder.h"

#include "qabstractmediaobject_p.h"
#include "qmediarecordercontrol.h"
#include "qmediarecorderservice.h"
#include "qmediasink.h"

/*!
    \class QMediaRecorder
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QMediaRecorderPrivate : public QAbstractMediaObjectPrivate
{
public:
    QMediaRecorderService*  service;
    QMediaRecorderControl* control;
};

QMediaRecorder::QMediaRecorder(QMediaRecorderService *service, QObject *parent)
    : QAbstractMediaObject(*new QMediaRecorderPrivate, parent)
{
    Q_D(QMediaRecorder);

    d->service = service;
    d->control = qobject_cast<QMediaRecorderControl *>(service->control("com.nokia.qt.MediaRecorderControl"));
}

QMediaRecorder::~QMediaRecorder()
{
    Q_D(QMediaRecorder);

    delete d->service;
}

void QMediaRecorder::setRecordingSource(QAbstractMediaObject* source)
{
}

void QMediaRecorder::setRecordingSink(QAbstractMediaObject* sink)
{
}

QMediaRecorder::State QMediaRecorder::state() const
{
    return QMediaRecorder::State(d_func()->control->state());
}

QMediaSink *QMediaRecorder::sink() const
{
    return d_func()->control->sink();
}

QAbstractMediaService* QMediaRecorder::service() const
{
    return d_func()->service;
}

//public Q_SLOTS:
void QMediaRecorder::record()
{
}

void QMediaRecorder::pause()
{
}

void QMediaRecorder::stop()
{
}

QMediaRecorderService* createMediaRecorderService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.Qt.RecorderService/1.0") : 0;

    if (object) {
        QMediaRecorderService *service = qobject_cast<QMediaRecorderService *>(object);

        if (service)
            return service;

        delete object;
    }
    return 0;
}
