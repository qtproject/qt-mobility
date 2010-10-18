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

#include "databaseoperations_p.h"
#include "qlandmarkfilehandler_gpx_p.h"
#include "qlandmarkfilehandler_lmx_p.h"
#include "qlandmarkmanagerengine_sqlite_p.h"

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

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlDatabase>

#include <QFile>
#include <QDebug>
#include <math.h>
#include <QUuid>
#include <QSet>
#include <QMetaMethod>
#include <qnumeric.h>
#include <QXmlStreamReader>
#include <QStringList>

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

QVariant getCategoryAttribute(QLandmarkCategory *category, const QString &key)
{
    if (key.compare("name",Qt::CaseSensitive) == 0) {
        return category->name();
    } else if (key.compare("iconUrl",Qt::CaseSensitive) ==0) {
        return category->iconUrl();
    }

    Q_ASSERT(false);//This should not be reachable
    return QVariant();
}

void setCategoryAttribute(QLandmarkCategory *category, const QString &key, const QVariant &value)
{
    if (key.compare("name",Qt::CaseInsensitive) == 0) {
        category->setName(value.toString());
        return;
    } else if (key.compare("iconUrl",Qt::CaseInsensitive) ==0) {
        category->setIconUrl(QUrl(value.toUrl()));
        return;
    }

    Q_ASSERT(false);//This should not be reachable
    return;
}

QVariant getLandmarkAttribute(QLandmark *landmark, const QString &key) {
    if (key.compare("name",Qt::CaseSensitive) == 0) {
        return landmark->name();
    } else if (key.compare("description", Qt::CaseSensitive) == 0) {
        return landmark->description();
    } else if (key.compare("iconUrl",Qt::CaseSensitive) ==0) {
        return landmark->iconUrl();
    } else if (key.compare("radius", Qt::CaseSensitive) == 0) {
        return landmark->radius();
    } else if (key.compare("phoneNumber", Qt::CaseSensitive) == 0) {
        return landmark->phoneNumber();
    } else if (key.compare("url", Qt::CaseSensitive) ==0 ) {
        return landmark->url();
    } else if (key.compare("latitude", Qt::CaseSensitive)== 0) {
        return landmark->coordinate().latitude();
    } else if (key.compare("longitude", Qt::CaseSensitive) ==0) {
        return landmark->coordinate().longitude();
    } else if (key.compare("altitude", Qt::CaseSensitive) ==0) {
        return landmark->coordinate().altitude();
    } else if (key.compare("country", Qt::CaseSensitive) ==0 ){
        return landmark->address().country();
    }  else if (key.compare("countryCode", Qt::CaseSensitive) ==0 ){
        return landmark->address().countryCode();
    } else if (key.compare("state", Qt::CaseSensitive) ==0 ){
        return landmark->address().state();
    } else if (key.compare("county", Qt::CaseSensitive) ==0 ){
        return landmark->address().county();
    } else if (key.compare("city", Qt::CaseSensitive) ==0 ){
        return landmark->address().city();
    } else if (key.compare("district", Qt::CaseSensitive) ==0 ){
        return landmark->address().district();
    } else if (key.compare("street", Qt::CaseSensitive) ==0 ){
        return landmark->address().street();
    } else if (key.compare("postcode", Qt::CaseSensitive) ==0 ){
        return landmark->address().postcode();
    }
    Q_ASSERT(false);//It shouldn't be possible to get here
    return QVariant();
}

