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

#include <QtCore/qmetaobject.h>

#include <qabstractmediaobject_p.h>

#include <qabstractmediaservice.h>
#include <qmetadataprovidercontrol.h>

void QAbstractMediaObjectPrivate::_q_notify()
{
    Q_Q(QAbstractMediaObject);

    const int len = notifyProperties.length();

    for (int i = 0; i < len; ++i) {
        const char *name = notifyProperties.at(i).constData();

        const QMetaObject* m = q->metaObject();

        int pi = m->indexOfProperty(name);
        if (pi == -1)
            continue;

        QMetaProperty p = m->property(pi);
        if (p.hasNotifySignal())
            p.notifySignal().invoke(q, QGenericArgument(QMetaType::typeName(p.userType()),
                                                        p.read(q).data()));
    }
}


/*!
    \class QAbstractMediaObject
    \ingroup multimedia
    \ingroup multimedia-serv

    \preliminary
    \brief The QAbstractMediaObject class supplies the base Multimedia
    object.

    \sa QAbstractMediaService, QAbstractMediaControl
*/

/*!
    \enum QAbstractMediaObject::MetaData

    Common attributes
    \value Title The title of the media.  QString.
    \value SubTitle The sub-title of the media. QString.
    \value Author The authors of the media. QStringList.
    \value Comment A user comment about the media. QString.
    \value Description A description of the media.  QString
    \value Category The category of the media.  QStringList.
    \value Genre The genre of the media.  QStringList.
    \value Year The year of release of the media.  int.
    \value Date The date of the media. QDate.
    \value UserRating A user rating of the media. int [0..100].
    \value Keywords A list of keywords describing the media.  QStringList.
    \value Language

    \value Publisher The publisher of the media.  QString.
    \value Copyright The media's copyright notice.  QString.
    \value ParentalRating  The parental rating of the media.  QString.
    \value RatingOrganisation The organisation responsible for the parental rating of the media.
    QString.

    Media attributes
    \value Size The size in bytes of the media. qint64
    \value MediaType The type of the media (audio, video, etc).  QString.
    \value Duration The duration in millseconds of the media.  qint64.

    Audio attributes
    \value AudioBitrate The bit rate of the media's audio stream in bits per second.  int.
    \value AudioCodec The codec of the media's audio stream.  QString.
    \value AverageLevel The average volume level of the media.  int.
    \value Channels The number of channels in the media's audio stream. int.
    \value PeakValue The peak volume of the media's audio stream. int
    \value Frequency The frequency of the media's audio stream. int

    Music attributes
    \value AlbumTitle The title of the album the media belongs to.  QString.
    \value AlbumArtist The principal artist of the album the media belongs to.  QString.
    \value ContributingArtist The artists contributing to the media.  QStringList.
    \value Composer The composer of the media.  QStringList.
    \value Conductor The conductor of the media. QString.
    \value Lyrics The lyrics to the media. QString.
    \value Mood The mood of the media.  QString.
    \value TrackNumber The track number of the media.  int.
    \value TrackCount The number of tracks on the album containing the media.  int.

    \value CoverArtUriSmall The URI of a small cover art image. QUrl.
    \value CoverArtUriLarge The URI of a large cover art image. QUrl.

    Image and video attributes
    \value Resolution The dimensions of an image or video.  QSize.
    \value PixelAspectRatio The pixel aspect ratio of an image or video.  QSize.

    Video attributes
    \value FrameRate The frame rate of the media's video stream.  QPair<int, int>.
    \value VideoBitRate The bit rate of the media's video stream in bits per second.  int.
    \value VideoCodec The codec of the media's video stream.  QString.

    \value PosterUri The URI of a poster image.  QUrl.

    Movie attributes
    \value ChapterNumber The chapter number of the media.  int.
    \value Director The director of the media.  QString.
    \value LeadPerformer The lead performer in the media.  QStringList.
    \value Writer The writer of the media.  QStringList.

    Photo attributes
    \value CameraManufacturer The manufacturer of the camera used to capture the media.  QString.
    \value CameraModel The model of the camera used to capture the media.  QString.
    \value Event The event during which the media was captured.  QString.
    \value Subject The subject of the media.  QString.
*/

/*!
    Destroys the QAbstractMediaObject object.
*/

QAbstractMediaObject::~QAbstractMediaObject()
{
    delete d_ptr;
}

/*!
    \fn QAbstractMediaService* QAbstractMediaObject::service() const

    Returns the media service that provide the functionality for the Multimedia object.
*/

/*!
    \fn bool QAbstractMediaObject::isValid() const

    Returns true if the concrete media object is capable of normal operation;
    false otherwise.
*/

/*!
*/
void QAbstractMediaObject::registerService(QAbstractMediaService *service)
{
    Q_D(QAbstractMediaObject);

    d->service = service;

    if (d->metaDataControl) {
        disconnect(d->metaDataControl, SIGNAL(metaDataChanged()), this, SIGNAL(metaDataChanged()));
        disconnect(
                d->metaDataControl, SIGNAL(metaDataAvailableChanged(bool)),
                this, SIGNAL(metaDataAvailableChanged(bool)));
        disconnect(
                d->metaDataControl, SIGNAL(writableChanged(bool)),
                this, SIGNAL(metaDataWritableChanged(bool)));
    }

    if (service) {
        d->metaDataControl = qobject_cast<QMetaDataProviderControl *>(
                service->control(QMetaDataProviderControl_iid));

        if (d->metaDataControl) {
            connect(d->metaDataControl, SIGNAL(metaDataChanged()), this, SIGNAL(metaDataChanged()));
            connect(d->metaDataControl,
                    SIGNAL(metaDataAvailableChanged(bool)),
                    SIGNAL(metaDataAvailableChanged(bool)));
            connect(d->metaDataControl,
                    SIGNAL(writableChanged(bool)),
                    SIGNAL(metaDataWritableChanged(bool)));
        }
    } else {
        d->metaDataControl = 0;
    }
}

