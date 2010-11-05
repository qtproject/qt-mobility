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

#include "databaseoperations_p.h"
#include "qlandmarkfilehandler_gpx_p.h"
#include "qlandmarkfilehandler_lmx_p.h"
#include "qlandmarkmanagerengine_qsparql_p.h"

#include <qlandmarknamefilter.h>
#include <qlandmarkproximityfilter.h>
#include <qlandmarkcategoryfilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkintersectionfilter.h>
#include <qlandmarkunionfilter.h>
#include <qlandmarkattributefilter.h>

#include <qlandmarksortorder.h>
#include <qlandmarknamesort.h>
#include <qlandmarkidfilter.h>

#include <qlandmarkabstractrequest.h>
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarkfetchbyidrequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarkcategoryidfetchrequest.h>
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategoryfetchbyidrequest.h>
#include <qlandmarkcategorysaverequest.h>
#include <qlandmarkcategoryremoverequest.h>
#include <qlandmarkimportrequest.h>
#include <qlandmarkexportrequest.h>

#include <QFile>
#include <QDebug>
#include <math.h>
#include <QUuid>
#include <QSet>
#include <QMetaMethod>
#include <qnumeric.h>
#include <QXmlStreamReader>
#include <QStringList>

#include <QtSparql/QSparqlConnection>
#include <QtSparql/QSparqlConnectionOptions>
#include <QtSparql/QSparqlQuery>
#include <QtSparql/QSparqlResult>

QTM_USE_NAMESPACE

QTM_BEGIN_NAMESPACE
uint qHash(const QLandmarkId& key) {
    return qHash(key.localId());
}
QTM_END_NAMESPACE

namespace DatabaseOperationsHelpers{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double EARTH_MEAN_RADIUS = 6371.0072;

QString quoteString(const QString &s)
{
    QString q = "\"";
    q += s;
    q += "\"";
    return q;
}

bool isValidLat(double lat)
{
    return -90.0 <= lat && lat <=90.0;
}

bool isValidLong(double lng)
{
    return -180.0 <= lng && lng <= 180.0;
}

bool matchString(const QString &sourceString, const QString &matchString, QLandmarkFilter::MatchFlags matchFlags )
{
    Qt::CaseSensitivity cs;
    if (matchFlags & QLandmarkFilter::MatchCaseSensitive)
        cs = Qt::CaseSensitive;
    else
        cs = Qt::CaseInsensitive;
    if ((matchFlags & 3) == QLandmarkFilter::MatchEndsWith) {
        return sourceString.endsWith(matchString, cs);
    } else if ((matchFlags & 3) == QLandmarkFilter::MatchStartsWith) {
        return sourceString.startsWith(matchString, cs);
    } else if ((matchFlags & 3) == QLandmarkFilter::MatchContains) {
        return sourceString.contains(matchString,cs);
    } else if (matchFlags & QLandmarkFilter::MatchFixedString) {
        return sourceString.compare(matchString,cs) == 0;
    } else {
        return QVariant(sourceString) == QVariant(matchString);
    }
}

/*
    \internal
    Returns true if value is in between min and max(inclusive of min and max)
*/
bool inBetween(double value, double min, double max)
{
    if((min < value || qFuzzyCompare(value,min)) && (value < max || qFuzzyCompare(value,max)))
        return true;
    else
        return false;
}

/*
    \internal
    Returns true if value is outside the range from min to max.  Values at exactly min or max
    are considered outside the range.
*/
bool outside(double value, double min, double max)
{
    if ((value < min || qFuzzyCompare(value,min)) || (value > max || qFuzzyCompare(value,max)))
        return true;
    else
        return false;
}

double normalizeLongitude(double degrees)
{
    double newDegree = degrees;
    while (newDegree <= -180) newDegree += 360;
    while (newDegree > 180) newDegree -= 360;
    return newDegree;
}

double normalizeLatitude(double degrees)
{
    double newDegree = degrees;
    while (newDegree < -90) newDegree = -90;
    while (newDegree > 90) newDegree = 90;
    return newDegree;
}

void shiftCoordinate(QGeoCoordinate *coord,double bearing, double distance)
{
    if (!coord)
        return;

    //convert from degrees to radians
    double lat1= coord->latitude() * M_PI / 180;
    double long1 = coord->longitude() * M_PI / 180;
    double bear = bearing * M_PI / 180;

    double lat2 = asin(sin(lat1) * cos(distance/(EARTH_MEAN_RADIUS *1000))
                    + cos(lat1) * sin(distance/(EARTH_MEAN_RADIUS*1000)) * cos(bear));
    double long2 = long1 + atan2(sin(bear) * sin(distance/(EARTH_MEAN_RADIUS*1000)) * cos(lat1),
                                            cos(distance/(EARTH_MEAN_RADIUS * 1000)) - sin(lat1) * sin(lat2));

    //convert from radians to degrees
    lat2 = lat2 * 180.0 / M_PI;
    long2 = long2 * 180.0 / M_PI;

    coord->setLatitude(normalizeLatitude(lat2));
    coord->setLongitude(normalizeLongitude(long2));
}

class LandmarkPoint
{
    public:
        QGeoCoordinate coordinate;
        QLandmarkId landmarkId;
};

int compareDistance(const QGeoCoordinate &a, const QGeoCoordinate &b, const QGeoCoordinate &c)
{
    int result = 0;


    if (a.isValid()) {
        if (b.isValid()) {
            qreal da = c.distanceTo(a);
            qreal db = c.distanceTo(b);

            if (qFuzzyCompare(da,db)) {
                result = 0;
            } else if (da < db) {
                result = -1;
            } else if (da > db) {
                result = 1;
            }
        } else {
            result = -1;
        }
    } else {
        if (b.isValid()) {
            result = 1;
        } else {
            result = 0;
        }
    }

    return result;
}

void addSortedPoint(QList<LandmarkPoint>* sorted, const LandmarkPoint& point, const QGeoCoordinate &center)
{

        for (int i = 0; i < sorted->size(); i++) {
            // check to see if the new landmark should be inserted here
            int comparison = compareDistance(sorted->at(i).coordinate, point.coordinate,center);
            if (comparison > 0) {
                sorted->insert(i, point);
                return;
            }
        }

    // hasn't been inserted yet?  append to the list.
    sorted->append(point);
}

//query must have selected columns: id, latitude, longitude
QList<QLandmarkId> sortQueryByDistance(QSparqlResult *qsparqlResult, const QLandmarkProximityFilter &proximityFilter,
                                       QLandmarkManager::Error * error, QString *errorString,
                                       const QString &managerUri,
                                       QueryRun * queryRun =0){
    QList<QLandmarkId> result;
    QList<LandmarkPoint>  sortedPoints;
    LandmarkPoint point;

    qreal radius = proximityFilter.radius();
    QGeoCoordinate center = proximityFilter.center();

    while(qsparqlResult->next()) {
        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Fetch operation canceled";
            return QList<QLandmarkId>();
        }

        point.coordinate.setLatitude(qsparqlResult->value(1).toDouble());
        point.coordinate.setLongitude(qsparqlResult->value(2).toDouble());

        point.landmarkId.setManagerUri(managerUri);
        point.landmarkId.setLocalId(QString::number(qsparqlResult->value(0).toInt()));

        if (radius < 0 || (point.coordinate.distanceTo(center) < radius)
            || qFuzzyCompare(point.coordinate.distanceTo(center), radius)) {
            addSortedPoint(&sortedPoints,point,center);
        }
    }

    for (int i=0;i < sortedPoints.count(); ++i) {
        result << sortedPoints.at(i).landmarkId;
    }
    return result;
}

QSparqlResult* executeQuery(QSparqlConnection *conn, const QString &statement, const QMap<QString,QVariant> &bindValues,
                QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    QSparqlQuery qsparqlSelectQuery = QSparqlQuery(statement, QSparqlQuery::SelectStatement);
    qsparqlSelectQuery.unbindValues();
    QStringList keys = bindValues.keys();
    if (bindValues.count()!=0) {
        foreach(const QString &key, keys)
            qsparqlSelectQuery.bindValue(key, bindValues.value(key));
    }
   QSparqlResult* queryResult = conn->exec(qsparqlSelectQuery);
   queryResult->waitForFinished();

   if (!queryResult->hasError()) {
       *error = QLandmarkManager::NoError;
       *errorString ="";
   } else {
       *error = QLandmarkManager::UnknownError;
       *errorString ="";
   }
   return queryResult;
}

QString queryStringForRadius(const QGeoCoordinate &coord, qreal radius)
{
    Q_UNUSED(radius);
    Q_UNUSED(coord);
    return "select ?u ?latitude ?longitude { ?g a slo:GeoLocation ; "
       "nie:title ?name ; "
       "slo:latitude ?latitude ; "
       "slo:longitude ?longitude . "
       "?u slo:location ?g }";
}

QString landmarkIdsDefaultQueryString()
{
     return "select ?u ?latitude ?longitude { ?g a slo:GeoLocation ; "
        "slo:latitude ?latitude ; "
        "slo:longitude ?longitude . "
        "?u slo:location ?g }";
 }

QString landmarkIdsQueryString(const QList<QLandmarkId> ids)
{
    QString queryString = "select ?u ?latitude ?longitude "
    "{?g a slo:GeoLocation ; nie:title ?name ; slo:latitude ?latitude ; "
    "slo:longitude ?longitude . ?u slo:location ?g . FILTER (?name IN ('u',";
    foreach(const QLandmarkId &id, ids) {
        queryString += "'";
        queryString += id.localId() += "',";
    }
    if (ids.count() > 0)
        queryString.chop(1);

    queryString += "))}";
    return queryString;
}

QString landmarkIdsNameQueryString(const QLandmarkNameFilter &filter)
{
    if (filter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
        return QString("select ?u  {?g a slo:GeoLocation ; nie:title ?name . ?u slo:location ?g . FILTER regex( ?name, '%1') }").arg(filter.name());
    else
        return QString("select ?u  {?g a slo:GeoLocation ; nie:title ?name . ?u slo:location ?g . FILTER regex( ?name, '%1', 'i') }").arg(filter.name());
}

QString landmarkIdsCategoryQueryString(const QLandmarkCategoryFilter &filter)
{
    return QString("select ?u {?u slo:belongsToCategory '^%1$' . }").arg(filter.categoryId().localId());
}

QString landmarkIdsBoxQueryString(const QLandmarkBoxFilter &filter)
{
    double tly = filter.boundingBox().topLeft().latitude();
    double bry = filter.boundingBox().bottomRight().latitude();
    double tlx = filter.boundingBox().topLeft().longitude();
    double brx = filter.boundingBox().bottomRight().longitude();

    //note: it is already assumed tly > bry
    bool lonWrap = (tlx > brx);

    QString queryString = "select ?u ?latitude ?longitude {?g a slo:GeoLocation ; nie:title ?name ; "
        "slo:latitude ?latitude ; slo:longitude ?longitude . ?u slo:location ?g . FILTER ((";

    //TODO: handle case where the poles are covered by the bounding box

    queryString += QString::number(bry);
    queryString += " <= ?latitude && ?latitude <= ";
    queryString += QString::number(tly);

    queryString += ") && (";

    if (lonWrap) {
        queryString += QString::number(tlx - 0.001);
        queryString += " <= ?longitude || ?longitude <= ";
        queryString += QString::number(brx + 0.001);
    } else {
        queryString += QString::number(tlx - 0.001);
        queryString += " <= ?longitude && ?longitude <= ";
        queryString += QString::number(brx + 0.001);
    }

    queryString += "))}";
    return queryString;
}

bool removeLandmarkHelper(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString,
        const QString &managerUri)
{
    if (landmarkId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return false;
    }
    QSparqlConnection conn("QTRACKER");
    QSparqlQuery qsparqlDeleteQuery = QSparqlQuery(  
            "delete { ?pn a rdfs:Resource . }"
            "WHERE { ?c nco:hasPhoneNumber ?pn . "
            "?c a nco:PersonContact . "
            "?:landmark_uri slo:hasContact ?c . }"
            "delete { ?c a nco:PersonContact . }"
            "WHERE { ?:landmark_uri slo:hasContact ?c . } "
            "delete { ?pa a nco:PostalAddress . }"
            "WHERE { ?g slo:postalAddress ?pa . "
            "?:landmark_uri slo:location ?g . }"
            "delete { ?g a slo:GeoLocation . }"
            "WHERE { ?:landmark_uri slo:location ?g . } "
            "delete { ?:landmark_uri a slo:Landmark . }",
            QSparqlQuery::DeleteStatement);

    qsparqlDeleteQuery.unbindValues();
    qsparqlDeleteQuery.bindValue("landmark_uri", QUrl(landmarkId.localId()));

    QSparqlResult* deleteResult = conn.exec(qsparqlDeleteQuery);
    deleteResult->waitForFinished();
    if (deleteResult->hasError()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Unable to execute remove landmark statement.2");
        return false;
    }
    return true;
}

bool categoryNameCompare(const QLandmarkCategory &cat1, const QLandmarkCategory &cat2) {
    return (cat1.name() < cat2.name());
}

}

