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

#include <QDebug>

#include <multimedia/qabstractmediaobject_p.h>
#include <multimedia/qaudiosource.h>
#include <multimedia/qaudiosourceservice.h>
#include <multimedia/qaudiorecorderservice.h>
#include <multimedia/qaudioencodercontrol.h>
#include <multimedia/qmediarecordercontrol.h>

/*!
    \class QAudioSource
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

/*!
    \a internal
*/

QAbstractMediaService* createAudioSourceService()
{
    QMediaServiceProvider *provider = defaultServiceProvider("audiorecorder");
    QObject *object = provider ? provider->createObject(QAudioRecorderService_iid) : 0;

    if (object != 0) {
        QAbstractMediaService *service = qobject_cast<QAbstractMediaService*>(object);
        if (service)
            return service;

        delete object;
    }

    return 0;
}

class QAudioSourcePrivate : public QAbstractMediaObjectPrivate
{
public:
    QAbstractMediaService *service;
    QAudioEncoderControl  *audioEncoderControl;
    QMediaRecorderControl *mediaRecorderControl;
    bool ownService;
};

/*!
    Construct a QAudioSource from \a service with \a parent.
*/

QAudioSource::QAudioSource(QObject *parent, QAbstractMediaService *service):
    QAbstractMediaObject(*new QAudioSourcePrivate, parent)
{
    Q_D(QAudioSource);

    if (service) {
        d->service = service;
        d->ownService = false;
    } else {
        d->service = createAudioSourceService();
        d->ownService = true;
    }

    Q_ASSERT(d->service != 0);

    if (d->service) {
        d->audioEncoderControl = qobject_cast<QAudioEncoderControl *>(d->service->control(QAudioEncoderControl_iid));
        d->mediaRecorderControl = qobject_cast<QMediaRecorderControl *>(d->service->control(QMediaRecorderControl_iid));
        registerService(d->service);
    } else {
        d->audioEncoderControl = 0;
        d->mediaRecorderControl = 0;
    }
}

/*!
    Destroys the audiosource object.
*/

QAudioSource::~QAudioSource()
{
    Q_D(QAudioSource);

    registerService(0);

    if (d->ownService) {
        delete d->service;
        d->service = 0;
    }
}

/*!
    Returns true if audiosource device available.
*/

bool QAudioSource::isValid() const
{
    return (d_func()->audioEncoderControl != NULL && d_func()->mediaRecorderControl != NULL);
}

/*!
    Returns the service.
*/

QAbstractMediaService *QAudioSource::service() const
{
    return d_func()->service;
}

/*!
    Returns the audio format.
*/

QAudioFormat QAudioSource::format() const
{
    if(d_func()->audioEncoderControl) {
        QAudioFormat fmt;
        fmt.setFrequency(d_func()->audioEncoderControl->frequency());
        fmt.setChannels(d_func()->audioEncoderControl->channels());
        fmt.setSampleSize(d_func()->audioEncoderControl->sampleSize());
        fmt.setCodec(d_func()->audioEncoderControl->audioCodec());
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        fmt.setByteOrder(QAudioFormat::LittleEndian);
        return fmt;
    }

    return QAudioFormat();
}

/*!
    Sets the audio format to \a format.
*/

void QAudioSource::setFormat(const QAudioFormat &format)
{
    if(d_func()->audioEncoderControl) {
        d_func()->audioEncoderControl->setAudioCodec(format.codec());
        d_func()->audioEncoderControl->setFrequency(format.frequency());
        d_func()->audioEncoderControl->setChannels(format.channels());
        d_func()->audioEncoderControl->setSampleSize(format.sampleSize());
    }
}

/*!
    Returns true if able to set the audio format to \a format.
*/

bool QAudioSource::isFormatSupported(const QAudioFormat &format) const
{
    if(d_func()->audioEncoderControl) {
        if(!d_func()->audioEncoderControl->supportedAudioCodecs().contains(format.codec()))
            return false;
        if(!d_func()->audioEncoderControl->supportedFrequencies().contains(format.frequency()))
            return false;
        if(!d_func()->audioEncoderControl->supportedChannelCounts().contains(format.channels()))
            return false;
        if(!d_func()->audioEncoderControl->supportedSampleSizes().contains(format.sampleSize()))
            return false;

        return true;
    }

    return false;
}

/*!
    Returns a list of available codecs.
*/

QStringList QAudioSource::supportedCodecs() const
{
    QStringList list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedAudioCodecs();

    return list;
}

/*!
    Returns a list of available frequencies.
*/

QList<int> QAudioSource::supportedFrequencies() const
{
    QList<int> list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedFrequencies();

    return list;
}

/*!
    Returns a list of available channels.
*/

QList<int> QAudioSource::supportedChannels() const
{
    QList<int> list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedChannelCounts();

    return list;
}

/*!
    Returns a list of available sample sizes.
*/

QList<int> QAudioSource::supportedSampleSizes() const
{
    QList<int> list;
    if(d_func()->audioEncoderControl)
        list << d_func()->audioEncoderControl->supportedSampleSizes();

    return list;
}

