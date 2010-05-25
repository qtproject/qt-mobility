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

#include "qgeoplacesmanager.h"
#include "qgeoplacesmanager_p.h"

#include "qlandmarkmanager.h"

QTM_BEGIN_NAMESPACE

/*!
  class doc
*/

/*!
\enum QGeoPlacesManager::SearchType

\value SearchGeocodeOnly
\value SearchLandmarksOnly
\value SearchAll
*/

/*!
*/
QGeoPlacesManager::QGeoPlacesManager(QObject *parent)
    : QObject(parent),
    d_ptr(new QGeoPlacesManagerPrivate()) {}

/*!
*/
QGeoPlacesManager::~QGeoPlacesManager()
{
    delete d_ptr;
}

/*!
\fn QGeoPlacesReply* QGeoPlacesManager::geocode(const QGeoAddress &address, const QGeoBoundingBox &bounds)

    Begins the geocoding of \a address. Geocoding is the process of finding a
    coordinate that corresponds to a given address.

    A QGeoPlacesReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This manager and the returned QGeoPlacesReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false an
    QGeoPlacesReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoPlacesReply::places() can be used to
    retrieve the results, which will consist of a list of QGeoPlace objects.
    These object represent a combination of coordinate and address data.

    The address data returned in the results may be different from \a address.
    This will usually occur if the geocoding service backend uses a different
    canonical form of addresses or if \a address was only partially filled out.

    If \a bounds is a valid QGeoBoundingBox it will be used to limit the
    geocoding results to those that are contained by \a bounds. This is
    particularly useful if \a address is only partially filled out, as the
    service will attempt to geocode all matches for the specified data. Note
    that \a bounds will only be used if supportsViewportBiasing() returns true.
*/

/*!
\fn QGeoPlacesReply* QGeoPlacesManager::geocode(const QGeoCoordinate &coordinate, const QGeoBoundingBox &bounds)

    Begins the reverse geocoding of \a coordinate. Reverse geocoding is the
    process of finding an address that corresponds to a given coordinate.

    A QGeoPlacesReply object will be returned, which can be used to manage the
    reverse geocoding operation and to return the results of the operation.

    This manager and the returned QGeoPlacesReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false an
    QGeoPlacesReply::UnsupportedOptionError will occur.

    At that point QGeoPlacesReply::places() can be used to retrieve the
    results, which will consist of a list of QGeoPlace objects. These object
    represent a combination of coordinate and address data.

    The coordinate data returned in the results may be different from \a
    coordinate. This will usually occur if the reverse geocoding service
    backend shifts the coordinates to be closer to the matching addresses, or
    if the backend returns results at multiple levels of detail.

    If multiple results are returned by the reverse geocoding service backend
    they will be provided in order of specificity. This normally occurs if the
    backend is configured to reverse geocode across multiple levels of detail.
    As an example, some services will return address and coordinate pairs for
    the street address, the city, the state and the country.

    If \a bounds is a valid QGeoBoundingBox it will be used to limit the
    geocoding results to those that are contained by \a bounds. Note that \a
    bounds will only be used if supportsViewportBiasing() returns true.
*/

/*!
\fn QGeoPlacesReply* QGeoPlacesManager::placesSearch(const QString &searchString, QGeoPlacesManager::SearchTypes searchTypes, const QGeoBoundingBox &bounds)



*/

/*!
    Sets whether this manager supports viewport biasing to \a supported.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it does not support viewport biasing.

*/
void QGeoPlacesManager::setSupportsViewportBiasing(bool supported)
{
    d_ptr->supportsViewportBiasing = supported;
}

/*!
    Returns whether this manager supports viewport biasing.
*/
bool QGeoPlacesManager::supportsViewportBiasing() const
{
    return d_ptr->supportsViewportBiasing;
}

/*!
    Sets whether this manager supports geocoding operations to \a supported.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it does not support geocoding operations.
*/
void QGeoPlacesManager::setSupportsGeocoding(bool supported)
{
    d_ptr->supportsGeocoding = supported;
}

/*!
    Returns whether this manager supports geocoding operations.
*/
bool QGeoPlacesManager::supportsGeocoding() const
{
    return d_ptr->supportsGeocoding;
}

/*!
    Sets the search types supported by the placesSearch() with this manager to \a searchTypes.

    It is important that subclasses use this method to ensure that the manager
    reports its capabilities correctly.  If this function is not used the
    manager will report that it does not support any search types.
*/
void QGeoPlacesManager::setSupportedSearchTypes(QGeoPlacesManager::SearchTypes searchTypes)
{
    d_ptr->supportedSearchTypes = searchTypes;
}

/*!
    Returns the search types supported by the placesSearch() with this manager.
*/
QGeoPlacesManager::SearchTypes QGeoPlacesManager::supportedSearchTypes() const
{
    return d_ptr->supportedSearchTypes;
}

/*!
    Sets the landmark managers to be used with placesSearch() to \a landmarkManagers.
*/
void QGeoPlacesManager::setLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers)
{
    for (int i = 0; i < landmarkManagers.size(); ++i)
        if (landmarkManagers.at(i))
            d_ptr->landmarkManagers.append(landmarkManagers.at(i));
}

/*!
    Returns the landmark managers that will be used with placesSearch().
*/
QList<QLandmarkManager *> QGeoPlacesManager::landmarkManagers() const
{
    return d_ptr->landmarkManagers;
}

/*!
    Adds \a landmarkManager to the list of landmark managers that will be used with placesSearch().
*/
void QGeoPlacesManager::addLandmarkManager(QLandmarkManager *landmarkManager)
{
    if (landmarkManager)
        d_ptr->landmarkManagers.append(landmarkManager);
}

/*!
\fn void QGeoPlacesManager::finished(QGeoPlacesReply* reply)

This signal is emitted when \a reply has finished processing.

If reply::error() equals QGeoPlacesReply::NoError then the processing
finished successfully.

This signal and QGeoPlacesReply::finished() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*!
\fn void QGeoPlacesManager::error(QGeoPlacesReply* reply, QGeoPlacesReply::Error error, QString errorString)

This signal is emitted when an error has been detected in the processing of
\a reply.  The QGeoPlacesManager::finished() signal will probably follow.

The error will be described by the error code \error.  If \a errorString is
not empty it will contain a textual description of the error.

This signal and QGeoPlacesReply::error() will be emitted at the same time.

\note Do no delete the \a reply object in the slot connected to this signal.
Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoPlacesManagerPrivate::QGeoPlacesManagerPrivate()
    : supportsViewportBiasing(false),
    supportsGeocoding(false) {}

QGeoPlacesManagerPrivate::QGeoPlacesManagerPrivate(const QGeoPlacesManagerPrivate &other)
    : landmarkManagers(other.landmarkManagers),
    supportsViewportBiasing(other.supportsViewportBiasing),
    supportsGeocoding(other.supportsGeocoding),
    supportedSearchTypes(other.supportedSearchTypes) {}

QGeoPlacesManagerPrivate::~QGeoPlacesManagerPrivate()
{
    qDeleteAll(landmarkManagers);
}

QGeoPlacesManagerPrivate& QGeoPlacesManagerPrivate::operator= (const QGeoPlacesManagerPrivate &other)
{
    landmarkManagers = other.landmarkManagers;
    supportsViewportBiasing = other.supportsViewportBiasing;
    supportsGeocoding = other.supportsGeocoding;
    supportedSearchTypes = other.supportedSearchTypes;

    return *this;
}

#include "moc_qgeoplacesmanager.cpp"

QTM_END_NAMESPACE