/////////////////////////////////////////////////////////////
/////////////// Database Operation functions
///////////////
/////////////////////////////////////////////////////////////

using namespace DatabaseOperationsHelpers;

DatabaseOperations::DatabaseOperations()
    : connectionName(QString()),
    managerUri(QString()),
    queryRun(0)
{
    m_conn = new QSparqlConnection("QTRACKER");
}

DatabaseOperations::~DatabaseOperations()
{
    delete m_conn;
}

QLandmark DatabaseOperations::retrieveLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";
    QString uri = managerUri;
    if (landmarkId.managerUri() != uri) {
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return QLandmark();
    }

    if (landmarkId.localId().isEmpty()) {
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = "Landmark local id is empty";
        return QLandmark();
    }
    QLandmark lm;
    if (queryRun && queryRun->isCanceled) {
        return QLandmark();
    }

    QString queryString = QString("select ?u ?name ?latitude ?longitude ?altitude {?g a slo:GeoLocation . ?u slo:location ?g . "
        "OPTIONAL { ?g nie:title ?name } . OPTIONAL { ?g slo:latitude ?latitude } . "
        "OPTIONAL { ?g slo:longitude ?longitude } . OPTIONAL { ?g slo:altitude ?altitude } . "
        "FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());

    QSparqlQuery qsparqlQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);

    QSparqlResult* qsparqlResult = m_conn->exec(qsparqlQuery);
    qsparqlResult->waitForFinished();
    if (qsparqlResult->hasError()) {
        if (error)
            *error =  QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = QString("Landmark retrieve Query Failed.");
        return QLandmark();
    }

    if (queryRun && queryRun->isCanceled) {
         return QLandmark();
     }

    bool found=false;
    while (qsparqlResult->next()) {
        if (found) {
            if (error)
                *error = QLandmarkManager::UnknownError;
            if (errorString)
                *errorString = QString("Landmark database is corrupted.  Two landmarks were found with the same id: %1")
                                .arg(landmarkId.localId());
            return QLandmark();
        } else {
            found = true;
        }
        if (qsparqlResult->value(0).toString().isEmpty()) {
            if (error)
                *error = QLandmarkManager::LandmarkDoesNotExistError;
            if (errorString)
                *errorString = "Landmark does not exist in database.";
            return QLandmark();
        }
        if (!qsparqlResult->value(1).toString().isEmpty()) {
            lm.setName(qsparqlResult->value(1).toString());
        }
        bool ok;
        QGeoCoordinate coord;

        if (!qsparqlResult->value(2).toString().isEmpty()) {
            coord.setLatitude(qsparqlResult->value(2).toDouble(&ok));
            if (!ok) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Landmark database is corrupted. Latitude is not a double for landmark id: %1")
                               .arg(landmarkId.localId());
                return QLandmark();
            }
        }
        if (!qsparqlResult->value(3).toString().isEmpty()) {
            coord.setLongitude(qsparqlResult->value(3).toDouble(&ok));
            if (!ok) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Landmark database is corrupted. Longitude is not a double for landmark id: %1")
                               .arg(landmarkId.localId());
                return QLandmark();
            }
        }
        if (!qsparqlResult->value(4).toString().isEmpty()) {
            coord.setAltitude(qsparqlResult->value(4).toDouble(&ok));
            if (!ok) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Landmark database is corrupted. Altitude is not a double for landmark id: %1")
                               .arg(landmarkId.localId());
                return QLandmark();
            }
        }
        lm.setCoordinate(coord);
    }
    if (found) {
        lm.setLandmarkId(landmarkId);
        QString queryString2 = QString("select ?u {?u a slo:LandmarkCategory . "
                                              "?landmarkId slo:belongsToCategory ?u . FILTER regex( ?landmarkId, '^%1$') }").arg(landmarkId.localId());
        QSparqlQuery qsparqlQuery2 = QSparqlQuery(queryString2, QSparqlQuery::SelectStatement);
        QSparqlResult* qsparqlResult2 = m_conn->exec(qsparqlQuery2);
        qsparqlResult2->waitForFinished();

        if (qsparqlResult2->hasError()) {
            if (error)
                *error =  QLandmarkManager::BadArgumentError;
            if (errorString)
                 *errorString = QString("Retrieve Query Failed.");
           return QLandmark();
        }
        while (qsparqlResult2->next()) {
            if (queryRun && queryRun->isCanceled) {
                return QLandmark();
            }
            if (!qsparqlResult2->value(0).toString().isEmpty()) {
                QLandmarkCategoryId id;
                id.setManagerUri(uri);
                id.setLocalId(qsparqlResult2->value(0).toString());
                lm.addCategoryId(id);
            }
        }
        QMap<QString,QVariant> bindValues;
        QString q0 = QString("select ?a {?u a slo:Landmark ; slo:iconUrl ?a . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
        QSparqlResult* queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);
        if(!queryResult->hasError()) {
            queryResult->next();
            if (!queryResult->value(0).isNull())
                lm.setIconUrl(QUrl(queryResult->value(0).toString()));
        }
        q0 = QString("select ?r { ?u a slo:Landmark ; slo:hasContact ?c . ?c a nco:PersonContact ; nco:url ?r"
            " . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
        queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);
        if(!queryResult->hasError()) {
            queryResult->next();
            if (!queryResult->value(0).isNull())
                lm.setUrl(QUrl(queryResult->value(0).toString()));
        }
        q0 = QString("select ?a {?u a slo:Landmark ; nie:description ?a . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
        queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);
        if(!queryResult->hasError()) {
            queryResult->next();
            if (!queryResult->value(0).isNull())
                lm.setDescription(queryResult->value(0).toString());
        }
        q0 = QString("select ?a {?g a slo:GeoLocation ; "
                     "slo:radius ?a . "
                     "?u slo:location ?g . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
        queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);
        if(!queryResult->hasError()) {
            queryResult->next();
            if (!queryResult->value(0).isNull())
            lm.setRadius(queryResult->value(0).toDouble());
        }     
        q0 = QString("select ?country ?region ?city ?street ?postcode {?g a slo:GeoLocation . "
                     "?u slo:location ?g . ?g slo:postalAddress ?pa . "
                     "OPTIONAL { ?pa nco:country ?country } . OPTIONAL { ?pa nco:region ?region } . "
                     "OPTIONAL { ?pa nco:locality ?city } . OPTIONAL { ?pa nco:streetAddress ?street } . "
                     "OPTIONAL { ?pa nco:postalcode ?postcode } . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
        queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);
        if(!queryResult->hasError()) {
            QGeoAddress address = lm.address();
            queryResult->next();
            if (!queryResult->value(0).toString().isEmpty())
                address.setCountry(queryResult->value(0).toString());
            if (!queryResult->value(1).toString().isEmpty())
                address.setState(queryResult->value(1).toString());
            if (!queryResult->value(2).toString().isEmpty())
                address.setCity(queryResult->value(2).toString());
            if (!queryResult->value(3).toString().isEmpty())
                address.setStreet(queryResult->value(3).toString());
            if (!queryResult->value(4).toString().isEmpty())
                address.setPostcode(queryResult->value(4).toString());
            lm.setAddress(address);
        }
        q0 = QString("select ?p { ?u a slo:Landmark ; slo:hasContact ?c . ?c a nco:PersonContact ; nco:hasPhoneNumber ?pn"
            " . ?pn a nco:PhoneNumber ; nco:phoneNumber ?p . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
        queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);
        if(!queryResult->hasError()) {
            queryResult->next();
            if (!queryResult->value(0).toString().isEmpty())
                lm.setPhoneNumber(queryResult->value(0).toString());
        }
    } else {
        Q_ASSERT(error);
        Q_ASSERT(errorString);
        *error = QLandmarkManager::LandmarkDoesNotExistError;
        *errorString = QString("Landmark with id, %1, does not exist.").arg(landmarkId.localId());
    }
    return lm;
}

