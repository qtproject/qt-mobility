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

#include <multimedia/qmediacontent.h>


class QMediaContentPrivate : public QSharedData
{
public:
    QMediaContentPrivate() {}
    QMediaContentPrivate(const QMediaResourceList &r):
        resources(r) {}
    /*
       Uncomment if QMediaContent acquires mutating members.
    QMediaContentPrivate(const QMediaContentPrivate &other):
        QSharedData(other),
        resources(other.resources) {}

    QMediaContentPrivate& operator=(const QMediaContentPrivate &other)
    {
        resources = other.resources;
        return *this;
    }
    */

    QMediaResourceList  resources;
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
    the content available through the contentUri() or contentResource()
    methods, all other resources are optional.
*/


/*!
    Constructs a null QMediaContent.
*/

QMediaContent::QMediaContent()
{
}

/*!
    Constructs a media source with \a contentUri providing a reference to the content.
*/

QMediaContent::QMediaContent(const QUrl &contentUri):
    d(new QMediaContentPrivate)
{
    d->resources << QMediaResource(contentUri);
}

/*!
    Constructs a media source with \a contentResource providing a reference to the content.
*/

QMediaContent::QMediaContent(const QMediaResource &contentResource):
    d(new QMediaContentPrivate)
{
    d->resources << contentResource;
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
    Destroys the media source object.
*/

QMediaContent::~QMediaContent()
{
}

/*!
    Assigns the value of \a other to this media source.
*/

QMediaContent& QMediaContent::operator=(const QMediaContent &other)
{
    d = other.d;
    return *this;
}

/*!
    Returns true if \a other is equivalent to this media source; false otherwise.
*/

bool QMediaContent::operator==(const QMediaContent &other) const
{
    return (d.constData() == 0 && other.d.constData() == 0) ||
            (d.constData() != 0 && other.d.constData() != 0 &&
             d->resources == other.d->resources);
}

/*!
    Returns true if \a other is not equivalent to this media source; false otherwise.
*/

bool QMediaContent::operator!=(const QMediaContent &other) const
{
    return !(*this == other);
}

/*!
    Returns true if this media source is null (uninitialized); false otherwise.
*/

bool QMediaContent::isNull() const
{
    return d.constData() == 0;
}

/*!
    Returns a QUrl that represents that canonical content resource for this media content.
*/

QUrl QMediaContent::contentUri() const
{
    return contentResource().uri();
}

/*!
    Returns a QMediaResource that represents that canonical content resource for this media content.
*/

QMediaResource QMediaContent::contentResource() const
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

QMediaResourceList QMediaContent::resources(QMediaResource::ResourceRole role) const
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

