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
#include "qgeoplacesmanagerengine.h"

#include "qlandmarkmanager.h"

QTM_BEGIN_NAMESPACE

/*!
    \class QGeoPlacesManager

    \brief The QGeoPlacesManager class provides support for searching
    operations related to geographic information.

    \ingroup maps-places

    The geocode(QGeoAddress), geocode(QGeoCoordinate) and placesSearch()
    methods return QGeoPlacesReply objects, which manage these operations and
    report on the result of the operations and any errors which may have
    occurred.

    The QGeoPlacesManager class also contains functions which provide
    information on the capabilities and features supported by QGeoPlacesManager
    instances. Those who write subclasses of QGeoPlacesManager should take care
    to make sure that this capability information is set up correctly,
    otherwise clients may be denied access to functionality they would
    otherwise expect.

    Instances of QGeoPlacesManager can be accessed with
    QGeoServiceProvider::placesManager().
*/

/*!
\enum QGeoPlacesManager::SearchType

Describes the type of search that should be performed by placesSearch().

\value SearchNone
    Do not use the search string.
\value SearchGeocode
    Use the search string as a textual address in a geocoding operation.
\value SearchLandmarks
    Use the search string for free-text search against the available landmark
    information sources.
\value SearchAll
    All available information sources should be used as part of the search.
*/

/*!
    Constructs a new manager with the specified \a parent and with the
    implementation provided by \a engine.

    This constructor is used interally by QGeoServiceProviderFactory. Regular
    users should aquire instance of QGeoPlacesManager with
    QGeoServiceProvider::placesManager();
*/
QGeoPlacesManager::QGeoPlacesManager(QGeoPlacesManagerEngine *engine, QObject *parent)
        : QObject(parent),
        d_ptr(new QGeoPlacesManagerPrivate())
{
    d_ptr->engine = engine;
    if (d_ptr->engine) {
        d_ptr->engine->setParent(this);

        connect(d_ptr->engine,
                SIGNAL(finished(QGeoPlacesReply*)),
                this,
                SIGNAL(finished(QGeoPlacesReply*)));

        connect(d_ptr->engine,
                SIGNAL(error(QGeoPlacesReply*, QGeoPlacesReply::Error, QString)),
                this,
                SIGNAL(error(QGeoPlacesReply*, QGeoPlacesReply::Error, QString)));
    } else {
        qFatal("The places manager engine that was set for this places manager was NULL.");
    }
}

/*!
    Destroys this manager.
*/
QGeoPlacesManager::~QGeoPlacesManager()
{
    delete d_ptr;
}

/*!
    Returns the name of the engine which implements the behaviour of this
    places manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
QString QGeoPlacesManager::managerName() const
{
//    if (!d_ptr->engine)
//        return QString();

    return d_ptr->engine->managerName();
}

/*!
    Returns the parameters used in the creation of this places manager.
*/
QMap<QString, QString> QGeoPlacesManager::managerParameters() const
{
//    if (!d_ptr->engine)
//        return QMap<QString, QString>();

    return d_ptr->engine->managerParameters();
}

/*!
    Returns the version of the engine which implements the behaviour of this
    places manager.

    The combination of managerName() and managerVersion() should be unique
    amongst the plugin implementations.
*/
int QGeoPlacesManager::managerVersion() const
{
//    if (!d_ptr->engine)
//        return -1;

    return d_ptr->engine->managerVersion();
}

/*!
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

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoPlacesManager::finished(),
    QGeoPlacesManager::error(), QGeoPlacesReply::finished() or
    QGeoPlacesReply::error() with deleteLater().
*/
QGeoPlacesReply* QGeoPlacesManager::geocode(const QGeoAddress &address, const QGeoBoundingBox &bounds)
{
//    if (!d_ptr->engine)
//        return new QGeoPlacesReply(QGeoPlacesReply::EngineNotSetError, "The places manager was not created with a valid engine.", this);

    return d_ptr->engine->geocode(address, bounds);
}


/*!
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

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoPlacesManager::finished(),
    QGeoPlacesManager::error(), QGeoPlacesReply::finished() or
    QGeoPlacesReply::error() with deleteLater().
*/
QGeoPlacesReply* QGeoPlacesManager::geocode(const QGeoCoordinate &coordinate, const QGeoBoundingBox &bounds)
{
//    if (!d_ptr->engine)
//        return new QGeoPlacesReply(QGeoPlacesReply::EngineNotSetError, "The places manager was not created with a valid engine.", this);

    return d_ptr->engine->geocode(coordinate, bounds);
}

