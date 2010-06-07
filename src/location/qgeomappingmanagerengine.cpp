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

#include "qgeomappingmanagerengine.h"
#include "qgeomappingmanagerengine_p.h"

#include <QNetworkProxy>

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoMappingManagerEngine
    \brief The QGeoMappingManagerEngine class manages requests to and replies from
    a geographical map image service.
    \ingroup maps-impl

    The request functions return a QGeoMapReply instance, which is responsible
    for delivering the status and results of the request.

    The rest of the class consists of functions providing metadata about the
    service provider, primarily dealing with the capabilities of the service
    and any limitations that may apply to the request inputs.

    The QGeoMapReply objects and the QGeoMappingManagerEngine instance will both
    emit signals to indicate that a request has completed successfully or
    has resulted in an error.

    \note After the request has finished, it is the responsibility of the user
    to delete the QGeoMapReply object at an appropriate time. Do not
    directly delete it inside the slot connected to replyFinished() or
    replyError() - the deleteLater() function should be used instead.

    \sa QGeoMapReply
*/


/*!
    \enum QGeoMappingManagerEngine::ErrorCode

    Describes the type of error which occurred when a QGeoMappingManagerEngine instance
    attempted to process a request.

    \value NoError
        No error occurred.
*/

/*!
    Constructs a QGeoMappingManagerEngine object.
*/
QGeoMappingManagerEngine::QGeoMappingManagerEngine(const QMap<QString, QString> &parameters, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoMappingManagerEnginePrivate(parameters)) {}

QGeoMappingManagerEngine::QGeoMappingManagerEngine(QGeoMappingManagerEnginePrivate *dd, QObject *parent)
        : QObject(parent),
        d_ptr(dd) {}

/*!
    Destroys this QGeoMappingManagerEngine object.
*/
QGeoMappingManagerEngine::~QGeoMappingManagerEngine()
{
    Q_D(QGeoMappingManagerEngine);
    delete d;
}

/*!
*/
void QGeoMappingManagerEngine::setManagerName(const QString &managerName)
{
    d_ptr->managerName = managerName;
}

/*!
*/
QString QGeoMappingManagerEngine::managerName() const
{
    return d_ptr->managerName;
}

/*!
*/
QMap<QString, QString> QGeoMappingManagerEngine::managerParameters() const
{
    return d_ptr->managerParameters;
}

/*!
*/
void QGeoMappingManagerEngine::setManagerVersion(int managerVersion)
{
    d_ptr->managerVersion = managerVersion;
}

/*!
*/
int QGeoMappingManagerEngine::managerVersion() const
{
    return d_ptr->managerVersion;
}

void QGeoMappingManagerEngine::removeViewport(QGeoMapViewport *viewport)
{
    Q_UNUSED(viewport)
}

/*!
    Returns a list of the map types supported by this QGeoMappingManagerEngine
    instance.
*/
QList<QGeoMapWidget::MapType> QGeoMappingManagerEngine::supportedMapTypes() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->supportedMapTypes;
}

/*!
    Returns the minimum zoom level supported by this QGeoMappingManagerEngine
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManagerEngine::minimumZoomLevel() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->minimumZoomLevel;
}

/*!
    Returns the maximum zoom level supported by this QGeoMappingManagerEngine
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.
*/
qreal QGeoMappingManagerEngine::maximumZoomLevel() const

{
    Q_D(const QGeoMappingManagerEngine);
    return d->maximumZoomLevel;
}

/*!
    Returns the size of the smallest map image which is supported by this
    QGeoMappingManagerEngine instance.

    An invalid size indicates that this QGeoMappingManagerEngine instance places
    no restrictions on the minimum size of the map image.
*/
QSize QGeoMappingManagerEngine::minimumImageSize() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->minimumImageSize;
}

/*!
    Returns the size of the largest map image which is supported by this
    QGeoMappingManagerEngine instance.

    An invalid size indicates that this QGeoMappingManagerEngine instance places
    no restrictions on the maximum size of the map image.
*/
QSize QGeoMappingManagerEngine::maximumImageSize() const
{
    Q_D(const QGeoMappingManagerEngine);
    return d->maximumImageSize;
}

