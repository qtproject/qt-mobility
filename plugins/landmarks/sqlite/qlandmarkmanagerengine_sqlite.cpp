/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qlandmarkmanagerengine_sqlite_p.h"
#include <qlandmark.h>
#include <qlandmarkid.h>
#include <qlandmarkcategory.h>
#include <qlandmarkcategoryid.h>
#include <qgeoplace.h>
#include <qgeoaddress.h>
#include <qgeocoordinate.h>

#include <qlandmarknamefilter.h>
#include <qlandmarkproximityfilter.h>
#include <qlandmarkcategoryfilter.h>
#include <qlandmarkboxfilter.h>
#include <qlandmarkintersectionfilter.h>
#include <qlandmarkunionfilter.h>
#include <qlandmarkattributefilter.h>

#include <qlandmarksortorder.h>
#include <qlandmarknamesort.h>
#include <qlandmarkdistancesort.h>
#include <qlandmarkidfilter.h>

#include <qlandmarkabstractrequest.h>
#include <qlandmarkidfetchrequest.h>
#include <qlandmarkfetchrequest.h>
#include <qlandmarksaverequest.h>
#include <qlandmarkremoverequest.h>
#include <qlandmarkcategoryidfetchrequest.h>
#include <qlandmarkcategoryfetchrequest.h>
#include <qlandmarkcategorysaverequest.h>
#include <qlandmarkcategoryremoverequest.h>
#include <qlandmarkimportrequest.h>
#include <qlandmarkexportrequest.h>

#include "qlandmarkfilehandler_gpx_p.h"
#include "qlandmarkfilehandler_lmx_p.h"

#include <math.h>
#include <qnumeric.h>
#include <QMetaMethod>

#include <QDebug>

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QRectF>
#include <QUrl>
#include <QSet>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QThreadPool>
#include <QUuid>
#include <math.h>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDateTime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QLandmarkCategoryId>);
Q_DECLARE_METATYPE(QList<QLandmarkId>);
Q_DECLARE_METATYPE(QList<QLandmark>);
Q_DECLARE_METATYPE(QList<QLandmarkCategory>);
Q_DECLARE_METATYPE(QLandmarkAbstractRequest::State)
Q_DECLARE_METATYPE(QLandmarkAbstractRequest *)
Q_DECLARE_METATYPE(QLandmarkIdFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkManager::Error)
Q_DECLARE_METATYPE(QLandmarkSaveRequest *)
Q_DECLARE_METATYPE(QLandmarkRemoveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategorySaveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryRemoveRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryIdFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkCategoryFetchRequest *)
Q_DECLARE_METATYPE(QLandmarkImportRequest *)
Q_DECLARE_METATYPE(QLandmarkExportRequest *)
Q_DECLARE_METATYPE(ERROR_MAP)

static const double EARTH_MEAN_RADIUS = 6371.0072;

class QueryRun : public QRunnable
{
public:
    QueryRun(QLandmarkAbstractRequest *req =0, const QString &uri=QString(), QLandmarkManagerEngineSqlite *eng =0);
    ~QueryRun();
    void run();

    QLandmarkAbstractRequest *request;
    QString connectionName;
    QLandmarkManager::Error error;
    QString errorString;
    QMap<int, QLandmarkManager::Error> errorMap;
    QString managerUri;
    volatile bool isCanceled;
    QLandmarkManagerEngineSqlite *engine;
    QLandmarkFileHandlerGpx *gpxHandler;
};

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