/*!
    Begins searching for a place matching \a searchString.  The value of
    \a searchTypes will determine whether the search is for addresses only,
    for landmarks only or for both.

    A QGeoPlacesReply object will be returned, which can be used to manage the
    geocoding operation and to return the results of the operation.

    This manager and the returned QGeoPlacesReply object will emit signals
    indicating if the operation completes or if errors occur.

    If supportsGeocoding() returns false and \a searchTypes is
    QGeoPlacesManager::SearchGeocode an
    QGeoPlacesReply::UnsupportedOptionError will occur.

    Once the operation has completed, QGeoPlacesReply::places() can be used to
    retrieve the results, which will consist of a list of QGeoPlace objects.
    These object represent a combination of coordinate and address data.

    If any of the QGeoPlace instances in the results have landmark associated
    data, QGeoPlace::type() will return QGeoPlace::LandmarkType and
    QLandmark::QLandmark(const QGeoPlace &place) can be used to convert the
    QGeoPlace instance into a QLandmark instance.

    If \a searchTypes is QGeoPlacesManager::SearchLandmarks or
    QGeoPlacesManager::SearchAll, a free text landmark search will be
    performed. The results will be a combination of the backend specific
    landmark search and the same free text search applied to each of the
    QLandmarkManager instances in landmarkManagers().

    If \a bounds is a valid QGeoBoundingBox it will be used to limit the
    geocoding results to those that are contained by \a bounds. Note that \a
    bounds will only be used for the geocoding part of the search if
    supportsViewportBiasing() returns true.

    The user is responsible for deleting the returned reply object, although
    this can be done in the slot connected to QGeoPlacesManager::finished(),
    QGeoPlacesManager::error(), QGeoPlacesReply::finished() or
    QGeoPlacesReply::error() with deleteLater().
*/
QGeoPlacesReply* QGeoPlacesManager::placesSearch(const QString &searchString, QGeoPlacesManager::SearchTypes searchTypes, const QGeoBoundingBox &bounds)
{
//    if (!d_ptr->engine)
//        return new QGeoPlacesReply(QGeoPlacesReply::EngineNotSetError, "The places manager was not created with a valid engine.", this);

    return d_ptr->engine->placesSearch(searchString, searchTypes, bounds);
}

/*!
    Returns whether this manager supports viewport biasing.
*/
bool QGeoPlacesManager::supportsViewportBiasing() const
{
//    if (!d_ptr->engine)
//        return false;

    return d_ptr->engine->supportsViewportBiasing();
}

/*!
    Returns whether this manager supports geocoding operations.
*/
bool QGeoPlacesManager::supportsGeocoding() const
{
//    if (!d_ptr->engine)
//        return false;

    return d_ptr->engine->supportsGeocoding();
}

/*!
    Returns the search types supported by the placesSearch() with this manager.
*/
QGeoPlacesManager::SearchTypes QGeoPlacesManager::supportedSearchTypes() const
{
//    if (!d_ptr->engine)
//        return QGeoPlacesManager::SearchTypes();

    return d_ptr->engine->supportedSearchTypes();
}

/*!
    Returns the landmark manager provided by the service provider for
    use with placesSearch().

    Will return 0 if the no landmark manager is associated with the service
    provider. This does not indicate that placesSearch() does not support
    landmark searching, only that any landmark searching which occurs within in
    placesSearch() is done without the use of a QLandmarkManager.
*/
QLandmarkManager* QGeoPlacesManager::defaultLandmarkManager() const
{
//    if (!d_ptr->engine)
//        return 0;

    return d_ptr->engine->defaultLandmarkManager();
}

/*!
    Sets the landmark managers to be used with placesSearch() to \a landmarkManagers.

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
void QGeoPlacesManager::setAdditionalLandmarkManagers(const QList<QLandmarkManager *> &landmarkManagers)
{
//    if (d_ptr->engine)
        d_ptr->engine->setAdditionalLandmarkManagers(landmarkManagers);
}

/*!
    Returns the landmark managers that will be used with placesSearch().

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
QList<QLandmarkManager *> QGeoPlacesManager::additionalLandmarkManagers() const
{
//    if (!d_ptr->engine)
//        return QList<QLandmarkManager *>();

    return d_ptr->engine->additionalLandmarkManagers();
}

/*!
    Adds \a landmarkManager to the list of landmark managers that will be used with placesSearch().

    These landmark managers will be used along with the landmark manager returned
    by defaultLandmarkManager().
*/
void QGeoPlacesManager::addAdditionalLandmarkManager(QLandmarkManager *landmarkManager)
{
//    if (d_ptr->engine && landmarkManager)
    if(landmarkManager)
        d_ptr->engine->addAdditionalLandmarkManager(landmarkManager);
}

/*!
\fn void QGeoPlacesManager::finished(QGeoPlacesReply* reply)

    This signal is emitted when \a reply has finished processing.

    If reply::error() equals QGeoPlacesReply::NoError then the processing
    finished successfully.

    This signal and QGeoPlacesReply::finished() will be emitted at the same
    time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*!
\fn void QGeoPlacesManager::error(QGeoPlacesReply* reply, QGeoPlacesReply::Error error, QString errorString)

    This signal is emitted when an error has been detected in the processing of
    \a reply. The QGeoPlacesManager::finished() signal will probably follow.

    The error will be described by the error code \a error. If \a errorString is
    not empty it will contain a textual description of the error.

    This signal and QGeoPlacesReply::error() will be emitted at the same time.

    \note Do no delete the \a reply object in the slot connected to this
    signal. Use deleteLater() instead.
*/

/*******************************************************************************
*******************************************************************************/

QGeoPlacesManagerPrivate::QGeoPlacesManagerPrivate()
        : engine(0) {}

QGeoPlacesManagerPrivate::QGeoPlacesManagerPrivate(const QGeoPlacesManagerPrivate &other)
        : engine(other.engine) {}

QGeoPlacesManagerPrivate::~QGeoPlacesManagerPrivate()
{
    delete engine;
}

QGeoPlacesManagerPrivate& QGeoPlacesManagerPrivate::operator= (const QGeoPlacesManagerPrivate & other)
{
    engine = other.engine;

    return *this;
}

#include "moc_qgeoplacesmanager.cpp"

QTM_END_NAMESPACE
