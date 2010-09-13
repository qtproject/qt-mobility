/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qgalleryremoverequest.h"

#include "qgalleryabstractrequest_p.h"

QTM_BEGIN_NAMESPACE

class QGalleryRemoveRequestPrivate : public QGalleryAbstractRequestPrivate
{
public:
    QGalleryRemoveRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::RemoveRequest)
    {
    }

    QVariant itemId;
};

/*!
    \class QGalleryRemoveRequest

    \ingroup gallery
    \ingroup gallery-requests

    \inmodule QtGallery

    \brief The QGalleryRemoveRequest class provides a request which removes
    items from a gallery.
*/

/*!
    Constructs a new gallery remove request.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryRemoveRequestPrivate(0), parent)
{

}

/*!
    Contructs a new remove request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryRemoveRequest::QGalleryRemoveRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryRemoveRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery remove request.
*/

QGalleryRemoveRequest::~QGalleryRemoveRequest()
{
}

/*!
    \property QGalleryRemoveRequest::itemId

    \brief The ID of an item to remove from a gallery.
*/

QVariant QGalleryRemoveRequest::itemId() const
{
    Q_D(const QGalleryRemoveRequest);

    return d->itemId;
}

void QGalleryRemoveRequest::setItemId(const QVariant &id)
{
    Q_D(QGalleryRemoveRequest);

    d->itemId = id;
}

/*!
    \reimp
*/

void QGalleryRemoveRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_UNUSED(response);
}

#include "moc_qgalleryremoverequest.cpp"

QTM_END_NAMESPACE
