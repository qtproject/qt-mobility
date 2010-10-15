/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qgalleryresource.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGalleryResource

    \ingroup gallery

    \inmodule QtGallery

    \brief The QGalleryResource class provides a resource identifier for a
    gallery item.

    A gallery resource is URL and a set of disambiguating meta-data properties
    used to identify a single representation of a gallery item that may be
    accessible using multiple protocols or in multiple formats.  This may be
    useful when querying something like a media server which can serve the same
    video in multiple resolutions, instead of listing the same video multiple
    times it would list it only once but include a resource for each resolution.
*/

/*!
    Constructs a null resource.
*/

QGalleryResource::QGalleryResource()
{
}

/*!
    Constructs a resource from a \a url.
*/

QGalleryResource::QGalleryResource(const QUrl &url)
    : m_url(url)
{
}

/*!
    Constructs a resource from a \a url and a set of \a attributes.
*/

QGalleryResource::QGalleryResource(const QUrl &url, QMap<int, QVariant> attributes)
    : m_url(url)
    , m_attributes(attributes)
{
}

/*!
    Constructs a copy of a \a resource.
*/

QGalleryResource::QGalleryResource(const QGalleryResource &resource)
    : m_url(resource.m_url)
    , m_attributes(resource.m_attributes)
{
}

/*!
    Destroys a resource.
*/

QGalleryResource::~QGalleryResource()
{
}

/*!
    Assigns the value of \a resource to another resource.
*/

QGalleryResource &QGalleryResource::operator =(const QGalleryResource &resource)
{
    m_url = resource.m_url;
    m_attributes = resource.m_attributes;

    return *this;
}

/*!
    Compares \a resource to another resource.

    Returns true if both resources are the same, and false otherwise.
*/

bool QGalleryResource::operator ==(const QGalleryResource &resource) const
{
    return m_url == resource.m_url && m_attributes == resource.m_attributes;
}

/*!
    Compares \a resource to another resource.

    Returns true if the resource are not the same, and false otherwise.
*/

bool QGalleryResource::operator !=(const QGalleryResource &resource) const
{
    return m_url != resource.m_url || m_attributes != resource.m_attributes;
}

/*!
    Returns the URL of a resource.
*/

QUrl QGalleryResource::url() const
{
    return m_url;
}

/*!
    Returns the attributes of a resource.
*/

QMap<int, QVariant> QGalleryResource::attributes() const
{
    return m_attributes;
}

/*!
    Returns the value of the resource attribute identified by \a key.
*/

QVariant QGalleryResource::attribute(int key) const
{
    return m_attributes.value(key);
}

QTM_END_NAMESPACE
