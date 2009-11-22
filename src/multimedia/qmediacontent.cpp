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

#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

#include <qmediacontent.h>

QTM_BEGIN_NAMESPACE


class QMediaContentPrivate : public QSharedData
{
public:
    QMediaContentPrivate() {}
    QMediaContentPrivate(const QMediaResourceList &r):
        resources(r) {}

    QMediaContentPrivate(const QMediaContentPrivate &other):
        QSharedData(other),
        resources(other.resources),
        posterUri(other.posterUri),
        coverArtUriSmall(other.coverArtUriSmall),
        coverArtUriLarge(other.coverArtUriLarge),
        thumbnailUriSmall(other.thumbnailUriSmall),
        thumbnailUriLarge(other.thumbnailUriLarge)
    {}

    bool operator ==(const QMediaContentPrivate &other) const
    {
        return resources == other.resources
                && posterUri == other.posterUri
                && coverArtUriSmall == other.coverArtUriSmall
                && coverArtUriLarge == other.coverArtUriLarge
                && thumbnailUriSmall == other.thumbnailUriSmall
                && thumbnailUriLarge == other.thumbnailUriLarge;
    }

    QMediaResourceList  resources;
    QUrl posterUri;
    QUrl coverArtUriSmall;
    QUrl coverArtUriLarge;
    QUrl thumbnailUriSmall;
    QUrl thumbnailUriLarge;

private:
    QMediaContentPrivate& operator=(const QMediaContentPrivate &other);
};


/*!
    \class QMediaContent
    \preliminary
    \brief The QMediaContent class provides access to the resources relating to a media content.

    QMediaContent is used within the multimedia framework as the logical handle
    to media content.  Media content can have multiple forms or other meta-data
    like items attached, some examples would be different quality variants of
    the primary stream, or extended meta-data such as a Poster for a movie.

    A non-null QMediaContent will always have a primary or canonical reference to
    the content available through the canonicalUri() or canonicalResource()
    methods, any additional resources are optional.
*/


/*!
    Constructs a null QMediaContent.
*/

QMediaContent::QMediaContent()
{
}

/*!
    Constructs a media source with \a uri providing a reference to the content.
*/

QMediaContent::QMediaContent(const QUrl &uri):
    d(new QMediaContentPrivate)
{
    d->resources << QMediaResource(uri);
}

/*!
    Constructs a media source with \a resource providing a reference to the content.
*/

QMediaContent::QMediaContent(const QMediaResource &resource):
    d(new QMediaContentPrivate)
{
    d->resources << resource;
}

/*!
    Constructs a media source with \a resources providing a reference to the content.
*/

QMediaContent::QMediaContent(const QMediaResourceList &resources):
    d(new QMediaContentPrivate(resources))
{
}

/*!
    Constructs a copy of media source \a other.
*/

QMediaContent::QMediaContent(const QMediaContent &other):
    d(other.d)
{
}

/*!
    Destroys the media content object.
*/

QMediaContent::~QMediaContent()
{
}

/*!
    Assigns the value of \a other to this media content.
*/

QMediaContent& QMediaContent::operator=(const QMediaContent &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equivalent to this media content; false otherwise.
*/

bool QMediaContent::operator==(const QMediaContent &other) const
{
    return (d.constData() == 0 && other.d.constData() == 0) ||
            (d.constData() != 0 && other.d.constData() != 0 &&
             *d.constData() == *other.d.constData());
}

/*!
    Returns true if \a other is not equivalent to this media content; false otherwise.
*/

bool QMediaContent::operator!=(const QMediaContent &other) const
{
    return !(*this == other);
}

/*!
    Returns true if this media content is null (uninitialized); false otherwise.
*/

bool QMediaContent::isNull() const
{
    return d.constData() == 0;
}

/*!
    Returns a QUrl that represents that canonical resource for this media content.
*/

QUrl QMediaContent::canonicalUri() const
{
    return canonicalResource().uri();
}

/*!
    Returns a QMediaResource that represents that canonical resource for this media content.
*/

QMediaResource QMediaContent::canonicalResource() const
{
    return d.constData() != 0
            ?  d->resources.value(0)
            : QMediaResource();
}

/*!
    Returns a list of alternative resources for this media content.  The first item in this list
    is always the canonical resource.
*/

QMediaResourceList QMediaContent::resources() const
{
    return d.constData() != 0
            ? d->resources
            : QMediaResourceList();
}

/*!
    Returns the URI of a poster image for this media content.
*/

QUrl QMediaContent::posterUri() const
{
    return d.constData() != 0
            ? d->posterUri
            : QUrl();
}

/*!
    Sets the \a uri of a poster image for this media content.
*/

void QMediaContent::setPosterUri(const QUrl &uri)
{
    if (d.constData() != 0)
        d->posterUri = uri;
}

/*!
    Returns the URI of a small cover art image for this media content.
*/
QUrl QMediaContent::coverArtUriSmall() const
{
    return d.constData() != 0
            ? d->coverArtUriSmall
            : QUrl();
}

/*!
    Sets the \a uri of a small cover art image for this media content.
*/

void QMediaContent::setCoverArtUriSmall(const QUrl &uri)
{
    if (d.constData() != 0)
        d->coverArtUriSmall = uri;
}

/*!
    Returns the URI of a large cover art image for this media content.
*/

QUrl QMediaContent::coverArtUriLarge() const
{
    return d.constData() != 0
            ? d->coverArtUriLarge
            : QUrl();
}

/*!
    Sets the \a uri of a large cover art image for this media content.
*/

void QMediaContent::setCoverArtUriLarge(const QUrl &uri)
{
    if (d.constData() != 0)
        d->coverArtUriLarge = uri;
}

/*!
    Returns the URI of a small thumbnail image for this media content.
*/

QUrl QMediaContent::thumbnailUriSmall() const
{
    return d.constData() != 0
            ? d->thumbnailUriSmall
            : QUrl();
}

/*!
    Sets the \a uri of a small thumbanil image for this media content.
*/

void QMediaContent::setThumbnailUriSmall(const QUrl &uri)
{
    if (d.constData() != 0)
        d->thumbnailUriSmall = uri;
}

/*!
    Returns the URI of a large thumbnail image for this media content.
*/

QUrl QMediaContent::thumbnailUriLarge() const
{
    return d.constData() != 0
            ? d->thumbnailUriLarge
            : QUrl();
}

/*!
    Sets the \a uri of a large thumbnail image for this media content.
*/

void QMediaContent::setThumbnailUriLarge(const QUrl &uri)
{
    if (d.constData() != 0)
        d->thumbnailUriLarge = uri;
}
QTM_END_NAMESPACE

