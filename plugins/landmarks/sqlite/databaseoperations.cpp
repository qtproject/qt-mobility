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

QTM_USE_NAMESPACE

namespace DatabaseOperationsHelpers{

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const double EARTH_MEAN_RADIUS = 6371.0072;
static const QStringList supportedSearchableAttributes = QStringList() << "name"
                                                                       << "description"
                                                                       << "phoneNumber"
                                                                       << "country"
                                                                       << "countryCode"
                                                                       << "state"
                                                                       << "county"
                                                                       << "city"
                                                                       << "district"
                                                                       << "street"
                                                                       << "postCode";

QString quoteString(const QString &s)
{
    QString q = "\"";
    q += s;
    q += "\"";
    return q;
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

bool executeQuery(QSqlQuery *query, const QString &statement, const QMap<QString,QVariant> &bindValues,
                QLandmarkManager::Error *error, QString *errorString)
{
    Q_ASSERT(query != NULL);
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    bool success = false;
    enum {Prepare =0 , Execute=1};
    for(int stage=Prepare; stage <= Execute; ++stage) {
        if ( stage == Prepare)
            success = query->prepare(statement);
        else // stage == Execute
            success = query->exec();

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
            }
            else if ( result == 8) {//SQLITE_READONLY
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
        if (stage == Prepare) {
            foreach(const QString &key, keys)
                query->bindValue(QString(":") + key,bindValues.value(key));
        }
    }

    *error = QLandmarkManager::NoError;
    *errorString ="";
    return true;
}

QString queryStringForRadius(const QGeoCoordinate &coord, double radius)
{
    return "SELECT id, latitude, longitude FROM landmark;";
}

QString landmarkIdsDefaultQueryString()
{
    return QString("SELECT id FROM landmark ");
}

QString landmarkIdsQueryString(const QLandmarkIdFilter &filter)
{
    QString queryString = "SELECT id FROM landmark WHERE id IN (";
    QList<QLandmarkId> ids = filter.landmarkIds();
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

QString landmarkIdsNearestQueryString(const QLandmarkProximityFilter &filter)
{
   return QString("SELECT id, latitude, longitude FROM landmark ");
}

bool removeLandmarkHelper(const QString &connectionName, const QLandmarkId &landmarkId,
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

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QString q0 = QString("SELECT 1 FROM landmark WHERE id = %1;").arg(landmarkId.localId());
    QSqlQuery query(q0, db);
    if (!query.next()) {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
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
    return true;
}

bool categoryNameCompare(const QLandmarkCategory &cat1, const QLandmarkCategory &cat2) {
    return (cat1.name() < cat2.name());
}

bool importLandmarksLmx(const QString &connectionName,
                        QIODevice *device,
                        QLandmarkManager::TransferOption option,
                        const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri)
{
    QLandmarkFileHandlerLmx lmxHandler(connectionName, managerUri);
    lmxHandler.setTransferOption(option);
    lmxHandler.setCategoryId(categoryId);
    bool result = lmxHandler.importData(device);
    if (!result) {
        *error = lmxHandler.errorCode();
        *errorString = lmxHandler.errorString();
    } else {
        *error = QLandmarkManager::NoError;
        *errorString = "";
    }

    return result;
}

bool importLandmarksGpx(const QString &connectionName,
                        QIODevice *device,
                        QLandmarkManager::TransferOption option,
                        const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri,
                        QueryRun *queryRun)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QLandmarkCategory category;
    if (option == QLandmarkManager::AttachSingleCategory) {
        category = DatabaseOperations::category(connectionName, categoryId, error, errorString, managerUri);
        if ((*error) != QLandmarkManager::NoError) {
            return false;
        }
    }

    QLandmarkFileHandlerGpx *gpxHandler = new QLandmarkFileHandlerGpx;
    if (queryRun) {
        queryRun->gpxHandler = gpxHandler;
        queryRun->gpxHandler->setAsync(true);
    }

    bool result = false;
    if (gpxHandler->importData(device)) {
            QList<QLandmark> landmarks = gpxHandler->waypoints();
            if (option == QLandmarkManager::AttachSingleCategory) {

                for (int i =0; i < landmarks.count(); ++i) {
                    landmarks[i].addCategoryId(categoryId);
                }
            }
            saveLandmarks(connectionName, &landmarks, 0, error, errorString, managerUri);

        if (*error != QLandmarkManager::NoError) {
            result = false;
        } else  {
            if (errorString)
                *errorString = "";
            result = true;
        }
    } else {
        *error = QLandmarkManager::ParsingError;
        *errorString = gpxHandler->errorString();
        result = false;
    }

    if (!queryRun)
        delete gpxHandler;
   //the query run will delete it's own gpx handler

    return result;
}

bool exportLandmarksGpx(const QString &connectionName,
                        QIODevice *device,
                        QList<QLandmarkId> landmarkIds,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri,
                        QueryRun *queryRun)
{
    QLandmarkFileHandlerGpx gpxHandler;

    QList<QLandmarkSortOrder> sortOrders;
    QLandmarkFilter filter;

    QList<QLandmark> lms;
    if (landmarkIds.count() > 0) {
        lms = ::landmarks(connectionName, landmarkIds,0,error, errorString,managerUri,queryRun);
        gpxHandler.setBehavior(QLandmarkFileHandlerGpx::ExportAll);
    } else {
        lms = ::landmarks(connectionName,filter, sortOrders, -1, 0, error, errorString, managerUri);
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

bool exportLandmarksLmx(const QString &connectionName,
                        QIODevice *device,
                        QList<QLandmarkId> landmarkIds,
                        QLandmarkManager::TransferOption option,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri,
                        QueryRun *queryRun =0)
{
    QLandmarkFileHandlerLmx lmxHandler(connectionName, managerUri);

    QLandmarkFilter filter;
    QList<QLandmark> lms;
    if (landmarkIds.count() >0)
        lms = DatabaseOperations::landmarks(connectionName, landmarkIds, 0, error, errorString, managerUri, queryRun);
    else {
        QList<QLandmarkSortOrder> sortOrders;
        lms = ::landmarks(connectionName, filter, sortOrders, -1, 0, error, errorString, managerUri, queryRun);
    }

    if (error && *error != QLandmarkManager::NoError)
        return false;

    lmxHandler.setTransferOption(option);
    lmxHandler.setLandmarks(lms);

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
}

/////////////////////////////////////////////////////////////
/////////////// Database Operation functions
///////////////
/////////////////////////////////////////////////////////////

using namespace DatabaseOperationsHelpers;

QLandmark DatabaseOperations::retrieveLandmark(const QString &connectionName, const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";

    //it is assumed the connection name is valid when this call is made.
    QSqlDatabase db = QSqlDatabase::database(connectionName,false);

    QString uri = managerUri;
    if (landmarkId.managerUri() != uri) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return QLandmark();
    }

    if (landmarkId.localId().isEmpty()) {
        *error = QLandmarkManager::BadArgumentError;
        *errorString = "Landmark local id is empty";
        return QLandmark();
    }

    QLandmark lm;

    QStringList columns;
    columns << "name";
    columns << "description";
    columns << "iconUrl";
    columns << "latitude";
    columns << "longitude";
    columns << "altitude";
    columns << "radius";
    columns << "topLeftLat";
    columns << "topLeftLon";
    columns << "bottomRightLat";
    columns << "bottomRightLon";
    columns << "country";
    columns << "countryCode";
    columns << "state";
    columns << "county";
    columns << "district";
    columns << "city";
    columns << "street";
    columns << "streetNumber";
    columns << "postcode";
    columns << "postOfficeBox";
    columns << "phoneNumber";
    columns << "url";

    if (queryRun && queryRun->isCanceled) {
        db.rollback();
        return QLandmark();
    }

    QString queryString = QString("SELECT %1 FROM landmark WHERE id = %2;").arg(columns.join(",")).arg(landmarkId.localId());
    bool transacting = db.transaction();
     QSqlQuery query1(db);
     if (!query1.exec(queryString)) {
        if (error)
            *error =  QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2").arg(query1.lastQuery()).arg(query1.lastError().text());

        if (transacting)
            db.rollback();
        return QLandmark();
     }

     if (queryRun && queryRun->isCanceled) {
         db.rollback();
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
            if (transacting)
                db.rollback();
            return QLandmark();
        } else {
            found = true;
        }

        if (!query1.value(0).isNull()) {
            lm.setName(query1.value(0).toString());
        }
        if (!query1.value(1).isNull())
            lm.setDescription(query1.value(1).toString());

        if (!query1.value(2).isNull())
            lm.setIconUrl(query1.value(2).toString());

        QGeoCoordinate coord;
        QGeoAddress address;

        bool ok = false;
        if (!query1.value(3).isNull()) {
            coord.setLatitude(query1.value(3).toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted.  Latitude is not a double for landmark id: %1")
                                    .arg(landmarkId.localId());

                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        if (!query1.value(4).isNull()) {
            coord.setLongitude(query1.value(4).toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted.  Longitude is not a double for landmark id: %1")
                                    .arg(landmarkId.localId());
                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        if (!query1.value(5).isNull()) {
            coord.setAltitude(query1.value(5).toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted.  Altitude is not a double for landmark id: %1")
                                    .arg(landmarkId.localId());

                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        lm.setCoordinate(coord);

        ok = false;
        if (!query1.value(6).isNull()) {
            lm.setRadius(query1.value(6).toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted.  Radius is not a double for landmark id: %1")
                                    .arg(landmarkId.localId());

                qWarning() << "Not a double";
                if (transacting)
                     db.rollback();
                return QLandmark();
            }
        }

/* TODO: remove since QLandmark no longer
         has a geolocation and hence no bounding box.
        QRectF rect;

        ok = false;
        double tly = 0.0;
        if (!query1.value(7).isNull()) {
            tly = query1.value(7).toDouble(&ok);
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        double tlx = 0.0;
        if (!query1.value(8).isNull()) {
            tlx = query1.value(8).toDouble(&ok);
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        double bry = 0.0;
        if (!query1.value(9).isNull()) {
            bry = query1.value(9).toDouble(&ok);
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        double brx = 0.0;
        if (!query1.value(10).isNull()) {
            brx = query1.value(10).toDouble(&ok);
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    db.rollback();
                return QLandmark();
            }
        }

        rect.setTopLeft(QPointF(tlx, tly));
        rect.setBottomRight(QPointF(brx, bry));
*/
        if (!query1.value(11).isNull())
            address.setCountry(query1.value(11).toString());

        if (!query1.value(12).isNull())
            address.setCountryCode(query1.value(12).toString());

        if (!query1.value(13).isNull())
            address.setState(query1.value(13).toString());

        if (!query1.value(14).isNull())
            address.setCounty(query1.value(14).toString());

        if (!query1.value(15).isNull())
            address.setDistrict(query1.value(15).toString());

        if (!query1.value(16).isNull())
            address.setCity(query1.value(16).toString());

        if (!query1.value(17).isNull())
            address.setStreet(query1.value(17).toString());

        if (!query1.value(18).isNull())
            address.setStreetNumber(query1.value(18).toString());

        if (!query1.value(19).isNull())
            address.setPostCode(query1.value(19).toString());

        //if (!query1.value(20).isNull())
        //    address.setPostOfficeBox(query1.value(20).toString());

        lm.setAddress(address);

        if (!query1.value(21).isNull())
            lm.setPhoneNumber(query1.value(21).toString());

        if (!query1.value(22).isNull())
            lm.setUrl(query1.value(22).toString());

        lm.setLandmarkId(landmarkId);
    }

    if (found) {
        QString q2 = QString("SELECT categoryId FROM landmark_category WHERE landmarkId = %1;").arg(landmarkId.localId());
        QSqlQuery query2(db);
        if (!query2.exec(q2)) {
            if (error)
                *error =  QLandmarkManager::BadArgumentError;
            if (errorString)
                *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2").arg(query2.lastQuery()).arg(query2.lastError().text());

           if (transacting)
                db.rollback();
           return QLandmark();
        }

        while (query2.next()) {
            if (queryRun && queryRun->isCanceled) {
                db.rollback();
                return QLandmark();
            }

            QLandmarkCategoryId id;
            id.setManagerUri(uri);
            id.setLocalId(query2.value(0).toString());
            lm.addCategoryId(id);
        }

        QSqlQuery query(db);
        if (!query.prepare("SELECT key, value from landmark_attribute WHERE landmarkId=:lmId")) {
                *error = QLandmarkManager::UnknownError;
                *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2").arg(query.lastQuery()).arg(query.lastError().text());
                return QLandmark();
        }

        query.bindValue(":lmId", lm.landmarkId().localId());

        if (!query.exec()) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2").arg(query.lastQuery()).arg(query.lastError().text());
            return QLandmark();
        }

        while(query.next()) {
            lm.setCustomAttribute(query.value(0).toString(),query.value(1));
        }

        if (transacting)
            db.commit();

        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";

    } else {
        Q_ASSERT(error);
        Q_ASSERT(errorString);
        *error = QLandmarkManager::DoesNotExistError;
        *errorString = QString("Landmark with id, %1, does not exist.").arg(landmarkId.localId());
        if (transacting)
            db.rollback();
    }

    return lm;
}

QList<QLandmarkId> DatabaseOperations::landmarkIds(const QString &connectionName, const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        int limit, int offset,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun * queryRun)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";

    QList<QLandmarkId> result;
    bool alreadySorted = false;

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
    switch (filter.type()) {
    case QLandmarkFilter::InvalidFilter:
        break;
    case QLandmarkFilter::DefaultFilter:
        queryString = landmarkIdsDefaultQueryString();
        break;
    case QLandmarkFilter::AttributeFilter: {
            QLandmarkAttributeFilter attributeFilter = filter;
            QStringList attributeKeys = attributeFilter.attributeKeys();
            if (attributeKeys.count() > 0) {
                QSqlQuery query(db);
                QMap<QString, QVariant> bindValues;

                QString queryString;
                QString key;
                QStringList filterKeys = attributeFilter.attributeKeys();

                if (attributeFilter.attributeType() ==  QLandmarkAttributeFilter::ManagerAttributes) {
                    foreach(const QString key, filterKeys) {
                        if (!supportedSearchableAttributes.contains(key)) {
                            *error = QLandmarkManager::NotSupportedError;
                            *errorString = QString("Attribute key not searchable: ").arg(key);
                            return QList<QLandmarkId>();
                        }
                    }
                    QString attributeValue;

                    //try to see if we need to select all landmarks
                    //ie OR operation with a single invalid QVariant parameter
                    //AND operation with all invalid QVariant parameter
                    bool selectAll =false;
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
                    } else {
                        queryString = QString("SELECT id FROM landmark WHERE ");
                        for ( int i = 0; i < filterKeys.count(); ++i) {
                            key = filterKeys.at(i);
                            attributeValue = attributeFilter.attribute(key).toString();

                            //if we're doing an or operation with an invalid attribute value we need to return all landmarks
                            //since all landmarks will have that attribute key

                            if (!attributeFilter.attribute(key).isValid()) {
                                if( attributeFilter.operationType() == QLandmarkAttributeFilter::OrOperation) {
                                    queryString = "SELECT id FROM landmark";
                                    break;
                                } else {
                                    continue;
                                }
                            }


                            if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchExactly) {
                                queryString.append(key + " = :" + key + " ");
                                bindValues.insert(key, attributeValue);
                            } else {
                                queryString.append(key + " LIKE :" + key + " ");

                                if ((attributeFilter.matchFlags(key) & 3) == QLandmarkFilter::MatchEndsWith)
                                    bindValues.insert(key, QString("%") + attributeValue);
                                else if ((attributeFilter.matchFlags(key) & 3) == QLandmarkFilter::MatchStartsWith)
                                    bindValues.insert(key, attributeValue + "%");
                                else if ((attributeFilter.matchFlags(key) & 3) == QLandmarkFilter::MatchContains)
                                    bindValues.insert(key, QString("%") + attributeValue + "%");
                                else if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchFixedString)
                                    bindValues.insert(key, attributeValue);
                            }

                            if (i < (filterKeys.count() -1)) {
                                if (attributeFilter.operationType() == QLandmarkAttributeFilter::AndOperation)
                                    queryString.append(" AND ");
                                else
                                    queryString.append(" OR ");
                            }
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
                        result << landmarkId;
                        idsFound = true;
                    }

                } else { //must be custom attributes
                    if (attributeFilter.operationType() == QLandmarkAttributeFilter::AndOperation) {
                        bindValues.insert("key", attributeKeys.at(0));
                        queryString = "SELECT landmarkId FROM landmark_attribute WHERE landmark_attribute.key = :key";
                        if (!executeQuery(&query, queryString,
                                          bindValues,error,errorString)) {
                            return QList<QLandmarkId>();
                        }

                        QStringList lmLocalIds;
                        while(query.next()) {
                            lmLocalIds << query.value(0).toString();
                        }

                        QLandmarkId id;
                        id.setManagerUri(managerUri);
                        queryString = "SELECT key, value FROM landmark_attribute WHERE landmarkId=:lmId";
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
                            foreach(const QString &filterAttributeKey, attributeKeys) {
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
                        queryString = "SELECT landmarkId FROM landmark_attribute WHERE ";

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
            queryString = landmarkIdsNameQueryString(nameFilter);
            break;
        }
    case QLandmarkFilter::LandmarkIdFilter: {
            QLandmarkIdFilter idFilter = filter;
            if (sortOrders.length() == 1
                && sortOrders.at(0).type() == QLandmarkSortOrder::NameSort) {
                //provide a query string exeecute so to that sqlite can handle sorting by name
                queryString = landmarkIdsQueryString(idFilter);
            } else {
                result = idFilter.landmarkIds();
                idsFound = true;
            }
            break;
        }
    case QLandmarkFilter::CategoryFilter: {
        QLandmarkCategoryFilter categoryFilter = filter;
        queryString = landmarkIdsCategoryQueryString(categoryFilter);
        break;
        }
    case QLandmarkFilter::ProximityFilter: {
            QLandmarkProximityFilter proximityFilter = filter;
            if (proximityFilter.radius() < 0) {
                if (proximityFilter.selection() == QLandmarkProximityFilter::SelectNearestOnly) {
                    queryString = ::landmarkIdsNearestQueryString(proximityFilter);
                } else {
                    queryString =  ::landmarkIdsDefaultQueryString();
                }
                break;
            }
        }//fall through if we have a radius
    case QLandmarkFilter::BoxFilter: {
            QLandmarkBoxFilter boxFilter;
            if (filter.type() == QLandmarkFilter::BoxFilter) {
                boxFilter = filter;
            } else {
                QGeoCoordinate center;
                double radius;
                if(filter.type() == QLandmarkFilter::ProximityFilter) {
                    QLandmarkProximityFilter proximityFilter;
                    proximityFilter = filter;
                    center = proximityFilter.coordinate();
                    radius = proximityFilter.radius();
                }

                QGeoCoordinate topLeft = center;
                shiftCoordinate(&topLeft, 0, radius+1000);
                shiftCoordinate(&topLeft, 270, radius+1000);

                QGeoCoordinate bottomRight = center;
                shiftCoordinate(&bottomRight, 180, radius+1000);
                shiftCoordinate(&bottomRight, 90, radius+1000);

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
                result = ::landmarkIds( connectionName, filters.at(0),
                                QList<QLandmarkSortOrder>(), limit, offset, error, errorString, managerUri,queryRun);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else  {
                QSet<QString> ids;
                QList<QLandmarkId> firstResult = landmarkIds(connectionName,filters.at(0),
                                                QList<QLandmarkSortOrder>(), limit, offset, error, errorString,
                                                managerUri, queryRun);
                for (int j = 0; j < firstResult.size(); ++j) {
                    if (firstResult.at(j).isValid())
                        ids.insert(firstResult.at(j).localId());
                }

                for (int i = 1; i < filters.size(); ++i) {
                    if (queryRun && queryRun->isCanceled) {
                        *error = QLandmarkManager::CancelError;
                        *errorString = "Fetch operation canceled";
                        result.clear();
                        return result;
                    }

                    QList<QLandmarkId> subResult = landmarkIds(connectionName, filters.at(i),
                                                QList<QLandmarkSortOrder>(), limit, offset, error, errorString,
                                                managerUri, queryRun);

                    if (*error != QLandmarkManager::NoError) {
                        result.clear();
                        return result;
                    }
                    QSet<QString> subIds;
                    for (int j = 0; j < subResult.size(); ++j) {
                        if (subResult.at(j).isValid())
                            subIds.insert(subResult.at(j).localId());
                    }
                    ids &= subIds;
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
    case QLandmarkFilter::UnionFilter: {
            QLandmarkUnionFilter unionFilter = filter;
            QSet<QString> ids;
            QList<QLandmarkFilter> filters = unionFilter.filters();

            if (filters.size() == 0) {
                //do nothing
            } else if (filters.size() == 1) {
                result =  ::landmarkIds(connectionName, filters.at(0),
                                        QList<QLandmarkSortOrder>(), limit, offset, error, errorString,
                                        managerUri, queryRun);
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
                    QList<QLandmarkId> subResult = landmarkIds(connectionName, filters.at(i),
                                                               QList<QLandmarkSortOrder>(),
                                                               limit, offset,
                                                               error, errorString,
                                                               managerUri, queryRun);

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
        if (!query.exec(queryString)) {
            if (error)
                *error =  QLandmarkManager::BadArgumentError;
            if (errorString)
                *errorString = QString("Query Failed: \n Query: %1 \n Reason: %2")
                .arg(query.lastQuery()).arg(query.lastError().text());
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
                        *errorString = "Fetch operation failed";
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

                qreal radius = proximityFilter.radius();
                QGeoCoordinate center = proximityFilter.coordinate();
                if (radius > -1) {
                    QGeoCoordinate coordinate;

                    do {
                        if (queryRun && queryRun->isCanceled) {
                            *error = QLandmarkManager::CancelError;
                            *errorString = "Fetch operation failed";
                            return QList<QLandmarkId>();
                        }

                        coordinate.setLatitude(query.value(1).toDouble());
                        coordinate.setLongitude(query.value(2).toDouble());

                        if (coordinate.distanceTo(center) < radius || qFuzzyCompare(coordinate.distanceTo(center), radius))
                        {
                            id.setManagerUri(managerUri);
                            id.setLocalId(QString::number(query.value(0).toInt()));
                            result << id;
                        }
                    } while (query.next());
                } else { //no radius
                    if (proximityFilter.selection() == QLandmarkProximityFilter::SelectNearestOnly) {
                        bool ok = false;
                        double lat;
                        double lon;
                        int minId = -1;
                        double minDist = 0.0;
                        double dist;

                        do {
                            if (queryRun && queryRun->isCanceled) {
                                *error = QLandmarkManager::CancelError;
                                *errorString = "Fetch operation failed";
                                return QList<QLandmarkId>();
                            }
                            lat = query.value(1).toDouble(&ok);
                            Q_ASSERT(ok);
                            lon = query.value(2).toDouble(&ok);
                            Q_ASSERT(ok);

                            dist = QGeoCoordinate(lat, lon).distanceTo(proximityFilter.coordinate());
                            if (radius == -1 || dist < radius) {
                                if (minId == -1 || dist < minDist) {
                                    minId = query.value(0).toInt();
                                    minDist = dist;
                                }
                            }
                        } while (query.next());

                        if (minId != -1) {
                            QLandmarkId id;
                            id.setManagerUri(managerUri);
                            id.setLocalId(QString::number(minId));
                            result << id;
                        }
                    } else { //accept all results since we are proximity filter with no radius
                        id.setManagerUri(managerUri);
                        id.setLocalId(QString::number(query.value(0).toInt()));
                        result << id;
                    }
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
                *errorString = "Fetch operation failed";
                return QList<QLandmarkId>();
            }
            landmark = ::retrieveLandmark(connectionName,result.at(i),error,errorString, managerUri, queryRun);
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

QList<QLandmark> DatabaseOperations::landmarks(const QString &connectionName, const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        int limit, int offset,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun)
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

    QList<QLandmarkId> ids = ::landmarkIds(connectionName, filter, sortOrders, limit, offset, error, errorString, managerUri, queryRun);
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
            *errorString  = "Fetch operation failed";
            return QList<QLandmark>();
        }

        lm = ::retrieveLandmark(connectionName,id,error,errorString, managerUri);
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

QList<QLandmark> DatabaseOperations::landmarks(const QString &connectionName, const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun)
{
    QList<QLandmark> result;
    QLandmark lm;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString="";
    for (int i=0; i < landmarkIds.count(); ++i){
        lm = DatabaseOperations::retrieveLandmark(connectionName,landmarkIds.at(i),error,errorString,managerUri,queryRun);
        if (*error == QLandmarkManager::NoError) {
            result << lm;
        } else {
            if (errorMap)
                errorMap->insert(i, *error);
            lastError = *error;
            lastErrorString = *errorString;
        }
    }
    *error = lastError;
    *errorString =lastErrorString;
    return result;
}

//Performs the save sql queries but does not handle
//starting or rolling back/comitting transactions.
bool DatabaseOperations::saveLandmarkHelper(const QString &connectionName, QLandmark *landmark,
                  QLandmarkManager::Error *error, QString *errorString,
                  const QString &managerUri)
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

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QHash<QString, QVariant> bindValues;

    if (update) {
        bindValues.insert("id",landmark->landmarkId().localId());

        QString q0 = QString("SELECT 1 FROM landmark WHERE id = %1;").arg(landmark->landmarkId().localId());
        QSqlQuery query0(q0, db);
        if (!query0.next()) {
            if (error)
                *error = QLandmarkManager::DoesNotExistError;
            if (errorString)
                *errorString = "Landmark id does not exist in this landmark manager.";

            return false;
        }
    }

    if (!landmark->name().isEmpty())
        bindValues.insert("name", landmark->name());
    else
        bindValues.insert("name", QVariant());

    if (!landmark->description().isEmpty())
        bindValues.insert("description", landmark->description());
    else
        bindValues.insert("description", QVariant());

    if (!landmark->iconUrl().isEmpty())
        bindValues.insert("iconUrl",landmark->iconUrl().toString());
    else
        bindValues.insert("iconUrl", QVariant());

    QGeoCoordinate coord = landmark->coordinate();

    if (!qIsNaN(coord.latitude()))
        bindValues.insert("latitude", coord.latitude());
    else
        bindValues.insert("latitude", QVariant());

    if (!qIsNaN(coord.longitude()))
        bindValues.insert("longitude", coord.longitude());
    else
        bindValues.insert("longitude", QVariant());

    if (!qIsNaN(coord.altitude()))
        bindValues.insert("altitude", coord.altitude());
    else
        bindValues.insert("altitude",QVariant());

    if (landmark->radius() != -1.0)
        bindValues.insert("radius", landmark->radius());
    else
        bindValues.insert("radius", QVariant());

/*
    //TODO: remove since qlandmark no longer has a
    //      qgeolocation and hence no bounding box

    QRectF rect;

    columns << "top_left_lat";
    columns << "top_left_lon";
    columns << "bottom_right_lat";
    columns << "bottom_right_lon";

    if (!rect.isNull()) {
        values << QString::number(rect.topLeft().y());
        values << QString::number(rect.topLeft().x());
        values << QString::number(rect.bottomRight().y());
        values << QString::number(rect.bottomRight().x());
    } else {
        values << "null";
        values << "null";
        values << "null";
        values << "null";
    }
*/
    QGeoAddress address = landmark->address();

    if (!address.country().isEmpty())
        bindValues.insert("country", address.country());
    else
        bindValues.insert("country", QVariant());

    if (!address.countryCode().isEmpty())
        bindValues.insert("countryCode", address.countryCode());
    else
        bindValues.insert("countryCode", QVariant());

    if (!address.state().isEmpty())
        bindValues.insert("state", address.state());
    else
        bindValues.insert("state", QVariant());

    if (!address.county().isEmpty())
        bindValues.insert("county", address.county());
    else
        bindValues.insert("county", QVariant());

    if (!address.district().isEmpty())
        bindValues.insert("district", address.district());
    else
        bindValues.insert("district", QVariant());

    if (!address.city().isEmpty())
        bindValues.insert("city", address.city());
    else
        bindValues.insert("city", QVariant());

    if (!address.street().isEmpty())
        bindValues.insert("street", address.street());
    else
        bindValues.insert("street", QVariant());

    if (!address.streetNumber().isEmpty())
        bindValues.insert("streetNumber", address.streetNumber());
    else
        bindValues.insert("streetNumber", QVariant());

    if (!address.postCode().isEmpty())
        bindValues.insert("postCode", address.postCode());
    else
        bindValues.insert("postCode", QVariant());

/*
    if (!address.postOfficeBox().isEmpty())
        bindValues.insert("postOfficeBox", address.postOfficeBox());
    else
        bindValues.insert("postOfficeBox", QVariant());
*/
    if (!landmark->phoneNumber().isEmpty())
        bindValues.insert("phoneNumber", landmark->phoneNumber());
    else
        bindValues.insert("phoneNumber", QVariant());

    if (!landmark->url().isEmpty())
        bindValues.insert("url", landmark->url().toString());
    else
        bindValues.insert("url", QVariant());

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
        else
            landmark->removeCategoryId(id);
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
                *error = QLandmarkManager::BadArgumentError;
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

    QStringList attributekeys = landmark->customAttributeKeys();
    QSqlQuery query(db);
    if( !query.prepare("DELETE FROM landmark_attribute WHERE landmarkId= :lmId"))
    {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Unable to prepare statement: %1 \nReason: %2")
                       .arg(query.lastQuery()).arg(query.lastError().text());
        return false;
    }

    query.bindValue(":lmId", landmark->landmarkId().localId());

    if (!query.exec()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Unable to execute statement: %1\nReason:%2")
                       .arg(query.lastQuery()).arg(query.lastError().text());
        return false;
    }

    for (int i =0; i < attributekeys.count(); ++i) {
        if (!query.prepare("INSERT INTO landmark_attribute (landmarkId,key,value) VALUES(:lmId,:key,:value)")) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Unable to prepare statement: %1 \nReason: %2")
                           .arg(query.lastQuery()).arg(query.lastError().text());
            return false;
        }

        query.bindValue(":lmId", landmark->landmarkId().localId());
        query.bindValue(":key", attributekeys[i]);
        query.bindValue(":value", landmark->customAttribute(attributekeys.at(i)));

        if (!query.exec()) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Unable to execute statement: %1\nReason:%2")
                           .arg(query.lastQuery()).arg(query.lastError().text());
            return false;
        }
    }

    /*
    // grab keys from attributes tables for current id
    // delete those we no longer have
    // use replace for the rest

    // loop through attributes
    */

    return true;
}

bool DatabaseOperations::saveLandmark(const QString &connectionName, QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri)
{
    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save landmark: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    bool result = saveLandmarkHelper(connectionName, landmark, error, errorString, managerUri);

    if (result)
        db.commit();
    else
        db.rollback();
    return result;
}

bool DatabaseOperations::saveLandmarks(const QString &connectionName, QList<QLandmark> * landmark,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

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
            result = saveLandmarkHelper(connectionName, &(landmark->operator [](i)), &loopError, &loopErrorString, managerUri);
        }

        if (errorMap)
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

bool DatabaseOperations::removeLandmark(const QString &connectionName, const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString,
        const QString &managerUri)
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

bool DatabaseOperations::removeLandmarks(const QString &connectionName, const QList<QLandmarkId> &landmarkIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString, const QString &managerUri,
                    QueryRun *queryRun)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

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

        if (errorMap)
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

QList<QLandmarkCategoryId> DatabaseOperations::categoryIds(const QString &connectionName,
                                       const QLandmarkNameSort &nameSort,
                                       int limit, int offset,
                                       QLandmarkManager::Error *error, QString *errorString,
                                       const QString &managerUri,
                                       QueryRun *queryRun)
{
    QList<QLandmarkCategoryId> result;

    QString uri = managerUri;
    QSqlDatabase db = QSqlDatabase::database(connectionName);

    QSqlQuery query(db);
    QString queryString("SELECT id FROM category ORDER BY name ");
    if (nameSort.caseSensitivity() == Qt::CaseInsensitive)
        queryString.append("COLLATE NOCASE ");

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

QLandmarkCategory DatabaseOperations::category(const QString &connectionName, const QLandmarkCategoryId &landmarkCategoryId,
              QLandmarkManager::Error *error,
              QString *errorString, const QString &managerUri)
{
    if (!landmarkCategoryId.isValid() || landmarkCategoryId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Category id is not valid for this manager";
        return QLandmarkCategory();
    }

    QLandmarkCategory cat;

    QStringList columns;
    columns << "name";
    columns << "description";
    columns << "iconUrl";

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
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "None of the existing categories match the given category id.";
    } else {
        QMap<QString,QVariant> bindValues;
        bindValues.insert("catId", cat.categoryId().localId());
        if (!executeQuery(&query, "SELECT key, value from category_attribute WHERE categoryId=:catId",bindValues, error, errorString )) {
            return QLandmarkCategory();
         }

        while(query.next()) {
            cat.setCustomAttribute(query.value(0).toString(),query.value(1));
        }

        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }
    return cat;
}

QList<QLandmarkCategory> DatabaseOperations::categories(const QString &connectionName,
                const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                const QLandmarkNameSort &nameSort,
                int limit, int offset,
                QLandmarkManager::Error *error, QString *errorString,
                const QString &managerUri, bool needAll,
                QueryRun *queryRun)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    *error = QLandmarkManager::NoError;
    errorString->clear();

    QList<QLandmarkCategory> result;
    QList<QLandmarkCategoryId> ids = landmarkCategoryIds;
    if (ids.size() == 0) {
        ids = ::categoryIds(connectionName, nameSort, limit, offset, error, errorString, managerUri, queryRun);

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

        QLandmarkCategory cat = ::category(connectionName,ids.at(i), error,errorString, managerUri);
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


QList<QLandmarkCategory> DatabaseOperations::categories(const QString &connectionName,
                const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                QMap<int, QLandmarkManager::Error> *errorMap,
                QLandmarkManager::Error *error, QString *errorString,
                const QString &managerUri,
                QueryRun *queryRun)
{
    QList<QLandmarkCategory> result;
    QLandmarkCategory cat;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString ="";
    for (int i=0; i < landmarkCategoryIds.count(); ++i) {
        cat = DatabaseOperations::category(connectionName, landmarkCategoryIds.at(i),error,errorString,managerUri);

        if (*error == QLandmarkManager::NoError)
        {
            result << cat;
        } else {
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

bool DatabaseOperations::saveCategoryHelper(const QString &connectionName, QLandmarkCategory *category,
                QLandmarkManager::Error *error,
                QString *errorString, const QString &managerUri)
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
            *error = QLandmarkManager::DoesNotExistError;
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
                *error = QLandmarkManager::DoesNotExistError;
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

//    if (!category->description().isEmpty())
//        bindValues.insert("description", category->description());
//    else
//        bindValues.insert("description", QVariant());

    if (!category->iconUrl().isEmpty())
        bindValues.insert("iconUrl", category->iconUrl().toString());
    else
        bindValues.insert("iconUrl", QVariant());

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

    bindValues.clear();
    bindValues.insert("catId",category->categoryId().localId());
    QStringList attributekeys = category->customAttributeKeys();
    if (!executeQuery(&query,"DELETE FROM category_attribute WHERE categoryId= :catId", bindValues, error, errorString)) {
        return false;
    }

    for (int i =0; i < attributekeys.count(); ++i) {
        bindValues.clear();
        bindValues.insert("catId",category->categoryId().localId());
        bindValues.insert("key",attributekeys[i]);
        bindValues.insert("value",category->customAttribute(attributekeys.at(i)));

        if (!executeQuery(&query,"INSERT INTO category_attribute (categoryId,key,value) VALUES(:catId,:key,:value)", bindValues,
                         error, errorString)) {
            return false;
        }
    }

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    return true;
}

bool DatabaseOperations::saveCategory(const QString &connectionName, QLandmarkCategory *category,
                                      QLandmarkManager::Error *error, QString *errorString,
                                      const QString &managerUri)
{

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save Category: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    bool result = saveCategoryHelper(connectionName, category, error, errorString, managerUri);
    if (result)
        db.commit();
    else
        db.rollback();
    return result;
}

bool DatabaseOperations::saveCategories(const QString &connectionName, QList<QLandmarkCategory> * categories,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri)
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

        bool result = saveCategory(connectionName, &(categories->operator [](i)), &loopError, &loopErrorString, managerUri);

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

    //TODO: Notifications
    //if (addedIds.size() != 0)
    //    emit landmarksAdded(addedIds);

    //TODO: Notifications
    //if (changedIds.size() != 0)
    //    emit landmarksChanged(changedIds);

    return noErrors;
}

bool DatabaseOperations::removeCategory(const QString &connectionName, const QLandmarkCategoryId &categoryId,
                QLandmarkManager::Error *error,
                QString *errorString, const QString &managerUri)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (categoryId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Category id comes from different landmark manager.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save landmark: unable to begin transaction, reason: %1").arg(db.lastError().text());
        return false;
    }

    QMap<QString,QVariant> bindValues;
    bindValues.insert("catId", categoryId.localId());
    QString q0 = QString("SELECT 1 FROM category WHERE id = :catId");

    QSqlQuery query(db);
    if(!executeQuery(&query,q0,bindValues,error,errorString)) {
        db.rollback();
        return false;
    }

    if (!query.next()) {
        db.rollback();
        *error = QLandmarkManager::DoesNotExistError;
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
            db.rollback();
            return false;
        }
    }

     db.commit();
     *error = QLandmarkManager::NoError;
     *errorString = "";

    return true;
}

bool DatabaseOperations::removeCategories(const QString &connectionName, const QList<QLandmarkCategoryId> &categoryIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString, const QString &managerUri)
{
    QList<QLandmarkCategoryId> removedIds;

    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;
    for (int i = 0; i < categoryIds.size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString.clear();

        bool result = removeCategory(connectionName, categoryIds.at(i), &loopError, &loopErrorString, managerUri);

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

    //TODO: notifications
    //if (removedIds.size() != 0)
    //    emit landmarksRemoved(removedIds);

    return noErrors;
}

bool DatabaseOperations::importLandmarks(const QString &connectionName,
                     QIODevice *device,
                     const QString &format,
                     QLandmarkManager::TransferOption option,
                     const QLandmarkCategoryId &categoryId,
                     QLandmarkManager::Error *error,
                     QString *errorString, const QString &managerUri,
                     QueryRun *queryRun)
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
            result = importLandmarksLmx(connectionName, device, option, categoryId, error, errorString, managerUri);
            device->close();
            return result;
    } else if (detectedFormat == QLandmarkManager::Gpx) {
           result = importLandmarksGpx(connectionName, device, option, categoryId, error, errorString, managerUri, queryRun);
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

bool DatabaseOperations::exportLandmarks(const QString &connectionName,
                     QIODevice *device,
                     const QString &format,
                     QList<QLandmarkId> landmarkIds,
                     QLandmarkManager::TransferOption option,
                     QLandmarkManager::Error *error,
                     QString *errorString,
                     const QString &managerUri,
                     QueryRun *queryRun)
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
        result = exportLandmarksLmx(connectionName, device, landmarkIds, option, error, errorString, managerUri, queryRun);
        device->close();
        return result;
    } else if (format == QLandmarkManager::Gpx) {
        result = exportLandmarksGpx(connectionName, device, landmarkIds, error, errorString, managerUri, queryRun);
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

QLandmarkManager::SupportLevel DatabaseOperations::filterSupportLevel(const QLandmarkFilter &filter)
{
    switch(filter.type()) {
        case QLandmarkFilter::DefaultFilter:
            return QLandmarkManager::Native;
        case QLandmarkFilter::AttributeFilter:
        {
            const QLandmarkAttributeFilter attribFilter(filter);
            QStringList filterKeys = attribFilter.attributeKeys();

            QStringList landmarkKeys;
            if (attribFilter.attributeType() == QLandmarkAttributeFilter::ManagerAttributes) {
                foreach(const QString key, filterKeys) {
                    if (!supportedSearchableAttributes.contains(key))
                        return QLandmarkManager::None;
                }
            }
            foreach (const QString &key, filterKeys) {
                if (attribFilter.matchFlags(key) & QLandmarkFilter::MatchCaseSensitive)
                    return QLandmarkManager::None;
            }
        }
        case QLandmarkFilter::BoxFilter:
        {
            return QLandmarkManager::Native;
        }
        case QLandmarkFilter::CategoryFilter:
        {
            return QLandmarkManager::Native;
        }
        case QLandmarkFilter::IntersectionFilter:
        {
            const QLandmarkIntersectionFilter andFilter(filter);
            const QList<QLandmarkFilter>& terms = andFilter.filters();
            QLandmarkManager::SupportLevel currentLevel = QLandmarkManager::Native;
            if (terms.count() ==0)
                return currentLevel;

            for(int i=0; i < terms.count();i++) {
                    if (filterSupportLevel(terms.at(i)) == QLandmarkManager::None)
                        return QLandmarkManager::None;
                    else if (filterSupportLevel(terms.at(i)) == QLandmarkManager::Emulated)
                        currentLevel = QLandmarkManager::Emulated;
            }
            return currentLevel;
        }
        case QLandmarkFilter::LandmarkIdFilter:
        {
            return QLandmarkManager::Native;
        }
        case QLandmarkFilter::InvalidFilter:
        {
            return QLandmarkManager::Native;
        }
        case QLandmarkFilter::NameFilter:
        {
            const QLandmarkNameFilter nameFilter(filter);
            if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                return QLandmarkManager::None;
            else
                return QLandmarkManager::Native;
        }
        case QLandmarkFilter::ProximityFilter:
        {
            return QLandmarkManager::Native;
        }
        case QLandmarkFilter::UnionFilter:
        {
            const QLandmarkUnionFilter orFilter(filter);
            const QList<QLandmarkFilter>& terms = orFilter.filters();
            QLandmarkManager::SupportLevel currentLevel = QLandmarkManager::Native;
            if (terms.count() == 0)
                return currentLevel;

            for (int i=0; i < terms.count(); i++) {
                if (filterSupportLevel(terms.at(i)) == QLandmarkManager::None)
                    return QLandmarkManager::None;
                else if (filterSupportLevel(terms.at(i)) == QLandmarkManager::Emulated)
                    currentLevel = QLandmarkManager::Emulated;
            }

            return currentLevel;
        }
        default: {
            return QLandmarkManager::None;
        }
    }
    return QLandmarkManager::None;
}

QLandmarkManager::SupportLevel DatabaseOperations::sortOrderSupportLevel(const QList<QLandmarkSortOrder> &sortOrders)
{
    QLandmarkManager::SupportLevel currentLevel = QLandmarkManager::Native;
    foreach(const QLandmarkSortOrder &sortOrder, sortOrders){
        switch(sortOrder.type()) {
            case (QLandmarkSortOrder::DefaultSort):
                continue;
            case (QLandmarkSortOrder::NameSort):
                continue;
            default:
                currentLevel = QLandmarkManager::None;
        }
    }
    return currentLevel;
}

DatabaseOperations::QueryRun::QueryRun(QLandmarkAbstractRequest *req, const QString &uri, QLandmarkManagerEngineSqlite *eng)
    : request(req),
      error(QLandmarkManager::NoError),
      errorString(QString()),
      errorMap(QMap<int,QLandmarkManager::Error>()),
      managerUri(uri),
      isCanceled(false),
      engine(eng),
      gpxHandler(0)
{
};

DatabaseOperations::QueryRun::~QueryRun()
{
    if (gpxHandler)
        delete gpxHandler;
    gpxHandler = 0;
}

void DatabaseOperations::QueryRun::run()
{
    {
        connectionName = QUuid::createUuid().toString();//each connection needs a unique name
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        db.setDatabaseName(engine->m_dbFilename);

        if (!db.open()) {
            qWarning() << db.lastError().text();
        }

        error = QLandmarkManager::NoError;
        errorString ="";
        errorMap.clear();
        isCanceled = false;

        QMetaObject::invokeMethod(engine, "updateRequestState",
                                  Qt::QueuedConnection,
                                  Q_ARG(QLandmarkAbstractRequest *, request),
                                  Q_ARG(QLandmarkAbstractRequest::State, QLandmarkAbstractRequest::ActiveState));

        switch(request->type()){
        case QLandmarkAbstractRequest::LandmarkIdFetchRequest: {
                QLandmarkIdFetchRequest *idFetchRequest = static_cast<QLandmarkIdFetchRequest *>(request);
                QList<QLandmarkId> lmIds = DatabaseOperations::landmarkIds(connectionName, idFetchRequest->filter(),
                                                   idFetchRequest->sorting(), idFetchRequest->limit(), idFetchRequest->offset(),
                                                   &error, &errorString, managerUri, this);

                QMetaObject::invokeMethod(engine, "updateLandmarkIdFetchRequest",
                                          Q_ARG(QLandmarkIdFetchRequest *, idFetchRequest),
                                          Q_ARG(QList<QLandmarkId>,lmIds),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
            }
        case QLandmarkAbstractRequest::LandmarkFetchRequest: {
                QLandmarkFetchRequest *fetchRequest = static_cast<QLandmarkFetchRequest *>(request);
                QList<QLandmark> lms = DatabaseOperations::landmarks(connectionName, fetchRequest->filter(),
                                                fetchRequest->sorting(), fetchRequest->limit(), fetchRequest->offset(),
                                                &error, &errorString, managerUri, this);


                QMetaObject::invokeMethod(engine, "updateLandmarkFetchRequest",
                                          Q_ARG(QLandmarkFetchRequest *,fetchRequest),
                                          Q_ARG(QList<QLandmark>,lms),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));
                break;
            }
        case QLandmarkAbstractRequest::LandmarkFetchByIdRequest :
            {

                QLandmarkFetchByIdRequest *byIdRequest = static_cast<QLandmarkFetchByIdRequest *> (request);
                QList<QLandmarkId> lmIds= byIdRequest->landmarkIds();
                QList<QLandmark> lms = DatabaseOperations::landmarks(connectionName, lmIds, &errorMap,
                                                                &error, &errorString, managerUri, this);

                QMetaObject::invokeMethod(engine, "updateLandmarkFetchByIdRequest",
                                          Q_ARG(QLandmarkFetchByIdRequest *,byIdRequest),
                                          Q_ARG(QList<QLandmark>,lms),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(ERROR_MAP, errorMap),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
            }
        case QLandmarkAbstractRequest::LandmarkSaveRequest :
        {
            QLandmarkSaveRequest *saveRequest = static_cast<QLandmarkSaveRequest *> (request);
            QList<QLandmark> lms = saveRequest->landmarks();
            DatabaseOperations::saveLandmarks(connectionName, &lms, &errorMap, &error, &errorString, managerUri, this);

            QMetaObject::invokeMethod(engine, "updateLandmarkSaveRequest",
                                      Q_ARG(QLandmarkSaveRequest *,saveRequest),
                                      Q_ARG(QList<QLandmark>,lms),
                                      Q_ARG(QLandmarkManager::Error, error),
                                      Q_ARG(QString, errorString),
                                      Q_ARG(ERROR_MAP, errorMap),
                                      Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
        }
        case QLandmarkAbstractRequest::LandmarkRemoveRequest :
        {
            QLandmarkRemoveRequest *removeRequest = static_cast<QLandmarkRemoveRequest *> (request);
            QList<QLandmarkId> lmIds = removeRequest->landmarkIds();
            DatabaseOperations::removeLandmarks(connectionName, lmIds,
                                                &errorMap, &error, &errorString,
                                                managerUri, this);

            QMetaObject::invokeMethod(engine, "updateLandmarkRemoveRequest",
                                      Q_ARG(QLandmarkRemoveRequest *,removeRequest),
                                      Q_ARG(QLandmarkManager::Error, error),
                                      Q_ARG(QString, errorString),
                                      Q_ARG(ERROR_MAP, errorMap),
                                      Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));
            break;
        }
        case QLandmarkAbstractRequest::CategoryIdFetchRequest :
            {
                QLandmarkCategoryIdFetchRequest *catIdFetchRequest = static_cast<QLandmarkCategoryIdFetchRequest *> (request);
                QLandmarkNameSort nameSort = catIdFetchRequest->sorting();
                QList<QLandmarkCategoryId> catIds = DatabaseOperations::categoryIds(connectionName, nameSort,
                                                                        catIdFetchRequest->limit(),
                                                                        catIdFetchRequest->offset(),
                                                                        &error, &errorString, managerUri,this);

                QMetaObject::invokeMethod(engine, "updateLandmarkCategoryIdFetchRequest",
                                          Q_ARG(QLandmarkCategoryIdFetchRequest *,catIdFetchRequest),
                                          Q_ARG(QList<QLandmarkCategoryId>,catIds),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
            }
        case QLandmarkAbstractRequest::CategoryFetchRequest :
            {
                QLandmarkCategoryFetchRequest *fetchRequest = static_cast<QLandmarkCategoryFetchRequest *> (request);
                QLandmarkNameSort nameSort = fetchRequest->sorting();
                bool needAll = true;
                QList<QLandmarkCategoryId> categoryIds;

                QList <QLandmarkCategory> cats = DatabaseOperations::categories(connectionName, categoryIds, nameSort,
                                                                fetchRequest->limit(), fetchRequest->offset(),
                                                                &error, &errorString, managerUri, needAll, this);

                QMetaObject::invokeMethod(engine, "updateLandmarkCategoryFetchRequest",
                                          Q_ARG(QLandmarkCategoryFetchRequest *,fetchRequest),
                                          Q_ARG(QList<QLandmarkCategory>,cats),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));
                break;
            }
        case QLandmarkAbstractRequest::CategoryFetchByIdRequest :
            {
                QLandmarkCategoryFetchByIdRequest *byIdRequest = static_cast<QLandmarkCategoryFetchByIdRequest *> (request);
                QList<QLandmarkCategoryId> categoryIds = byIdRequest->categoryIds();
                QList<QLandmarkCategory> categories =  DatabaseOperations::categories(connectionName, categoryIds,&errorMap,
                                                                                    &error, &errorString,managerUri, this);

                if (this->isCanceled) {
                    categories.clear();
                    error = QLandmarkManager::CancelError;
                    errorString = "Category save request was canceled";
                }

                QMetaObject::invokeMethod(engine, "updateLandmarkCategoryFetchByIdRequest",
                                          Q_ARG(QLandmarkCategoryFetchByIdRequest *,byIdRequest),
                                          Q_ARG(QList<QLandmarkCategory>, categories),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(ERROR_MAP, errorMap),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
            }
        case QLandmarkAbstractRequest::CategorySaveRequest :
        {
            QLandmarkCategorySaveRequest *saveRequest = static_cast<QLandmarkCategorySaveRequest *> (request);
            QList<QLandmarkCategory> categories = saveRequest->categories();
            DatabaseOperations::saveCategories(connectionName, &categories,
                                               &errorMap, &error,
                                               &errorString, managerUri);

            if (this->isCanceled) {
                categories.clear();
                error = QLandmarkManager::CancelError;
                errorString = "Category save request was canceled";
            }

            QMetaObject::invokeMethod(engine, "updateLandmarkCategorySaveRequest",
                                      Q_ARG(QLandmarkCategorySaveRequest *,saveRequest),
                                      Q_ARG(QList<QLandmarkCategory>,categories),
                                      Q_ARG(QLandmarkManager::Error, error),
                                      Q_ARG(QString, errorString),
                                      Q_ARG(ERROR_MAP, errorMap),
                                      Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

            break;
        }
        case QLandmarkAbstractRequest::CategoryRemoveRequest :
            {
                QLandmarkCategoryRemoveRequest *removeRequest = static_cast<QLandmarkCategoryRemoveRequest *> (request);
                QList<QLandmarkCategoryId> categoryIds = removeRequest->categoryIds();
               DatabaseOperations::removeCategories(connectionName, categoryIds,
                                                &errorMap, &error,
                                                &errorString, managerUri);

               if (this->isCanceled) {
                   categoryIds.clear();
                   error = QLandmarkManager::CancelError;
                   errorString = "Category remove request was canceled";
               }
               QMetaObject::invokeMethod(engine, "updateLandmarkCategoryRemoveRequest",
                                         Q_ARG(QLandmarkCategoryRemoveRequest *,removeRequest),
                                         Q_ARG(QLandmarkManager::Error, error),
                                         Q_ARG(QString, errorString),
                                         Q_ARG(ERROR_MAP, errorMap),
                                         Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

               break;
            }
        case QLandmarkAbstractRequest::ImportRequest :
            {
                QLandmarkImportRequest *importRequest = static_cast<QLandmarkImportRequest *> (request);

                DatabaseOperations::importLandmarks(connectionName, importRequest->device(),
                                                    importRequest->format(), importRequest->transferOption(),
                                                    importRequest->categoryId(),
                                                    &error, &errorString,
                                                    managerUri, this);
                if (this->gpxHandler) {
                    delete gpxHandler;
                    gpxHandler = 0;
                }

                if (this->isCanceled) {
                    error = QLandmarkManager::CancelError;
                    errorString = "Landmark import request was canceled";

                }

                QMetaObject::invokeMethod(engine, "updateLandmarkImportRequest",
                                          Q_ARG(QLandmarkImportRequest *, importRequest),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
            }
        case QLandmarkAbstractRequest::ExportRequest :
            {
                QLandmarkExportRequest *exportRequest = static_cast<QLandmarkExportRequest *> (request);

                DatabaseOperations::exportLandmarks(connectionName, exportRequest->device(),
                                                    exportRequest->format(), exportRequest->landmarkIds(),
                                                    exportRequest->transferOption(),
                                                    &error, &errorString, managerUri);

                if (this->isCanceled) {
                    error = QLandmarkManager::CancelError;
                    errorString = "Landmark export request was canceled";
                }

                QMetaObject::invokeMethod(engine, "updateLandmarkExportRequest",
                                          Q_ARG(QLandmarkExportRequest *, exportRequest),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));

                break;
            }
        default:
            break;
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(connectionName);
}
