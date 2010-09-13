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

#include "qgallerytyperequest.h"
#include "qgalleryabstractrequest_p.h"

#include "qgallerynullresultset_p.h"
#include "qgalleryresource.h"

QTM_BEGIN_NAMESPACE

class QGalleryTypeRequestPrivate : public QGalleryAbstractRequestPrivate
{
    Q_DECLARE_PUBLIC(QGalleryTypeRequest)
public:
    QGalleryTypeRequestPrivate(QAbstractGallery *gallery)
        : QGalleryAbstractRequestPrivate(gallery, QGalleryAbstractRequest::TypeRequest)
        , autoUpdate(false)
        , resultSet(0)
        , internalResultSet(0)
    {
        internalResultSet = &nullResultSet;
    }

    void _q_itemsInserted(int index, int)
    {
        if (index == 0)
            resultSet->fetch(0);
    }

    void _q_itemsRemoved(int index, int)
    {
        if (index == 0)
            resultSet->fetch(0);
    }

    void _q_itemsMoved(int from, int to, int)
    {
        if (from == 0 || to == 0)
            resultSet->fetch(0);
    }

    void _q_currentItemChanged()
    {
        emit q_func()->typeChanged();

        if (!propertyKeys.isEmpty())
            emit q_func()->metaDataChanged(propertyKeys);
    }

    void _q_metaDataChanged(int index, int, const QList<int> &keys)
    {
        if (index == 0)
            emit q_func()->metaDataChanged(keys);
    }

    bool autoUpdate;
    QGalleryResultSet *resultSet;
    QGalleryResultSet *internalResultSet;
    QGalleryNullResultSet nullResultSet;
    QStringList propertyNames;
    QString itemType;
    QList<int> propertyKeys;
};

/*!
    \class QGalleryTypeRequest

    \ingroup gallery
    \ingroup gallery-requests

    \inmodule QtGallery

    \brief The QGalleryTypeRequest class provides an interface for requesting
    the properties of a type from a gallery.

*/
/*!
    Constructs a new gallery type request.

    The \a parent is passed to QObject.
*/


QGalleryTypeRequest::QGalleryTypeRequest(QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryTypeRequestPrivate(0), parent)
{
}
/*!
    Contructs a new type request for the given \a gallery.

    The \a parent is passed to QObject.
*/

QGalleryTypeRequest::QGalleryTypeRequest(QAbstractGallery *gallery, QObject *parent)
    : QGalleryAbstractRequest(*new QGalleryTypeRequestPrivate(gallery), parent)
{
}

/*!
    Destroys a gallery type request.
*/

QGalleryTypeRequest::~QGalleryTypeRequest()
{
}
/*!
    \property QGalleryTypeRequest::propertyNames

    \brief A list of names of meta-data properties a request should return
    values for.
*/


QStringList QGalleryTypeRequest::propertyNames() const
{
    return d_func()->propertyNames;
}

void QGalleryTypeRequest::setPropertyNames(const QStringList &names)
{
    d_func()->propertyNames = names;
}

/*!
    \property QGalleryTypeRequest::autoUpdate

    \brief Whether a the results of a request should be updated after a request
    has finished.

    If this is true the request will go into the Idle state when the request has
    finished rather than returning to Inactive.
*/


bool QGalleryTypeRequest::autoUpdate() const
{
    return d_func()->autoUpdate;
}

void QGalleryTypeRequest::setAutoUpdate(bool enabled)
{
    d_func()->autoUpdate = enabled;
}

/*!
    \property QGalleryTypeRequest::itemType

    \brief the type a request should return the properties of.

*/

QString QGalleryTypeRequest::itemType() const
{
    return d_func()->itemType;
}

void QGalleryTypeRequest::setItemType(const QString &itemType)
{
    d_func()->itemType = itemType;

    emit itemTypeChanged();
}

/*!
    \fn QGalleryTypeRequest::itemTypeChanged()

    Signals that the \l itemType property has changed.
*/

/*!
    Returns the result set containing the meta-data of a type.
*/

QGalleryResultSet *QGalleryTypeRequest::resultSet() const
{
    return d_func()->resultSet;
}

/*!
    \fn QGalleryTypeRequest::resultSetChanged(QGalleryResultSet *resultSet)

    Signals that the \a resultSet containing the meta-data of a type has
    changed.
*/

/*!
    \fn QGalleryTypeRequest::typeChanged()

    Signals that the properties of a type have changed.
*/

/*!
    Returns the key of \a property.
*/

int QGalleryTypeRequest::propertyKey(const QString &property) const
{
    return d_func()->internalResultSet->propertyKey(property);
}

/*!
    Returns the attributes of the property identified by \a key.
*/

QGalleryProperty::Attributes QGalleryTypeRequest::propertyAttributes(int key) const
{
    return d_func()->internalResultSet->propertyAttributes(key);
}

/*!
    Returns the type of the property identified by \a key.
*/

QVariant::Type QGalleryTypeRequest::propertyType(int key) const
{
    return d_func()->internalResultSet->propertyType(key);
}

/*!
    \property QGalleryTypeRequest::valid

    \brief Whether the request currently holds valid type information.
*/

bool QGalleryTypeRequest::isValid() const
{
    return d_func()->internalResultSet->isValid();
}

/*!
    Returns the value of a meta-data property identified by \a key.
*/


QVariant QGalleryTypeRequest::metaData(int key) const
{
    return d_func()->internalResultSet->metaData(key);
}

/*!
    Returns the value of a meta-data \a property.
*/

QVariant QGalleryTypeRequest::metaData(const QString &property) const
{
    return d_func()->internalResultSet->metaData(
            d_func()->internalResultSet->propertyKey(property));
}

/*!
    \fn QGalleryTypeRequest::metaDataChanged(const QList<int> &keys)

    Signals that the values of meta-data properties identified by \a keys
    have changed.
*/

/*!
    \reimp
*/

void QGalleryTypeRequest::setResponse(QGalleryAbstractResponse *response)
{
    Q_D(QGalleryTypeRequest);

    const bool wasValid = d->internalResultSet->isValid();

    d->resultSet = qobject_cast<QGalleryResultSet *>(response);
    d->propertyKeys.clear();

    if (d->resultSet) {
        d->internalResultSet = d->resultSet;

        connect(d->resultSet, SIGNAL(itemsInserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));
        connect(d->resultSet, SIGNAL(itemsRemoved(int,int)), this, SLOT(_q_itemsRemoved(int,int)));
        connect(d->resultSet, SIGNAL(itemsMoved(int,int,int)),
                this, SLOT(_q_itemsMoved(int,int,int)));
        connect(d->resultSet, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));
        connect(d->resultSet, SIGNAL(currentItemChanged()), this, SLOT(_q_currentItemChanged()));

        typedef QStringList::const_iterator iterator;
        for (iterator it = d->propertyNames.constBegin(), end = d->propertyNames.constEnd();
                it != end;
                ++it) {
            const int propertyKey = d->resultSet->propertyKey(*it);

            if (propertyKey != -1)
                d->propertyKeys.append(propertyKey);
        }
    } else {
        d->internalResultSet = &d->nullResultSet;
    }

    emit resultSetChanged(d->resultSet);

    if (d->internalResultSet->itemCount() > 0)
        d->internalResultSet->fetch(0);
    else if (wasValid)
        emit typeChanged();
}

#include "moc_qgallerytyperequest.cpp"

QTM_END_NAMESPACE