/*!
    Sets the list of map types supported by this QGeoMappingManagerEngine instance to
    \a mapTypes.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedMapTypes() provides accurate information.

    \sa QGeoMapWidget::MapType
    \sa QGeoMappingManagerEngine::supportedMapTypes()
*/
void QGeoMappingManagerEngine::setSupportedMapTypes(const QList<QGeoMapWidget::MapType> &mapTypes)
{
    Q_D(QGeoMappingManagerEngine);
    d->supportedMapTypes = mapTypes;
}

/*!
    Sets the minimum zoom level supported by this QGeoMappingManagerEngine
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumZoomLevel() provides accurate information.
*/
void QGeoMappingManagerEngine::setMinimumZoomLevel(qreal minimumZoom)
{
    Q_D(QGeoMappingManagerEngine);
    d->minimumZoomLevel = minimumZoom;
}

/*!
    Sets the maximum zoom level supported by this QGeoMappingManagerEngine
    instance.

    Larger values of the zoom level correspond to more detailed views of the
    map.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumZoomLevel() provides accurate information.
*/
void QGeoMappingManagerEngine::setMaximumZoomLevel(qreal maximumZoom)
{
    Q_D(QGeoMappingManagerEngine);
    d->maximumZoomLevel = maximumZoom;
}

/*!
    Sets the size of the smallest map image which is supported by this
    QGeoMappingManagerEngine instance.

    An invalid size indicates that this QGeoMappingManagerEngine instance places
    no restrictions on the minimum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    minimumImageSize() provides accurate information.
*/
void QGeoMappingManagerEngine::setMinimumImageSize(const QSize &minimumImageSize)
{
    Q_D(QGeoMappingManagerEngine);
    d->minimumImageSize = minimumImageSize;
}

/*!
    Sets the size of the largest map image which is supported by this
    QGeoMappingManagerEngine instance.

    An invalid size indicates that this QGeoMappingManagerEngine instance places
    no restrictions on the maximum size of the map image.

    Subclasses of QGeoCodingService should use this function to ensure that
    maximumImageSize() provides accurate information.
*/
void QGeoMappingManagerEngine::setMaximumImageSize(const QSize &maximumImageSize)
{
    Q_D(QGeoMappingManagerEngine);
    d->maximumImageSize = maximumImageSize;
}


/*******************************************************************************
*******************************************************************************/

QGeoMappingManagerEnginePrivate::QGeoMappingManagerEnginePrivate(const QMap<QString, QString> &parameters)
        : managerParameters(parameters),
        managerVersion(-1) {}

QGeoMappingManagerEnginePrivate::QGeoMappingManagerEnginePrivate(const QGeoMappingManagerEnginePrivate &other)
        : managerName(other.managerName),
        managerParameters(other.managerParameters),
        managerVersion(other.managerVersion),
        supportedMapTypes(other.supportedMapTypes),
        minimumZoomLevel(other.minimumZoomLevel),
        maximumZoomLevel(other.maximumZoomLevel),
        minimumImageSize(other.minimumImageSize),
        maximumImageSize(other.maximumImageSize) {}

QGeoMappingManagerEnginePrivate::~QGeoMappingManagerEnginePrivate() {}

QGeoMappingManagerEnginePrivate& QGeoMappingManagerEnginePrivate::operator= (const QGeoMappingManagerEnginePrivate & other)
{
    managerName = other.managerName;
    managerParameters = other.managerParameters;
    managerVersion = other.managerVersion;
    supportedMapTypes = other.supportedMapTypes;
    minimumZoomLevel = other.minimumZoomLevel;
    maximumZoomLevel = other.maximumZoomLevel;
    minimumImageSize = other.minimumImageSize;
    maximumImageSize = other.maximumImageSize;

    return *this;
}

#include "moc_qgeomappingmanagerengine.cpp"

QTM_END_NAMESPACE