int QAbstractMediaObject::notifyInterval() const
{
    return d_func()->notifyTimer->interval();
}

void QAbstractMediaObject::setNotifyInterval(int milliSeconds)
{
    Q_D(QAbstractMediaObject);

    if (d->notifyTimer->interval() != milliSeconds) {
        d->notifyTimer->setInterval(milliSeconds);

        emit notifyIntervalChanged(milliSeconds);
    }
}

/*!
  \internal
*/
void QAbstractMediaObject::bind(QObject*)
{
}

/*!
    Construct a QAbstractMediaObject with \a parent. This class is meant as a
    base class for Multimedia objects so this constructor is protected.
*/

QAbstractMediaObject::QAbstractMediaObject(QObject *parent)
    : QObject(parent)
    , d_ptr(new QAbstractMediaObjectPrivate)

{
    Q_D(QAbstractMediaObject);

    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));
}

/*!
    \internal
*/

QAbstractMediaObject::QAbstractMediaObject(QAbstractMediaObjectPrivate &dd, QObject *parent)
    : QObject(parent)
    , d_ptr(&dd)
{
    Q_D(QAbstractMediaObject);
    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));
}

/*!
    Watch the property \a name. The property's notify signal will be emitted
    once every notifyInterval milliseconds.

    \sa notifyInterval
*/

void QAbstractMediaObject::addPropertyWatch(QByteArray const &name)
{
    Q_D(QAbstractMediaObject);

    d->notifyProperties << name;

    if (!d->notifyTimer->isActive())
        d->notifyTimer->start();
}

/*!
    Remove property \a name from the list of properties whose changes are
    regularly signaled.

    \sa notifyInterval
*/

void QAbstractMediaObject::removePropertyWatch(QByteArray const &name)
{
    Q_D(QAbstractMediaObject);

    d->notifyProperties.removeAll(name);

    if (d->notifyProperties.isEmpty())
        d->notifyTimer->stop();
}

/*!
    \property QAbstractMediaObject::notifyInterval

    The interval at which notifiable properties will update.

    The interval is expressed in milliseconds, the default value is 1000.

    \sa addPropertyWatch(), removePropertyWatch()
*/

/*!
    \fn void QAbstractMediaObject::notifyIntervalChanged(int milliSeconds)

    Signal a change in the notify interval period to \a milliSeconds.
*/

/*!
    \property QAbstractMediaObject::metaDataAvailable
    \brief Identifies if access to a media object's meta-data is available.
*/

/*!
    Identifies if access to a media object's meta-data is available.

    Returns true if the meta-data is available and false otherwise.
*/
bool QAbstractMediaObject::isMetaDataAvailable() const
{
    Q_D(const QAbstractMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->isMetaDataAvailable()
            : false;
}

/*!
    \fn QAbstractMediaObject::metaDataAvailableChanged(bool available)

    Signals that the \a available state of a media object's meta-data has changed.
*/

/*!
    \property QAbstractMediaObject::metaDataWritable
    \brief Identifies if a media object's meta-data is writable.
*/

/*!
    Identifies if a media object's meta-data is writable.

    Returns true if the meta-data can be edited and false otherwise.
*/
bool QAbstractMediaObject::isMetaDataWritable() const
{
    Q_D(const QAbstractMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->isWritable()
            : false;
}

/*!
    \fn QAbstractMediaObject::metaDataWritableChanged(bool writable)

    Signals that the \a writable state of a media object's meta-data has changed.
*/

/*!
    Returns the value associated with a meta-data \a key.
*/
QVariant QAbstractMediaObject::metaData(MetaData key) const
{
    Q_D(const QAbstractMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->metaData(key)
            : QVariant();
}

/*!
    Sets a \a value for a meta-data \a key.
*/
void QAbstractMediaObject::setMetaData(MetaData key, const QVariant &value)
{
    Q_D(QAbstractMediaObject);

    if (d->metaDataControl)
        d->metaDataControl->setMetaData(key, value);
}

/*!
    \fn QAbstractMediaObject::metaDataChanged()

    Signals that a media object's meta-data has changed.
*/

/*!
    Returns the value associated with a meta-data \a key.

    The naming and type of extended meta-data is not standardized, so the values and meaning
    of keys may vary between backends.
*/
QVariant QAbstractMediaObject::extendedMetaData(const QString &key) const
{
    Q_D(const QAbstractMediaObject);

    return d->metaDataControl
            ? d->metaDataControl->extendedMetaData(key)
            : QVariant();
}

/*!
    Sets a \a value for a meta-data \a key.

    The naming and type of extended meta-data is not standardized, so the values and meaning
    of keys may vary between backends.
*/
void QAbstractMediaObject::setExtendedMetaData(const QString &key, const QVariant &value)
{
    Q_D(QAbstractMediaObject);

    if (d->metaDataControl)
        d->metaDataControl->setExtendedMetaData(key, value);
}

#include "moc_qabstractmediaobject.cpp"