QList<QLandmarkId> DatabaseOperations::landmarkIds(const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        int limit, int offset,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";

    QList<QLandmarkId> result;
    bool alreadySorted = false;

    for (int i=0; i < sortOrders.count(); ++i) {
        if (sortOrders.at(i).type() == QLandmarkSortOrder::NameSort) {
            QLandmarkNameSort nameSort = sortOrders.at(i);
            if (nameSort.caseSensitivity() == Qt::CaseSensitive) {
                *error = QLandmarkManager::NotSupportedError;
                *errorString = "Case sensitive sorting is not supported";
                return QList<QLandmarkId>();
            }
        }
    }
    if (queryRun && queryRun->isCanceled) {
        *error = QLandmarkManager::CancelError;
        *errorString = "Fetch operation canceled";
        return QList<QLandmarkId>();
    }
    bool idsFound = false;
    QString queryString;
    QMap<QString, QVariant> bindValues;
    switch (filter.type()) {
    case QLandmarkFilter::InvalidFilter:
        break;
    case QLandmarkFilter::DefaultFilter:
        queryString = landmarkIdsDefaultQueryString();
        break;
    case QLandmarkFilter::AttributeFilter: {
            QLandmarkAttributeFilter attributeFilter = filter;
            QStringList filterKeys = attributeFilter.attributeKeys();

            if (filterKeys.count() > 0) {
                QMap<QString, QVariant> bindValues;

                QString key;
                QList<QLandmarkId> lmIds;

                bool selectAll =false;

                foreach(const QString key, filterKeys) {
                    if (!supportedSearchableAttributes.contains(key)) {
                        *error = QLandmarkManager::NotSupportedError;
                        *errorString = QString("Attribute key not searchable: ").arg(key);
                        return QList<QLandmarkId>();
                    }
                }
                QVariant attributeValue;

                //try to see if we need to select all landmarks
                //ie OR operation with a single invalid QVariant parameter
                //AND operation with all invalid QVariant parameter
                for ( int i = 0; i < filterKeys.count(); ++i) {
                    key = filterKeys.at(i);
                    attributeValue = attributeFilter.attribute(key).toString();
                    if (!attributeFilter.attribute(key).isValid()) {
                        if( attributeFilter.operationType() == QLandmarkAttributeFilter::OrOperation) {
                            selectAll = true;
                            break;
                        } else {
                            selectAll = true;
                            continue;
                        }
                    } else {
                        if (attributeFilter.operationType() == QLandmarkAttributeFilter::OrOperation)
                            continue;
                        else{
                            selectAll = false;
                            break;
                        }
                    }
                }

                if (selectAll) {
                    queryString = QString("select ?u { ?u a slo:Landmark . }");
                    break;
                } else {
                    if (attributeFilter.operationType() == QLandmarkAttributeFilter::AndOperation) {
                        QStringList lmLocalIds;
                        bool filterAdded = false;
                        queryString = QString("select ?u {?g a slo:GeoLocation . ?u slo:location ?g . "
                            "?g slo:postalAddress ?pa ."
                            "OPTIONAL { ?g nie:title ?name } . OPTIONAL { ?g slo:latitude ?latitude } . "
                            "OPTIONAL { ?g slo:longitude ?longitude } . OPTIONAL { ?g slo:altitude ?altitude } . "
                            "OPTIONAL { ?g slo:radius ?radius } . OPTIONAL { ?u nie:description ?description } . "
                            "OPTIONAL { ?pa nco:country ?country } . OPTIONAL { ?pa nco:region ?region } . "
                            "OPTIONAL { ?pa nco:locality ?locality } . OPTIONAL { ?pa nco:streetAddress ?street } . FILTER ( ");

                        if (attributeFilter.attributeKeys().contains("name")) {
                            QString nameString = QString("regex( ?name, '%1') && ").arg(attributeFilter.attribute("name").toString());
                            queryString.append(nameString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("latitude")) {
                            QString latitudeString = QString("regex( ?latitude, '%1') && ").arg(attributeFilter.attribute("latitude").toString());
                            queryString.append(latitudeString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("longitude")) {
                            QString longitudeString = QString("regex( ?longitude, '%1') && ").arg(attributeFilter.attribute("longitude").toString());
                            queryString.append(longitudeString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("altitude")) {
                            QString altitudeString = QString("regex( ?altitude, '%1') && ").arg(attributeFilter.attribute("altitude").toString());
                            queryString.append(altitudeString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("radius")) {
                            QString radiusString = QString("regex( ?radius, '%1') && ").arg(attributeFilter.attribute("radius").toString());
                            queryString.append(radiusString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("description")) {
                            QString descriptionString = QString("regex( ?description, '%1') && ").arg(attributeFilter.attribute("description").toString());
                            queryString.append(descriptionString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("state")) {
                            QString stateString = QString("regex( ?region, '%1') && ").arg(attributeFilter.attribute("state").toString());
                            queryString.append(stateString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("country")) {
                            QString countryString = QString("regex( ?country, '%1') && ").arg(attributeFilter.attribute("country").toString());
                            queryString.append(countryString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("city")) {
                            QString cityString = QString("regex( ?locality, '%1') && ").arg(attributeFilter.attribute("city").toString());
                            queryString.append(cityString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("street")) {
                            QString streetString = QString("regex( ?street, '%1') && ").arg(attributeFilter.attribute("street").toString());
                            queryString.append(streetString);
                            filterAdded = true;
                        }

                        if (filterAdded) {
                            queryString.chop(3);
                            queryString.append(" )");
                        } else {
                            queryString.chop(7);
                        }
                        queryString.append("}");

                        QSparqlQuery qsparqlQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);
                        QSparqlResult* qsparqlResult = m_conn->exec(qsparqlQuery);
                        qsparqlResult->waitForFinished();
                        if (qsparqlResult->hasError()) {
                            if (error)
                                *error =  QLandmarkManager::BadArgumentError;
                            if (errorString)
                                *errorString = QString("Attribute filtered Query Failed.");
                            return QList<QLandmarkId>();
                        }
                        while(qsparqlResult->next()) {
                            if (!qsparqlResult->value(0).toString().isEmpty()) {
                                lmLocalIds << qsparqlResult->value(0).toString();
                            }
                        }
                        QLandmarkId id;
                        id.setManagerUri(managerUri);
                        foreach(const QString &localId, lmLocalIds) {
                            id.setLocalId(localId);
                            result << id;
                        }
                    } else {
                        QStringList lmLocalIds;
                        bool filterAdded = false;
                        queryString = QString("select ?u {?g a slo:GeoLocation . ?u slo:location ?g . "
                            "?g slo:postalAddress ?pa ."
                            "OPTIONAL { ?g nie:title ?name } . OPTIONAL { ?g slo:latitude ?latitude } . "
                            "OPTIONAL { ?g slo:longitude ?longitude } . OPTIONAL { ?g slo:altitude ?altitude } . "
                            "OPTIONAL { ?g slo:radius ?radius } . OPTIONAL { ?u nie:description ?description } . "
                            "OPTIONAL { ?pa nco:country ?country } . OPTIONAL { ?pa nco:region ?region } . "
                            "OPTIONAL { ?pa nco:locality ?locality } . OPTIONAL { ?pa nco:streetAddress ?street } . FILTER ( ");

                        if (attributeFilter.attributeKeys().contains("name")) {
                            QString nameString = QString("regex( ?name, '%1') || ").arg(attributeFilter.attribute("name").toString());
                            queryString.append(nameString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("latitude")) {
                            QString latitudeString = QString("regex( ?latitude, '%1') ||  ").arg(attributeFilter.attribute("latitude").toString());
                            queryString.append(latitudeString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("longitude")) {
                            QString longitudeString = QString("regex( ?longitude, '%1') || ").arg(attributeFilter.attribute("longitude").toString());
                            queryString.append(longitudeString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("altitude")) {
                            QString altitudeString = QString("regex( ?altitude, '%1') || ").arg(attributeFilter.attribute("altitude").toString());
                            queryString.append(altitudeString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("radius")) {
                            QString radiusString = QString("regex( ?radius, '%1') || ").arg(attributeFilter.attribute("radius").toString());
                            queryString.append(radiusString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("description")) {
                            QString descriptionString = QString("regex( ?description, '%1') || ").arg(attributeFilter.attribute("description").toString());
                            queryString.append(descriptionString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("state")) {
                            QString stateString = QString("regex( ?region, '%1') || ").arg(attributeFilter.attribute("state").toString());
                            queryString.append(stateString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("country")) {
                            QString countryString = QString("regex( ?country, '%1') || ").arg(attributeFilter.attribute("country").toString());
                            queryString.append(countryString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("city")) {
                            QString cityString = QString("regex( ?locality, '%1') || ").arg(attributeFilter.attribute("city").toString());
                            queryString.append(cityString);
                            filterAdded = true;
                        }
                        if (attributeFilter.attributeKeys().contains("street")) {
                            QString streetString = QString("regex( ?street, '%1') || ").arg(attributeFilter.attribute("street").toString());
                            queryString.append(streetString);
                            filterAdded = true;
                        }

                        if (filterAdded) {
                            queryString.chop(3);
                            queryString.append(" )");
                        } else {
                            queryString.chop(7);
                        }
                        queryString.append("}");

                        QSparqlQuery qsparqlQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);
                        QSparqlResult* qsparqlResult = m_conn->exec(qsparqlQuery);
                        qsparqlResult->waitForFinished();
                        if (qsparqlResult->hasError()) {
                            if (error)
                                *error =  QLandmarkManager::BadArgumentError;
                            if (errorString)
                                *errorString = QString("Attribute filtered Query Failed.");
                            return QList<QLandmarkId>();
                        }
                        while(qsparqlResult->next()) {
                            if (!qsparqlResult->value(0).toString().isEmpty()) {
                                lmLocalIds << qsparqlResult->value(0).toString();
                            }
                        }

                        QLandmarkId id;
                        id.setManagerUri(managerUri);
                        foreach(const QString &localId, lmLocalIds) {
                            id.setLocalId(localId);
                            result << id;
                        }
                    }
                }
            }
        idsFound = true;
        break;
    }
    case QLandmarkFilter::NameFilter: {
            QLandmarkNameFilter nameFilter;
            nameFilter = filter;
            if (DatabaseOperations::filterSupportLevel(nameFilter)== QLandmarkManager::NoSupport) {
                *error = QLandmarkManager::NotSupportedError;
                *errorString = "The name filter's configuration is not supported";
                return result;
            }
            queryString = QString("select ?u { ?g a slo:GeoLocation ; nie:title ?name . ?u slo:location ?g . FILTER ");
            QString nameKey = "?name";
            QString nameValue = nameFilter.name();

            if (nameValue.isEmpty()) {
                    if (nameFilter.matchFlags() == QLandmarkFilter::MatchExactly
                        || nameFilter.matchFlags() == QLandmarkFilter::MatchFixedString) {                     
                        queryString.append("isBlank(?name) }" );
                    } else {
                        queryString = QString("select ?u { ?u a slo:Landmark . }");
                    }
            } else if (nameFilter.matchFlags() == QLandmarkFilter::MatchExactly) {
                queryString.append("regex( ?name, '%1' ) }").arg(nameValue);
            } else {
                if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchEndsWith)
                    queryString.append("regex( ?name, '%1$', 'i') }").arg(nameValue);
                else if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchStartsWith)
                    queryString.append("regex( ?name, '^%1', 'i') }").arg(nameValue);
                else if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchContains)
                    queryString.append("regex( ?name, '%1', 'i') }").arg(nameValue);
                else if (nameFilter.matchFlags() == QLandmarkFilter::MatchFixedString)
                    queryString.append("regex( ?name, '%1', 'i') }").arg(nameValue);
            }
            break;
        }
    case QLandmarkFilter::LandmarkIdFilter: {
            QLandmarkIdFilter idFilter = filter;
                //provide a query string exeecute so to that qsparql can handle sorting by name
                queryString = landmarkIdsQueryString(idFilter.landmarkIds());
            break;
        }
    case QLandmarkFilter::CategoryFilter: {
        QMap<QString, QVariant> bindValues;
        QLandmarkCategoryFilter categoryFilter = filter;
        QLandmarkCategoryId categoryId = categoryFilter.categoryId();

        if (categoryId.managerUri() != managerUri) {
            *error = QLandmarkManager::NoError;
            *errorString = "";
            return result;
        } else if (categoryId.localId().isEmpty()) {
            *error = QLandmarkManager::NoError;
            *errorString = "";
            return result;
        }
        bindValues.clear();

        QString query = QString("select ?c { ?c a slo:LandmarkCategory . FILTER regex( ?c, '^%1$') }").arg(categoryId.localId());
        QSparqlResult* qsparqlResult = executeQuery(m_conn, query, bindValues, error, errorString);

        if (qsparqlResult->hasError()){
            return result;
        }

        if (!qsparqlResult->next()) {
            *error = QLandmarkManager::NoError;
            *errorString = "";
            return result;
        }

        queryString = landmarkIdsCategoryQueryString(categoryFilter);
        break;
        }
    case QLandmarkFilter::ProximityFilter: {
            QLandmarkProximityFilter proximityFilter = filter;
            QGeoCoordinate center = proximityFilter.center();
            if ( (qIsNaN(center.latitude()) || qIsNaN(center.longitude())
                || !isValidLat(center.latitude()) || !isValidLong(center.longitude()))
                ) {
                *error = QLandmarkManager::BadArgumentError;
                *errorString = "Center coordinate of proximity filter is invalid";
                return result;
            }

            if (proximityFilter.radius() < 0) {
                queryString =  ::landmarkIdsDefaultQueryString();
                break;
            }
           //fall through if we have a radius, we can use a box filter
           //to quickly cull out landmarks
        }
    case QLandmarkFilter::BoxFilter: {
            QLandmarkBoxFilter boxFilter;
            if (filter.type() == QLandmarkFilter::BoxFilter) {
                boxFilter = filter;
            } else {
                QGeoCoordinate center;
                double radius; //use double since these are going to be usd in calculation with lat/long
                if(filter.type() == QLandmarkFilter::ProximityFilter) {
                    QLandmarkProximityFilter proximityFilter;
                    proximityFilter = filter;
                    center = proximityFilter.center();
                    radius = proximityFilter.radius();
                }

                //for a given latitude, find the how many degrees longitude a given distance could possibly cover.
                //for a given center, we "shift" it north or south by the radius then find
                //how many degrees longitude E or W the given radius could cover.
                double maxLongAbs = 0.0;
                if (center.latitude() > 0.0 ) { //coordinate must be in northern hemisphere so "shift" north first
                    QGeoCoordinate coord = center;
                    shiftCoordinate(&coord,0, radius);
                    shiftCoordinate(&coord, 270, radius);
                    maxLongAbs = qAbs(center.longitude() - coord.longitude());

                    double maxLat = center.latitude() + radius / ( 2.0 * M_PI * EARTH_MEAN_RADIUS *1000) *360;
                    if (maxLat > 90.0 || qFuzzyCompare(maxLat, 90.0)) {
                        *error = QLandmarkManager::BadArgumentError;
                        *errorString = "The proximity filter covers the north pole which is not allowed";
                        result.clear();
                        return result;
                    }
                } else { //coordinate must be in southerh hemisphere so "shift" south first
                    QGeoCoordinate coord = center;
                    shiftCoordinate(&coord,180, radius);
                    shiftCoordinate(&coord, 90, radius);
                    maxLongAbs = qAbs(center.longitude() - coord.longitude());

                    double minLat = center.latitude() - radius / ( 2.0 * M_PI * EARTH_MEAN_RADIUS *1000) *360;
                    if (minLat < -90.0 || qFuzzyCompare(minLat, -90.0)) {
                        *error = QLandmarkManager::BadArgumentError;
                        *errorString = "The proximity filter covers the south pole which is not allowed";
                        result.clear();
                        return result;
                    }
                }
                if (maxLongAbs > 180)
                    maxLongAbs = 360.0 - maxLongAbs;

                QGeoCoordinate topLeft = center;
                shiftCoordinate(&topLeft, 0, radius);
                topLeft.setLongitude(normalizeLongitude(topLeft.longitude() - maxLongAbs));

                QGeoCoordinate bottomRight = center;
                shiftCoordinate(&bottomRight, 180, radius);
                bottomRight.setLongitude(normalizeLongitude(bottomRight.longitude()+ maxLongAbs));

                QGeoBoundingBox box;
                box.setTopLeft(topLeft);
                box.setBottomRight(bottomRight);

                boxFilter.setBoundingBox(box);

                //TODO: handle poles
            }

            if (!boxFilter.boundingBox().isValid()) {
                if (error)
                    *error =  QLandmarkManager::BadArgumentError;
                if (errorString)
                    *errorString = QString("Invalid bounding box for landmark box filter");
                return result;
            }

            //TODO: handle box covering poles

            if (boxFilter.boundingBox().topLeft().latitude()
                < boxFilter.boundingBox().bottomRight().latitude()) {
                if (error)
                    *error = QLandmarkManager::BadArgumentError;
                if (errorString)
                    *errorString = QString("Box filter top left coordinate latitude, %1,  is less than "
                                           "bottom right coordinate latitude, %2")
                            .arg(boxFilter.boundingBox().topLeft().latitude())
                            .arg(boxFilter.boundingBox().bottomRight().latitude());
                 return result;
            }

            queryString = landmarkIdsBoxQueryString(boxFilter);
            break;
        }
    case QLandmarkFilter::IntersectionFilter: {
            QLandmarkIntersectionFilter intersectionFilter = filter;
            QList<QLandmarkFilter> filters = intersectionFilter.filters();

            if (filters.size() == 0) {
                //do nothing
            } else if (filters.size() == 1) {
                result = landmarkIds( filters.at(0), QList<QLandmarkSortOrder>(), limit, offset, error, errorString);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else  {
                bool haveProximityFilter = false;
                QLandmarkProximityFilter proximityFilter;
                int originalFilterCount = filters.count();
                for (int i=0; i < originalFilterCount ; ++i) {
                    if (filters.at(i).type() == QLandmarkFilter::ProximityFilter) {
                        proximityFilter = filters.takeAt(i);
                        haveProximityFilter = true;

                        break;
                    }
                }

                QSet<QLandmarkId> ids;
                QList<QLandmarkId> firstResult = landmarkIds(filters.at(0),
                                                QList<QLandmarkSortOrder>(), limit, offset, error, errorString);
                ids = firstResult.toSet();

                for (int i = 1; i < filters.size(); ++i) {
                    if (queryRun && queryRun->isCanceled) {
                        *error = QLandmarkManager::CancelError;
                        *errorString = "Fetch operation canceled";
                        result.clear();
                        return result;
                    }

                    QList<QLandmarkId> subResult = landmarkIds(filters.at(i),
                                                QList<QLandmarkSortOrder>(), limit, offset, error, errorString);

                    if (*error != QLandmarkManager::NoError) {
                        result.clear();
                        return result;
                    }
                    ids &= subResult.toSet();
                }

                QList<QLandmarkId> idList = ids.toList();
                if (haveProximityFilter) {
                    QList<LandmarkPoint> sortedPoints;
                    QMap<QString,QVariant> bindValues;
                    QSparqlResult* qsparqlResult  = executeQuery(m_conn, landmarkIdsQueryString(idList),bindValues,error,errorString);
                    if (*error != QLandmarkManager::NoError) {
                        result.clear();
                        return result;
                    }
                    result = sortQueryByDistance(qsparqlResult,proximityFilter,error,errorString,managerUri,queryRun);
                    if (*error != QLandmarkManager::NoError) {
                        result.clear();
                        return result;
                    }
                } else {
                        result << idList;
                }
            }
            idsFound = true;
        }
        break;
    case QLandmarkFilter::UnionFilter: {
            QLandmarkUnionFilter unionFilter = filter;
            QSet<QString> ids;
            QList<QLandmarkFilter> filters = unionFilter.filters();

            if (filters.size() == 0) {
                //do nothing
            } else if (filters.size() == 1) {
                result =  landmarkIds(filters.at(0),
                                        QList<QLandmarkSortOrder>(), limit, offset, error, errorString);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else {
                for (int i = 0; i < filters.size(); ++i) {
                    if (queryRun && queryRun->isCanceled) {
                        *error = QLandmarkManager::CancelError;
                        *errorString = "Fetch operation canceled";
                        result.clear();
                        return result;
                    }
                    QList<QLandmarkId> subResult = landmarkIds(filters.at(i),
                                                               QList<QLandmarkSortOrder>(),
                                                               limit, offset,
                                                               error, errorString);

                    if (*error != QLandmarkManager::NoError) {
                        result.clear();
                        return result;
                    }

                    for (int j = 0; j < subResult.size(); ++j) {
                        if (subResult.at(j).isValid())
                            ids.insert(subResult.at(j).localId());
                    }
                }

                QList<QString> idList = ids.toList();
                for (int i = 0; i < idList.size(); ++i) {
                    QLandmarkId id;
                    id.setManagerUri(managerUri);
                    id.setLocalId(idList.at(i));
                    result << id;
                }
            }
            idsFound = true;
        }
        break;
    default:
        if(error)
            *error = QLandmarkManager::BadArgumentError;
        if(errorString)
            *errorString = QString("Unrecognised filter type");
        break;
    }

    if (!idsFound) {
        if (sortOrders.length() == 1 && sortOrders.at(0).type() == QLandmarkSortOrder::NameSort) {
            QLandmarkNameSort nameSort = sortOrders.at(0);

            queryString.append(" ORDER BY  ");

            //if (nameSort.caseSensitivity() == Qt::CaseInsensitive)
            //    queryString.append("COLLATE NOCASE ");

            if (nameSort.direction() == Qt::AscendingOrder)
                queryString.append("ASC(?name)");
            else
                queryString.append("DESC(?name)");
             alreadySorted = true;
        }

        //queryString.append(";");
        QSparqlResult* qsparqlResult = executeQuery(m_conn, queryString,bindValues,error,errorString);

        if (qsparqlResult->hasError()) {
            *error = QLandmarkManager::UnknownError;
            return result;
        }

        QLandmarkId id;
        qsparqlResult->current();
        while (qsparqlResult->next()) {

            if (queryRun && queryRun->isCanceled) {
                *error = QLandmarkManager::CancelError;
                *errorString = "Fetch operation canceled";
                return QList<QLandmarkId>();
            }

            if (filter.type() == QLandmarkFilter::BoxFilter) {
                //need to do a fuzzy compare on the coordinates (using inBetween() and outside() fns)
                //since sqlite doeesn't handle comparison of floating point numbers
                QLandmarkBoxFilter boxFilter = filter;
                double maxLat = boxFilter.boundingBox().topLeft().latitude();
                double minLat = boxFilter.boundingBox().bottomRight().latitude();
                double minLong;
                double maxLong;

                //special edge case if the bounding box crosses the dateline
                bool longWrap = boxFilter.boundingBox().topLeft().longitude() > boxFilter.boundingBox().bottomRight().longitude();
                if (longWrap) {
                    minLong = boxFilter.boundingBox().bottomRight().longitude();
                    maxLong = boxFilter.boundingBox().topLeft().longitude();
                } else {
                    minLong = boxFilter.boundingBox().topLeft().longitude();
                    maxLong = boxFilter.boundingBox().bottomRight().longitude();
                }

                do {
                    if (queryRun && queryRun->isCanceled) {
                        *error = QLandmarkManager::CancelError;
                        *errorString = "Fetch operation was canceled";
                        return QList<QLandmarkId>();
                    }
                    if (inBetween(qsparqlResult->value(1).toDouble(),minLat, maxLat)
                        &&  (longWrap) ? outside(qsparqlResult->value(2).toDouble(),minLong, maxLong)
                            : inBetween(qsparqlResult->value(2).toDouble(),minLong, maxLong)) {
                        id.setManagerUri(managerUri);
                        id.setLocalId(qsparqlResult->value(0).toString());
                        result << id;
                    }
                } while (qsparqlResult->next());
            } else if ( filter.type() == QLandmarkFilter::ProximityFilter) {
                QLandmarkProximityFilter proximityFilter;
                proximityFilter = filter;
                qsparqlResult->previous();
                result << sortQueryByDistance(qsparqlResult, proximityFilter,error,errorString,managerUri,queryRun);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else {
                id.setManagerUri(managerUri);
                id.setLocalId(qsparqlResult->value(0).toString());
                result << id;
            }
        }
    }

    //TODO: optimize
    if (offset >= result.count()) {
        result.clear();
        return result;
    }

    if (!alreadySorted && sortOrders.count() > 0 ) {
        //TODO: optimize this
        QList<QLandmark> landmarks;
        QLandmark landmark;
        for (int i=0; i < result.count(); ++i) {
            if (queryRun && queryRun->isCanceled) {
                *error = QLandmarkManager::CancelError;
                *errorString = "Fetch operation canceled";
                return QList<QLandmarkId>();
            }
            landmark = retrieveLandmark(result.at(i),error,errorString);
            if (*error != QLandmarkManager::NoError) {
                result.clear();
                return result;
            }
            landmarks.append(landmark);
        }
        result = QLandmarkManagerEngineQsparql::sortLandmarks(landmarks, sortOrders);
    }

    if (offset < 0 )
        offset = 0;

    return result.mid(offset, limit);
}

QList<QLandmark> DatabaseOperations::landmarks(const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        int limit, int offset,
        QLandmarkManager::Error *error,
        QString *errorString)  const
{
    QList<QLandmark> result;
    QList<QLandmarkId> ids = landmarkIds(filter, sortOrders, limit, offset, error, errorString);
    if (error && *error != QLandmarkManager::NoError) {
        return result;
    }

    bool needAll = false;
    if (filter.type() == QLandmarkFilter::LandmarkIdFilter) {
        QLandmarkIdFilter idFilter = filter;
    }

    QLandmark lm;
    foreach(const QLandmarkId &id, ids) {
         if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString  = "Fetch operation canceled";
            return QList<QLandmark>();
        }

        lm = retrieveLandmark(id,error,errorString);
        if (lm.landmarkId().isValid() && *error == QLandmarkManager::NoError) {
            result.append(lm);
         } else {
            if (*error == QLandmarkManager::DoesNotExistError) {
                if (!needAll) {
                    continue;
                 }
                else {
                    result.clear();
                    return result;
                }
            } else {
                result.clear();
                return result;
            }
         }
     }

     *error = QLandmarkManager::NoError;
     *errorString = "";
    return result;
}

QList<QLandmark> DatabaseOperations::landmarks(const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)  const
{
    if (errorMap)
        errorMap->clear();

    QList<QLandmark> result;
    QLandmark lm;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString="";
    for (int i=0; i < landmarkIds.count(); ++i){
        lm = retrieveLandmark(landmarkIds.at(i),error,errorString);
        if (*error == QLandmarkManager::NoError) {
            result << lm;
        } else {
            if (errorMap)
                errorMap->insert(i, *error);
            lastError = *error;
            lastErrorString = *errorString;
            result << QLandmark();
        }
    }
    *error = lastError;
    *errorString =lastErrorString;
    return result;
}

//Performs the insert qsparql queries
bool DatabaseOperations::saveLandmarkHelper(QLandmark *landmark,
                  QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString="";

    if (!landmark->landmarkId().managerUri().isEmpty() && landmark->landmarkId().managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return false;
    }
    bool update = landmark->landmarkId().isValid();

    if (update) {
        bool removeResult = removeLandmarkHelper(landmark->landmarkId(), error, errorString, managerUri);
        if(!removeResult) {
            *error = QLandmarkManager::UnknownError;
            *errorString = "Unable to execute update statement. ";
            return false;
        }
    }
    QSparqlConnection conn("QTRACKER"); 
    QString queryString;

    queryString.append("insert { ?:uri_value a slo:Landmark ");

    for (int i = landmark->categoryIds().size() - 1; i >= 0; --i) {
        QLandmarkCategoryId catId = landmark->categoryIds().at(i);
        if (catId.managerUri() == managerUri) {
            queryString.append("; slo:belongsToCategory \'");
            queryString.append(catId.localId());
            queryString.append("\' ");
        } else {
            landmark->removeCategoryId(catId);
        }
    }
    if (!landmark->description().isEmpty()) {
        queryString.append("; nie:description \'");
        queryString.append(landmark->description());
        queryString.append("\' ");
    }
    if (!landmark->iconUrl().toString().isEmpty()) {
        queryString.append("; slo:iconUrl \'");
        queryString.append(landmark->iconUrl().toString());
        queryString.append("\' ");
    }
    queryString.append("; slo:hasContact _:c ");

    queryString.append("; slo:location _:x . ");
    queryString.append("_:x a slo:GeoLocation ");

    if (!landmark->name().isEmpty()) {
        queryString.append("; nie:title \'");
        queryString.append(landmark->name());
        queryString.append("\' ");
    }
    QGeoCoordinate geoCoord;
    geoCoord = landmark->coordinate();
    if (!qIsNaN(geoCoord.latitude())) {
        queryString.append("; slo:latitude ");
        queryString.append(QString::number(geoCoord.latitude()));
        queryString.append(" ");
    }
    if (!qIsNaN(geoCoord.longitude())) {
        queryString.append("; slo:longitude ");
        queryString.append(QString::number(geoCoord.longitude()));
        queryString.append(" ");
    }
    if (!qIsNaN(geoCoord.altitude())) {
        queryString.append("; slo:altitude ");
        queryString.append(QString::number(geoCoord.altitude()));
        queryString.append(" ");
    }
    if (!qIsNaN(landmark->radius())) {
        queryString.append("; slo:radius ");
        queryString.append(QString::number(landmark->radius()));
        queryString.append(" ");
    }
    queryString.append("; slo:postalAddress _:pa . ");
    queryString.append("_:pa a nco:PostalAddress ");

    if (!landmark->address().country().isEmpty()) {
        queryString.append("; nco:country \'");
        queryString.append(landmark->address().country());
        queryString.append("\' ");
    }
    if (!landmark->address().state().isEmpty()) {
        queryString.append("; nco:region \'");
        queryString.append(landmark->address().state());
        queryString.append("\' ");
    } else if (!landmark->address().county().isEmpty()) {
        queryString.append("; nco:region \'");
        queryString.append(landmark->address().county());
        queryString.append("\' ");
    }
    if (!landmark->address().city().isEmpty()) {
        queryString.append("; nco:locality \'");
        queryString.append(landmark->address().city());
        queryString.append("\' ");
    }
    if (!landmark->address().street().isEmpty()) {
        queryString.append("; nco:streetAddress \'");
        queryString.append(landmark->address().street());
        queryString.append("\' ");
    }
    if (!landmark->address().postcode().isEmpty()) {
        queryString.append("; nco:postalcode \'");
        queryString.append(landmark->address().postcode());
        queryString.append("\' ");
    }
    queryString.append(". _:c a nco:PersonContact");
    if (!landmark->url().toString().isEmpty()) {
        queryString.append(" ; nco:url \'");
        queryString.append(landmark->url().toString());
        queryString.append("\' ");
    }
    if (!landmark->phoneNumber().isEmpty()) {
        queryString.append("; nco:hasPhoneNumber _:pn . ");
        queryString.append("_:pn a nco:PhoneNumber ; ");
        queryString.append("nco:phoneNumber \'");
        queryString.append(landmark->phoneNumber());
        queryString.append("\' ");
    }
    queryString.append(". }");

    QSparqlQuery qsparqlInsertQuery = QSparqlQuery(queryString, QSparqlQuery::InsertStatement);

    qsparqlInsertQuery.unbindValues();
    QSparqlBinding uriValue;
    if (update) {
        qsparqlInsertQuery.bindValue( "uri_value",  landmark->landmarkId().localId());
    } else {
        uriValue = conn.createUrn("uri_value");
        qsparqlInsertQuery.bindValue(uriValue);
    }
    QSparqlResult* insertResult = conn.exec(qsparqlInsertQuery);
    insertResult->waitForFinished();

    if (!insertResult->hasError()) {
        QLandmarkId id;
        id.setManagerUri(managerUri);
        if (update)
            id.setLocalId(landmark->landmarkId().localId());
        else
            id.setLocalId(uriValue.value().toString());
        landmark->setLandmarkId(id);
    } else {
            *error = QLandmarkManager::UnknownError;
            *errorString = "Unable to execute insert statement. ";
            return false;
    }
 return true;
}

bool DatabaseOperations::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkId> addedIds;
    QList<QLandmarkId> changedIds;
    bool update = landmark->landmarkId().isValid();
    bool result = saveLandmarkHelper(landmark, error, errorString);

    if (result) {
        if(update) {
            changedIds << landmark->landmarkId();
            emit landmarksChanged(changedIds);
        }
        else {
            addedIds << landmark->landmarkId();
            emit landmarksAdded(addedIds);
        }
    }
   return result;
}

bool DatabaseOperations::saveLandmarks(QList<QLandmark> * landmark,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();

    QList<QLandmarkId> addedIds;
    QList<QLandmarkId> changedIds;
    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    bool result;
    for (int i = 0; i < landmark->size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString = "";

        if (queryRun && queryRun->isCanceled) {
            lastError = QLandmarkManager::CancelError;
            lastErrorString = "Landmark save was canceled";
            if (errorMap) {
                //for (i; i < landmark->size(); ++i)
                errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }   
        bool update = landmark->at(i).landmarkId().isValid();
        result = saveLandmarkHelper(&(landmark->operator [](i)), &loopError, &loopErrorString);

        if (errorMap && (loopError != QLandmarkManager::NoError))
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        } else {
          if(update)
              changedIds << landmark->at(i).landmarkId();
          else
              addedIds << landmark->at(i).landmarkId();
        }
    }

    if (noErrors) {
        *error = QLandmarkManager::NoError;
        *errorString = "";
    } else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }

    if (addedIds.size() != 0) {
        if (addedIds.size() < 50)
            emit landmarksAdded(addedIds);
        else
            emit dataChanged();
    }
    if (changedIds.size() != 0) {
        if (changedIds.size() < 50)
            emit landmarksChanged(changedIds);
        else
            emit dataChanged();
    }
    return noErrors;
}

bool DatabaseOperations::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkId> removedIds;
    bool result = removeLandmarkHelper(landmarkId, error, errorString, managerUri);

    if (result) {
        removedIds.append(landmarkId);
        emit landmarksRemoved(removedIds);
    }
    return result;
}

bool DatabaseOperations::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();

    QList<QLandmarkId> removedIds;
    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    bool result;
    for (int i = 0; i < landmarkIds.size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString.clear();

        if (queryRun && queryRun->isCanceled) {
            lastError = QLandmarkManager::CancelError;
            lastErrorString = "Landmark remove was canceled";
            if (errorMap) {
                //for (i; i < landmarkIds.size(); ++i)
                errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }
       result = removeLandmarkHelper(landmarkIds.at(i), &loopError, &loopErrorString, managerUri);
       if (result) {
            removedIds << landmarkIds.at(i);
        } else {
            loopError = QLandmarkManager::UnknownError;
            loopErrorString = QString("Remove landmarks error");
        }
        if (errorMap && loopError != QLandmarkManager::NoError)
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }
    }
    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }
    if (removedIds.size() != 0)
        emit landmarksRemoved(removedIds);

    return noErrors;
}

QList<QLandmarkCategoryId> DatabaseOperations::categoryIds(const QLandmarkNameSort &nameSort,
                                       int limit, int offset,
                                       QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmarkCategoryId> result;

    QString uri = managerUri;
    QString queryString = QString("select ?u {?u a slo:LandmarkCategory ; nie:title ?name . } ORDER BY");

    /*
    if (nameSort.caseSensitivity() == Qt::CaseInsensitive)
        queryString.append("COLLATE NOCASE ");
    else {
        *error = QLandmarkManager::NotSupportedError;
        *errorString = "Case sensitive name sorting of categories is not supported";
        return QList<QLandmarkCategoryId>();
    }
    */
    if (nameSort.direction() == Qt::AscendingOrder)
        queryString.append(" ASC(?name) ");
    else
        queryString.append(" DESC(?name) ");

    QSparqlQuery qsparqlSelectQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);
    QSparqlResult*queryResult  = m_conn->exec(qsparqlSelectQuery);
    queryResult->waitForFinished();

    if (queryResult->hasError()){
        if (error)
            *error = QLandmarkManager::UnknownError;
         if (errorString)
            *errorString = QString("Unable to execute categoryIds query.");
        return result;
    }

    while (queryResult->next()) {
        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Fetch operation was canceled";
            result.clear();
            return result;
        }
        QLandmarkCategoryId id;
        id.setManagerUri(managerUri);
        id.setLocalId(queryResult->value(0).toString());
        result << id;
    }

    //TODO: optimize
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    if (offset>= result.count()) {
        result.clear();
        return result;
    }

    if (offset < 0)
        offset = 0;

    return result.mid(offset,limit);
}

QLandmarkCategory DatabaseOperations::category(const QLandmarkCategoryId &landmarkCategoryId,
              QLandmarkManager::Error *error,
              QString *errorString) const
{
    if (!landmarkCategoryId.isValid() || landmarkCategoryId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::CategoryDoesNotExistError;
        if (errorString)
            *errorString = "Category id is not valid for this manager";
        return QLandmarkCategory();
    }

    QLandmarkCategory cat;
    QString q1 = QString("select ?u ?name ?iconUrl {?u a slo:LandmarkCategory . OPTIONAL { ?u nie:title ?name } . OPTIONAL { ?u slo:categoryIconUrl ?iconUrl } . FILTER regex( ?u, '^%1$') }").
        arg(landmarkCategoryId.localId());
    QSparqlConnection conn("QTRACKER");
    QSparqlQuery qsparqlSelectQuery = QSparqlQuery(q1, QSparqlQuery::SelectStatement);
    QSparqlResult*queryResult  = conn.exec(qsparqlSelectQuery);
    queryResult->waitForFinished();

    if (queryResult->hasError()){
        *error = QLandmarkManager::UnknownError;
        return QLandmarkCategory();
    }

    bool found = false;
    while (queryResult->next()) {
        if (found) {
            // TODO set error - should never happen
            if (error)
                *error = QLandmarkManager::UnknownError;
            if (errorString)
                *errorString = "Database corruption, non-unique primary key in table \"category\"";
            return QLandmarkCategory();
        } else {
            found = true;
        }
        if (queryResult->value(0).toString().isEmpty()) {
            if (error)
                *error = QLandmarkManager::CategoryDoesNotExistError;
            if (errorString)
                *errorString = "Category does not exist in database.";
            return QLandmarkCategory();
        }
        if (!queryResult->value(1).toString().isEmpty())
            cat.setName(queryResult->value(1).toString());

        // TODO: category description is not a common data attribute, make it a platform specific attribute
        //        if (!query.value(?).isNull())
        //            cat.setDescription(query.value(?).toString());

        if (!queryResult->value(2).toString().isEmpty())
            cat.setIconUrl(queryResult->value(2).toString());

        cat.setCategoryId(landmarkCategoryId);
    }

    if (!found) {
            *error = QLandmarkManager::CategoryDoesNotExistError;
            *errorString = "None of the existing categories match the given category id.";
    } else {
        *error = QLandmarkManager::NoError;
        *errorString = "";
    }
    return cat;
}

QList<QLandmarkCategory> DatabaseOperations::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                const QLandmarkNameSort &nameSort,
                int limit, int offset,
                QLandmarkManager::Error *error, QString *errorString,
                bool needAll) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    *error = QLandmarkManager::NoError;
    errorString->clear();

    QList<QLandmarkCategory> result;
    QList<QLandmarkCategoryId> ids = landmarkCategoryIds;
    if (ids.size() == 0) {
        ids = categoryIds(nameSort, limit, offset, error, errorString);

        if (*error != QLandmarkManager::NoError) {
            return result;
        }
    }

    for (int i = 0; i < ids.size(); ++i) {
        *error = QLandmarkManager::NoError;
        (*errorString).clear();

        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Fetch operation was canceled";
            result.clear();
            return result;
        }

        QLandmarkCategory cat = category(ids.at(i), error,errorString);
        if (*error == QLandmarkManager::NoError)
            result << cat;
         else  {
            if (*error == QLandmarkManager::DoesNotExistError) {
                if (!needAll)
                    continue;
            } else {
                result.clear();
                return result;
            }
            result.clear();
            return result;
         }
    }

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";


    return result;
}


QList<QLandmarkCategory> DatabaseOperations::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                QMap<int, QLandmarkManager::Error> *errorMap,
                QLandmarkManager::Error *error, QString *errorString) const
{
    if (errorMap)
        errorMap->clear();
    QList<QLandmarkCategory> result;
    QLandmarkCategory cat;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString ="";
    for (int i=0; i < landmarkCategoryIds.count(); ++i) {
        cat = category(landmarkCategoryIds.at(i),error,errorString);

        if (*error == QLandmarkManager::NoError)
        {
            result << cat;
        } else {
            result << QLandmarkCategory();
            if (errorMap)
                errorMap->insert(i, *error);
            lastError = *error;
            lastErrorString = *errorString;
        }
    }
    *error = lastError;
    *errorString = lastErrorString;

    return result;
}

bool DatabaseOperations::saveCategoryHelper(QLandmarkCategory *category,
                QLandmarkManager::Error *error,
                QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (category->name().isEmpty()) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Category name must not be empty";
        return false;
    }

    if (!category->categoryId().managerUri().isEmpty() && category->categoryId().managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::CategoryDoesNotExistError;
        if (errorString)
            *errorString = "Category id comes from different landmark manager.";
        return false;
    }

    bool update = category->categoryId().isValid();

    if (update) {
        bool removeResult = removeCategoryHelper(category->categoryId(), error, errorString);
        if(!removeResult) {
            *error = QLandmarkManager::UnknownError;
            *errorString = "Unable to execute category update statement. ";
            return false;
        }
    }

    QSparqlConnection conn("QTRACKER");

    QString queryString = QString("insert { ?:uri_value a slo:LandmarkCategory ");

    if (!category->name().isEmpty()) {
        queryString.append("; nie:title \'");
        queryString.append(category->name());
        queryString.append("\' ");
    }
    if (!category->iconUrl().toString().isEmpty()) {
        queryString.append("; slo:categoryIconUrl \'");
        queryString.append(category->iconUrl().toString());
        queryString.append("\' ");
    }
    queryString.append("; slo:isRemovable 'true' . }");

    QSparqlQuery qsparqlInsertQuery = QSparqlQuery(queryString, QSparqlQuery::InsertStatement);
    qsparqlInsertQuery.unbindValues();
    QSparqlBinding uriValue;
    if (update) {
        qsparqlInsertQuery.bindValue( "uri_value",  category->categoryId().localId());
    } else {
        uriValue = conn.createUrn("uri_value");
        qsparqlInsertQuery.bindValue(uriValue);
    }
    QSparqlResult* insertResult = conn.exec(qsparqlInsertQuery);

    if (!insertResult->hasError()) {
        QLandmarkCategoryId id;
        id.setManagerUri(managerUri);
        if (!update) {
            id.setLocalId(uriValue.value().toString());
        } else {
             id.setLocalId(category->categoryId().localId());
        }
        category->setCategoryId(id);
    } else {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = "Landmark Category adding failed.";
        return false;
    }
return true;
}

bool DatabaseOperations::saveCategory(QLandmarkCategory *category,
                                      QLandmarkManager::Error *error, QString *errorString)
{
    QList<QLandmarkCategoryId> addedIds;
    QList<QLandmarkCategoryId> changedIds;
    bool update = category->categoryId().isValid();
    bool result = saveCategoryHelper(category, error, errorString);

    if (result) {
        if(update) {
            changedIds << category->categoryId();
            emit categoriesChanged(changedIds);
        }
        else {
            addedIds << category->categoryId();
            emit categoriesAdded(addedIds);
        }
    }
   return result;
}

bool DatabaseOperations::saveCategories(QList<QLandmarkCategory> * categories,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkCategoryId> addedIds;
    QList<QLandmarkCategoryId> changedIds;
    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    for (int i = 0; i < categories->size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString = "";
        bool added = false;
        bool changed = false;

        bool result = saveCategoryHelper(&(categories->operator [](i)), &loopError, &loopErrorString);

        if (errorMap)
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }

        if (added)
            addedIds << categories->at(i).categoryId();
        if (changed)
            changedIds << categories->at(i).categoryId();
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }
    if (addedIds.size() != 0) {
        if (addedIds.size() < 50)
            emit categoriesAdded(addedIds);
        else
            emit dataChanged();
    }
    if (changedIds.size() != 0) {
        if (changedIds.size() < 50)
            emit categoriesChanged(changedIds);
        else
            emit dataChanged();
    }
    return noErrors;
}

bool DatabaseOperations::removeCategory(const QLandmarkCategoryId &categoryId,
    QLandmarkManager::Error *error, QString *errorString)
{
    QList<QLandmarkCategoryId> removedIds;
    bool result = removeCategoryHelper(categoryId, error, errorString);

    if (result) {
        removedIds.append(categoryId);
        emit categoriesRemoved(removedIds);
    }
    return result;
}

bool DatabaseOperations::removeCategoryHelper(const QLandmarkCategoryId &categoryId,
                QLandmarkManager::Error *error,
                QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (categoryId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::CategoryDoesNotExistError;
        if (errorString)
            *errorString = "Category id comes from different landmark manager.";
        return false;
    }
    QMap<QString,QVariant> bindValues;
    QString q0 = QString("select ?u ?removable {?u a slo:LandmarkCategory ; slo:isRemovable ?removable . "
         "FILTER regex( ?u, '^%1$') }").arg(categoryId.localId());
    QSparqlResult *queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);

    if (queryResult->hasError()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Unable to remove category.");
        return false;
    }

    if (!queryResult->next()) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = QString("Category with local id %1, does not exist in database")
                        .arg(categoryId.localId());
        return false;
    }

    if((queryResult->value(1).toString().compare("false", Qt::CaseSensitive)) == 0) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Category is not removable.");
        return false;
    }

    QSparqlQuery qsparqlDeleteQuery = QSparqlQuery(
            "delete { ?:category_uri a rdfs:Resource . }",
             QSparqlQuery::DeleteStatement);

    qsparqlDeleteQuery.unbindValues();
    qsparqlDeleteQuery.bindValue("category_uri", categoryId.localId());
    QSparqlResult* deleteResult = m_conn->exec(qsparqlDeleteQuery);

    deleteResult->waitForFinished();
    if (deleteResult->hasError()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Unable to execute remove category statement.");
        return false;
    }
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return true;
}

