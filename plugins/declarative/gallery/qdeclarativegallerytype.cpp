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

#include "qdeclarativegallerytype.h"

#include <qgalleryresultset.h>

#include <QtDeclarative/qdeclarativepropertymap.h>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass GalleryType QDeclarativeGalleryType

    \inmodule QtGallery
    \ingroup qml-gallery

    \brief The GalleryType element allows you to request information about an
    item type from a gallery

    This element is part of the \bold {QtMobility.gallery 1.1} module.

    \sa GalleryQueryModel, GalleryItem
*/

QDeclarativeGalleryType::QDeclarativeGalleryType(QObject *parent)
    : QObject(parent)
    , m_metaData(0)
    , m_status(Null)
    , m_complete(false)
{
    connect(&m_request, SIGNAL(stateChanged(QGalleryAbstractRequest::State)),
            this, SLOT(_q_stateChanged()));
    connect(&m_request, SIGNAL(resultChanged()), this, SLOT(_q_stateChanged()));
    connect(&m_request, SIGNAL(progressChanged(int,int)), this, SIGNAL(progressChanged()));

    connect(&m_request, SIGNAL(typeChanged()),
            this, SLOT(_q_typeChanged()));
    connect(&m_request, SIGNAL(metaDataChanged(QList<int>)),
            this, SLOT(_q_metaDataChanged(QList<int>)));

    m_metaData = new QDeclarativePropertyMap(this);
}

QDeclarativeGalleryType::~QDeclarativeGalleryType()
{
}

/*!
    \qmlproperty QAbstractGallery GalleryType::gallery

    This property holds the gallery type information should be requested from.
*/

/*!
    \qmlproperty enum GalleryType::status

    This property holds the status of a type request.  It can be one of:

    \list
    \o Null No \l itemType has been specified.
    \o Active Information about an \l itemType is being fetched from the gallery.
    \o Finished Information about an \l itemType is available.
    \o Idle Information about an \l itemType which will be automatically
    updated is available.
    \o Cancelling The query was cancelled but hasn't yet reached the
    cancelled status.
    \o Cancelled The query was cancelled.
    \o Error Information about a type could not be retrieved due to an error.
    \endlist
*/

/*!
    \qmlproperty real GalleryType::progress

    This property holds the current progress of the request, from 0.0 (started)
    to 1.0 (finished).
*/

/*!
    \qmlproperty int GalleryType::maximumProgress

    This property holds the maximum progress value.
*/

/*!
    \qmlproperty QStringList GalleryType::properties

    This property holds the type properties a request should return values for.
*/

/*!
    \qmlproperty bool GalleryType::autoUpdate

    This property holds whether a request should refresh its results
    automatically.
*/

/*!
    \qmlproperty string GalleryType::itemType

    This property holds the item type that a request fetches information about.
*/

/*!
    \qmlproperty bool GalleryType::available

    This property holds whether the meta-data of a type is available.
*/

/*!
    \qmlproperty object GalleryType::metaData

    This property holds the meta-data of an item type/
*/

/*!
    \qmlmethod GalleryType::reload()

    Re-queries the gallery.
*/

/*!
    \qmlmethod GalleryType::cancel()

    Cancels an executing request.
*/

/*!
    \qmlmethod GalleryType::clear()

    Clears the results of a request.
*/

/*!
    \qmlsignal GalleryType::onSucceeded()

    Signals that a request has finished successfully.
*/

/*!
    \qmlsignal GalleryType::onCancelled()

    Signals that a request was cancelled.
*/

/*!
    \qmlsignal GalleryType::onFailed(error)

    Signals that a request failed with the given \a error.
*/

/*!
    \qmlsignal GalleryType::onFinished(result)

    Signals that a request finished with the given \a result.
*/

void QDeclarativeGalleryType::classBegin()
{
}

void QDeclarativeGalleryType::componentComplete()
{
    m_complete = true;

    if (!m_request.itemType().isEmpty())
        m_request.execute();
}

void QDeclarativeGalleryType::_q_stateChanged()
{
    Status status = m_status;

    switch (m_request.state()) {
    case QGalleryAbstractRequest::Inactive: {
            switch (m_request.result()) {
            case QGalleryAbstractRequest::NoResult:
                status = Null;
                break;
            case QGalleryAbstractRequest::Cancelled:
                status = Cancelled;
                break;
            case QGalleryAbstractRequest::Succeeded:
                status = Finished;
                break;
            default:
                status = Error;
                break;
            }
        }
    case QGalleryAbstractRequest::Active:
        status = Active;
        break;
    case QGalleryAbstractRequest::Cancelling:
        status = Cancelling;
        break;
    case QGalleryAbstractRequest::Idle:
        status = Idle;
        break;
    }

    if (m_status != status) {
        m_status = status;

        emit statusChanged();
    }
}

void QDeclarativeGalleryType::_q_typeChanged()
{
    if (m_request.isValid()) {
        for (QHash<int, QString>::const_iterator it = m_propertyKeys.constBegin();
                it != m_propertyKeys.constEnd();
                ++it) {
            if (m_request.propertyKey(it.value()) < 0)
                m_metaData->clear(it.value());
        }

        m_propertyKeys.clear();

        const QStringList propertyNames = m_request.propertyNames();

        for (QStringList::const_iterator it = propertyNames.begin(); it != propertyNames.end(); ++it) {
            const int key = m_request.propertyKey(*it);

            if (key >= 0) {
                m_propertyKeys.insert(key, *it);

                QVariant value = m_request.metaData(key);
                m_metaData->insert(*it, value.isNull()
                        ? QVariant(m_request.propertyType(key))
                        : value);
            }
        }
    } else {
        typedef QHash<int, QString>::const_iterator iterator;
        for (iterator it = m_propertyKeys.constBegin(); it != m_propertyKeys.constEnd(); ++it)
            m_metaData->clear(it.value());

        m_propertyKeys.clear();
    }

    emit availableChanged();
}

void QDeclarativeGalleryType::_q_metaDataChanged(const QList<int> &keys)
{
    typedef QList<int>::const_iterator iterator;
    for (iterator it = keys.begin(); it != keys.end(); ++it){
        QVariant value = m_request.metaData(*it);
        m_metaData->insert(m_propertyKeys.value(*it), value.isNull()
                ? QVariant(m_request.propertyType(*it))
                : value);
    }
}


#include "moc_qdeclarativegallerytype.cpp"

QTM_END_NAMESPACE
