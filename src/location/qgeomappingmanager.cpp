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

#include "qgeomappingmanager.h"
#include "qgeomappingmanager_p.h"
#include "qgeomappingmanagerengine.h"

#include <QNetworkProxy>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManager
    \brief The QGeoMappingManager class manages requests to and replies from
    a geographical map image service.
    \ingroup maps

    The request functions return a QGeoMapReply instance, which is responsible
    for delivering the status and results of the request.

    The rest of the class consists of functions providing metadata about the
    service provider, primarily dealing with the capabilities of the service
    and any limitations that may apply to the request inputs.

    The QGeoMapReply objects and the QGeoMappingManager instance will both
    emit signals to indicate that a request has completed successfully or
    has resulted in an error.

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoMapReply object at an appropriate time. Do not
    directly delete it inside the slot connected to replyFinished() or
    replyError() - the deleteLater() function should be used instead.

    \sa QGeoMapReply
*/


/*!
    \enum QGeoMappingManager::ErrorCode

    Describes the type of error which occurred when a QGeoMappingManager instance
    attempted to process a request.

    \value NoError
        No error occurred.
*/

/*!
    Constructs a QGeoMappingManager object.
*/
QGeoMappingManager::QGeoMappingManager(QGeoMappingManagerEngine *engine, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoMappingManagerPrivate)
{
    d_ptr->engine = engine;
    if (d_ptr->engine) {
        d_ptr->engine->setParent(this);

        /*
        connect(d_ptr->engine,
                SIGNAL(finished(QGeoMapReply*)),
                this,
                SIGNAL(finished(QGeoMapReply*)));

        connect(d_ptr->engine,
                SIGNAL(error(QGeoMapReply*,QGeoMapReply::Error,QString)),
                this,
                SIGNAL(error(QGeoMapReply*,QGeoMapReply::Error,QString)));
        */
    }
}

/*!
    Destroys this QGeoMappingManager object.
*/
QGeoMappingManager::~QGeoMappingManager()
{
    delete d_ptr;
}

QGeoMapViewport* QGeoMappingManager::createViewport(QGeoMapWidget *widget)
{
    if (!d_ptr->engine)
        return 0;

    return d_ptr->engine->createViewport(widget);
}

void QGeoMappingManager::removeViewport(QGeoMapViewport *viewport)
{
    if (d_ptr->engine)
        d_ptr->engine->removeViewport(viewport);
}

void QGeoMappingManager::updateMapImage(QGeoMapViewport *viewport)
{
    if (d_ptr->engine)
        d_ptr->engine->updateMapImage(viewport);
}

/*!
    Returns a list of the map types supported by this QGeoMappingManager
    instance.
*/
QList<QGeoMapWidget::MapType> QGeoMappingManager::supportedMapTypes() const
{
    if (!d_ptr->engine)
        return QList<QGeoMapWidget::MapType>();

    return d_ptr->engine->supportedMapTypes();
}

/*!
    Returns the minimum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManager::minimumZoomLevel() const
{
    // TODO document this behaviour
    if (!d_ptr->engine)
        return -1.0;

    return d_ptr->engine->minimumZoomLevel();
}

/*!
    Returns the maximum zoom level supported by this QGeoMappingManager
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManager::maximumZoomLevel() const

{
    // TODO document this behaviour
    if (!d_ptr->engine)
        return -1.0;

    return d_ptr->engine->maximumZoomLevel();
}

/*!
    Returns the size of the smallest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the minimum size of the map image.
*/
QSize QGeoMappingManager::minimumImageSize() const
{
    if (!d_ptr->engine)
        return QSize();

    return d_ptr->engine->minimumImageSize();
}

/*!
    Returns the size of the largest map image which is supported by this
    QGeoMappingManager instance.

    An invalid size indicates that this QGeoMappingManager instance places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingManager::maximumImageSize() const
{
    if (!d_ptr->engine)
        return QSize();

    return d_ptr->engine->maximumImageSize();
}

/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate()
        : engine(0) {}

QGeoMappingManagerPrivate::QGeoMappingManagerPrivate(const QGeoMappingManagerPrivate &other)
        : engine(other.engine) {}

QGeoMappingManagerPrivate::~QGeoMappingManagerPrivate()
{
    delete engine;
}

QGeoMappingManagerPrivate& QGeoMappingManagerPrivate::operator= (const QGeoMappingManagerPrivate & other)
{
    engine = other.engine;

    return *this;
}

#include "moc_qgeomappingmanager.cpp"

QTM_END_NAMESPACE