bool DatabaseOperations::removeCategories(const QList<QLandmarkCategoryId> &categoryIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString)
{
    QList<QLandmarkCategoryId> removedIds;
    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    bool result;
    for (int i = 0; i < categoryIds.size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString.clear();

        result = removeCategoryHelper(categoryIds.at(i), &loopError, &loopErrorString);

        if (errorMap)
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }
        if (result)
            removedIds << categoryIds.at(i);
    }
    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }
    if (removedIds.size() != 0)
        emit categoriesRemoved(removedIds);

    return noErrors;
}

bool DatabaseOperations::importLandmarks(QIODevice *device,
                     const QString &format,
                     QLandmarkManager::TransferOption option,
                     const QLandmarkCategoryId &categoryId,
                     QLandmarkManager::Error *error,
                     QString *errorString,
                     QueryRun *queryRun,
                     QList<QLandmarkId> *landmarkIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (!device) {
       *error = QLandmarkManager::BadArgumentError;
       *errorString = "Invalid io device pointer";
       return false;
    }

    QFile *file = qobject_cast<QFile *>(device);
    if (file)
    {
        if (!file->exists()) {
            *error = QLandmarkManager::DoesNotExistError;
            *errorString = QString("Import operation failed, file does not exist: %1").arg(file->fileName());
            return false;
        }

        if (!file->open(QIODevice::ReadOnly)) {
            if (file->error() == QFile::OpenError) {
                *error = QLandmarkManager::PermissionsError;
                *errorString = QString("Insufficient permissions to open file");
                return false;
            } else {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Unable to open file for importing landmarks");
                return false;
            }
        }
    } else if(!device->open(QIODevice::ReadOnly)) {
        *error = QLandmarkManager::UnknownError;
        *errorString = "Unable to open io device for importing landmarks";
        return false;
    }

    QString detectedFormat = format;
    if (detectedFormat == "") {
        QXmlStreamReader reader(device);
        if (!reader.readNextStartElement()) {
            *error = QLandmarkManager::ParsingError;
            *errorString = "Could not read root element of io device";
            device->reset();
            return false;
        } else {
            if (reader.name() == "lmx")
                detectedFormat = (const QLatin1String)QLandmarkManager::Lmx;
            else if (reader.name() == "gpx")
                detectedFormat = (const QLatin1String)QLandmarkManager::Gpx;
            else {
                *error = QLandmarkManager::NotSupportedError;
                *errorString = "Unknown import file format";
                device->reset();
                return false;
            }
        }
        device->reset();
    }

    bool result = false;
    if (detectedFormat ==  QLandmarkManager::Lmx) {
        result = importLandmarksLmx(device, option, categoryId, error, errorString, queryRun, landmarkIds);
        device->close();
        return result;
    } else if (detectedFormat == QLandmarkManager::Gpx) {
        result = importLandmarksGpx(device, option, categoryId, error, errorString, queryRun, landmarkIds);
        device->close();
        return result;
    } else {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString = "The given format is not supported at this time";
        device->close();
        return false;
    }
}

