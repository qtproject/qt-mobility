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

#include "galleryitemrequest.h"

#include <QtDeclarative/qdeclarativepropertymap.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryItem GalleryItemRequest
    \brief The GalleryItem element allows you to request information about a
    single item from a gallery

    This element is part of the \bold {QtMobility.gallery 1.0} module.

    \sa GalleryQueryModel, GalleryQueryCount
*/

GalleryItemRequest::GalleryItemRequest(QObject *parent)
    : QObject(parent)
    , m_itemList(0)
    , m_metaData(0)
    , m_complete(false)
{
    connect(&m_request, SIGNAL(succeeded()), this, SIGNAL(succeeded()));
    connect(&m_request, SIGNAL(cancelled()), this, SIGNAL(cancelled()));
    connect(&m_request, SIGNAL(stateChanged(QGalleryAbstractRequest::State)),
            this, SIGNAL(stateChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SIGNAL(resultChanged()));
    connect(&m_request, SIGNAL(progressChanged(int,int)), this, SIGNAL(progressChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SIGNAL(resultChanged()));
    connect(&m_request, SIGNAL(failed(int)), this, SIGNAL(failed(int)));
    connect(&m_request, SIGNAL(finished(int)), this, SIGNAL(finished(int)));

    connect(&m_request, SIGNAL(itemChanged(QGalleryItemList*)),
            this, SLOT(_q_itemListChanged(QGalleryItemList*)));

    m_metaData = new QDeclarativePropertyMap(this);

    connect(m_metaData, SIGNAL(valueChanged(QString,QVariant)),
            this, SLOT(_q_valueChanged(QString,QVariant)));
}

GalleryItemRequest::~GalleryItemRequest()
{
}

/*!
    \qmlproperty QAbstractGallery GalleryItem::gallery

    This property holds the gallery an item should be requested from.
*/

/*!
    \qmlproperty enum GalleryItem::state

    This property holds the state of an item request.  It can be one of:

    \list
    \o Inactive The request has finished.
    \o Active The request is currently executing.
    \o Cancelling The request has been cancelled, but has yet reached the
    Inactive state.
    \o Idle The request has finished and is monitoring its result set for
    changes.
    \endlist
*/

/*!
    \qmlproperty enum GalleryItem::result

    The property holds the result of an item request. It can be one of:

    \list
    \o NoResult The request is still executing.
    \o Succeeded The request finished successfully.
    \o Cancelled The request was cancelled.
    \o NoGallery No \l gallery was specified.
    \o NotSupported Item requests are not supported by the \l gallery.
    \o ConnectionError The request failed due to a connection error.
    \o InvalidItemError The request failed because the value of \l item
    is not a valid item ID.
    \endlist
*/

/*!
    \qmlproperty int GalleryItem::currentProgress

    This property holds the current progress value.
*/

/*!
    \qmlproperty int GalleryItem::maximumProgress

    This property holds the maximum progress value.
*/

/*!
    \qmlproperty QStringList GalleryItem::properties

    This property holds the item properties a request should return values for.
*/

/*!
    \qmlproperty bool GalleryItem::live

    This property holds whether a request should refresh its results
    automatically.
*/

/*!
    \qmlproperty variant GalleryItem::item

    This property holds the id of the item to return information about.
*/

/*!
    \qmlproperty bool GalleryItem::available

    This property holds whether the meta-data of an item is available.
*/

/*!
    \qmlproperty bool GalleryItem::reading

    This property holds whether the meta-data of an item is currently being
    read.
*/

/*!
    \qmlproperty bool GalleryItem::writing

    This property holds whether the meta-data of an item is currently being
    written.
*/

/*!
    \qmlproperty string GalleryItem::itemType

    This property holds the type of a gallery item.
*/

/*!
    \qmlproperty url GalleryItem::itemUrl

    This property holds the URL of a gallery item.
*/

/*!
    \qmlproperty object GalleryItem::metaData

    This property holds the meta-data of a gallery item.
*/

/*!
    \qmlmethod GalleryItem::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod GalleryItem::cancel()

    Cancels an executing request.
*/

/*!
    \qmlmethod GalleryItem::clear()

    Clears the results of a request.
*/

/*!
    \qmlsignal GalleryItem::onSucceeded()

    Signals that a request has finished successfully.
*/

/*!
    \qmlsignal GalleryItem::onCancelled()

    Signals that a request was cancelled.
*/

/*!
    \qmlsignal GalleryItem::onFailed(error)

    Signals that a request failed with the given \a error.
*/

/*!
    \qmlsignal GalleryItem::onFinished(result)

    Signals that a request finished with the given \a result.
*/

void GalleryItemRequest::classBegin()
{
}

void GalleryItemRequest::componentComplete()
{
    m_complete = true;

    if (m_request.itemId().isValid())
        m_request.execute();
}

void GalleryItemRequest::_q_itemListChanged(QGalleryItemList *items)
{
    if (m_itemList && m_itemList->count() > 0) {
        typedef QHash<int, QString>::const_iterator iterator;
        for (iterator it = m_propertyKeys.constBegin(); it != m_propertyKeys.constEnd(); ++it)
            m_metaData->insert(it.value(), QVariant(m_itemList->propertyType(it.key())));
    }

    m_itemList = items;
    m_propertyKeys.clear();

    if (m_itemList) {
        const QStringList propertyNames = m_itemList->propertyNames();

        typedef QStringList::const_iterator iterator;
        if (m_itemList->count() > 0) {
            for (iterator it = propertyNames.begin(); it != propertyNames.end(); ++it) {
                const int key = m_itemList->propertyKey(*it);

                m_propertyKeys.insert(key, *it);

                QVariant value = m_itemList->metaData(0, key);
                m_metaData->insert(*it, value.isNull()
                        ? QVariant(m_itemList->propertyType(key))
                        : value);
            }
        } else {
            for (iterator it = propertyNames.begin(); it != propertyNames.end(); ++it) {
                const int key = m_itemList->propertyKey(*it);
                m_propertyKeys.insert(key, *it);

                m_metaData->insert(*it, QVariant(m_itemList->propertyType(key)));
            }
        }

        connect(m_itemList, SIGNAL(inserted(int,int)), this, SLOT(_q_itemsInserted(int,int)));
        connect(m_itemList, SIGNAL(removed(int,int)), this, SLOT(_q_itemsRemoved(int,int)));
        connect(m_itemList, SIGNAL(metaDataChanged(int,int,QList<int>)),
                this, SLOT(_q_metaDataChanged(int,int,QList<int>)));
    }

    emit statusChanged();
    emit availableChanged();
}

void GalleryItemRequest::_q_itemsInserted(int index, int)
{
    if (index == 0) {
        typedef QHash<int, QString>::const_iterator iterator;
        for (iterator it = m_propertyKeys.constBegin(); it != m_propertyKeys.constEnd(); ++it) {
            QVariant value = m_itemList->metaData(0, it.key());
            m_metaData->insert(it.value(), value.isNull()
                    ? QVariant(m_itemList->propertyType(it.key()))
                    : value);
        }

        emit statusChanged();
        emit availableChanged();
    }
}

void GalleryItemRequest::_q_itemsRemoved(int index, int)
{
    if (index == 0) {
        typedef QHash<int, QString>::const_iterator iterator;
        for (iterator it = m_propertyKeys.constBegin(); it != m_propertyKeys.constEnd(); ++it)
            m_metaData->insert(it.value(), QVariant(m_itemList->propertyType(it.key())));

        emit statusChanged();
        emit availableChanged();
    }
}

void GalleryItemRequest::_q_statusChanged(int index, int)
{
    if (index == 0)
        emit statusChanged();
}

void GalleryItemRequest::_q_metaDataChanged(int index, int, const QList<int> &keys)
{
    if (index == 0 && keys.isEmpty()) {
        typedef QHash<int, QString>::const_iterator iterator;
        for (iterator it = m_propertyKeys.constBegin(); it != m_propertyKeys.constEnd(); ++it) {
            QVariant value = m_itemList->metaData(0, it.key());
            m_metaData->insert(it.value(), value.isNull()
                    ? QVariant(m_itemList->propertyType(it.key()))
                    : value);
        }
    } else if (index == 0) {
        typedef QList<int>::const_iterator iterator;
        for (iterator it = keys.begin(); it != keys.end(); ++it){
            QVariant value = m_itemList->metaData(0, *it);
            m_metaData->insert(m_propertyKeys.value(*it), value.isNull()
                    ? QVariant(m_itemList->propertyType(*it))
                    : value);
        }
    }
}

void GalleryItemRequest::_q_valueChanged(const QString &key, const QVariant &value)
{
    if (m_itemList)
        m_itemList->setMetaData(0, key, value);
}

#include "moc_galleryitemrequest.cpp"

QTM_END_NAMESPACE
