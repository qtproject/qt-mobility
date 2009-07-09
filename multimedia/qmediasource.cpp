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

#include "qmediasource.h"

class QMediaSourcePrivate : public QSharedData
{
public:
    QMediaSourcePrivate()
        :QSharedData() { ref.ref(); }

    QMediaSourcePrivate(const QString &mimeType, const QVariant &url)
        :QSharedData(),
         mimeType(mimeType),
         url(url),
         duration(-1)
    {}

    QMediaSourcePrivate(const QMediaSourcePrivate &other)
        :QSharedData(other),
         mimeType(other.mimeType),
         url(other.url),
         duration(other.duration),
         alternativeRepresentations(other.alternativeRepresentations)
    {}

    ~QMediaSourcePrivate() {}

    QString mimeType;
    QVariant url;
    int duration;
    QList<QMediaSource> alternativeRepresentations;
};

Q_GLOBAL_STATIC(QMediaSourcePrivate, qt_sharedMediaSource)

/*!
    \class QMediaSource
    \ingroup multimedia

    \preliminary
    \brief The QMediaSource class provides an media source location.

    \sa
*/

/*!
  Construct an empty media source object.
*/
QMediaSource::QMediaSource()
    :d(qt_sharedMediaSource())
{
}

/*!
  Construct a media source by passing \a mimeType and \a url.
*/
QMediaSource::QMediaSource(const QString &mimeType, const QVariant &url)
    :d(new QMediaSourcePrivate(mimeType, url))
{
}

/*!
  Constructs a copy of \a other.
*/
QMediaSource::QMediaSource(const QMediaSource &other)
    :d(other.d)
{
}

/*!
  Assigns the specified media source to this object.
*/
QMediaSource &QMediaSource::operator =(const QMediaSource &other)
{
    d = other.d;
    return *this;
}

/*!
  Destroys the media source.
*/
QMediaSource::~QMediaSource()
{
}

/*!
  Returns true if this media source is null; otherwise returns false.
*/
bool QMediaSource::isNull() const
{
    return d == qt_sharedMediaSource();
}

/*!
  Returns the mime type of this media source if available; otherwise returns an empty string.
*/
QString QMediaSource::mimeType() const
{
    return d->mimeType;
}

/*!
  Set the mime type of this media source to \a mimeType.
*/
void QMediaSource::setMimeType(QString const& mimeType)
{
    d->mimeType = mimeType;
}

/*!
  Returns the data location of this media source.
*/
QVariant QMediaSource::dataLocation() const
{
    return d->url;
}

/*!  
  Set the data location of this media source to \a url.
  \sa dataLocation()
*/
void QMediaSource::setDataLocation(QVariant const& url)
{
    d->url = url;
}

/*!
  Return the duration of media in miliseconds
  or -1 if duration is not available.
*/
int QMediaSource::duration() const
{
    return d->duration;
}

/*!
  Set the duration of media in miliseconds.
  \sa duration()
*/
void QMediaSource::setDuration(int ms)
{
    d->duration = ms;
}

/*!
  Returns list of alternative representations of this media source.
*/
QList<QMediaSource> QMediaSource::alternativeRepresentations() const
{
    return d->alternativeRepresentations;
}

/*!
  Set the list of alternative representations of this media source.
*/
void QMediaSource::setAlternativeRepresentations(const QList<QMediaSource> &sources)
{
    d->alternativeRepresentations = sources;
}

/*!
  Returns true if this media source and the given source are equal; otherwise returns false.
*/
bool QMediaSource::operator ==(const QMediaSource& other) const
{
    return d->url == other.d->url;
}

/*!
  Returns true if this media source and the given source are not equal; otherwise returns false.
*/
bool QMediaSource::operator !=(const QMediaSource& other) const
{
    return d->url != other.d->url;
}