bool DatabaseOperations::exportLandmarks( QIODevice *device,
                     const QString &format,
                     const QList<QLandmarkId> &landmarkIds,
                     QLandmarkManager::TransferOption option,
                     QLandmarkManager::Error *error,
                     QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (!device) {
       *error = QLandmarkManager::BadArgumentError;
       *errorString = "Invalid io device pointer";
       return false;
    }

    QFile *file = qobject_cast<QFile *>(device);
    if (file) {

        if (!file->open(QIODevice::WriteOnly)) {
            if (file->error() == QFile::OpenError) {
                *error = QLandmarkManager::PermissionsError;
                *errorString = QString("Insufficient permissions to open file");
                return false;
            } else {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Unable to open file for importing landmarks");
                return false;
            }
        }
    } else if (!device->open(QIODevice::WriteOnly)) {
        *error = QLandmarkManager::UnknownError;
        *errorString = "Unable to open io device for importing landmarks";
        return false;
    }

    bool result = false;
    if (format ==  QLandmarkManager::Lmx) {
        result = exportLandmarksLmx(device, landmarkIds, option, error, errorString);
        device->close();
        return result;
    } else if (format == QLandmarkManager::Gpx) {
        result = exportLandmarksGpx(device, landmarkIds, error, errorString);
        device->close();
        return result;
    }  else if (format =="") {
        *error = QLandmarkManager::BadArgumentError;
        *errorString =  "No format provided";
        device->close();
         return false;
    } else {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString = "The given format is not supported at this time";
        device->close();
        return false;
    }
}

