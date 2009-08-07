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

#include "qradioplayer.h"

#include "qabstractmediaobject_p.h"
#include "qradiotuner.h"
#include "qradioservice.h"

/*!
    \class QRadioPlayer
    \ingroup multimedia

    \preliminary
    \brief

    \sa
*/

class QRadioPlayerPrivate : public QAbstractMediaObjectPrivate
{
public:
    QRadioService*  service;
    QRadioTuner*    control;
};


QRadioPlayer::QRadioPlayer(QRadioService* service, QObject *parent):
    QAbstractMediaObject(*new QRadioPlayerPrivate, parent)
{
    Q_ASSERT(service != 0);

    Q_D(QRadioPlayer);

    d->service = service;
    d->control = qobject_cast<QRadioTuner *>(service->control("com.nokia.qt.RadioPlayerControl"));

    if(d->control) {
        connect(d->control,SIGNAL(bandChanged(QRadioPlayer::Band)),this,SIGNAL(bandChanged(QRadioPlayer::Band)));
        connect(d->control,SIGNAL(frequencyChanged(int)),this,SIGNAL(frequencyChanged(int)));
        connect(d->control,SIGNAL(stereoStatusChanged(bool)),this,SIGNAL(stereoStatusChanged(bool)));
        connect(d->control,SIGNAL(searchingStatusChanged(bool)),this,SIGNAL(searchingStatusChanged(bool)));
        connect(d->control,SIGNAL(signalStrengthChanged(int)),this,SIGNAL(signalStrengthChanged(int)));
        connect(d->control,SIGNAL(durationChanged(qint64)),this,SIGNAL(durationChanged(qint64)));
        connect(d->control,SIGNAL(volumeChanged(int)),this,SIGNAL(volumeChanged(int)));
        connect(d->control,SIGNAL(mutingChanged(bool)),this,SIGNAL(mutingChanged(bool)));
    }

    addPropertyWatch("duration");
}

QRadioPlayer::~QRadioPlayer()
{
    Q_D(QRadioPlayer);

    delete d->control;
    delete d->service;
}

QRadioPlayer::Band QRadioPlayer::band() const
{
    return QRadioPlayer::Band(d_func()->control->band());
}

int QRadioPlayer::frequency() const
{
    return d_func()->control->frequency();
}

bool QRadioPlayer::isStereo() const
{
    return d_func()->control->isStereo();
}

bool QRadioPlayer::isSupportedBand(int b) const
{
    return d_func()->control->isSupportedBand(b);
}

int QRadioPlayer::signalStrength() const
{
    return d_func()->control->signalStrength();
}

qint64 QRadioPlayer::duration() const
{
    return d_func()->control->duration();
}

int QRadioPlayer::volume() const
{
    return d_func()->control->volume();
}

bool QRadioPlayer::isMuted() const
{
    return d_func()->control->isMuted();
}

QAbstractMediaService* QRadioPlayer::service() const
{
    return d_func()->service;
}

void QRadioPlayer::setBand(QRadioPlayer::Band band)
{
    d_func()->control->setBand(band);
}

void QRadioPlayer::setFrequency(int frequency)
{
    d_func()->control->setFrequency(frequency);
}

void QRadioPlayer::setStereo(bool stereo)
{
    d_func()->control->setStereo(stereo);
}

void QRadioPlayer::setVolume(int volume)
{
    d_func()->control->setVolume(volume);
}

void QRadioPlayer::setMuted(bool muted)
{
    d_func()->control->setMuted(muted);
}

bool QRadioPlayer::isValid() const
{
    return d_func()->control != NULL;
}

bool QRadioPlayer::isSearching() const
{
    return d_func()->control->isSearching();
}

void QRadioPlayer::searchForward()
{
    d_func()->control->searchForward();
}

void QRadioPlayer::searchBackward()
{
    d_func()->control->searchBackward();
}

void QRadioPlayer::cancelSearch()
{
    d_func()->control->cancelSearch();
}

QRadioService* createRadioService(QMediaServiceProvider *provider)
{
    QObject *object = provider ? provider->createObject("com.nokia.qt.RadioService/1.0") : 0;

    if (object) {
        QRadioService *service = qobject_cast<QRadioService *>(object);

        if (service)
            return service;

        delete object;
    }
    return 0;
}