void setLandmarkAttribute(QLandmark *landmark, const QString &key, const QVariant &value)
{
    if (key.compare("name",Qt::CaseSensitive) == 0) {
        landmark->setName(value.toString());
        return;
    } else if (key.compare("description", Qt::CaseSensitive) == 0) {
        landmark->setDescription(value.toString());
        return;
    } else if (key.compare("iconUrl",Qt::CaseSensitive) ==0) {
        landmark->setIconUrl(QUrl(value.toUrl()));
        return;
    } else if (key.compare("radius", Qt::CaseSensitive) == 0) {
        landmark->setRadius(value.toReal());
        return;
    } else if (key.compare("phoneNumber", Qt::CaseSensitive) == 0) {
        landmark->setPhoneNumber(value.toString());
        return;
    } else if (key.compare("url", Qt::CaseSensitive) ==0 ) {
        landmark->setUrl(QUrl(value.toUrl()));
        return;
    } else if (key.compare("latitude", Qt::CaseSensitive)== 0) {
        QGeoCoordinate coord = landmark->coordinate();
        coord.setLatitude(value.toDouble());
        landmark->setCoordinate(coord);
        return;
    } else if (key.compare("longitude", Qt::CaseSensitive) ==0) {
        QGeoCoordinate coord = landmark->coordinate();
        coord.setLongitude(value.toDouble());
        landmark->setCoordinate(coord);
        return;
    } else if (key.compare("altitude", Qt::CaseSensitive) ==0) {
        QGeoCoordinate coord = landmark->coordinate();
        coord.setAltitude(value.toDouble());
        landmark->setCoordinate(coord);
        return;
    } else if (key.compare("country", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setCountry(value.toString());
        landmark->setAddress(address);
        return;
    }  else if (key.compare("countryCode", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setCountryCode(value.toString());
        landmark->setAddress(address);
        return;
    } else if (key.compare("state", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setState(value.toString());
        landmark->setAddress(address);
        return;
    } else if (key.compare("county", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setCounty(value.toString());
        landmark->setAddress(address);
        return;
    } else if (key.compare("city", Qt::CaseSensitive) == 0 ){
        QGeoAddress address = landmark->address();
        address.setCity(value.toString());
        landmark->setAddress(address);
        return;
    } else if (key.compare("district", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setDistrict(value.toString());
        landmark->setAddress(address);
        return;
    } else if (key.compare("street", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setStreet(value.toString());
        landmark->setAddress(address);
        return;
    } else if (key.compare("postcode", Qt::CaseSensitive) ==0 ){
        QGeoAddress address = landmark->address();
        address.setPostcode(value.toString());
        landmark->setAddress(address);
        return;
    }
    Q_ASSERT(false);//it shouldn't be possible to get here
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
QList<QLandmarkId> sortQueryByDistance(QSqlQuery *query, const QLandmarkProximityFilter &proximityFilter,
                                       QLandmarkManager::Error * error, QString *errorString,
                                       const QString &managerUri,
                                       QueryRun * queryRun =0){
    QList<QLandmarkId> result;
    QList<LandmarkPoint>  sortedPoints;
    LandmarkPoint point;

    qreal radius = proximityFilter.radius();
    QGeoCoordinate center = proximityFilter.center();

    while(query->next()) {
        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Fetch operation canceled";
            return QList<QLandmarkId>();
        }

        point.coordinate.setLatitude(query->value(1).toDouble());
        point.coordinate.setLongitude(query->value(2).toDouble());

        point.landmarkId.setManagerUri(managerUri);
        point.landmarkId.setLocalId(QString::number(query->value(0).toInt()));

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

bool executeQuery(QSqlQuery *query, const QString &statement, const QMap<QString,QVariant> &bindValues,
                QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(query != NULL);
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    bool success = false;
    enum {Prepare =0 , Execute=1};
    for(int stage=Prepare; stage <= Execute; ++stage) {
        if ( stage == Prepare) {
            if (bindValues.count() != 0)
                success = query->prepare(statement);
            else
                 success = true;
        }
        else { // stage == Execute
            if (bindValues.count() != 0)
                success = query->exec();
            else
                success = query->exec(statement);
        }

        if (!success) {
            QString errorText;
            errorText = "Problem: Could not %1 statement: %2"
                "Reason: %3"
                "Parameters: %4\n";
            QString parameters;

            QStringList keys = bindValues.keys();
            if (keys.count() > 0) {
                for(int i = 0; i < keys.count(); ++i) {
                    parameters.append(QString("\n\t[:") + keys.at(i) + "]: " + bindValues.value(keys.at(i)).toString());
                }
            } else {
                parameters = "None";
            }

            int result = query->lastError().number();
            if (result == 26 || result == 11) {//SQLILTE_NOTADB || SQLITE_CORRUPT
                *error = QLandmarkManager::UnknownError;
            } else if ( result == 8) {//SQLITE_READONLY
                *error = QLandmarkManager::PermissionsError;
            }
            else {
                *error = QLandmarkManager::UnknownError;
            }

            *errorString =  errorText.arg(stage == Prepare ?"prepare":"execute")
                                     .arg(statement)
                                     .arg(query->lastError().text())
                                     .arg(parameters);

            query->finish();
            query->clear();
            return false;
        }

        QStringList keys = bindValues.keys();
        if (stage == Prepare && bindValues.count()!=0) {
            foreach(const QString &key, keys)
                query->bindValue(QString(":") + key,bindValues.value(key));
        }
    }

    *error = QLandmarkManager::NoError;
    *errorString ="";
    return true;
}

QString queryStringForRadius(const QGeoCoordinate &coord, qreal radius)
{
    Q_UNUSED(radius);
    return "SELECT id, latitude, longitude FROM landmark;";
}

QString landmarkIdsDefaultQueryString()
{
    return QString("SELECT id, latitude, longitude FROM landmark ");
}

QString landmarkIdsQueryString(const QList<QLandmarkId> ids)
{
    QString queryString = "SELECT id, latitude, longitude FROM landmark WHERE id IN (";
    foreach(const QLandmarkId &id, ids) {
        queryString += id.localId() += ",";
    }
    if (ids.count() > 0)
        queryString.chop(1);

    queryString += ") ";
    return queryString;
}

QString landmarkIdsNameQueryString(const QLandmarkNameFilter &filter)
{
    if (filter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
        return QString("SELECT id FROM landmark WHERE name = \"%1\" ").arg(filter.name());
    else
        return QString("SELECT id FROM landmark WHERE name LIKE \"%1\" ").arg(filter.name());
}

QString landmarkIdsCategoryQueryString(const QLandmarkCategoryFilter &filter)
{
    return QString("SELECT landmarkId FROM landmark_category WHERE categoryId=%1 ").arg(filter.categoryId().localId());
}

QString landmarkIdsBoxQueryString(const QLandmarkBoxFilter &filter)
{
    double tly = filter.boundingBox().topLeft().latitude();
    double bry = filter.boundingBox().bottomRight().latitude();
    double tlx = filter.boundingBox().topLeft().longitude();
    double brx = filter.boundingBox().bottomRight().longitude();

    //note: it is already assumed tly > bry
    bool lonWrap = (tlx > brx);

    QString queryString = "SELECT id, latitude, longitude FROM landmark WHERE (";

    //TODO: handle case where the poles are covered by the bounding box

    queryString += QString::number(bry);
    queryString += " <= latitude AND latitude <= ";
    queryString += QString::number(tly);

    queryString += ") AND (";

    if (lonWrap) {
        queryString += QString::number(tlx - 0.001);
        queryString += " <= longitude OR longitude <= ";
        queryString += QString::number(brx + 0.001);
    } else {
        queryString += QString::number(tlx - 0.001);
        queryString += " <= longitude AND longitude <= ";
        queryString += QString::number(brx + 0.001);
    }

    queryString += ") ";
    return queryString;
}

bool removeLandmarkHelper(const QString &connectionName, const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString,
        const QString &managerUri)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (landmarkId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QString q0 = QString("SELECT 1 FROM landmark WHERE id = %1;").arg(landmarkId.localId());
    QSqlQuery query(q0, db);
    if (!query.next()) {
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
         if (errorString)
            *errorString = QString("Landmark with local id, %1, does not exist in database").arg(landmarkId.localId());
        return false;
    }

    QStringList queryStrings;
    queryStrings << "DELETE FROM landmark WHERE id = :lmId;";
    queryStrings << "DELETE FROM landmark_category WHERE landmarkId = :lmId;";
    queryStrings << "DELETE FROM landmark_attribute WHERE landmarkId=:lmId";

    foreach(const QString &queryString, queryStrings) {
        if(!query.prepare(queryString)) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Unable to prepare statement: %1\nReason:%2")
                           .arg(query.lastQuery())
                           .arg(query.lastError().text());
            return false;
        }

        query.bindValue(":lmId", landmarkId.localId());

        if (!query.exec()) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Unable to execute statement: %1\nReason: %2")
                               .arg(query.lastQuery())
                               .arg(query.lastError().text());
            return false;
        }

    }
    *error = QLandmarkManager::NoError;
    *errorString ="";
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
}

QLandmark DatabaseOperations::retrieveLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    //it is assumed the connection name is valid when this call is made.
    QSqlDatabase db = QSqlDatabase::database(connectionName,false);

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

    QStringList columns;
    columns << "name";
    columns << "latitude";
    columns << "longitude";
    columns << "altitude";

    if (queryRun && queryRun->isCanceled) {
        return QLandmark();
    }

    QString queryString = QString("SELECT %1 FROM landmark WHERE id = %2;").arg(columns.join(",")).arg(landmarkId.localId());
     QSqlQuery query1(db);
     if (!query1.exec(queryString)) {
        if (error)
            *error =  QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2").arg(query1.lastQuery()).arg(query1.lastError().text());
        return QLandmark();
     }

     if (queryRun && queryRun->isCanceled) {
         return QLandmark();
     }

      bool found=false;
      while (query1.next()) {
        if (found) {
            if (error)
                *error = QLandmarkManager::UnknownError;
            if (errorString)
                *errorString = QString("Landmark database is corrupted.  Two landmarks were found with the same id: %1")
                                .arg(landmarkId.localId());

            qWarning() << "Non-unique primary key in table \"landmark\"";
            return QLandmark();
        } else {
            found = true;
        }

        if (!query1.value(0).isNull()) {
            lm.setName(query1.value(0).toString());
        }

        bool ok;
        QGeoCoordinate coord;

        if (!query1.value(1).isNull()) {
            coord.setLatitude(query1.value(1).toDouble(&ok));
            if (!ok) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Landmark database is corrupted. Latitude is not a double for landmark id: %1")
                               .arg(landmarkId.localId());
                return QLandmark();
            }
        }

        if (!query1.value(2).isNull()) {
            coord.setLongitude(query1.value(2).toDouble(&ok));
            if (!ok) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Landmark database is corrupted. Longitude is not a double for landmark id: %1")
                               .arg(landmarkId.localId());
                return QLandmark();
            }
        }

        if (!query1.value(3).isNull()) {
            coord.setAltitude(query1.value(3).toDouble(&ok));
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
        QString q2 = QString("SELECT categoryId FROM landmark_category WHERE landmarkId = %1;").arg(landmarkId.localId());
        QSqlQuery query2(db);
        if (!query2.exec(q2)) {
            if (error)
                *error =  QLandmarkManager::BadArgumentError;
            if (errorString)
                *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2").arg(query2.lastQuery()).arg(query2.lastError().text());

           return QLandmark();
        }

        while (query2.next()) {
            if (queryRun && queryRun->isCanceled) {
                return QLandmark();
            }

            QLandmarkCategoryId id;
            id.setManagerUri(uri);
            id.setLocalId(query2.value(0).toString());
            lm.addCategoryId(id);
        }

        query2.clear();
        query2.finish();

        QMap<QString,QVariant> bindValues;
        bindValues.insert("lmId", landmarkId.localId());
        if (!executeQuery(&query2,"SELECT key,value FROM landmark_attribute WHERE landmarkId = :lmId",bindValues,error,errorString)){
            return QLandmark();
        }

        while(query2.next()) {
            QString key = query2.value(0).toString();
            if (coreGenericAttributes.contains(key)) {
                //TODO: optimization
                setLandmarkAttribute(&lm, key, query2.value(1));
            } else {
                qWarning() << "Database is corrupt it contains an unrecognised generic key: " << key;
            }
        }

        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";

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

    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (!db.isValid()) {
        if(error)
            *error = QLandmarkManager::UnknownError;
        if(errorString)
            *errorString = QString("Invalid QSqlDatabase object used in landmark retrieval, "
                                   "connection name = %1").arg(connectionName);
        return result;
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
                QSqlQuery query(db);
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
                    queryString = QString("SELECT id FROM landmark");

                    break;
                } else {
                    if (attributeFilter.operationType() == QLandmarkAttributeFilter::AndOperation) {
                        QStringList lmLocalIds;
                        if (attributeFilter.attributeKeys().contains("name")) {
                            QLandmarkNameFilter nameFilter;
                            nameFilter.setName(attributeFilter.attribute("name").toString());
                            nameFilter.setMatchFlags(attributeFilter.matchFlags("name"));
                            QList<QLandmarkSortOrder> sortOrders;
                            QList<QLandmarkId> lmIds = landmarkIds(nameFilter,sortOrders,limit,offset,error,errorString);
                            if (*error != QLandmarkManager::NoError) {
                                return QList<QLandmarkId>();
                            }

                            foreach(const QLandmarkId &id, lmIds){
                                lmLocalIds << id.localId();
                            }
                            filterKeys.removeAll("name");
                        } else {
                            bindValues.insert("key", filterKeys.at(0));
                            queryString = QString("SELECT landmarkId FROM %1 WHERE key = :key");
                            queryString = queryString.arg("landmark_attribute");

                            if (!executeQuery(&query, queryString,
                                              bindValues,error,errorString)) {
                                return QList<QLandmarkId>();
                            }

                            while(query.next()) {
                                lmLocalIds << query.value(0).toString();
                            }
                        }

                        if (filterKeys.count() > 0) {
                            QLandmarkId id;
                            id.setManagerUri(managerUri);

                            queryString = QString("SELECT key, value FROM %1 WHERE landmarkId=:lmId");
                            queryString = queryString.arg("landmark_attribute");

                            for (int i=0; i < lmLocalIds.count(); ++i) {
                                bindValues.clear();
                                bindValues.insert("lmId", lmLocalIds.at(i));
                                if (!executeQuery(&query, queryString,
                                                  bindValues, error, errorString)) {
                                    return QList<QLandmarkId>();
                                }

                                QMap<QString,QVariant> lmAttributes;
                                while(query.next()) {
                                    lmAttributes.insert(query.value(0).toString(), query.value(1));
                                }

                                bool isMatch = true;
                                QVariant lmAttributeValue;
                                QVariant filterAttributeValue;
                                foreach(const QString &filterAttributeKey, filterKeys) {
                                    if (!lmAttributes.contains(filterAttributeKey)) {
                                        isMatch = false;
                                        break;
                                    }

                                    if (!attributeFilter.attribute(filterAttributeKey).isValid()) {
                                        continue;
                                    } else  {
                                        lmAttributeValue = lmAttributes.value(filterAttributeKey);

                                        filterAttributeValue = attributeFilter.attribute(filterAttributeKey);

                                        if (filterAttributeValue.type() == QVariant::String) {

                                            if (matchString(lmAttributeValue.toString(),
                                                            filterAttributeValue.toString(),
                                                            attributeFilter.matchFlags(filterAttributeKey))) {
                                                continue;
                                            }
                                        } else {
                                            if (lmAttributeValue == filterAttributeValue) {
                                                continue;
                                            }
                                        }
                                        //didn't find a match
                                        isMatch = false;
                                        break;
                                    }
                                }

                                if (isMatch) {
                                    id.setLocalId(lmLocalIds.at(i));
                                    result << id;
                                }
                            }
                        } else {
                            QLandmarkId id;
                            id.setManagerUri(managerUri);
                            foreach(const QString &localId, lmLocalIds) {
                                id.setLocalId(localId);
                                result << id;
                            }
                        }
                    } else {
                        if (attributeFilter.attributeKeys().contains("name")) {
                            QLandmarkNameFilter nameFilter;
                            nameFilter.setName(attributeFilter.attribute("name").toString());
                            nameFilter.setMatchFlags(attributeFilter.matchFlags("name"));
                            QList<QLandmarkSortOrder> sortOrders;
                            QList<QLandmarkId> lmIds = landmarkIds(nameFilter,sortOrders,limit,offset,error,errorString);
                            if (*error != QLandmarkManager::NoError) {
                                return QList<QLandmarkId>();
                            }
                            result << lmIds;
                            filterKeys.removeAll("name");
                        }

                        queryString = QString("SELECT landmarkId FROM %1 WHERE ");
                        queryString = queryString.arg("landmark_attribute");

                        QVariant attributeValue;
                        for (int i=0; i < filterKeys.count(); ++i) {
                            key = filterKeys.at(i);
                            attributeValue = attributeFilter.attribute(key);

                            queryString.append(QString("(key = :key_") + key + " ");
                            bindValues.insert(QString("key_")+key, key);
                            if (attributeValue.isValid()) {
                                queryString.append("AND ");
                                if (attributeValue.type() == QVariant::String) {
                                    if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchExactly) {
                                        queryString.append("value  = :" + key + " ");
                                        bindValues.insert(key, attributeValue);
                                    } else {
                                        queryString.append("value LIKE :" + key + " ");

                                        if ((attributeFilter.matchFlags(key) & 3) == QLandmarkFilter::MatchEndsWith)
                                            bindValues.insert(key, QString("%") + attributeValue.toString());
                                        else if ((attributeFilter.matchFlags(key) & 3) == QLandmarkFilter::MatchStartsWith)
                                            bindValues.insert(key, attributeValue.toString() + "%");
                                        else if ((attributeFilter.matchFlags(key) & 3) == QLandmarkFilter::MatchContains)
                                            bindValues.insert(key, QString("%") + attributeValue.toString() + "%");
                                        else if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchFixedString)
                                            bindValues.insert(key, attributeValue.toString());
                                    }
                                } else {
                                    queryString.append("value  = :" + key + " ");
                                    bindValues.insert(key, attributeValue);
                                }
                            }
                            queryString.append(")");

                            if (i < (filterKeys.count() -1)) {
                                queryString.append(" OR ");
                            }
                        }
                        queryString.append(";");

                        if (!executeQuery(&query, queryString,
                                          bindValues,error,errorString)) {
                            return QList<QLandmarkId>();
                        }

                        QLandmarkId landmarkId;
                        while(query.next()) {
                            landmarkId.setLocalId(query.value(0).toString());
                            landmarkId.setManagerUri(managerUri);
                            if (!result.contains(landmarkId))
                                result << landmarkId;
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

            queryString = QString("SELECT id FROM landmark WHERE ");
            QString nameKey = "name";
            QString nameValue = nameFilter.name();

            if (nameValue.isEmpty()) {
                    if (nameFilter.matchFlags() == QLandmarkFilter::MatchExactly
                        || nameFilter.matchFlags() == QLandmarkFilter::MatchFixedString) {
                        queryString.append(nameKey + " IS NULL ");
                    } else {
                        queryString = "SELECT id from landmark ";
                    }
            } else if (nameFilter.matchFlags() == QLandmarkFilter::MatchExactly) {
                queryString.append(nameKey + " = :" + nameKey + " ");
                bindValues.insert(nameKey, nameValue);
            } else {
                queryString.append(nameKey + " LIKE :" + nameKey + " ");

                if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchEndsWith)
                    bindValues.insert(nameKey, QString("%") + nameValue );
                else if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchStartsWith)
                    bindValues.insert(nameKey, nameValue + "%");
                else if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchContains)
                    bindValues.insert(nameKey, QString("%") + nameValue + "%");
                else if (nameFilter.matchFlags() == QLandmarkFilter::MatchFixedString)
                    bindValues.insert(nameKey, nameValue);
            }
            break;
        }
    case QLandmarkFilter::LandmarkIdFilter: {
            QLandmarkIdFilter idFilter = filter;
                //provide a query string exeecute so to that sqlite can handle sorting by name
                queryString = landmarkIdsQueryString(idFilter.landmarkIds());
            break;
        }
    case QLandmarkFilter::CategoryFilter: {
        QSqlQuery query(db);
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
        bindValues.insert("catId",categoryId.localId());
        if (!executeQuery(&query,"SELECT * from category WHERE id = :catId", bindValues, error, errorString)){
            return result;
        }

        if (!query.next()) {
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
                    QSqlQuery idsQuery(db);
                        executeQuery(&idsQuery,landmarkIdsQueryString(idList),bindValues,error,errorString);
                        if (*error != QLandmarkManager::NoError) {
                            result.clear();
                            return result;
                        }

                        result = sortQueryByDistance(&idsQuery,proximityFilter,error,errorString,managerUri,queryRun);
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

            queryString.append("ORDER BY name ");

            if (nameSort.caseSensitivity() == Qt::CaseInsensitive)
                queryString.append("COLLATE NOCASE ");

            if (nameSort.direction() == Qt::AscendingOrder)
                queryString.append("ASC ");
            else
                queryString.append("DESC ");
             alreadySorted = true;
        }

        queryString.append(";");
        QSqlQuery query(db);
        if (!executeQuery(&query, queryString,bindValues,error,errorString)) {
            return result;
        }

        QLandmarkId id;
        while (query.next()) {
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

                    if (inBetween(query.value(1).toDouble(),minLat, maxLat)
                        &&  (longWrap) ? outside(query.value(2).toDouble(),minLong, maxLong)
                            : inBetween(query.value(2).toDouble(),minLong, maxLong)) {
                        id.setManagerUri(managerUri);
                        id.setLocalId(QString::number(query.value(0).toInt()));
                        result << id;
                    }
                } while (query.next());
            } else if ( filter.type() == QLandmarkFilter::ProximityFilter) {
                QLandmarkProximityFilter proximityFilter;
                proximityFilter = filter;
                query.previous();
                result << sortQueryByDistance(&query, proximityFilter,error,errorString,managerUri,queryRun);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else {
                id.setManagerUri(managerUri);
                id.setLocalId(QString::number(query.value(0).toInt()));
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
        result = QLandmarkManagerEngineSqlite::sortLandmarks(landmarks, sortOrders);
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

    QSqlDatabase db = QSqlDatabase::database(connectionName, false);
    if (!db.isValid()) {
        if(error)
            *error = QLandmarkManager::UnknownError;
         if(errorString)
            *errorString = QString("Invalid QSqlDatabase object used in landmark retrieval, "
                                    "connection name = %1").arg(connectionName);
        return result;
    }

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

//Performs the save sql queries but does not handle
//starting or rolling back/comitting transactions.
bool DatabaseOperations::saveLandmarkHelper(QLandmark *landmark,
                  QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (!landmark->landmarkId().managerUri().isEmpty() && landmark->landmarkId().managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return false;
    }

    bool update = landmark->landmarkId().isValid();

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QHash<QString, QVariant> bindValues;

    if (update) {
        bindValues.insert("id",landmark->landmarkId().localId());

        QString q0 = QString("SELECT 1 FROM landmark WHERE id = %1;").arg(landmark->landmarkId().localId());
        QSqlQuery query0(q0, db);
        if (!query0.next()) {
            if (error)
                *error = QLandmarkManager::LandmarkDoesNotExistError;
            if (errorString)
                *errorString = "Landmark id does not exist in this landmark manager.";

            return false;
        }
    }
    if (!landmark->name().isEmpty())
        bindValues.insert("name", landmark->name());
    else
        bindValues.insert("name", QVariant());

    QGeoCoordinate coord;
    coord = landmark->coordinate();

    if (qIsNaN(coord.latitude()) && qIsNaN(coord.longitude())) {
        bindValues.insert("latitude", QVariant());
        bindValues.insert("longitude", QVariant());
    } else if (!qIsNaN(coord.latitude()) && !qIsNaN(coord.longitude())
        && isValidLat(coord.latitude()) && isValidLong(coord.longitude())) {
        bindValues.insert("latitude", coord.latitude());
        bindValues.insert("longitude", coord.longitude());
    } else {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Landmark coordinate is not valid, latitude must between -90 and 90 and longitude must be between -180 and 180, or both "
                       "latitude and longitude are NaN";
        return false;
    }

    if (!qIsNaN(coord.altitude()))
        bindValues.insert("altitude", coord.altitude());
    else
        bindValues.insert("altitude",QVariant());

    QString q1;
    QStringList keys = bindValues.keys();

    if (update) {
        QStringList placeholderKeys = keys;
        for (int i=0; i < placeholderKeys.count(); ++i) {
            placeholderKeys[i] = placeholderKeys[i] + "= :" + placeholderKeys[i];
        }
        q1 = QString("UPDATE landmark SET %1 WHERE id = :lmId;").arg(placeholderKeys.join(","));
        bindValues.insert("lmId", landmark->landmarkId().localId());
    } else {
        q1 = QString("REPLACE INTO landmark (%1) VALUES (%2);").arg(keys.join(",")).arg(QString(":").append(keys.join(", :")));
    }

    QSqlQuery query1(db);

    if (!query1.prepare(q1)) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Unable to prepare statment: ") + q1 + "\nReason:" + query1.lastError().text();
        return false;
    }

    foreach(const QString &key, bindValues.keys()) {
        query1.bindValue(QString(":").append(key), bindValues.value(key));
    }

    if (!query1.exec()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = "Unable to execute statment: " + q1 + "\nReason:" + query1.lastError().text();
        return false;
    }

    QLandmarkId id;
    id.setManagerUri(managerUri);
    if (update)
        id.setLocalId(landmark->landmarkId().localId());
    else
        id.setLocalId(query1.lastInsertId().toString());

    QStringList lmCats;

    for (int i = landmark->categoryIds().size() - 1; i >= 0; --i) {
        QLandmarkCategoryId id = landmark->categoryIds().at(i);
        if (id.managerUri() == managerUri)
            lmCats << id.localId();
        else {
            *error = QLandmarkManager::CategoryDoesNotExistError;
            *errorString = "Landmark contains category that belongs to another manager";
            return false;
        }

    }

    QStringList queries;

    QString queryString;

    QSqlQuery catQuery(db);
    for (int i=0;i < lmCats.size(); ++i) {
        queryString = QString("SELECT id FROM category WHERE id = %1").arg(lmCats.at(i));
        if (!catQuery.exec(queryString)) {
            return false;
        }

        if (!catQuery.next()) {
            if (error)
                *error = QLandmarkManager::CategoryDoesNotExistError;
            if (errorString)
                *errorString = "Landmark contains a category id that does not exist";
            return false;
        }
    }
    catQuery.finish();

    queries << "CREATE TEMP TABLE IF NOT EXISTS lc_refresh (i INTEGER);";

    for (int i = 0; i < lmCats.size(); ++i) {
        queries << QString("INSERT INTO lc_refresh (i) VALUES (%1);").arg(lmCats.at(i));
    }

    queries << QString("DELETE FROM landmark_category WHERE landmarkId = %1 AND categoryId NOT IN lc_refresh;").arg(id.localId());
    queries << QString("REPLACE INTO landmark_category SELECT %1, i FROM lc_refresh;").arg(id.localId());
    queries << "DROP TABLE IF EXISTS lc_refresh;";

    for (int i = 0; i < queries.size(); ++i) {
        QSqlQuery query(db);
        if (!query.exec(queries.at(i))) {
            qWarning() << query.lastError().databaseText();
            *error = QLandmarkManager::UnknownError;
            *errorString = "Unable to execute statment: " + query.lastQuery() + "\nReason:" + query.lastError().text();
            return false;
        }
    }

    if (!update) {
        landmark->setLandmarkId(id);
    }

    QStringList attributeKeys = coreGenericAttributes;
    QSqlQuery query(db);

    foreach(const QString &key, attributeKeys) {
        if (!query.prepare("REPLACE INTO landmark_attribute (landmarkId,key,value) VALUES(:lmId,:key,:value)")) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Unable to prepare statement: %1 \nReason: %2")
                           .arg(query.lastQuery()).arg(query.lastError().text());
            return false;
        }

        query.bindValue(":lmId", landmark->landmarkId().localId());
        query.bindValue(":key", key);
        query.bindValue(":value", getLandmarkAttribute(landmark, key));

        if (!query.exec()) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Unable to execute statement: %1\nReason:%2")
                           .arg(query.lastQuery()).arg(query.lastError().text());
            return false;
        }
    }

    *error = QLandmarkManager::NoError;
    *errorString="";
    return true;
}

bool DatabaseOperations::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save landmark: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    bool result = saveLandmarkHelper(landmark, error, errorString);

    if (result)
        db.commit();
    else
        db.rollback();
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

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save landmarks: unable to begin transaction, reason: %1").arg(db.lastError().text());

        if (errorMap) {
            for (int i=0; i < landmark->size(); ++i)
                errorMap->insert(i, *error);
        }
        return false;
    }

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
                for (i; i < landmark->size(); ++i)
                    errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }

        QSqlQuery query(db);
        if (!query.exec("SAVEPOINT save")) {
            loopError = QLandmarkManager::UnknownError;
            loopErrorString = QString("Save landmarks: could not execute statement: %1\nReason:%2").arg(query.lastQuery()).arg(query.lastError().text());
            result = false;
        } else {
            result = saveLandmarkHelper(&(landmark->operator [](i)), &loopError, &loopErrorString);
        }

        if (errorMap && (loopError != QLandmarkManager::NoError))
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
            query.exec("ROLLBACK TO SAVEPOINT save");
        } else {
            query.exec("RELEASE SAVEPOINT save");
        }
    }

    db.commit();

    if (noErrors) {
        *error = QLandmarkManager::NoError;
        *errorString = "";
    } else {
        if (error)
            *error = lastError;
        if (errorString)
            *errorString = lastErrorString;
    }

    return noErrors;
}