bool DatabaseOperations::importLandmarksLmx(QIODevice *device,
                        QLandmarkManager::TransferOption option,
                        const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        QueryRun *queryRun,
                        QList<QLandmarkId>  *landmarkIds)
{
    QLandmarkFileHandlerLmx lmxHandler(queryRun?&(queryRun->isCanceled):0);

    if (option == QLandmarkManager::AttachSingleCategory) {
        QLandmarkCategory singleCategory;
        singleCategory = category(categoryId,error,errorString);
        if (*error != QLandmarkManager::NoError) {
            return false;
        }
    }

    bool result = lmxHandler.importData(device);
    if (!result) {
        *error = lmxHandler.errorCode();
        *errorString = lmxHandler.errorString();
        return false;
    }

    QList<QLandmark> landmarks = lmxHandler.landmarks();

    QHash<QString, QLandmarkCategoryId> catIdLookup;
    QList<QStringList> landmarkCategoryNames;
    if (option == QLandmarkManager::IncludeCategoryData) {
        landmarkCategoryNames = lmxHandler.landmarkCategoryNames();

        QList<QLandmarkCategory> categories;
        categories = this->categories(QList<QLandmarkCategoryId>(),QLandmarkNameSort(),-1, 0,error,errorString,true);
        if (*error != QLandmarkManager::NoError) {
            return false;
        }

        foreach(const QLandmarkCategory &category, categories) {
            catIdLookup.insert(category.name(), category.categoryId());
        }
    }

    QStringList categoryNames;
    QList<QLandmarkCategoryId> categoryIds;
    for (int i=0; i < landmarks.count(); ++i) {
        categoryIds.clear();
        if (option == QLandmarkManager::IncludeCategoryData) {
            categoryNames = landmarkCategoryNames.at(i);
            foreach(const QString &name, categoryNames){
                if (!catIdLookup.contains(name)) {
                    QLandmarkCategory category;
                    category.setName(name);
                    saveCategoryHelper(&category,error,errorString);
                    if (*error != QLandmarkManager::NoError) {
                        if (landmarkIds)
                            landmarkIds->clear();
                        return false;
                    }
                    catIdLookup.insert(category.name(), category.categoryId());
                }
                categoryIds.append(catIdLookup.value(name));
            }
        } else if (option == QLandmarkManager::AttachSingleCategory) {
            categoryIds.append(categoryId);
        }

        landmarks[i].setCategoryIds(categoryIds);

        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Import of lmx file canceled";
        } else {
            saveLandmarkHelper(&(landmarks[i]),error, errorString);
        }

        if (*error != QLandmarkManager::NoError) {
            if(landmarkIds)
                landmarkIds->clear();
            return false;
        }
        if (landmarkIds)
            landmarkIds->append(landmarks[i].landmarkId());
    }

    *error = QLandmarkManager::NoError;
    *errorString = "";
    return true;
}

