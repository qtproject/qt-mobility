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

#include "qgallery.h"

/*!
    \class QAbstractGallery

    \ingroup gallery
    \ingroup gallery-galleries

    \brief The QAbstractGallery class provides a base class for gallery
    implementations.
*/

/*!
    Constructs a new gallery.

    The \a parent is passed to QObject.
*/

QAbstractGallery::QAbstractGallery(QObject *parent )
{

}

/*!
    Destroys a gallery.
*/

QAbstractGallery::~QAbstractGallery()
{

}

/*!
    \fn QAbstractGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const

    Identifies if a gallery supports a request \a type.

    Returns true if the request is supported, and false otherwise.
*/

/*!
    \property QAbstractGallery::rootDocumentId

    \brief The document ID of a gallery's root container.
*/

/*!
    \fn QAbstractGallery::rootDocumentIdChanged()

    Signals the \l rootDocumentId property has changed.
*/

/*!
    \property QAbstractGallery::supportedDocumentTypes

    \brief A list of document types that may be found in a gallery.
*/

/*!
    \fn QAbstractGallery::supportedDocumentTypesChanged()

    Signals the \l supportedDocumentTypes property has changed.
*/

/*!
    \fn QAbstractGallery::createResponse(QGalleryAbstractRequest *request)

    Creates a response to a gallery \a request.
*/

/*!
    \class QDocumentGallery

    \ingroup gallery
    \ingroup gallery-galleries

    \brief The QDocumentGallery class provides a gallery of documents.
*/

/*!
    Constructs a new document gallery.

    The \a parent is passed to QAbstractGallery.
*/

QDocumentGallery::QDocumentGallery(QObject *parent)
{

}

/*!
    Destroys a document gallery.
*/

QDocumentGallery::~QDocumentGallery()
{

}

/*!
    \reimp
*/

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::Type type) const
{

}

/*!
    \reimp
*/

QString QDocumentGallery::rootDocumentId() const
{

}

/*!
    \reimp
*/

QStringList QDocumentGallery::supportedDocumentTypes() const
{

}

/*!
    \reimp
*/

QGalleryAbstractResponse *QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{

}
