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

#include <QtCore/qurl.h>

#include <multimedia/qmediasource.h>


class QMediaSourcePrivate : public QSharedData
{
public:
    QMediaSourcePrivate() {}
    QMediaSourcePrivate(const QMediaResourceList &r):
        resources(r) {}
    /*
       Uncomment if QMediaSource acquires mutating members.
    QMediaSourcePrivate(const QMediaSourcePrivate &other):
        QSharedData(other),
        resources(other.resources) {}

    QMediaSourcePrivate& operator=(const QMediaSourcePrivate &other)
    {
        resources = other.resources;
        return *this;
    }
    */

    QMediaResourceList  resources;
};


/*!
    \class QMediaSource
    \preliminary
    \brief The QMediaSource class provides access to the resources relating to a media content.

    QMediaSource is used within the multimedia framework as the logical handle
    to media content.  Media content can have multiple forms or other meta-data
    like items attached, some examples would be different quality variants of
    the primary stream, or extended meta-data such as a Poster for a movie.

    A non-null QMediaSource will always have a primary or canonical reference to
    the content available through the contentUri() or contentResource()
    methods, all other resources are optional.
*/


/*!
    Constructs a null QMediaSource.
*/

QMediaSource::QMediaSource()
{
}

/*!
    Constructs a media source with \a contentUri providing a reference to the content.
*/

QMediaSource::QMediaSource(const QUrl &contentUri):
    d(new QMediaSourcePrivate)
{
    d->resources << QMediaResource(contentUri);
}

/*!
    Constructs a media source with \a contentResource providing a reference to the content.
*/

QMediaSource::QMediaSource(const QMediaResource &contentResource):
    d(new QMediaSourcePrivate)
{
    d->resources << contentResource;
}

/*!
    Constructs a media source with \a resources providing a reference to the content.
*/

QMediaSource::QMediaSource(const QMediaResourceList &resources):
    d(new QMediaSourcePrivate(resources))
{
}

/*!
    Constructs a copy of media source \a other.
*/

QMediaSource::QMediaSource(const QMediaSource &other):
    d(other.d)
{
}

/*!
    Destroys the media source object.
*/

QMediaSource::~QMediaSource()
{
}

/*!
    Assigns the value of \a other to this media source.
*/

QMediaSource& QMediaSource::operator=(const QMediaSource &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equivalent to this media source; false otherwise.
*/

bool QMediaSource::operator==(const QMediaSource &other) const
{
    return (d.constData() == 0 && other.d.constData() == 0) ||
            (d.constData() != 0 && other.d.constData() != 0 &&
             d->resources == other.d->resources);
}

/*!
    Returns true if \a other is not equivalent to this media source; false otherwise.
*/

bool QMediaSource::operator!=(const QMediaSource &other) const
{
    return !(*this == other);
}

/*!
    Returns true if this media source is null (uninitialized); false otherwise.
*/

bool QMediaSource::isNull() const
{
    return d.constData() == 0;
}

/*!
    Returns a QUrl that represents that canonical content resource for this media source.
*/

QUrl QMediaSource::contentUri() const
{
    return contentResource().uri();
}

/*!
    Returns a QMediaResource that represents that canonical content resource for this media source.
*/

QMediaResource QMediaSource::contentResource() const
{
    if (d.constData() != 0) {
        foreach (const QMediaResource &resource, d->resources) {
            if (resource.role() == QMediaResource::ContentRole)
                return resource;
        }
    }

    return QMediaResource();
}

/*!
    Returns a QMediaResourceList that contains all the QMediaResources that match the resource \a role.

    \sa QMediaResource::ResourceRole
*/

QMediaResourceList QMediaSource::resources(QMediaResource::ResourceRole role) const
{
    QMediaResourceList rc;

    if (d.constData() != 0) {
        foreach (const QMediaResource &resource, d->resources) {
            if (resource.role() == role)
                rc << resource;
        }
    }

    return rc;
}