bool DatabaseOperations::importLandmarksGpx(QIODevice *device,
                        QLandmarkManager::TransferOption option,
                        const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        QueryRun *queryRun,
                        QList<QLandmarkId> *landmarkIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QLandmarkCategory category;
    if (option == QLandmarkManager::AttachSingleCategory) {
        category = this->category(categoryId, error, errorString);
        if ((*error) != QLandmarkManager::NoError) {
            return false;
        }
    }

    QLandmarkFileHandlerGpx gpxHandler(queryRun?&(queryRun->isCanceled):0);
    if (!gpxHandler.importData(device)) {
        *error = gpxHandler.error();
        *errorString = gpxHandler.errorString();
        return false;
    }

    QList<QLandmark> landmarks = gpxHandler.waypoints();
    for(int i =0; i < landmarks.count(); ++i) {
        if (option == QLandmarkManager::AttachSingleCategory)
            landmarks[i].addCategoryId(categoryId);

        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Import of gpx file canceled";
        } else {
            saveLandmarkHelper(&(landmarks[i]),error, errorString);
        }

        if (*error != QLandmarkManager::NoError) {
            if (landmarkIds)
                landmarkIds->clear();
            return false;
        }
        if (landmarkIds)
            landmarkIds->append(landmarks[i].landmarkId());
    }

    *error = QLandmarkManager::NoError;
    *errorString = "";
    return true;
}

bool DatabaseOperations::exportLandmarksLmx(QIODevice *device,
                        const QList<QLandmarkId> &landmarkIds,
                        QLandmarkManager::TransferOption option,
                        QLandmarkManager::Error *error,
                        QString *errorString) const
{
    QLandmarkFileHandlerLmx lmxHandler(queryRun?&(queryRun->isCanceled):0);

    QLandmarkFilter filter;
    QList<QLandmark> lms;
    if (landmarkIds.count() >0)
        lms = landmarks(landmarkIds, 0, error, errorString);
    else {
        QList<QLandmarkSortOrder> sortOrders;
        lms = landmarks(filter, sortOrders, -1, 0, error, errorString);
    }

    if (error && *error != QLandmarkManager::NoError)
    {
        if (*error == QLandmarkManager::CancelError)
            *errorString = "Export of lmx file was canceled";
            return false;
    }

    QList<QLandmarkCategory> categories = this->categories(QList<QLandmarkCategoryId>(),QLandmarkNameSort(),-1,0,error,errorString,true);
    if (*error != QLandmarkManager::NoError) {
            return false;
    }

    QHash<QString,QString> categoryIdNameHash;//local id to name
    foreach(const QLandmarkCategory &category, categories) {
            categoryIdNameHash.insert(category.categoryId().localId(),category.name());
    }

    lmxHandler.setTransferOption(option);
    lmxHandler.setLandmarks(lms);
    lmxHandler.setCategoryIdNameHash(categoryIdNameHash);

    bool result = lmxHandler.exportData(device);

    if (!result) {
        if (errorString)
            *error = lmxHandler.errorCode();
            *errorString = lmxHandler.errorString();
    } else {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }

    return result;
}

bool DatabaseOperations::exportLandmarksGpx(QIODevice *device,
                        const QList<QLandmarkId> &landmarkIds,
                        QLandmarkManager::Error *error,
                        QString *errorString) const
{
    QLandmarkFileHandlerGpx gpxHandler(queryRun?&(queryRun->isCanceled):0);

    QList<QLandmarkSortOrder> sortOrders;
    QLandmarkFilter filter;

    QList<QLandmark> lms;
    if (landmarkIds.count() > 0) {
        lms = landmarks(landmarkIds,0,error, errorString);
        gpxHandler.setBehavior(QLandmarkFileHandlerGpx::ExportAll);
    } else {
        lms = landmarks(filter, sortOrders, -1, 0, error, errorString);
    }

    if (error && *error != QLandmarkManager::NoError)
        return false;

    gpxHandler.setWaypoints(lms);

    bool result = gpxHandler.exportData(device);

    if (!result) {
        *error = gpxHandler.error();
        *errorString = gpxHandler.errorString();
    } else {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }

    return result;
}

bool DatabaseOperations::isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    QMap<QString,QVariant> bindValues;
    QString q0 = QString("select ?removable {?u a slo:LandmarkCategory ; slo:isRemovable ?removable . "
         "FILTER regex( ?u, '^%1$') }").arg(categoryId.localId());
    QSparqlResult *queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);

    if (queryResult->hasError()) {
        return true;
    }
    if (!queryResult->next()) {
        return true;
    }
    if((queryResult->value(0).toString().compare("true", Qt::CaseSensitive)) == 0) {
        return false;
    } else {
        return true;
    }
}

QLandmarkManager::SupportLevel DatabaseOperations::filterSupportLevel(const QLandmarkFilter &filter) const
{
    switch(filter.type()) {
        case QLandmarkFilter::DefaultFilter:
            return QLandmarkManager::NativeSupport;
        case QLandmarkFilter::AttributeFilter:
        {
            const QLandmarkAttributeFilter attribFilter(filter);
            QStringList filterKeys = attribFilter.attributeKeys();

            QStringList landmarkKeys;

            foreach(const QString key, filterKeys) {
                if (!supportedSearchableAttributes.contains(key))
                    return QLandmarkManager::NoSupport;
            }

            foreach (const QString &key, filterKeys) {
                if (attribFilter.matchFlags(key) & QLandmarkFilter::MatchCaseSensitive)
                    return QLandmarkManager::NoSupport;
            }
        }
        case QLandmarkFilter::BoxFilter:
        {
            return QLandmarkManager::NativeSupport;
        }
        case QLandmarkFilter::CategoryFilter:
        {
            return QLandmarkManager::NativeSupport;
        }
        case QLandmarkFilter::IntersectionFilter:
        {
            const QLandmarkIntersectionFilter andFilter(filter);
            const QList<QLandmarkFilter>& terms = andFilter.filters();
            QLandmarkManager::SupportLevel currentLevel = QLandmarkManager::NativeSupport;
            if (terms.count() ==0)
                return currentLevel;

            for(int i=0; i < terms.count();i++) {
                    if (filterSupportLevel(terms.at(i)) == QLandmarkManager::NoSupport)
                        return QLandmarkManager::NoSupport;
                    else if (filterSupportLevel(terms.at(i)) == QLandmarkManager::EmulatedSupport)
                        currentLevel = QLandmarkManager::EmulatedSupport;
            }
            return currentLevel;
        }
        case QLandmarkFilter::LandmarkIdFilter:
        {
            return QLandmarkManager::NativeSupport;
        }
        case QLandmarkFilter::InvalidFilter:
        {
            return QLandmarkManager::NativeSupport;
        }
        case QLandmarkFilter::NameFilter:
        {
            const QLandmarkNameFilter nameFilter(filter);
            if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                return QLandmarkManager::NoSupport;
            else
                return QLandmarkManager::NativeSupport;
        }
        case QLandmarkFilter::ProximityFilter:
        {
            return QLandmarkManager::NativeSupport;
        }
        case QLandmarkFilter::UnionFilter:
        {
            const QLandmarkUnionFilter orFilter(filter);
            const QList<QLandmarkFilter>& terms = orFilter.filters();
            QLandmarkManager::SupportLevel currentLevel = QLandmarkManager::NativeSupport;
            if (terms.count() == 0)
                return currentLevel;

            for (int i=0; i < terms.count(); i++) {
                if (filterSupportLevel(terms.at(i)) == QLandmarkManager::NoSupport)
                    return QLandmarkManager::NoSupport;
                else if (filterSupportLevel(terms.at(i)) == QLandmarkManager::EmulatedSupport)
                    currentLevel = QLandmarkManager::EmulatedSupport;
            }

            return currentLevel;
        }
        default: {
            return QLandmarkManager::NoSupport;
        }
    }
    return QLandmarkManager::NoSupport;
}

QLandmarkManager::SupportLevel DatabaseOperations::sortOrderSupportLevel(const QLandmarkSortOrder &sortOrder) const
{
    QLandmarkManager::SupportLevel currentLevel = QLandmarkManager::NativeSupport;

    switch(sortOrder.type()) {
    case (QLandmarkSortOrder::NoSort):
        break;
    case (QLandmarkSortOrder::NameSort): {
        QLandmarkNameSort  nameSort = sortOrder;
        if (nameSort.caseSensitivity() == Qt::CaseSensitive)
            currentLevel = QLandmarkManager::NoSupport;
        break;
    }
    default:
        currentLevel = QLandmarkManager::NoSupport;
    }
    return currentLevel;
}

QueryRun::QueryRun(QLandmarkAbstractRequest *req, const QString &uri, QLandmarkManagerEngineQsparql *eng, unsigned int runId)
    : request(req),
      error(QLandmarkManager::NoError),
      errorString(QString()),
      errorMap(QMap<int,QLandmarkManager::Error>()),
      managerUri(uri),
      isCanceled(false),
      isDeleted(false),
      engine(eng),
      runId(runId)
{
};

QueryRun::~QueryRun()
{
}