QLandmark retrieveLandmark(const QString &connectionName, const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun=0)
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

    QLandmark lm;

    QStringList columns;
    columns << "name";
    columns << "description";
    columns << "icon_url";
    columns << "latitude";
    columns << "longitude";
    columns << "altitude";
    columns << "radius";
    columns << "top_left_lat";
    columns << "top_left_lon";
    columns << "bottom_right_lat";
    columns << "bottom_right_lon";
    columns << "country";
    columns << "country_code";
    columns << "state";
    columns << "county";
    columns << "district";
    columns << "city";
    columns << "street";
    columns << "street_number";
    columns << "postcode";
    columns << "post_office_box";
    columns << "phone";
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

        if (!query1.value(20).isNull())
            address.setPostOfficeBox(query1.value(20).toString());

        lm.setAddress(address);

        if (!query1.value(21).isNull())
            lm.setPhone(query1.value(21).toString());

        if (!query1.value(22).isNull())
            lm.setUrl(query1.value(22).toString());

        lm.setLandmarkId(landmarkId);
    }

    if (found) {
        QString q2 = QString("SELECT category_id FROM landmark_category WHERE landmark_id = %1;").arg(landmarkId.localId());
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
        if (!query.prepare("SELECT key, value from landmark_attribute WHERE landmark_id=:lmId")) {
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
            lm.setAttribute(query.value(0).toString(),query.value(1));
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
    if (filter.caseSensitivity() == Qt::CaseSensitive)
        return QString("SELECT id FROM landmark WHERE name = \"%1\" ").arg(filter.name());
    else
        return QString("SELECT id FROM landmark WHERE name LIKE \"%1\" ").arg(filter.name());
}

QString landmarkIdsCategoryQueryString(const QLandmarkCategoryFilter &filter)
{
    return QString("SELECT landmark_id FROM landmark_category WHERE category_id=%1 ").arg(filter.categoryId().localId());
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

QList<QLandmarkId> landmarkIds(const QString &connectionName, const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        const QLandmarkFetchHint &fetchHint,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun * queryRun =0)
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
            bindValues.insert("key", attributeKeys.at(0));
            if (!executeQuery(&query,"SELECT landmark_id FROM landmark_attribute WHERE landmark_attribute.key = :key",
                            bindValues,error,errorString)) {
                return QList<QLandmarkId>();
            }

            QStringList lmLocalIds;
            while(query.next()) {
                lmLocalIds << query.value(0).toString();
            }

            QLandmarkId id;
            id.setManagerUri(managerUri);
            for (int i=0; i < lmLocalIds.count(); ++i) {
                bindValues.clear();
                bindValues.insert("lmId", lmLocalIds.at(i));
                if (!executeQuery(&query, "SELECT key, value FROM landmark_attribute WHERE landmark_id=:lmId",
                        bindValues, error, errorString)) {
                    return QList<QLandmarkId>();
                }

                QMap<QString,QVariant> lmAttributes;
                while(query.next()) {
                    lmAttributes.insert(query.value(0).toString(), query.value(1));
                }

                bool isMatch = true;
                foreach(const QString &filterAttributeKey, attributeKeys) {
                    if (!lmAttributes.contains(filterAttributeKey)) {
                        isMatch = false;
                        break;
                    }

                    if (!attributeFilter.attribute(filterAttributeKey).isValid()) {
                        continue;
                    } else if (attributeFilter.attribute(filterAttributeKey) != lmAttributes.value(filterAttributeKey)) {
                        isMatch = false;
                        break;
                    }
                }

                if (isMatch) {
                    id.setLocalId(lmLocalIds.at(i));
                    result << id;
                }
            }
        }
        idsFound = true;
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
                                QList<QLandmarkSortOrder>(), fetchHint, error, errorString, managerUri,queryRun);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else  {
                QSet<QString> ids;
                QList<QLandmarkId> firstResult = landmarkIds(connectionName,filters.at(0),
                                                QList<QLandmarkSortOrder>(), fetchHint, error, errorString,
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
                                                QList<QLandmarkSortOrder>(), fetchHint, error, errorString,
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
                                        QList<QLandmarkSortOrder>(), fetchHint, error, errorString,
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
                                                               fetchHint,
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

    int offset = fetchHint.offset();
    if (offset < 0 )
        offset = 0;
    return result.mid(offset, fetchHint.maxItems());
}

QList<QLandmark> landmarks(const QString &connectionName, const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        const QLandmarkFetchHint &fetchHint,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun =0)
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

    QList<QLandmarkId> ids = ::landmarkIds(connectionName, filter, sortOrders, fetchHint, error, errorString, managerUri, queryRun);
    if (error && *error != QLandmarkManager::NoError) {
        return result;
    }

    bool needAll = false;
    if (filter.type() == QLandmarkFilter::LandmarkIdFilter) {
        QLandmarkIdFilter idFilter = filter;
        if (idFilter.matchingScheme() == QLandmarkIdFilter::MatchAll)
            needAll = true;
        else {
            needAll = false;
        }
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

QString quoteString(const QString &s)
{
    QString q = "\"";
    q += s;
    q += "\"";
    return q;
}

//Performs the save sql queries but does not handle
//starting or rolling back/comitting transactions.
bool saveLandmarkHelper(const QString &connectionName, QLandmark *landmark,
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
        bindValues.insert("icon_url",landmark->iconUrl().toString());
    else
        bindValues.insert("icon_url", QVariant());

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
        bindValues.insert("country_code", address.countryCode());
    else
        bindValues.insert("country_code", QVariant());

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
        bindValues.insert("street_number", address.streetNumber());
    else
        bindValues.insert("street_number", QVariant());

    if (!address.postCode().isEmpty())
        bindValues.insert("postcode", address.postCode());
    else
        bindValues.insert("postcode", QVariant());

    if (!address.postOfficeBox().isEmpty())
        bindValues.insert("post_office_box", address.postOfficeBox());
    else
        bindValues.insert("post_office_box", QVariant());

    if (!landmark->phone().isEmpty())
        bindValues.insert("phone", landmark->phone());
    else
        bindValues.insert("phone", QVariant());

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

    queries << QString("DELETE FROM landmark_category WHERE landmark_id = %1 AND category_id NOT IN lc_refresh;").arg(id.localId());
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

    QStringList attributekeys = landmark->attributeKeys();
    QSqlQuery query(db);
    if( !query.prepare("DELETE FROM landmark_attribute WHERE landmark_id= :lmId"))
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
        if (!query.prepare("INSERT INTO landmark_attribute (landmark_id,key,value) VALUES(:lmId,:key,:value)")) {
            *error = QLandmarkManager::UnknownError;
            *errorString = QString("Unable to prepare statement: %1 \nReason: %2")
                           .arg(query.lastQuery()).arg(query.lastError().text());
            return false;
        }

        query.bindValue(":lmId", landmark->landmarkId().localId());
        query.bindValue(":key", attributekeys[i]);
        query.bindValue(":value", landmark->attribute(attributekeys.at(i)));

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

bool saveLandmark(const QString &connectionName, QLandmark* landmark,
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

bool saveLandmarks(const QString &connectionName, QList<QLandmark> * landmark,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString, const QString &managerUri, QueryRun *queryRun =0)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Save landmarks: unable to begin transaction, reason: %1").arg(db.lastError().text());

        for (int i=0; i < landmark->size(); ++i)
            errorMap->insert(i, *error);
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
    queryStrings << "DELETE FROM landmark_category WHERE landmark_id = :lmId;";
    queryStrings << "DELETE FROM landmark_attribute WHERE landmark_id=:lmId";

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

bool removeLandmark(const QString &connectionName, const QLandmarkId &landmarkId,
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

bool removeLandmarks(const QString &connectionName, const QList<QLandmarkId> &landmarkIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString, const QString &managerUri,
                    QueryRun *queryRun = 0)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    if (!db.transaction()) {
        *error = QLandmarkManager::UnknownError;
        *errorString = QString("Remove landmars: unable to begin transaction, reason: %1").arg(db.lastError().text());

        for (int i=0; i < landmarkIds.size(); ++i)
            errorMap->insert(i, *error);
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

QList<QLandmarkCategoryId> categoryIds(const QString &connectionName,
                                       const QLandmarkNameSort &nameSort,
                                       QLandmarkManager::Error *error, QString *errorString,
                                       const QString &managerUri,
                                       QueryRun *queryRun = 0)
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

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    return result;
}

QLandmarkCategory category(const QString &connectionName, const QLandmarkCategoryId &landmarkCategoryId,
              QLandmarkManager::Error *error,
              QString *errorString, const QString &managerUri)
{
    if (landmarkCategoryId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Category id comes from different landmark manager.";
        return QLandmarkCategory();
    }

    QLandmarkCategory cat;

    QStringList columns;
    columns << "name";
    columns << "description";
    columns << "icon_url";

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    bool transacting = db.transaction();
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
            if (transacting)
                db.rollback();
            return QLandmarkCategory();
        } else {
            found = true;
        }

        if (!query.value(0).isNull())
            cat.setName(query.value(0).toString());

        if (!query.value(1).isNull())
            cat.setDescription(query.value(1).toString());

        if (!query.value(2).isNull())
            cat.setIconUrl(query.value(2).toString());

        cat.setCategoryId(landmarkCategoryId);
    }

    if (!found) {
        if (transacting)
            db.rollback();

        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "None of the existing categories match the given category id.";
    } else {
        QMap<QString,QVariant> bindValues;
        bindValues.insert("catId", cat.categoryId().localId());
        if (!executeQuery(&query, "SELECT key, value from category_attribute WHERE category_id=:catId",bindValues, error, errorString )) {
            if (transacting)
                db.rollback();
            return QLandmarkCategory();
         }

        while(query.next()) {
            cat.setAttribute(query.value(0).toString(),query.value(1));
        }

        if (transacting)
            db.commit();

        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }
    return cat;
}

bool categoryNameCompare(const QLandmarkCategory &cat1, const QLandmarkCategory &cat2) {
    return (cat1.name() < cat2.name());
}

QList<QLandmarkCategory> categories(const QString &connectionName,
                const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                const QLandmarkNameSort &nameSort,
                QLandmarkManager::Error *error, QString *errorString,
                const QString &managerUri, bool needAll,
                QueryRun *queryRun = 0)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    *error = QLandmarkManager::NoError;
    errorString->clear();

    QList<QLandmarkCategory> result;
    QList<QLandmarkCategoryId> ids = landmarkCategoryIds;
    if (ids.size() == 0) {
        ids = ::categoryIds(connectionName, nameSort, error, errorString, managerUri, queryRun);
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

bool saveCategory(const QString &connectionName, QLandmarkCategory *category,
                QLandmarkManager::Error *error,
                QString *errorString, const QString &managerUri)
{

    if (!category->categoryId().managerUri().isEmpty() && category->categoryId().managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "Category id comes from different landmark manager.";
        return false;
    }

    bool update = category->categoryId().isValid();

    QSqlDatabase db = QSqlDatabase::database(connectionName);

    bool transacting = db.transaction();

    QMap<QString, QVariant> bindValues;
    QSqlQuery query(db);
    if (update) {
        bindValues.insert("id",category->categoryId().localId());

        QString q0 = QString("SELECT 1 FROM category WHERE id = :id;");
        if (!executeQuery(&query,q0, bindValues,error,errorString)) {
            if (transacting)
                db.rollback();
             return false;
        }

        if (!query.next()) {
            if (transacting)
                db.rollback();

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

    if (!category->description().isEmpty())
        bindValues.insert("description", category->description());
    else
        bindValues.insert("description", QVariant());

    if (!category->iconUrl().isEmpty())
        bindValues.insert("icon_url", category->iconUrl().toString());
    else
        bindValues.insert("icon_url", QVariant());

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
        if (transacting)
            db.rollback();
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
    QStringList attributekeys = category->attributeKeys();
    if (!executeQuery(&query,"DELETE FROM category_attribute WHERE category_id= :catId", bindValues, error, errorString)) {
        if (transacting)
            db.rollback();
        return false;
    }

    for (int i =0; i < attributekeys.count(); ++i) {
        bindValues.clear();
        bindValues.insert("catId",category->categoryId().localId());
        bindValues.insert("key",attributekeys[i]);
        bindValues.insert("value",category->attribute(attributekeys.at(i)));

        if (!executeQuery(&query,"INSERT INTO category_attribute (category_id,key,value) VALUES(:catId,:key,:value)", bindValues,
                         error, errorString)) {
            if (transacting) {
                db.rollback();
            }
            return false;
        }
    }

    if (transacting)
        db.commit();

    //TODO: notifications
    /*if (!update) {
        QList<QLandmarkCategoryId> ids;
        ids << category->categoryId();
        emit categoriesAdded(ids);
    } else {
        QList<QLandmarkCategoryId> ids;
        ids << category->categoryId();
        emit categoriesChanged(ids);
    }*/

    /*
    // grab keys from attributes tables for current id
    // delete those we no longer have
    // use replace for the rest

    // loop through attributes

    */

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    return true;
}

bool saveCategories(const QString &connectionName, QList<QLandmarkCategory> * categories,
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

bool removeCategory(const QString &connectionName, const QLandmarkCategoryId &categoryId,
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
    queryStrings << "DELETE FROM landmark_category WHERE category_id = :catId";
    queryStrings << "DELETE FROM category_attribute WHERE category_id= :catId";

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

bool removeCategories(const QString &connectionName, const QList<QLandmarkCategoryId> &categoryIds,
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

bool importLandmarksLmx(const QString &connectionName,
                        QIODevice *device,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri)
{
    QLandmarkFileHandlerLmx lmxHandler;
    bool result = lmxHandler.importData(device);

    if (result) {
        saveLandmarks(connectionName, &lmxHandler.landmarks(), 0, error, errorString, managerUri);

        if (error && *error != QLandmarkManager::NoError) {
            result = false;
        } else {
            if (errorString)
                *errorString = "";
        }
    } else {
        if (errorString)
            *errorString = lmxHandler.errorString();
        // TODO set error code
    }

    return result;
}

bool importLandmarksGpx(const QString &connectionName,
                        QIODevice *device,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri,
                        QueryRun *queryRun =0)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QLandmarkFileHandlerGpx *gpxHandler = new QLandmarkFileHandlerGpx;
    if (queryRun) {
        queryRun->gpxHandler = gpxHandler;
        queryRun->gpxHandler->setAsync(true);
    }

    QLandmarkFileHandlerGpx::State state = gpxHandler->importData(device);
    bool result = false;
    if (state == QLandmarkFileHandlerGpx::DoneState) {
            saveLandmarks(connectionName, &(gpxHandler->waypoints()), 0, error, errorString, managerUri);

        if (*error != QLandmarkManager::NoError) {
            result = false;
        } else  {
            if (errorString)
                *errorString = "";
            result = true;
        }
    } else if (state == QLandmarkFileHandlerGpx::ErrorState){
        if (errorString)
            *errorString = gpxHandler->errorString();
        if (error)
            *error = QLandmarkManager::ParsingError;
            result = false;
    } else if (state == QLandmarkFileHandlerGpx::CanceledState) {
        *error = QLandmarkManager::CancelError;
        *errorString = "Import request was canceled";
        result = false;
    }
    if (!queryRun)
        delete gpxHandler;
   //the query run will delete it's own gpx handler

    return result;
}

bool importLandmarks(const QString &connectionName,
                     QIODevice *device,
                     const QString &format,
                     QLandmarkManager::Error *error,
                     QString *errorString, const QString &managerUri,
                     QueryRun *queryRun = 0)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QFile *file = qobject_cast<QFile *>(device);
    if (file)
    {
        if (!file->exists()) {
            *error = QLandmarkManager::DoesNotExistError;
            *errorString = QString("Import operation failed, file does not exist: %1").arg(file->fileName());
            return false;
        }
    }

    if (!device->open(QIODevice::ReadOnly)) {
        *error = QLandmarkManager::PermissionsError;
        *errorString = "Unable to open io device for importing landmarks";
        return false;
    }

    if (format ==  "LmxV1.0") {
            return importLandmarksLmx(connectionName, device, error, errorString, managerUri);
    } else if (format == "GpxV1.1") {
        return importLandmarksGpx(connectionName, device, error, errorString, managerUri, queryRun);
    } else {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString = "The given format is not supported at this time";
        return false;
    }
}

bool exportLandmarksGpx(const QString &connectionName,
                        QIODevice *device,
                        QList<QLandmarkId> landmarkIds,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri)
{
    QLandmarkFileHandlerGpx gpxHandler;

    QList<QLandmarkSortOrder> sortOrders;
    QLandmarkFetchHint fetchHint;
    QLandmarkFilter filter;
    if (landmarkIds.count() > 0)
        filter = QLandmarkIdFilter (landmarkIds, QLandmarkIdFilter::MatchSubset);

    QList<QLandmark> lms = ::landmarks(connectionName,filter, sortOrders, fetchHint, error, errorString, managerUri);

    if (error && *error != QLandmarkManager::NoError)
        return false;

    gpxHandler.setWaypoints(lms);

    bool result = gpxHandler.exportData(device);

    if (!result) {
        if (errorString)
            *errorString = gpxHandler.errorString();
        // TODO set error code
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
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        const QString &managerUri)
{
    QLandmarkFileHandlerLmx lmxHandler;

    QLandmarkIdFilter idFilter(landmarkIds, QLandmarkIdFilter::MatchAll);
    QList<QLandmarkSortOrder> sortOrders;
    QLandmarkFetchHint fetchHint;
    QList<QLandmark> lms = ::landmarks(connectionName, idFilter, sortOrders, fetchHint, error, errorString, managerUri);

    if (error && *error != QLandmarkManager::NoError)
        return false;

    lmxHandler.setLandmarks(lms);

    bool result = lmxHandler.exportData(device);

    if (!result) {
        if (errorString)
            *errorString = lmxHandler.errorString();
        // TODO set error code
    } else {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }

    return result;
}

bool exportLandmarks(const QString &connectionName,
                     QIODevice *device,
                     const QString &format,
                     QList<QLandmarkId> landmarkIds,
                     QLandmarkManager::Error *error,
                     QString *errorString,
                     const QString &managerUri)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    QFile *file = qobject_cast<QFile *>(device);
    if (!device->open(QIODevice::WriteOnly)) {
        *error = QLandmarkManager::PermissionsError;
        *errorString = "Unable to open io device for importing landmarks";
        return false;
    }

    bool result;
    if (format ==  "LmxV1.0") {
        result = exportLandmarksLmx(connectionName, device, landmarkIds, error, errorString, managerUri);
        device->close();
        return result;
    } else if (format == "GpxV1.1") {
        result = exportLandmarksGpx(connectionName, device, landmarkIds, error, errorString, managerUri);
        device->close();
        return result;
    } else {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString = "The given format is not supported at this time";
        return false;
    }
}

QueryRun::QueryRun(QLandmarkAbstractRequest *req, const QString &uri, QLandmarkManagerEngineSqlite *eng)
    : request(req),
      error(QLandmarkManager::NoError),
      errorString(QString()),
      errorMap(QMap<int,QLandmarkManager::Error>()),
      managerUri(uri),
      isCanceled(false),
      engine(eng)
{
};

QueryRun::~QueryRun()
{
    if (gpxHandler)
        delete gpxHandler;
    gpxHandler = 0;
}

void QueryRun::run()
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
                QList<QLandmarkId> lmIds = ::landmarkIds(connectionName, idFetchRequest->filter(), idFetchRequest->sorting(), idFetchRequest->fetchHint(),
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
                QList<QLandmark> lms = ::landmarks(connectionName, fetchRequest->filter(), fetchRequest->sorting(), fetchRequest->fetchHint(),
                                                &error, &errorString, managerUri, this);


                QMetaObject::invokeMethod(engine, "updateLandmarkFetchRequest",
                                          Q_ARG(QLandmarkFetchRequest *,fetchRequest),
                                          Q_ARG(QList<QLandmark>,lms),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));
                break;
            }
        case QLandmarkAbstractRequest::LandmarkSaveRequest :
        {
            QLandmarkSaveRequest *saveRequest = static_cast<QLandmarkSaveRequest *> (request);
            QList<QLandmark> lms = saveRequest->landmarks();
            saveLandmarks(connectionName, &lms, &errorMap, &error, &errorString, managerUri, this);

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
            ::removeLandmarks(connectionName, lmIds, &errorMap, &error, &errorString, managerUri, this);

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
                QList<QLandmarkCategoryId> catIds = ::categoryIds(connectionName, nameSort, &error, &errorString, managerUri,this);

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
                QList<QLandmarkCategoryId> categoryIds = fetchRequest->categoryIds();
                bool needAll = false;
                if (categoryIds.count() > 0 && fetchRequest->matchingScheme() == QLandmarkCategoryFetchRequest::MatchAll)
                    needAll = true;
                QLandmarkNameSort nameSort = fetchRequest->sorting();
                QList <QLandmarkCategory> cats = ::categories(connectionName, categoryIds, nameSort, &error, &errorString, managerUri, needAll, this);

                QMetaObject::invokeMethod(engine, "updateLandmarkCategoryFetchRequest",
                                          Q_ARG(QLandmarkCategoryFetchRequest *,fetchRequest),
                                          Q_ARG(QList<QLandmarkCategory>,cats),
                                          Q_ARG(QLandmarkManager::Error, error),
                                          Q_ARG(QString, errorString),
                                          Q_ARG(QLandmarkAbstractRequest::State,QLandmarkAbstractRequest::FinishedState));
                break;
            }
        case QLandmarkAbstractRequest::CategorySaveRequest :
        {
            QLandmarkCategorySaveRequest *saveRequest = static_cast<QLandmarkCategorySaveRequest *> (request);
            QList<QLandmarkCategory> categories = saveRequest->categories();
            ::saveCategories(connectionName, &categories, &errorMap, &error, &errorString, managerUri);

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
               ::removeCategories(connectionName, categoryIds, &errorMap, &error, &errorString, managerUri);

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

                ::importLandmarks(connectionName, importRequest->device(), importRequest->format(), &error, &errorString, managerUri, this);
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

                ::exportLandmarks(connectionName, exportRequest->device(), exportRequest->format(), exportRequest->landmarkIds(), &error, &errorString, managerUri);

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

QLandmarkManagerEngineSqlite::QLandmarkManagerEngineSqlite(const QString &filename)
        : m_dbFilename(filename),
        m_dbConnectionName("landmarks"),
        m_dbWatcher(NULL),
        m_latestTimestamp(0.0)
{
    qRegisterMetaType<ERROR_MAP >();
    qRegisterMetaType<QList<QLandmarkCategoryId> >();
    qRegisterMetaType<QList<QLandmarkId> >();
    qRegisterMetaType<QList<QLandmark> >();
    qRegisterMetaType<QList<QLandmarkCategory> >();
    qRegisterMetaType<QLandmarkAbstractRequest::State>();
    qRegisterMetaType<QLandmarkAbstractRequest *>();
    qRegisterMetaType<QLandmarkIdFetchRequest *>();
    qRegisterMetaType<QLandmarkFetchRequest *>();
    qRegisterMetaType<QLandmarkSaveRequest *>();
    qRegisterMetaType<QLandmarkRemoveRequest *>();
    qRegisterMetaType<QLandmarkCategoryIdFetchRequest *>();
    qRegisterMetaType<QLandmarkCategoryFetchRequest *>();
    qRegisterMetaType<QLandmarkCategorySaveRequest *>();
    qRegisterMetaType<QLandmarkCategoryRemoveRequest *>();
    qRegisterMetaType<QLandmarkImportRequest *>();
    qRegisterMetaType<QLandmarkExportRequest *>();
    qRegisterMetaType<QLandmarkManager::Error>();

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_dbConnectionName);

    if (m_dbFilename.isEmpty()) {
        QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                           QLatin1String("Nokia"), QLatin1String("QtLandmarks"));
        QFileInfo fi(settings.fileName());
        QDir dir = fi.dir();
        dir.mkpath(dir.path());
        m_dbFilename = dir.path() + QDir::separator() + QString("QtLandmarks") +  QLatin1String(".db");

    }

    db.setDatabaseName(m_dbFilename);
    if (!db.open()) {
        qWarning() << db.lastError().text();
    }

    if (filename == ":memory:")
        return;

    // check for fk support
    QSqlQuery checkForeignKeys("PRAGMA foreign_keys;", db);
    bool result = false;
    while (checkForeignKeys.next()) {
        QString r = checkForeignKeys.value(0).toString();
        if ((r == "0") || (r == "1"))
            result = true;
    }

    QFile file;
    if (result)
        file.setFileName(":/qlandmarkmanagerengine_sqlite_fk.ddl");
    else
        file.setFileName(":/qlandmarkmanagerengine_sqlite_no_fk.ddl");

    // read in ddl to set up tables
    // all tables and indices use IF NOT EXISTS to avoid duplication
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString s = in.readAll();
        QStringList queries = s.split("@@@");

        bool transacting = db.transaction();
        for (int i = 0; i < queries.size(); ++i) {
            QString q = queries.at(i).trimmed();
            if (q == "")
                continue;

            q += ";";
            QSqlQuery query(db);
            if (!query.exec(q)) {
                qWarning() << QString("Statement %1: %2").arg(i + 1).arg(query.lastError().databaseText());
            }
        }
        if (transacting)
            db.commit();
    }
    file.close();
}

QLandmarkManagerEngineSqlite::~QLandmarkManagerEngineSqlite()
{
    QThreadPool *threadPool = QThreadPool::globalInstance();
    threadPool->waitForDone();

    if (m_dbWatcher !=0)
        delete m_dbWatcher;

    QSqlDatabase::database(m_dbConnectionName).close();
    QSqlDatabase::removeDatabase(m_dbConnectionName);
}

/* URI reporting */
QString QLandmarkManagerEngineSqlite::managerName() const
{
    return "com.nokia.qt.landmarks.engines.sqlite";
}

QMap<QString, QString> QLandmarkManagerEngineSqlite::managerParameters() const
{
    QMap<QString, QString> parameters;

    parameters["filename"] = m_dbFilename;

    return parameters;
}

int QLandmarkManagerEngineSqlite::managerVersion() const
{
    return 1;
}

QString queryStringForRadius(const QGeoCoordinate &coord, double radius)
{
    return "SELECT id, latitude, longitude FROM landmark;";

    /*
    // TODO have to handle the case where lat wraps
    // - (lon match AND lat match) OR (lat overlap match AND (lon + 180.0) match)

    QString q = "SELECT id, latitude, longitude FROM landmark";

    double earthMeanRadius = 6371.0072 * 1000.0;
    double pi = 3.14159265358979323846;
    double degreeLimit = (180.0 * radius) / (2 * pi * earthMeanRadius);
    // add a safety margin
    degreeLimit *= 1.5;

    bool whereAdded = false;

    bool minLatWraps = false;
    bool maxLatWraps = false;

    QString lat1Query;
    QString lon1Query;
    QString lat2Query;
    QString lon2Query;

    if (degreeLimit < 90.0) {
        double minLat = coord.latitude() - degreeLimit;
        double maxLat = coord.latitude() + degreeLimit;

        if (QString::number(minLat, 'g', 4) == QString::number(coord.latitude(), 'g', 4))
            minLat -= 0.1;

        if (QString::number(maxLat, 'g', 4) == QString::number(coord.latitude(), 'g', 4))
            maxLat += 0.1;

        double wrappedMinLat = 0.0;
        double wrappedMaxLat = 0.0;

        if (minLat < -90.0) {
            minLatWraps = true;
            wrappedMinLat = -1.0 * (180.0 + minLat);
            minLat = 90.0;
        }

        if (maxLat > 90.0) {
            maxLatWraps = true;
            wrappedMaxLat = 180.0 - maxLat;
            maxLat = 90.0;
        }

        lat1Query = QString("(%1 <= latitude AND latitude <= %2)").arg(minLat, 0, 'g', 4).arg(maxLat, 0, 'g', 4);
        if (minLatWraps)
            lat2Query = QString("(latitude <= %1)").arg(minLat, 0, 'g', 4);
        else if (maxLatWraps)
            lat2Query = QString("(%1 <= latitude)").arg(wrappedMaxLat, 0, 'g', 4);
    }

    if (degreeLimit < 180.0) {
        double minLon = coord.longitude() - degreeLimit;
        double maxLon = coord.longitude() + degreeLimit;

        if (QString::number(minLon, 'g', 4) == QString::number(coord.longitude(), 'g', 4))
            minLon -= 0.1;

        if (QString::number(maxLon, 'g', 4) == QString::number(coord.longitude(), 'g', 4))
            maxLon += 0.1;

        bool minLonWraps = (minLon < -180.0);
        bool maxLonWraps = (maxLon > 180.0);

        if (minLonWraps)
            minLon += 360.0;

        if (maxLonWraps)
            maxLon -= 360.0;

        if (minLonWraps || maxLonWraps) {
            lon1Query = QString("(%1 <= longitude OR longitude <= %2)").arg(minLon, 0, 'g', 4).arg(maxLon, 0, 'g', 4);
        } else {
            lon1Query  += QString("(%1 <= longitude AND longitude <= %2)").arg(minLon, 0, 'g', 4).arg(maxLon, 0, 'g', 4);
        }
    }

    if (minLatWraps || maxLatWraps) {
        double minLon = coord.longitude() - degreeLimit + 180.0;
        if (minLon > 180.0)
            minLon -= 360.0;

        double maxLon = coord.longitude() + degreeLimit + 180.0;
        if (maxLon > 180.0)
            maxLon -= 360.0;

        if (QString::number(minLon, 'g', 4) == QString::number(coord.longitude(), 'g', 4))
            minLon -= 0.1;

        if (QString::number(maxLon, 'g', 4) == QString::number(coord.longitude(), 'g', 4))
            maxLon += 0.1;

        if (minLon > maxLon) {
            lon2Query = QString("(%1 <= longitude OR longitude <= %2)").arg(minLon, 0, 'g', 4).arg(maxLon, 0, 'g', 4);
        } else {
            lon2Query  += QString("(%1 <= longitude AND longitude <= %2)").arg(minLon, 0, 'g', 4).arg(maxLon, 0, 'g', 4);
        }

    }

    if (lat1Query.isEmpty() && lon1Query.isEmpty()) {
        q += ";";
        return q;
    }

    if (!lat1Query.isEmpty() || !lon1Query.isEmpty()) {
        q += " WHERE ";

        if (!lat1Query.isEmpty() && !lon1Query.isEmpty()) {
            q += "(";
            q += lat1Query;
            q += " AND ";
            q += lon1Query;
            q += ")";
        } else {
            if (!lat1Query.isEmpty())
                q += lat1Query;
            else if (!lon1Query.isEmpty())
                q += lon1Query;
        }
    }

    if (!lat2Query.isEmpty() || !lon2Query.isEmpty()) {
        q += " OR ";

        if (!lat2Query.isEmpty() && !lon2Query.isEmpty()) {
            q += "(";
            q += lat2Query;
            q += " AND ";
            q += lon2Query;
            q += ")";
        } else {
            if (!lat2Query.isEmpty())
                q += lat2Query;
            else if (!lon1Query.isEmpty())
                q += lon2Query;
        }
    }

    qWarning() << degreeLimit << coord.latitude() << coord.longitude() << q;

    q += ";";

    return q;
    */
}

class SortFunctor
{
public:
    SortFunctor(const QList<QLandmarkSortOrder>& sortOrders);

    bool operator()(const QLandmark &lm1, const QLandmark &lm2);

private:
    int compare(const QLandmark &lm1, const QLandmark &lm2) const;
    int compareName(const QLandmarkNameSort &sort, const QLandmark &lm1, const QLandmark &lm2) const;
    int compareDistance(const QLandmarkDistanceSort &sort, const QLandmark &lm1, const QLandmark &lm2) const;

    QList<QLandmarkSortOrder> m_sortOrders;
};

SortFunctor::SortFunctor(const QList<QLandmarkSortOrder> &sortOrders)
        : m_sortOrders(sortOrders) {}

bool SortFunctor::operator()(const QLandmark &lm1, const QLandmark &lm2)
{
    return (compare(lm1, lm2) == -1);
}

int SortFunctor::compare(const QLandmark &lm1, const QLandmark &lm2) const
{
    int result = 0;
    for (int i = 0; i < m_sortOrders.size(); ++i) {
        switch (m_sortOrders.at(i).type()) {
            case QLandmarkSortOrder::DefaultSort:
                continue;
            case QLandmarkSortOrder::NameSort:
            {
                QLandmarkNameSort nameSort = m_sortOrders.at(i);
                result = compareName(nameSort, lm1, lm2);
                break;
            }
            case QLandmarkSortOrder::DistanceSort:
            {
                QLandmarkDistanceSort distanceSort = m_sortOrders.at(i);
                result = compareDistance(distanceSort, lm1, lm2);
                break;
            }
            default:
                continue;
        }

        if (result != 0)
            return result;
    }
    return result;
}

int SortFunctor::compareName(const QLandmarkNameSort &sort, const QLandmark &lm1, const QLandmark &lm2) const
{
    int result = QString::compare(lm1.name(), lm2.name(), sort.caseSensitivity());

    if (sort.direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

int SortFunctor::compareDistance(const QLandmarkDistanceSort &sort, const QLandmark &lm1, const QLandmark &lm2) const
{
    int result = 0;

    if (lm1.coordinate().isValid()) {
        if (lm2.coordinate().isValid()) {
            double d1 = sort.coordinate().distanceTo(lm1.coordinate());
            double d2 = sort.coordinate().distanceTo(lm2.coordinate());

            if (d1 == d2) {
                result = 0;
            } else if (d1 < d2) {
                result = -1;
            } else if (d1 > d2) {
                result = 1;
            }
        } else {
            result = -1;
        }
    } else {
        if (lm2.coordinate().isValid()) {
            result = 1;
        } else {
            result = 0;
        }
    }

    if (sort.direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIds(const QLandmarkFilter& filter,
        const QList<QLandmarkSortOrder>& sortOrders,
        const QLandmarkFetchHint &fetchHint,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return ::landmarkIds(m_dbConnectionName,filter,sortOrders,fetchHint, error,errorString, managerUri() );
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineSqlite::categoryIds(const QLandmarkNameSort &nameSort, QLandmarkManager::Error *error,
        QString *errorString) const
{
    return ::categoryIds(m_dbConnectionName, nameSort, error, errorString, managerUri());
}

/* Retrieval */
QLandmark QLandmarkManagerEngineSqlite::landmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    return retrieveLandmark(m_dbConnectionName, landmarkId, error, errorString, managerUri());
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QLandmarkFilter &filter,
                                                         const QList<QLandmarkSortOrder>& sortOrders,
                                                         const QLandmarkFetchHint &fetchHint,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{

    return ::landmarks(m_dbConnectionName, filter, sortOrders, fetchHint, error, errorString, managerUri());
}

QLandmarkCategory QLandmarkManagerEngineSqlite::category(const QLandmarkCategoryId &landmarkCategoryId,
                                                         QLandmarkManager::Error *error,
                                                         QString *errorString) const
{
    return ::category(m_dbConnectionName, landmarkCategoryId, error, errorString, managerUri());
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    QLandmarkNameSort nameSort;// this should be ignored in the following call
    return ::categories(m_dbConnectionName, landmarkCategoryIds, nameSort, error, errorString, managerUri(), true);
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories( const QLandmarkNameSort &nameSort,
                                                                  QLandmarkManager::Error *error,
                                                                  QString *errorString) const
{
    QList<QLandmarkCategoryId> catIds;
    return ::categories(m_dbConnectionName, catIds, nameSort, error, errorString, managerUri(), false);
}

/*saving and removing*/
bool QLandmarkManagerEngineSqlite::saveLandmarkInternal(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString,
        bool *added,
        bool *changed)
{
    QString uri = managerUri();

    if (!landmark->landmarkId().managerUri().isEmpty() && landmark->landmarkId().managerUri() != uri) {
        if (error)
            *error = QLandmarkManager::DoesNotExistError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        return false;
    }

    bool update = landmark->landmarkId().isValid();

    if (added)
        *added = false;
    if (changed)
        *changed = false;

    QStringList columns;
    QStringList values;

    QSqlDatabase db = QSqlDatabase::database(m_dbConnectionName);

    bool transacting = db.transaction();

    if (update) {
        columns << "id";
        values << landmark->landmarkId().localId();

        QString q0 = QString("SELECT 1 FROM landmark WHERE id = %1;").arg(landmark->landmarkId().localId());
        QSqlQuery query0(q0, db);
        if (!query0.next()) {
            if (transacting)
                db.rollback();

            if (error)
                *error = QLandmarkManager::DoesNotExistError;
            if (errorString)
                *errorString = "Landmark id does not exist in this landmark manager.";

            return false;
        }
    }
    columns << "name";
    if (!landmark->name().isEmpty())
        values << quoteString(landmark->name());
    else
        values << "null";

    columns << "description";
    if (!landmark->description().isEmpty())
        values << quoteString(landmark->description());
    else
        values << "null";

    columns << "icon_url";
    if (!landmark->iconUrl().isEmpty())
        values << quoteString(landmark->iconUrl().toString());
    else
        values << "null";

    QGeoCoordinate coord = landmark->coordinate();

    columns << "latitude";
    if (!qIsNaN(coord.latitude()))
        values << QString::number(coord.latitude());
    else
        values << "null";

    columns << "longitude";
    if (!qIsNaN(coord.longitude()))
        values << QString::number(coord.longitude());
    else
        values << "null";

    columns << "altitude";
    if (!qIsNaN(coord.altitude()))
        values << QString::number(coord.altitude());
    else
        values << "null";

    columns << "radius";
    if (landmark->radius() != -1.0)
        values << QString::number(landmark->radius());
    else
        values << "null";

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

    columns << "country";
    if (!address.country().isEmpty())
        values << quoteString(address.country());
    else
        values << "null";

    columns << "country_code";
    if (!address.countryCode().isEmpty())
        values << quoteString(address.countryCode());
    else
        values << "null";

    columns << "state";
    if (!address.state().isEmpty())
        values << quoteString(address.state());
    else
        values << "null";

    columns << "county";
    if (!address.county().isEmpty())
        values << quoteString(address.county());
    else
        values << "null";

    columns << "district";
    if (!address.district().isEmpty())
        values << quoteString(address.district());
    else
        values << "null";

    columns << "city";
    if (!address.city().isEmpty())
        values << quoteString(address.city());
    else
        values << "null";

    columns << "street";
    if (!address.street().isEmpty())
        values << quoteString(address.street());
    else
        values << "null";

    columns << "street_number";
    if (!address.streetNumber().isEmpty())
        values << quoteString(address.streetNumber());
    else
        values << "null";

    columns << "postcode";
    if (!address.postCode().isEmpty())
        values << quoteString(address.postCode());
    else
        values << "null";

    columns << "post_office_box";
    if (!address.postOfficeBox().isEmpty())
        values << quoteString(address.postOfficeBox());
    else
        values << "null";

    columns << "phone";
    if (!landmark->phone().isEmpty())
        values << quoteString(landmark->phone());
    else
        values << "null";

    columns << "url";
    if (!landmark->url().isEmpty())
        values << quoteString(landmark->url().toString());
    else
        values << "null";

    QString q1 = QString("REPLACE INTO landmark (%1) VALUES (%2);").arg(columns.join(",")).arg(values.join(","));
    QSqlQuery query1(db);
    if (!query1.exec(q1)) {
        // TODO set error
        //qWarning() << query1.lastError().databaseText();
        if (transacting)
            db.rollback();
        return false;
    }

    if (!update) {
        QLandmarkId id;
        id.setManagerUri(managerUri());
        id.setLocalId(query1.lastInsertId().toString());
        landmark->setLandmarkId(id);
    }

    QStringList lmCats;

    for (int i = landmark->categoryIds().size() - 1; i >= 0; --i) {
        QLandmarkCategoryId id = landmark->categoryIds().at(i);
        if (id.managerUri() == uri)
            lmCats << id.localId();
        else
            landmark->removeCategoryId(id);
    }

    QStringList queries;
    queries << "CREATE TEMP TABLE IF NOT EXISTS lc_refresh (i INTEGER);";

    for (int i = 0; i < lmCats.size(); ++i) {
        queries << QString("INSERT INTO lc_refresh (i) VALUES (%1);").arg(lmCats.at(i));
    }

    queries << QString("DELETE FROM landmark_category WHERE landmark_id = %1 AND category_id NOT IN lc_refresh;").arg(landmark->landmarkId().localId());
    queries << QString("REPLACE INTO landmark_category SELECT %1, i FROM lc_refresh;").arg(landmark->landmarkId().localId());

    queries << "DROP TABLE IF EXISTS lc_refresh;";

    for (int i = 0; i < queries.size(); ++i) {
        QSqlQuery query(db);
        if (!query.exec(queries.at(i))) {
            qWarning() << query.lastError().databaseText();
            if (transacting)
                db.rollback();
            return false;
        }
    }

    if (transacting)
        db.commit();

    if (!update) {
        if (added)
            *added = true;
    } else {
        if (changed)
            *changed = true;
    }
    /*
    // grab keys from attributes tables for current id
    // delete those we no longer have
    // use replace for the rest

    // loop through attributes
    */

    return true;
}

bool QLandmarkManagerEngineSqlite::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{

    return ::saveLandmark(m_dbConnectionName, landmark, error, errorString, managerUri());

}

bool QLandmarkManagerEngineSqlite::saveLandmarks(QList<QLandmark> * landmarks,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return ::saveLandmarks(m_dbConnectionName, landmarks, errorMap, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  ::removeLandmark(m_dbConnectionName, landmarkId , error, errorString, managerUri());


    //TODO: notifications
}

bool QLandmarkManagerEngineSqlite::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return  ::removeLandmarks(m_dbConnectionName, landmarkIds , errorMap, error, errorString, managerUri());
    //TODO: notifications
}

bool QLandmarkManagerEngineSqlite::saveCategory(QLandmarkCategory* category,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return ::saveCategory(m_dbConnectionName, category, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::removeCategory(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return ::removeCategory(m_dbConnectionName, categoryId, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::importLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString)
{
    return ::importLandmarks(m_dbConnectionName, device, format, error, errorString, managerUri());
}

bool QLandmarkManagerEngineSqlite::exportLandmarks(QIODevice *device,
                                                   const QString &format,
                                                   QList<QLandmarkId> landmarkIds,
                                                   QLandmarkManager::Error *error,
                                                   QString *errorString) const
{
    return ::exportLandmarks(m_dbConnectionName, device, format, landmarkIds, error, errorString, managerUri());
}

QStringList QLandmarkManagerEngineSqlite::supportedFormats(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString ="";

    QStringList formats;
    formats << "GpxV1.1";
    return formats;
}

QLandmarkManager::FilterSupportLevel QLandmarkManagerEngineSqlite::filterSupportLevel(const QLandmarkFilter &filter, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return QLandmarkManager::Native;
}

bool QLandmarkManagerEngineSqlite::isFeatureSupported(QLandmarkManager::LandmarkFeature feature, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    switch(feature) {
        case (QLandmarkManager::GenericLandmarkAttributes):
        case (QLandmarkManager::GenericCategoryAttributes):
            return false;
            break;
        default:
            return false;
    }
}

bool QLandmarkManagerEngineSqlite::isReadOnly(QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";
    return false;
}

bool QLandmarkManagerEngineSqlite::isReadOnly(const QLandmarkId &landmarkId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return false;
}

bool QLandmarkManagerEngineSqlite::isReadOnly(const QLandmarkCategoryId &categoryId, QLandmarkManager::Error *error, QString *errorString) const
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    *error = QLandmarkManager::NoError;
    *errorString = "";

    return false;
}

/* Asynchronous Request Support */
void QLandmarkManagerEngineSqlite::requestDestroyed(QLandmarkAbstractRequest* request)
{
    QueryRun *queryRun;
    queryRun = m_requestRunHash.value(request);
    m_requestRunHash.remove(request);
    delete queryRun;
    queryRun = 0;
}

bool QLandmarkManagerEngineSqlite::startRequest(QLandmarkAbstractRequest* request)
{
    QueryRun *queryRun;
    if (!m_requestRunHash.contains(request)) {
        queryRun= new QueryRun(request, managerUri(), this);
        queryRun->setAutoDelete(false);
        m_requestRunHash.insert(request, queryRun);
    } else {
        queryRun = m_requestRunHash.value(request);
    }

    QThreadPool::globalInstance()->start(queryRun);
    return true;
}

bool QLandmarkManagerEngineSqlite::cancelRequest(QLandmarkAbstractRequest* request)
{
    m_requestRunHash.value(request)->isCanceled = true;

    if (request->type() == QLandmarkAbstractRequest::ImportRequest) {
        if (m_requestRunHash.value(request)->gpxHandler) {
            m_requestRunHash.value(request)->gpxHandler->cancel();
            QMetaObject::invokeMethod(m_requestRunHash.value(request)->gpxHandler,
                                "cancel", Qt::QueuedConnection);
        }
    }
    return true;
}

bool QLandmarkManagerEngineSqlite::waitForRequestFinished(QLandmarkAbstractRequest* request,
        int msecs)
{
    return false;
}

void QLandmarkManagerEngineSqlite::databaseChanged()
{
    QSqlDatabase db = QSqlDatabase::database(m_dbConnectionName);

    qreal latestLandmarkTimestamp = m_latestTimestamp;
    QSqlQuery query(db);
    if (!query.prepare("SELECT landmark_id,action, timestamp FROM landmark_notification WHERE timestamp > ?")) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not prepare statement: " << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    query.addBindValue(latestLandmarkTimestamp);
    if (!query.exec()) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not execute statement:" << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }

    QList<QLandmarkId> addedLandmarkIds;
    QList<QLandmarkId> changedLandmarkIds;
    QList<QLandmarkId> removedLandmarkIds;

    QString action;
    QLandmarkId landmarkId;
    landmarkId.setManagerUri(managerUri());
    bool ok;
    qreal timestamp;

    while(query.next()) {
        timestamp = query.value(2).toDouble(&ok);
        if (!ok) //this should never happen
            continue;

        if (timestamp > latestLandmarkTimestamp)
            latestLandmarkTimestamp = timestamp;

        action = query.value(1).toString();
        landmarkId.setLocalId((query.value(0).toString()));

        if (action == "ADD") {
            addedLandmarkIds << landmarkId;
        } else if (action == "CHANGE") {
            changedLandmarkIds << landmarkId;
        } else if (action == "REMOVE") {
            removedLandmarkIds << landmarkId;
        }
    }

    if (addedLandmarkIds.count() > 0)
        emit landmarksAdded(addedLandmarkIds);

    if (changedLandmarkIds.count() > 0)
        emit landmarksChanged(changedLandmarkIds);

    if (removedLandmarkIds.count() > 0)
        emit landmarksRemoved(removedLandmarkIds);

    //now check for added/modified/removed categories
    qreal latestCategoryTimestamp = m_latestTimestamp;
    if (!query.prepare("SELECT category_id,action, timestamp FROM category_notification WHERE timestamp > ?")) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not prepare statement: " << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    query.addBindValue(latestCategoryTimestamp);
    if (!query.exec()) {
#ifdef QT_LANDMARK_SQLITE_ENGINE_DEBUG
        qWarning() << "Could not execute statement:" << query.lastQuery() << " \nReason:" << query.lastError().text();
#endif
        return;
    }
    QList<QLandmarkCategoryId> addedCategoryIds;
    QList<QLandmarkCategoryId> changedCategoryIds;
    QList<QLandmarkCategoryId> removedCategoryIds;

    QLandmarkCategoryId categoryId;
    categoryId.setManagerUri(managerUri());

    while(query.next()) {
        timestamp = query.value(2).toDouble(&ok);
        if (!ok) //this should never happen
            continue;

        if (timestamp > latestCategoryTimestamp)
            latestCategoryTimestamp = timestamp;

        action = query.value(1).toString();
        categoryId.setLocalId(query.value(0).toString());
        if (action == "ADD") {
            addedCategoryIds << categoryId;
        } else if (action == "CHANGE") {
            changedCategoryIds << categoryId;
        } else if (action == "REMOVE") {
            removedCategoryIds << categoryId;
        }
    }

    if (addedCategoryIds.count() > 0) {
        emit categoriesAdded(addedCategoryIds);
     }

    if (changedCategoryIds.count() > 0)
        emit categoriesChanged(changedCategoryIds);

    if (removedCategoryIds.count() > 0) {
        emit categoriesRemoved(removedCategoryIds);
       }

    if (latestLandmarkTimestamp > m_latestTimestamp)
        m_latestTimestamp = latestLandmarkTimestamp;
    if (latestCategoryTimestamp > m_latestTimestamp)
        m_latestTimestamp = latestCategoryTimestamp;
}

void QLandmarkManagerEngineSqlite::setChangeNotificationsEnabled(bool enabled)
{
    if (!m_dbWatcher) {
        m_dbWatcher = new DatabaseFileWatcher(m_dbFilename);
        connect(m_dbWatcher, SIGNAL(notifyChange()),this,SLOT(databaseChanged()));
    }
    m_dbWatcher->setEnabled(enabled);
    if (enabled)
        m_latestTimestamp = QDateTime::currentDateTime().toTime_t();
}

void QLandmarkManagerEngineSqlite::connectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(landmarksAdded(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksChanged(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksRemoved(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)))
        {
            setChangeNotificationsEnabled(true);
        }
}
void QLandmarkManagerEngineSqlite::disconnectNotify(const char *signal)
{
    if (QLatin1String(signal) == SIGNAL(landmarksAdded(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksChanged(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(landmarksRemoved(QList<QLandmarkId>))
        || QLatin1String(signal) == SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))
        || QLatin1String(signal) == SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>))) {
        if (receivers(SIGNAL(landmarksAdded(QList<QLandmarkId>))) == 0
            && receivers(SIGNAL(landmarksChanged(QList<QLandmarkId>))) == 0
            && receivers(SIGNAL(landmarksRemoved(QList<QLandmarkId>))) == 0
            && receivers(SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>))) == 0
            && receivers(SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>))) == 0
            && receivers(SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>))) == 0
            )
            setChangeNotificationsEnabled(false);
    }
}

void QLandmarkManagerEngineSqlite::updateLandmarkIdFetchRequest(QLandmarkIdFetchRequest* req, const QList<QLandmarkId>& result,
                                  QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkIdFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkFetchRequest(QLandmarkFetchRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkFetchRequest(req, result, error, errorString,newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkSaveRequest(QLandmarkSaveRequest* req, const QList<QLandmark>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkSaveRequest(req, result, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkRemoveRequest(QLandmarkRemoveRequest* req, QLandmarkManager::Error error,
                             const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkRemoveRequest(req, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateRequestState(QLandmarkAbstractRequest *req, QLandmarkAbstractRequest::State state)
{
    QLandmarkManagerEngine::updateRequestState(req,state);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryIdFetchRequest(QLandmarkCategoryIdFetchRequest* req, const QList<QLandmarkCategoryId>& result,
        QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryIdFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryFetchRequest(QLandmarkCategoryFetchRequest* req, const QList<QLandmarkCategory>& result,
        QLandmarkManager::Error error, const QString &errorString, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryFetchRequest(req, result, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategorySaveRequest(QLandmarkCategorySaveRequest* req, const QList<QLandmarkCategory>& result,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategorySaveRequest(req, result, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkCategoryRemoveRequest(QLandmarkCategoryRemoveRequest* req,
                            QLandmarkManager::Error error, const QString &errorString, const ERROR_MAP &errorMap, QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkCategoryRemoveRequest(req, error, errorString, errorMap, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkImportRequest(QLandmarkImportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                 QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkImportRequest(req, error, errorString, newState);
}

void QLandmarkManagerEngineSqlite::updateLandmarkExportRequest(QLandmarkExportRequest *req, QLandmarkManager::Error error, const QString &errorString,
                                 QLandmarkAbstractRequest::State newState)
{
    QLandmarkManagerEngine::updateLandmarkExportRequest(req, error, errorString, newState);
}
