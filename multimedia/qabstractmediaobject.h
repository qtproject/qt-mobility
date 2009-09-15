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

#ifndef QABSTRACTMEDIAOBJECT_H
#define QABSTRACTMEDIAOBJECT_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

#include "qmultimediaglobal.h"

class QAbstractMediaService;

class QAbstractMediaObjectPrivate;
class Q_MEDIA_EXPORT QAbstractMediaObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int notifyInterval READ notifyInterval WRITE setNotifyInterval NOTIFY notifyIntervalChanged)
    Q_PROPERTY(bool metaDataAvailable READ isMetaDataAvailable NOTIFY metaDataAvailableChanged)
    Q_PROPERTY(bool metaDataWritable READ isMetaDataWritable NOTIFY metaDataWritableChanged)
public:
    enum MetaData
    {
        // Common
        Title,
        SubTitle,
        Author,
        Comment,
        Description,
        Category,
        Genre,
        Year,
        Date,
        UserRating,
        Keywords,
        Language,
        Publisher,
        Copyright,
        ParentalRating,
        RatingOrganisation,

        // Media
        Size,
        MediaType,
        Duration,

        // Audio
        AudioBitrate,
        AudioCodec,
        AverageLevel,
        Channels,
        PeakValue,
        Frequency,

        // Music
        AlbumTitle,
        AlbumArtist,
        ContributingArtist,
        Composer,
        Conductor,
        Lyrics,
        Mood,
        TrackNumber,
        TrackCount,

        CoverArtUriSmall,
        CoverArtUriLarge,

        // Image/Video
        Resolution,
        PixelAspectRatio,

        // Video
        FrameRate,
        VideoBitRate,
        VideoCodec,

        PosterUri,

        // Movie
        ChapterNumber,
        Director,
        LeadPerformer,
        Writer,

        // Photos
        CameraManufacturer,
        CameraModel,
        Event,
        Subject
    };

    ~QAbstractMediaObject();

    virtual QAbstractMediaService* service() const = 0;

    virtual bool isValid() const = 0;

    int notifyInterval() const;
    void setNotifyInterval(int milliSeconds);

    virtual void bind(QObject*);

    bool isMetaDataAvailable() const;
    bool isMetaDataWritable() const;

    QVariant metaData(MetaData key) const;
    void setMetaData(MetaData key, const QVariant &value);

    QVariant extendedMetaData(const QString &key) const;
    void setExtendedMetaData(const QString &key, const QVariant &value);

Q_SIGNALS:
    void notifyIntervalChanged(int milliSeconds);

    void metaDataAvailableChanged(bool available);
    void metaDataWritableChanged(bool writable);
    void metaDataChanged();

protected:
    QAbstractMediaObject(QObject *parent = 0);
    QAbstractMediaObject(QAbstractMediaObjectPrivate &dd, QObject *parent);

    void addPropertyWatch(QByteArray const &name);
    void removePropertyWatch(QByteArray const &name);

    void registerService(QAbstractMediaService *service);

    QAbstractMediaObjectPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(QAbstractMediaObject)
    Q_PRIVATE_SLOT(d_func(), void _q_notify())
};


#endif  // QABSTRACTMEDIAOBJECT_H