void QueryRun::run()
{
    {
        connectionName = QUuid::createUuid().toString();//each connection needs a unique name
        DatabaseOperations databaseOperations;
        databaseOperations.connectionName = connectionName;
        databaseOperations.managerUri = managerUri;
        databaseOperations.queryRun = this;

        QLandmarkAbstractRequest::RequestType requestType;

        QMutexLocker ml(&(engine->m_mutex));
        if (engine->m_requestRunHash.contains(request)) {
            requestType = request->type();
        }
        ml.unlock();
        error = QLandmarkManager::NoError;
        errorString ="";
        errorMap.clear();
        isCanceled = false;

        if (!isDeleted) {
            QMetaObject::invokeMethod(engine, "updateRequestState",
                                      Qt::QueuedConnection,
                                      Q_ARG(QLandmarkAbstractRequest *, request),
                                      Q_ARG(QLandmarkAbstractRequest::State, QLandmarkAbstractRequest::ActiveState),
                                      Q_ARG(unsigned int, runId));
        }

        switch(requestType){
        case QLandmarkAbstractRequest::LandmarkIdFetchRequest: {
                QLandmarkIdFetchRequest *idFetchRequest;
                QList<QLandmarkId> lmIds;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        idFetchRequest = static_cast<QLandmarkIdFetchRequest *>(request);
                        QLandmarkFilter filter = idFetchRequest->filter();
                        QList<QLandmarkSortOrder> sorting = idFetchRequest->sorting();
                        int limit = idFetchRequest->limit();
                        int offset = idFetchRequest->offset();
                        ml.unlock();
                        lmIds = databaseOperations.landmarkIds(filter,
                                                               sorting, limit, offset,
                                                               &error, &errorString);
                    }
                    ml.unlock();
                }

                if (!isDeleted) {                    
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkIdFetchRequest",
                                              Qt::QueuedConnection,
                                              Q_ARG(QLandmarkIdFetchRequest *, idFetchRequest),
                                              Q_ARG(QList<QLandmarkId>,lmIds),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }

                break;
            }
        case QLandmarkAbstractRequest::LandmarkFetchRequest: {
                QLandmarkFetchRequest *fetchRequest;
                QList<QLandmark> lms;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        fetchRequest = static_cast<QLandmarkFetchRequest *>(request);
                        QLandmarkFilter filter = fetchRequest->filter();
                        QList<QLandmarkSortOrder> sorting = fetchRequest->sorting();
                        int limit = fetchRequest->limit();
                        int offset = fetchRequest->offset();
                        ml.unlock();
                        lms = databaseOperations.landmarks(filter,
                                                           sorting, limit, offset,
                                                           &error, &errorString);
                    }
                    ml.unlock();
                }

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkFetchRequest",
                                              Qt::QueuedConnection,
                                              Q_ARG(QLandmarkFetchRequest *,fetchRequest),
                                              Q_ARG(QList<QLandmark>,lms),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }
                break;
            }
        case QLandmarkAbstractRequest::LandmarkFetchByIdRequest :
            {

                QLandmarkFetchByIdRequest *byIdRequest;
                QList<QLandmark> lms;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        byIdRequest = static_cast<QLandmarkFetchByIdRequest *> (request);
                        QList<QLandmarkId> lmIds= byIdRequest->landmarkIds();
                        ml.unlock();
                        lms = databaseOperations.landmarks(lmIds, &errorMap,
                                                           &error, &errorString);
                    }
                    ml.unlock();
                }
                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkFetchByIdRequest",
                                              Qt::QueuedConnection,
                                              Q_ARG(QLandmarkFetchByIdRequest *,byIdRequest),
                                              Q_ARG(QList<QLandmark>,lms),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(ERROR_MAP, errorMap),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }

                break;
            }
        case QLandmarkAbstractRequest::LandmarkSaveRequest :
        {
            QLandmarkSaveRequest *saveRequest;
            QList<QLandmark> lms;

            if (!isDeleted) {
                ml.relock();
                if (engine->m_requestRunHash.contains(request)) {
                    saveRequest = static_cast<QLandmarkSaveRequest *> (request);
                    lms = saveRequest->landmarks();
                    ml.unlock();
                    databaseOperations.saveLandmarks(&lms, &errorMap, &error, &errorString);
                }
                ml.unlock();
            }

            if (!isDeleted) {
                ml.relock();
                if (engine->m_requestRunHash.contains(request))
                    engine->m_requestRunHash.remove(request);
                ml.unlock();
                QMetaObject::invokeMethod(engine, "updateLandmarkSaveRequest",
                                          Qt::QueuedConnection,
                                          Q_ARG(QLandmarkSaveRequest *,saveRequest),
                                          Q_ARG(QList<QLandmark>,lms),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(ERROR_MAP, errorMap),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                          Q_ARG(unsigned int, runId));
            }

                break;
        }
        case QLandmarkAbstractRequest::LandmarkRemoveRequest :
        {
            QLandmarkRemoveRequest *removeRequest;

            if (!isDeleted) {
                ml.relock();
                if (engine->m_requestRunHash.contains(request)) {
                    removeRequest= static_cast<QLandmarkRemoveRequest *> (request);
                    QList<QLandmarkId> lmIds = removeRequest->landmarkIds();
                    ml.unlock();
                    databaseOperations.removeLandmarks(lmIds, &errorMap, &error, &errorString);
                }
                ml.unlock();
            }

            if (!isDeleted){
                ml.relock();
                if (engine->m_requestRunHash.contains(request))
                    engine->m_requestRunHash.remove(request);
                ml.unlock();
                QMetaObject::invokeMethod(engine, "updateLandmarkRemoveRequest",
                                          Qt::QueuedConnection,
                                          Q_ARG(QLandmarkRemoveRequest *,removeRequest),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(ERROR_MAP, errorMap),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                          Q_ARG(unsigned int, runId));
            }
            break;
        }
        case QLandmarkAbstractRequest::CategoryIdFetchRequest :
            {
                QLandmarkCategoryIdFetchRequest *catIdFetchRequest;
                QList<QLandmarkCategoryId> catIds;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        catIdFetchRequest = static_cast<QLandmarkCategoryIdFetchRequest *> (request);
                        QLandmarkNameSort nameSort = catIdFetchRequest->sorting();
                        int limit = catIdFetchRequest->limit();
                        int offset = catIdFetchRequest->offset();
                        ml.unlock();
                        catIds = databaseOperations.categoryIds(nameSort,
                                                                limit,
                                                                offset,
                                                                &error, &errorString);
                    }
                    ml.unlock();
                }
                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkCategoryIdFetchRequest",
                                              Qt::QueuedConnection,
                                              Q_ARG(QLandmarkCategoryIdFetchRequest *,catIdFetchRequest),
                                              Q_ARG(QList<QLandmarkCategoryId>,catIds),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }
                break;
            }
        case QLandmarkAbstractRequest::CategoryFetchRequest :
            {
                QLandmarkCategoryFetchRequest *fetchRequest;
                QList <QLandmarkCategory> cats;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        fetchRequest = static_cast<QLandmarkCategoryFetchRequest *> (request);
                        QList<QLandmarkCategoryId> categoryIds;
                        QLandmarkNameSort nameSort = fetchRequest->sorting();
                        int limit = fetchRequest->limit();
                        int offset = fetchRequest->offset();
                        bool needAll = true;
                        ml.unlock();
                        cats =databaseOperations.categories(categoryIds, nameSort,
                                                            limit, offset,
                                                            &error, &errorString, needAll);
                    }
                    ml.unlock();
                }
                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkCategoryFetchRequest",
                                              Q_ARG(QLandmarkCategoryFetchRequest *,fetchRequest),
                                              Q_ARG(QList<QLandmarkCategory>,cats),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }
                break;
            }
        case QLandmarkAbstractRequest::CategoryFetchByIdRequest :
            {
                QLandmarkCategoryFetchByIdRequest *byIdRequest;
                QList<QLandmarkCategory> categories;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        byIdRequest = static_cast<QLandmarkCategoryFetchByIdRequest *> (request);
                        QList<QLandmarkCategoryId> categoryIds = byIdRequest->categoryIds();
                        ml.unlock();
                        categories = databaseOperations.categories(categoryIds,&errorMap,
                                                                   &error, &errorString);
                    }
                    ml.unlock();
                }

                if (this->isCanceled) {
                    categories.clear();
                    error = QLandmarkManager::CancelError;
                    errorString = "Category save request was canceled";
                }

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkCategoryFetchByIdRequest",
                                              Q_ARG(QLandmarkCategoryFetchByIdRequest *,byIdRequest),
                                              Q_ARG(QList<QLandmarkCategory>, categories),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(ERROR_MAP, errorMap),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }
                break;
            }
        case QLandmarkAbstractRequest::CategorySaveRequest :
        {
            QLandmarkCategorySaveRequest *saveRequest;
            QList<QLandmarkCategory> categories;

            if (!isDeleted) {
                ml.relock();
                if (engine->m_requestRunHash.contains(request)) {
                    saveRequest = static_cast<QLandmarkCategorySaveRequest *> (request);
                    categories = saveRequest->categories();
                    ml.unlock();
                    databaseOperations.saveCategories( &categories, &errorMap, &error, &errorString);
                }
                ml.unlock();
            }

            if (this->isCanceled) {
                categories.clear();
                error = QLandmarkManager::CancelError;
                errorString = "Category save request was canceled";
            }

            if (!isDeleted) {
                ml.relock();
                if (engine->m_requestRunHash.contains(request))
                    engine->m_requestRunHash.remove(request);
                ml.unlock();
                QMetaObject::invokeMethod(engine, "updateLandmarkCategorySaveRequest",
                                          Q_ARG(QLandmarkCategorySaveRequest *,saveRequest),
                                          Q_ARG(QList<QLandmarkCategory>,categories),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(ERROR_MAP, errorMap),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                          Q_ARG(unsigned int, runId));
            }

            break;
        }
        case QLandmarkAbstractRequest::CategoryRemoveRequest :
            {
                QLandmarkCategoryRemoveRequest *removeRequest;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        removeRequest = static_cast<QLandmarkCategoryRemoveRequest *> (request);
                        QList<QLandmarkCategoryId> categoryIds = removeRequest->categoryIds();
                        ml.unlock();
                        databaseOperations.removeCategories(categoryIds,
                                                            &errorMap, &error,
                                                            &errorString);
                    }
                    ml.unlock();
                }

               if (this->isCanceled) {
                   error = QLandmarkManager::CancelError;
                   errorString = "Category remove request was canceled";
               }

               if (!isDeleted) {
                   ml.relock();
                   if (engine->m_requestRunHash.contains(request))
                       engine->m_requestRunHash.remove(request);
                   ml.unlock();
                   QMetaObject::invokeMethod(engine, "updateLandmarkCategoryRemoveRequest",
                                             Q_ARG(QLandmarkCategoryRemoveRequest *,removeRequest),
                                             Q_ARG(QLandmarkManager::Error, error),
                                             Q_ARG(QString, errorString),
                                             Q_ARG(ERROR_MAP, errorMap),
                                             Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                             Q_ARG(unsigned int, runId));
               }
               break;
            }
        case QLandmarkAbstractRequest::ImportRequest :
            {
               QLandmarkImportRequest *importRequest;
               QList<QLandmarkId> landmarkIds;

               if (!isDeleted) {
                   ml.relock();
                   if (engine->m_requestRunHash.contains(request)) {
                       importRequest = static_cast<QLandmarkImportRequest *> (request);
                       QIODevice *device = importRequest->device();
                       QString format = importRequest->format();
                       QLandmarkManager::TransferOption transferOption = importRequest->transferOption();
                       QLandmarkCategoryId categoryId = importRequest->categoryId();
                       ml.unlock();
                       databaseOperations.importLandmarks(device,
                                                          format, transferOption,
                                                          categoryId,
                                                          &error, &errorString, this, &landmarkIds);
                   }
                   ml.unlock();
               }

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkImportRequest",
                                              Q_ARG(QLandmarkImportRequest *, importRequest),
                                              Q_ARG(QList<QLandmarkId>, landmarkIds),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }

                break;
            }
        case QLandmarkAbstractRequest::ExportRequest :
            {
                QLandmarkExportRequest *exportRequest;

                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request)) {
                        exportRequest = static_cast<QLandmarkExportRequest *> (request);
                        QIODevice *device = exportRequest->device();
                        QString format = exportRequest->format();
                        QList<QLandmarkId> landmarkIds =  exportRequest->landmarkIds();
                        QLandmarkManager::TransferOption transferOption = exportRequest->transferOption();
                        ml.unlock();
                        databaseOperations.exportLandmarks(device,
                                                           format, landmarkIds,
                                                           transferOption,
                                                           &error, &errorString);
                    }
                    ml.unlock();
                }
                if (!isDeleted) {
                    ml.relock();
                    if (engine->m_requestRunHash.contains(request))
                        engine->m_requestRunHash.remove(request);
                    ml.unlock();
                    QMetaObject::invokeMethod(engine, "updateLandmarkExportRequest",
                                              Q_ARG(QLandmarkExportRequest *, exportRequest),
                                              Q_ARG(QLandmarkManager::Error, error),
                                              Q_ARG(QString, errorString),
                                              Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState),
                                              Q_ARG(unsigned int, runId));
                }
                break;
            }
        default:
            break;
        }
    }
}

const QStringList DatabaseOperations::coreAttributes = QStringList()
                                                      << "name"
                                                      << "latitude"
                                                      << "longitude"
                                                      << "altitude";

const QStringList DatabaseOperations::coreGenericAttributes = QStringList()
                                                      << "description"
                                                      << "iconUrl"
                                                      << "radius"
                                                      << "countryCode"
                                                      << "country"
                                                      << "state"
                                                      << "county"
                                                      << "city"
                                                      << "district"
                                                      << "street"
                                                      << "postCode"
                                                      << "phoneNumber"
                                                      << "url";

const QStringList DatabaseOperations::supportedSearchableAttributes = QStringList() << "name"
                                                     << "description"
                                                     << "countryCode"
                                                     << "country"
                                                     << "state"
                                                     << "county"
                                                     << "city"
                                                     << "district"
                                                     << "street"
                                                     << "postCode"
                                                     << "phoneNumber";

const QStringList DatabaseOperations::coreCategoryAttributes = QStringList()
                                                      << "name";

const QStringList DatabaseOperations::coreGenericCategoryAttributes = QStringList()
                                                     << "iconUrl";
