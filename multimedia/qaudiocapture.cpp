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

#include <QDebug>

#include "qaudiocapture.h"

#include "qabstractmediaobject_p.h"
#include "qaudiocapturecontrol.h"
#include "qaudiocaptureservice.h"

/*!
    \class QAudioCapture
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QAudioCapturePrivate : public QAbstractMediaObjectPrivate
{
public:
    QAudioCaptureService* service;
    QAudioCaptureControl* control;
};

QAudioCapture::QAudioCapture(QAudioCaptureService *service, QObject *parent)
    : QAbstractMediaObject(*new QAudioCapturePrivate, parent)
{
    Q_D(QAudioCapture);

    if(service) {
        d->service = service;
        d->control = qobject_cast<QAudioCaptureControl *>(service->control("audiocapture"));
    } else {
        d->service = 0;
        d->control = 0;
    }
}

QAudioCapture::~QAudioCapture()
{
}

void QAudioCapture::start()
{
    if(d_func()->control)
        d_func()->control->start();
}

void QAudioCapture::stop()
{
    if(d_func()->control)
        d_func()->control->stop();
}

#ifdef AUDIOSERVICES
QAudioFormat QAudioCapture::format() const
{
    if(d_func()->control)
        return d_func()->control->format();
    else
        return QAudioFormat();
}

bool QAudioCapture::setFormat(const QAudioFormat &format)
{
    Q_D(QAudioCapture);

    if(d->control)
        return d->control->setFormat(format);
    else
        return false;
}
#endif

void QAudioCapture::setAudioInput(QObject *input)
{
}

QAbstractMediaService *QAudioCapture::service() const
{
    return d_func()->service;
}

QAudioCaptureService* createAudioCaptureService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.AudioCapture/1.0") : 0;

    if (object) {
        QAudioCaptureService *service = qobject_cast<QAudioCaptureService *>(object);

        if (service)
            return service;

        delete service;
    }
    return 0;
}