bool DatabaseOperations::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Remove landmark: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    bool result = removeLandmarkHelper(connectionName, landmarkId, error, errorString, managerUri);
    if (result)
        db.commit();
    else
        db.rollback();
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

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Remove landmars: unable to begin transaction, reason: %1").arg(db.lastError().text());

        if (errorMap) {
            for (int i=0; i < landmarkIds.size(); ++i)
                errorMap->insert(i, *error);
        }
        return false;
    }

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
                for (i; i < landmarkIds.size(); ++i)
                    errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }

        QSqlQuery query(db);
        if (!query.exec("SAVEPOINT save")) {
            loopError = QLandmarkManager::UnknownError;
            loopErrorString = QString("Remove landmarks: could not execute statement: %1\nReason:%2").arg(query.lastQuery()).arg(query.lastError().text());
            result = false;
        } else {
            result = removeLandmarkHelper(connectionName, landmarkIds.at(i), &loopError, &loopErrorString, managerUri);
        }

        if (errorMap && loopError != QLandmarkManager::NoError)
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
            query.exec("ROLLBACK TO SAVEPOINT save");
        } else {
            query.exec("RELEASE SAVEPOINT save");
        }
    }

    db.commit();

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

    return noErrors;
}

QList<QLandmarkCategoryId> DatabaseOperations::categoryIds(const QLandmarkNameSort &nameSort,
                                       int limit, int offset,
                                       QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmarkCategoryId> result;

    QString uri = managerUri;
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QSqlQuery query(db);
    QString queryString("SELECT id FROM category ORDER BY name ");
    if (nameSort.caseSensitivity() == Qt::CaseInsensitive)
        queryString.append("COLLATE NOCASE ");
    else {
        *error = QLandmarkManager::NotSupportedError;
        *errorString = "Case sensitive name sorting of categories is not supported";
        return QList<QLandmarkCategoryId>();
    }

    if (nameSort.direction() == Qt::AscendingOrder)
        queryString.append("ASC;");
    else
        queryString.append("DESC;");

    if (!query.exec(queryString)) {
        if (error)
            *error = QLandmarkManager::UnknownError;
         if (errorString)
            *errorString = QString("Unable to execute query: %1 \nReason: %2").arg(query.lastQuery()).arg(query.lastError().text());
         return result;
    }

    while (query.next()) {
        if (queryRun && queryRun->isCanceled) {
            *error = QLandmarkManager::CancelError;
            *errorString = "Fetch operation was canceled";
            result.clear();
            return result;
        }

        QLandmarkCategoryId id;
        id.setManagerUri(managerUri);
        id.setLocalId(QString::number(query.value(0).toInt()));
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

    QStringList columns;
    columns << "name";

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QString q1 = QString("SELECT %1 FROM category WHERE id = %2;").arg(columns.join(",")).arg(landmarkCategoryId.localId());
    QSqlQuery query(q1, db);
    bool found = false;
    while (query.next()) {
        if (found) {
            // TODO set error - should never happen
            if (error)
                *error = QLandmarkManager::UnknownError;
            if (errorString)
                *errorString = "Database corruption, non-unique primary key in table \"category\"";
            qWarning() << "Non-unique primary key in table \"category\"";
            return QLandmarkCategory();
        } else {
            found = true;
        }

        if (!query.value(0).isNull())
            cat.setName(query.value(0).toString());
// TODO: category description is not a common data attribute, make it a platform specific attribute
//        if (!query.value(1).isNull())
//            cat.setDescription(query.value(1).toString());

        if (!query.value(2).isNull())
            cat.setIconUrl(query.value(2).toString());

        cat.setCategoryId(landmarkCategoryId);
    }

    if (!found) {
            *error = QLandmarkManager::CategoryDoesNotExistError;
            *errorString = "None of the existing categories match the given category id.";
    } else {
        query.finish();
        query.clear();
        QMap<QString,QVariant> bindValues;
        bindValues.insert("catId", landmarkCategoryId.localId());
        if (!executeQuery(&query,"SELECT key,value FROM category_attribute WHERE categoryId = :catId", bindValues, error, errorString)){
            return QLandmarkCategory();
        }

        while(query.next()) {
            QString key = query.value(0).toString();
            if (coreGenericCategoryAttributes.contains(key)) {
                setCategoryAttribute(&cat,key,query.value(1));
            } else {
                qWarning() << "Database is corrupt it contains an unrecognised generic key: " << key;
            }
        }


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
        if (errorString)

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

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QMap<QString, QVariant> bindValues;
    QSqlQuery query(db);
    if (update) {
        bindValues.insert("id",category->categoryId().localId());

        QString q0 = QString("SELECT 1 FROM category WHERE id = :id;");
        if (!executeQuery(&query,q0, bindValues,error,errorString)) {
            return false;
        }

        if (!query.next()) {
            if (error)
                *error = QLandmarkManager::CategoryDoesNotExistError;
            if (errorString)
                *errorString = "Category id does not exist in this landmark manager.";

            return false;
        }
        bindValues.clear();
    }

    if (!category->name().isEmpty())
        bindValues.insert("name", category->name());
    else
        bindValues.insert("name", QVariant());

    QMap<QString,QVariant> tempBindValues;
    tempBindValues.insert("name", bindValues.value("name"));
    if (!executeQuery(&query, "SELECT id FROM category WHERE name = :name", tempBindValues,error,errorString)) {
        return false;
    }


    if (query.next()) {
        if (!update || (update && (query.value(0).toString() != category->categoryId().localId()))) {
           *error = QLandmarkManager::AlreadyExistsError;
           *errorString = QString("Category with name: %1 already exists").arg(category->name());
           return false;
        }
    }

    QString q1;
    QStringList keys = bindValues.keys();

    if (update) {
        QStringList placeholderKeys = keys;
        for (int i=0; i < placeholderKeys.count(); ++i) {
            placeholderKeys[i] = placeholderKeys[i] + "= :" + placeholderKeys[i];
        }
        q1 = QString("UPDATE category SET %1 WHERE id = :id;").arg(placeholderKeys.join(","));
        bindValues.insert("id",category->categoryId().localId());

    } else {
        q1 = QString("REPLACE INTO category (%1) VALUES (%2);").arg(keys.join(",")).arg(QString(":").append(keys.join(",:")));
    }

    if (!executeQuery(&query,q1,bindValues,error,errorString)) {
        return false;
    }

    if (!update) {
        QLandmarkCategoryId id;
        id.setManagerUri(managerUri);
        id.setLocalId(query.lastInsertId().toString());
        category->setCategoryId(id);
    }

    query.clear();
    query.finish();
    bindValues.clear();
    bindValues.insert("catId",category->categoryId().localId());
    QStringList attributeKeys = coreGenericCategoryAttributes;

    foreach(const QString &key, attributeKeys) {
        bindValues.clear();
        bindValues.insert(":catId", category->categoryId().localId());
        bindValues.insert(":key", key);
        bindValues.insert(":value", getCategoryAttribute(category,key));
        if (!executeQuery(&query, "REPLACE INTO category_attribute(categoryId,key,value) VALUES(:catId,:key,:value)", bindValues,error,errorString)) {
            return false;
        }
    }


    *error = QLandmarkManager::NoError;
    *errorString = "";

    return true;
}

bool DatabaseOperations::saveCategory(QLandmarkCategory *category,
                                      QLandmarkManager::Error *error, QString *errorString)
{

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save Category: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    bool result = saveCategoryHelper(category, error, errorString);
    if (result)
        db.commit();
    else
        db.rollback();
    return result;
}

bool DatabaseOperations::saveCategories(QList<QLandmarkCategory> * categories,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();


    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save Categories: unable to begin transaction, reason: %1").arg(db.lastError().text());

        if (errorMap) {
            for (int i=0; i < categories->size(); ++i)
                errorMap->insert(i, *error);
        }
        return false;
    }

    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    bool result;
    for (int i = 0; i < categories->size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString = "";

        if (queryRun && queryRun->isCanceled) {
            lastError = QLandmarkManager::CancelError;
            lastErrorString = "Category save was canceled";
            if (errorMap) {
                for (i; i < categories->size(); ++i)
                    errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }
        QSqlQuery query(db);
        if (!query.exec("SAVEPOINT save")) {
            loopError = QLandmarkManager::UnknownError;
            loopErrorString = QString("Save categories: could not execute statement: %1\nReason:%2").arg(query.lastQuery()).arg(query.lastError().text());
            result = false;
        } else {
           result = saveCategoryHelper(&(categories->operator [](i)), &loopError, &loopErrorString);
        }

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;

            if (errorMap)
                errorMap->insert(i, loopError);
            query.exec("ROLLBACK TO SAVEPOINT save");
        } else {
            query.exec("RELEASE SAVEPOINT save");
        }
    }

    db.commit();

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

    return noErrors;
}

bool DatabaseOperations::removeCategory(const QLandmarkCategoryId &categoryId,
                                      QLandmarkManager::Error *error, QString *errorString)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Remove category: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    bool result = removeCategoryHelper(categoryId, error, errorString);
    if (result)
        db.commit();
    else
        db.rollback();
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

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QMap<QString,QVariant> bindValues;
    bindValues.insert("catId", categoryId.localId());
    QString q0 = QString("SELECT 1 FROM category WHERE id = :catId");

    QSqlQuery query(db);
    if(!executeQuery(&query,q0,bindValues,error,errorString)) {
        return false;
    }

    if (!query.next()) {
        *error = QLandmarkManager::CategoryDoesNotExistError;
        *errorString = QString("Category with local id %1, does not exist in database")
                        .arg(categoryId.localId());
        return false;
    }

    QStringList queryStrings;
    queryStrings << "DELETE FROM category WHERE id = :catId";
    queryStrings << "DELETE FROM landmark_category WHERE categoryId = :catId";
    queryStrings << "DELETE FROM category_attribute WHERE categoryId= :catId";

    foreach(const QString &queryString, queryStrings) {
        if (!executeQuery(&query, queryString, bindValues, error,errorString)) {
            return false;
        }
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
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Remove category: unable to begin transaction, reason: %1").arg(db.lastError().text());

        if (errorMap) {
            for (int i=0; i < categoryIds.size(); ++i)
                errorMap->insert(i, *error);
        }
        return false;
    }

    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    bool result;
    for (int i = 0; i < categoryIds.size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString.clear();

        if (queryRun && queryRun->isCanceled) {
            lastError = QLandmarkManager::CancelError;
            lastErrorString = "Category remove was canceled";
            if (errorMap) {
                for (i; i < categoryIds.size(); ++i)
                    errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }

        QSqlQuery query(db);
        if (!query.exec("SAVEPOINT save")) {
            loopError = QLandmarkManager::UnknownError;
            loopErrorString = QString("Remove category: could not execute statement: %1\nReason:%2").arg(query.lastQuery()).arg(query.lastError().text());
            result = false;
        } else {
            result = removeCategoryHelper(categoryIds.at(i), &loopError, &loopErrorString);
        }

        if (!result) {
            if (errorMap)
                errorMap->insert(i, loopError);

            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
            query.exec("ROLLBACK TO SAVEPOINT save");
        } else {
            query.exec("RELEASE SAVEPOINT save");
        }
    }

    db.commit();

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
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if (!db.transaction()) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Import operation failed, unable to begin transaction, reason: %1")
                      .arg(db.lastError().text());
            return false;
        }

        result = importLandmarksLmx(device, option, categoryId, error, errorString, queryRun, landmarkIds);
        if (result)
            db.commit();
        else
            db.rollback();

        device->close();
        return result;
    } else if (detectedFormat == QLandmarkManager::Gpx) {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        if (!db.transaction()) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Import operation failed, unable to begin transaction, reason: %1")
                           .arg(db.lastError().text());
            return false;
        }

        result = importLandmarksGpx(device, option, categoryId, error, errorString, queryRun, landmarkIds);
        if (result)
            db.commit();
        else
            db.rollback();

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

    if (*error != QLandmarkManager::NoError)
        return false;

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

QueryRun::QueryRun(QLandmarkAbstractRequest *req, const QString &uri, QLandmarkManagerEngineSqlite *eng, unsigned int runId)
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
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(engine->m_dbFilename);
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

        if (!db.open()) {
            qWarning() << db.lastError().text();
        }

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
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
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
                                                              << "postcode"
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
                                                         << "postcode"
                                                         << "phoneNumber";

const QStringList DatabaseOperations::coreCategoryAttributes = QStringList()
                                                      << "name";

const QStringList DatabaseOperations::coreGenericCategoryAttributes = QStringList()
                                                               << "iconUrl";
