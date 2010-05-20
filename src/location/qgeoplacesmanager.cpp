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
*/

/*!
\fn QGeoPlacesReply* QGeoPlacesManager::geocode(const QGeoCoordinate &coordinate, const QGeoBoundingBox &bounds)
*/

/*!
\fn QGeoPlacesReply* QGeoPlacesManager::placesSearch(const QString &searchString, QGeoPlacesManager::SearchTypes searchTypes, const QGeoBoundingBox &bounds)
*/

/*!
*/
void QGeoPlacesManager::setSupportsViewportBiasing(bool supported)
{
    d_ptr->supportsViewportBiasing = supported;
}

/*!
*/
bool QGeoPlacesManager::supportsViewportBiasing() const
{
    return d_ptr->supportsViewportBiasing;
}

/*!
*/
void QGeoPlacesManager::setSupportsGeocoding(bool supported)
{
    d_ptr->supportsGeocoding = supported;
}

/*!
*/
bool QGeoPlacesManager::supportsGeocoding() const
{
    return d_ptr->supportsGeocoding;
}

/*!
*/
void QGeoPlacesManager::setSupportedSearchTypes(QGeoPlacesManager::SearchTypes searchTypes)
{
    d_ptr->supportedSearchTypes = searchTypes;
}

/*!
*/
QGeoPlacesManager::SearchTypes QGeoPlacesManager::supportedSearchTypes() const
{
    return d_ptr->supportedSearchTypes;
}

///*!
//*/
//void QGeoPlacesManager::setLandmarkManagers(const QList<QLandmarkManager> &landmarkManagers)
//{
//    d_ptr->landmarkManagers = landmarkManagers;
//}

///*!
//*/
//QList<QLandmarkManager> QGeoPlacesManager::landmarkManagers() const
//{
//    return d_ptr->landmarkManagers;
//}

///*!
//*/
//void QGeoPlacesManager::addLandmarkManager(const QLandmarkManager &landmarkManager)
//{
//    d_ptr->landmarkManagers.append(landmarkManager);
//}

/*!
\fn void QGeoPlacesManager::finished(QGeoPlacesReply* reply)
*/

/*!
\fn void QGeoPlacesManager::error(QGeoPlacesReply* reply, QGeoPlacesReply::Error error, QString errorString)
*/

/*******************************************************************************
*******************************************************************************/

QGeoPlacesManagerPrivate::QGeoPlacesManagerPrivate()
    : supportsViewportBiasing(false),
    supportsGeocoding(false) {}

QGeoPlacesManagerPrivate::QGeoPlacesManagerPrivate(const QGeoPlacesManagerPrivate &other)
    : //landmarkManagers(other.landmarkManagers),
    supportsViewportBiasing(other.supportsViewportBiasing),
    supportsGeocoding(other.supportsGeocoding),
    supportedSearchTypes(other.supportedSearchTypes) {}

QGeoPlacesManagerPrivate::~QGeoPlacesManagerPrivate() {}

QGeoPlacesManagerPrivate& QGeoPlacesManagerPrivate::operator= (const QGeoPlacesManagerPrivate &other)
{
    //landmarkManagers = other.landmarkManagers;
    supportsViewportBiasing = other.supportsViewportBiasing;
    supportsGeocoding = other.supportsGeocoding;
    supportedSearchTypes = other.supportedSearchTypes;

    return *this;
}

#include "moc_qgeoplacesmanager.cpp"

QTM_END_NAMESPACE
