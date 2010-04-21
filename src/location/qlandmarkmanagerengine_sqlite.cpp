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
#include "qlandmark.h"
#include "qlandmarkid.h"
#include "qlandmarkcategory.h"
#include "qlandmarkcategoryid.h"
#include "qgeolocation.h"
#include "qgeoaddress.h"
#include "qgeocoordinate.h"

#include "qlandmarknamefilter.h"
#include "qlandmarkproximityfilter.h"
#include "qlandmarknearestfilter.h"
#include "qlandmarkcategoryfilter.h"
#include "qlandmarkboxfilter.h"
#include "qlandmarkintersectionfilter.h"
#include "qlandmarkunionfilter.h"
#include "qlandmarkcustomfilter.h"

#include "qlandmarksortorder.h"
#include "qlandmarknamesort.h"
#include "qlandmarkdistancesort.h"
#include "qlandmarkcustomsort.h"

#include <math.h>
#include <qnumeric.h>

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

QTM_BEGIN_NAMESPACE

QLandmarkManagerEngineSqlite::QLandmarkManagerEngineSqlite(const QString &filename)
        : m_dbFilename(filename)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE", "landmarks");
    m_db.setDatabaseName(filename);
    if (!m_db.open()) {
        qWarning() << m_db.lastError().text();
    }

    if (filename == ":memory:")
        return;

    // check for fk support
    QSqlQuery checkForeignKeys("PRAGMA foreign_keys;", m_db);
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
        QStringList queries = s.split(';');

        bool transacting = m_db.transaction();
        for (int i = 0; i < queries.size(); ++i) {
            QString q = queries.at(i).trimmed();
            if (q == "")
                continue;

            q += ";";
            QSqlQuery query(m_db);
            if (!query.exec(q)) {
                qWarning() << QString("Statement %1: %2").arg(i + 1).arg(query.lastError().databaseText());
            }
        }
        if (transacting)
            m_db.commit();
    }
    file.close();
}

QLandmarkManagerEngineSqlite::~QLandmarkManagerEngineSqlite()
{
    m_db.close();
    QSqlDatabase::removeDatabase("landmarks");
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

QString quoteString(const QString &s)
{
    QString q = "\"";
    q += s;
    q += "\"";
    return q;
}

/* Filtering */
QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsDefault(const QLandmarkFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    QSqlQuery query("SELECT id FROM landmark;", m_db);
    while (query.next()) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(QString::number(query.value(0).toInt()));
        result << id;
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsName(const QLandmarkNameFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    // TODO set errors

    QSqlQuery query(m_db);
    if (filter->caseSensitivity() == Qt::CaseSensitive) {
        if (!query.exec(QString("SELECT id FROM landmark WHERE name = \"%1\";").arg(filter->name()))) {
            return result;
        }
    } else if (filter->caseSensitivity() == Qt::CaseInsensitive) {
        if (!query.exec(QString("SELECT id FROM landmark WHERE name LIKE \"%1\";").arg(filter->name()))) {
            return result;
        }
    } else {
        return result;
    }

    while (query.next()) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(QString::number(query.value(0).toInt()));
        result << id;
    }

    return result;
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

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsProximity(const QLandmarkProximityFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    if (filter->radius() <= 0.0)
        return result;

    QGeoCoordinate coord = filter->coordinate();

    if (!coord.isValid())
        return result;

    double radius = filter->radius();

    QString q = queryStringForRadius(coord, radius);

    QSqlQuery query(q, m_db);
    while (query.next()) {
        bool ok = false;
        double lat = query.value(1).toDouble(&ok);
        double lon = query.value(2).toDouble(&ok);
        // TODO errors on false

        double dist = QGeoCoordinate(lat, lon).distanceTo(coord);

        if (dist <= radius) {
            QLandmarkId id;
            id.setManagerUri(uri);
            id.setId(QString::number(query.value(0).toInt()));
            result << id;
        }
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsNearest(const QLandmarkNearestFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    QGeoCoordinate coord = filter->coordinate();

    if (!coord.isValid())
        return result;

    int minId = -1;
    double minDist = 0.0;

    // TODO do distance -> degrees conversion, use to limit lats and longs searched
    double radius = filter->radius();

    QString q = "SELECT id, latitude, longitude FROM landmark;";
    if (radius != -1)
        q = queryStringForRadius(coord, radius);

    QSqlQuery query(q, m_db);
    while (query.next()) {
        bool ok = false;
        double lat = query.value(1).toDouble(&ok);
        double lon = query.value(2).toDouble(&ok);
        // TODO errors on false

        double dist = QGeoCoordinate(lat, lon).distanceTo(coord);

        if (radius == -1.0 || dist < radius) {
            if (minId == -1 || dist < minDist) {
                minId = query.value(0).toInt();
                minDist = dist;
            }
        }
    }

    if (minId != -1) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(QString::number(minId));
        result << id;
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsCategory(const QLandmarkCategoryFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    if (filter->categoryId().managerUri() != uri)
        return result;

    QSqlQuery query(QString("SELECT landmark_id FROM landmark_category WHERE category_id = %1;").arg(filter->categoryId().id()), m_db);
    while (query.next()) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(QString::number(query.value(0).toInt()));
        result << id;
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsBox(const QLandmarkBoxFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    if (!filter->topLeftCoordinate().isValid())
        return result;

    if (!filter->bottomRightCoordinate().isValid())
        return result;

    QString q = "SELECT id FROM landmark WHERE (";

    double tly = filter->topLeftCoordinate().latitude();
    double bry = filter->bottomRightCoordinate().latitude();
    double tlx = filter->topLeftCoordinate().longitude();
    double brx = filter->bottomRightCoordinate().longitude();

    bool latWrap = (tly < bry);
    bool lonWrap = (tlx > brx);

    if (latWrap)
        return result;

//    if (latWrap) {
//        q += QString::number(bry);
//        q += " <= latitude OR latitude <= ";
//        q += QString::number(tly);
//    } else {
    q += QString::number(bry);
    q += " <= latitude AND latitude <= ";
    q += QString::number(tly);
//    }

    q += ") AND (";

    if (lonWrap) {
        q += QString::number(tlx);
        q += " <= longitude OR longitude <= ";
        q += QString::number(brx);
    } else {
        q += QString::number(tlx);
        q += " <= longitude AND longitude <= ";
        q += QString::number(brx);
    }

    q += ");";

    QSqlQuery query(q, m_db);

    while (query.next()) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(QString::number(query.value(0).toInt()));
        result << id;
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsIntersection(const QLandmarkIntersectionFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    QSet<QString> ids;
    QList<const QLandmarkFilter*> filters = filter->filters();

    if (filters.size() == 0)
        return result;

    if (filters.size() == 1)
        return landmarkIds(filters.at(0), QList<const QLandmarkSortOrder*>(), error, errorString);

    QList<QLandmarkId> firstResult = landmarkIds(filters.at(0), QList<const QLandmarkSortOrder*>(), error, errorString);
    for (int j = 0; j < firstResult.size(); ++j) {
        if (firstResult.at(j).isValid() && firstResult.at(j).managerUri() == uri)
            ids.insert(firstResult.at(j).id());
    }

    for (int i = 0; i < filters.size(); ++i) {
        QList<QLandmarkId> subResult = landmarkIds(filters.at(i), QList<const QLandmarkSortOrder*>(), error, errorString);
        QSet<QString> subIds;
        for (int j = 0; j < subResult.size(); ++j) {
            if (subResult.at(j).isValid() && subResult.at(j).managerUri() == uri)
                subIds.insert(subResult.at(j).id());
        }
        ids &= subIds;
    }

    QList<QString> idList = ids.toList();
    for (int i = 0; i < idList.size(); ++i) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(idList.at(i));
        result << id;
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsUnion(const QLandmarkUnionFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    QSet<QString> ids;
    QList<const QLandmarkFilter*> filters = filter->filters();

    if (filters.size() == 0)
        return result;

    if (filters.size() == 1)
        return landmarkIds(filters.at(0), QList<const QLandmarkSortOrder*>(), error, errorString);

    for (int i = 0; i < filters.size(); ++i) {
        QList<QLandmarkId> subResult = landmarkIds(filters.at(i), QList<const QLandmarkSortOrder*>(), error, errorString);
        for (int j = 0; j < subResult.size(); ++j) {
            if (subResult.at(j).isValid() && subResult.at(j).managerUri() == uri)
                ids.insert(subResult.at(j).id());
        }
    }

    QList<QString> idList = ids.toList();
    for (int i = 0; i < idList.size(); ++i) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(idList.at(i));
        result << id;
    }

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIdsCustom(const QLandmarkCustomFilter* filter,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> result;
    QString uri = managerUri();

    QSqlQuery query("SELECT id FROM landmark;", m_db);
    while (query.next()) {
        QLandmarkId id;
        id.setManagerUri(uri);
        id.setId(QString::number(query.value(0).toInt()));

        QLandmarkManager::Error loopError;
        QLandmark lm = landmark(id, &loopError, 0);

        if ((loopError == QLandmarkManager::NoError) && filter->isMatch(lm))
            result << id;
    }

    return result;
}

class SortFunctor
{
public:
    SortFunctor(const QList<const QLandmarkSortOrder*>& sortOrders);

    bool operator()(const QLandmark &lm1, const QLandmark &lm2);

private:
    int compare(const QLandmark &lm1, const QLandmark &lm2) const;
    int compareName(const QLandmarkNameSort *sort, const QLandmark &lm1, const QLandmark &lm2) const;
    int compareDistance(const QLandmarkDistanceSort *sort, const QLandmark &lm1, const QLandmark &lm2) const;

    QList<const QLandmarkSortOrder*> m_sortOrders;
};

SortFunctor::SortFunctor(const QList<const QLandmarkSortOrder*> &sortOrders)
        : m_sortOrders(sortOrders) {}

bool SortFunctor::operator()(const QLandmark &lm1, const QLandmark &lm2)
{
    return (compare(lm1, lm2) == -1);
}

int SortFunctor::compare(const QLandmark &lm1, const QLandmark &lm2) const
{
    int result = 0;
    for (int i = 0; i < m_sortOrders.size(); ++i) {
        if (!m_sortOrders.at(i))
            continue;
        switch (m_sortOrders.at(i)->type()) {
            case QLandmarkSortOrder::DefaultSort:
                continue;
            case QLandmarkSortOrder::NameSort:
                result = compareName(static_cast<const QLandmarkNameSort*>(m_sortOrders.at(i)), lm1, lm2);
                break;
            case QLandmarkSortOrder::DistanceSort:
                result = compareDistance(static_cast<const QLandmarkDistanceSort*>(m_sortOrders.at(i)), lm1, lm2);
                break;
            case QLandmarkSortOrder::CustomSort:
                result = static_cast<const QLandmarkCustomSort*>(m_sortOrders.at(i))->compare(lm1, lm2);
                break;
            default:
                continue;
        }

        if (result != 0)
            return result;
    }
    return result;
}

int SortFunctor::compareName(const QLandmarkNameSort *sort, const QLandmark &lm1, const QLandmark &lm2) const
{
    int result = QString::compare(lm1.name(), lm2.name(), sort->caseSensitivity());

    if (sort->direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

int SortFunctor::compareDistance(const QLandmarkDistanceSort *sort, const QLandmark &lm1, const QLandmark &lm2) const
{
    int result = 0;

    if (lm1.coordinate().isValid()) {
        if (lm2.coordinate().isValid()) {
            double d1 = sort->coordinate().distanceTo(lm1.coordinate());
            double d2 = sort->coordinate().distanceTo(lm2.coordinate());

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

    if (sort->direction() == Qt::DescendingOrder)
        result *= -1;

    return result;
}

QList<QLandmarkId> QLandmarkManagerEngineSqlite::landmarkIds(const QLandmarkFilter* filter,
        const QList<const QLandmarkSortOrder*>& sortOrders,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkId> filterResult;

    if (!filter)
        return filterResult;

    switch (filter->type()) {
        case QLandmarkFilter::InvalidFilter:
            break;
        case QLandmarkFilter::DefaultFilter:
            filterResult = landmarkIdsDefault(filter, error, errorString);
            break;
        case QLandmarkFilter::NameFilter:
            filterResult = landmarkIdsName(static_cast<const QLandmarkNameFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::ProximityFilter:
            filterResult = landmarkIdsProximity(static_cast<const QLandmarkProximityFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::NearestFilter:
            filterResult = landmarkIdsNearest(static_cast<const QLandmarkNearestFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::CategoryFilter:
            filterResult = landmarkIdsCategory(static_cast<const QLandmarkCategoryFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::BoxFilter:
            filterResult = landmarkIdsBox(static_cast<const QLandmarkBoxFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::IntersectionFilter:
            filterResult = landmarkIdsIntersection(static_cast<const QLandmarkIntersectionFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::UnionFilter:
            filterResult = landmarkIdsUnion(static_cast<const QLandmarkUnionFilter*>(filter), error, errorString);
            break;
        case QLandmarkFilter::CustomFilter:
            filterResult = landmarkIdsCustom(static_cast<const QLandmarkCustomFilter*>(filter), error, errorString);
            break;
        default:
            break;
    }

    // don't sort if there's no sort order
    // sorting
    if ((sortOrders.length() == 0)
            || ((sortOrders.length() == 1)
                && (sortOrders.at(0)->type() == QLandmarkSortOrder::DefaultSort))) {

        // limit matches
        if (filter && filter->maximumMatches() != -1) {
            int matches = filter->maximumMatches();
            if (filterResult.length() < matches)
                return filterResult;

            QList<QLandmarkId> result;

            for (int i = 0; i < matches; ++i)
                result.append(filterResult.at(i));

            return result;
        }

        return filterResult;
    }

    QList<QLandmark> lms = landmarks(filterResult, 0, error, errorString);

    if (*error != QLandmarkManager::NoError) {
        return QList<QLandmarkId>();
    }

    /*
     TODO - examine types of the sort order
     if there are custom sort orders then we take the current code path
     otherwise we can use sql for the sorting

     create a temporary table with a column for the ids and a column
     for each sort order (ignore name sorts after the first one)

     ie CREATE TEMP TABLE tempsort (
        id INTEGER PRIMARY KEY,
        n1 TEXT,
        d1 REAL,
        d2 REAL
    );

    If the name sort is case insensitive insert the uppercased name of the
    landmark.

    Then we can do something like

    SELECT id FROM tempsort
    ORDER by n1 ASC, d1 DESC, d2 ASC
    LIMIT 50

    where the limit term is not used if maxMatches == -1
    */

    // sort
    SortFunctor f(sortOrders);
    qSort(lms.begin(), lms.end(), f);

    QList<QLandmarkId> result;
    int matches = lms.size();

    // limit matches
    if (filter && filter->maximumMatches() != -1) {
        if (filter->maximumMatches() < matches)
            matches = filter->maximumMatches();
    }

    for (int i = 0; i < matches; ++i)
        result << lms.at(i).id();

    return result;
}

QList<QLandmarkCategoryId> QLandmarkManagerEngineSqlite::categoryIds(QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmarkCategoryId> result;

    QString uri = managerUri();
    QSqlQuery query("SELECT id FROM category;", m_db);
    while (query.next()) {
        QLandmarkCategoryId id;
        id.setManagerUri(uri);
        id.setId(QString::number(query.value(0).toInt()));
        result << id;
    }

    return result;
}

/* Retrieval */
QLandmark QLandmarkManagerEngineSqlite::landmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QString uri = managerUri();
    if (landmarkId.managerUri() != uri) {
        // TODO set error
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
    columns << "thoroughfare_name";
    columns << "thoroughfare_number";
    columns << "postcode";
    columns << "post_office_box";
    columns << "phone";
    columns << "url";

    bool transacting = m_db.transaction();

    QString q1 = QString("SELECT %1 FROM landmark WHERE id = %2;").arg(columns.join(",")).arg(landmarkId.id());
    QSqlQuery query1(q1, m_db);
    bool found = false;
    while (query1.next()) {
        if (found) {
            // TODO set error - should never happen
            qWarning() << "Non-unique primary key in table \"landmark\"";
            if (transacting)
                m_db.rollback();
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

        QGeoLocation location;
        QGeoCoordinate coord;
        QGeoAddress address;

        bool ok = false;
        if (!query1.value(3).isNull()) {
            coord.setLatitude(query1.value(3).toDouble(&ok));
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    m_db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        if (!query1.value(4).isNull()) {
            coord.setLongitude(query1.value(4).toDouble(&ok));
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    m_db.rollback();
                return QLandmark();
            }
        }

        ok = false;
        if (!query1.value(5).isNull()) {
            coord.setAltitude(query1.value(5).toDouble(&ok));
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    m_db.rollback();
                return QLandmark();
            }
        }

        location.setCoordinate(coord);

        ok = false;
        if (!query1.value(6).isNull()) {
            lm.setRadius(query1.value(6).toDouble(&ok));
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    m_db.rollback();
                return QLandmark();
            }
        }

        QRectF rect;

        ok = false;
        double tly = 0.0;
        if (!query1.value(7).isNull()) {
            tly = query1.value(7).toDouble(&ok);
            if (!ok) {
                // TODO set error
                qWarning() << "Not a double";
                if (transacting)
                    m_db.rollback();
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
                    m_db.rollback();
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
                    m_db.rollback();
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
                    m_db.rollback();
                return QLandmark();
            }
        }

        rect.setTopLeft(QPointF(tlx, tly));
        rect.setBottomRight(QPointF(brx, bry));

        location.setBoundingBox(rect);

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
            address.setThoroughfareName(query1.value(17).toString());

        if (!query1.value(18).isNull())
            address.setThoroughfareNumber(query1.value(18).toString());

        if (!query1.value(19).isNull())
            address.setPostCode(query1.value(19).toString());

        if (!query1.value(20).isNull())
            address.setPostOfficeBox(query1.value(20).toString());

        location.setAddress(address);

        lm.setLocation(location);

        if (!query1.value(21).isNull())
            lm.setPhone(query1.value(21).toString());

        if (!query1.value(22).isNull())
            lm.setUrl(query1.value(22).toString());

        lm.setId(landmarkId);
    }

    if (found) {
        QString q2 = QString("SELECT category_id FROM landmark_category WHERE landmark_id = %1;").arg(landmarkId.id());
        QSqlQuery query2(q2, m_db);
        while (query2.next()) {
            QLandmarkCategoryId id;
            id.setManagerUri(uri);
            id.setId(query2.value(0).toString());
            lm.addCategory(id);
        }
    }

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    if (transacting)
        m_db.commit();

    return lm;

    // QString(SELECT key, value from landmark_attribute WHERE id = %1).arg(landmarkId.id())
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QLandmarkFilter *filter,
        const QList<const QLandmarkSortOrder*>& sortOrders,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmark> result;

    QList<QLandmarkId> ids = landmarkIds(filter, sortOrders, error, errorString);
    if (error && *error != QLandmarkManager::NoError) {
        return result;
    }

    return landmarks(ids, 0, error, errorString);
}

QList<QLandmark> QLandmarkManagerEngineSqlite::landmarks(const QList<QLandmarkId> &landmarkIds,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    QList<QLandmark> result;

    bool noErrors = true;
    for (int i = 0; i < landmarkIds.size(); ++i) {
        QLandmarkManager::Error loopError;
        QLandmark lm = landmark(landmarkIds.at(i), &loopError, 0);
        if (lm.id().isValid())
            result << lm;
        if (loopError != QLandmarkManager::NoError) {
            if (errorMap)
                errorMap->insert(i, loopError);
            noErrors = false;
        }
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = "Errors occured while retrieving landmarks.";
    }

    return result;
}

QLandmarkCategory QLandmarkManagerEngineSqlite::category(const QLandmarkCategoryId &landmarkCategoryId,
        QLandmarkManager::Error *error,
        QString *errorString) const
{
    if (landmarkCategoryId.managerUri() != managerUri()) {
        // TODO set error
        return QLandmarkCategory();
    }

    QLandmarkCategory cat;

    QStringList columns;
    columns << "name";
    columns << "description";
    columns << "icon_url";

    bool transacting = m_db.transaction();
    QString q1 = QString("SELECT %1 FROM category WHERE id = %2;").arg(columns.join(",")).arg(landmarkCategoryId.id());
    QSqlQuery query(q1, m_db);
    bool found = false;
    while (query.next()) {
        if (found) {
            // TODO set error - should never happen
            qWarning() << "Non-unique primary key in table \"category\"";
            if (transacting)
                m_db.rollback();
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

        cat.setId(landmarkCategoryId);
    }

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";

    if (transacting)
        m_db.commit();

    return cat;

    // QString(SELECT key, value from category_attribute WHERE id = %1).arg(landmarkCategoryId.id())
}

QList<QLandmarkCategory> QLandmarkManagerEngineSqlite::categories(const QList<QLandmarkCategoryId> &landmarkCategoryIds,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkCategory> result;

    bool noErrors = true;
    for (int i = 0; i < landmarkCategoryIds.size(); ++i) {
        QLandmarkManager::Error loopError;
        QLandmarkCategory cat = category(landmarkCategoryIds.at(i), &loopError, 0);
        if (cat.id().isValid())
            result << cat;
        if (loopError != QLandmarkManager::NoError)
            noErrors = false;
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString =  "Errors occurred while retrieving categories.";
    }

    return result;
}

/*saving and removing*/
bool QLandmarkManagerEngineSqlite::saveLandmarkInternal(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString,
        bool *added,
        bool *changed)
{
    QString uri = managerUri();
    bool update = (landmark->id().isValid() && landmark->id().managerUri() == uri);

    if (added)
        *added = false;
    if (changed)
        *changed = false;

    QStringList columns;
    QStringList values;

    if (update) {
        columns << "id";
        values << landmark->id().id();
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

    QGeoCoordinate coord = landmark->location().coordinate();

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

    QRectF rect = landmark->location().boundingBox();

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

    QGeoAddress address = landmark->location().address();

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

    columns << "thoroughfare_name";
    if (!address.thoroughfareName().isEmpty())
        values << quoteString(address.thoroughfareName());
    else
        values << "null";

    columns << "thoroughfare_number";
    if (!address.thoroughfareNumber().isEmpty())
        values << quoteString(address.thoroughfareNumber());
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
        values << quoteString(landmark->url());
    else
        values << "null";

    bool transacting = m_db.transaction();
    QString q1 = QString("REPLACE INTO landmark (%1) VALUES (%2);").arg(columns.join(",")).arg(values.join(","));
    QSqlQuery query(m_db);
    if (!query.exec(q1)) {
        // TODO set error
        qWarning() << query.lastError().databaseText();
        if (transacting)
            m_db.rollback();
        return false;
    }

    if (!update) {
        QLandmarkId id;
        id.setManagerUri(managerUri());
        id.setId(query.lastInsertId().toString());
        landmark->setId(id);
    }

    QStringList lmCats;

    for (int i = landmark->categories().size() - 1; i >= 0; --i) {
        QLandmarkCategoryId id = landmark->categories().at(i);
        if (id.managerUri() == uri)
            lmCats << id.id();
        else
            landmark->removeCategory(id);
    }

    QStringList queries;
    queries << "CREATE TEMP TABLE IF NOT EXISTS lc_refresh (i INTEGER);";

    for (int i = 0; i < lmCats.size(); ++i) {
        queries << QString("INSERT INTO lc_refresh (i) VALUES (%1);").arg(lmCats.at(i));
    }

    queries << QString("DELETE FROM landmark_category WHERE landmark_id = %1 AND category_id NOT IN lc_refresh;").arg(landmark->id().id());
    queries << QString("REPLACE INTO landmark_category SELECT %1, i FROM lc_refresh;").arg(landmark->id().id());

    queries << "DROP TABLE IF EXISTS lc_refresh;";

    for (int i = 0; i < queries.size(); ++i) {
        QSqlQuery query(m_db);
        if (!query.exec(queries.at(i))) {
            qWarning() << query.lastError().databaseText();
            if (transacting)
                m_db.rollback();
            return false;
        }
    }

    if (transacting)
        m_db.commit();

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
    bool added = false;
    bool changed = false;
    bool result = saveLandmarkInternal(landmark, error, errorString, &added, &changed);

    if (added) {
        QList<QLandmarkId> ids;
        ids << landmark->id();
        emit landmarksAdded(ids);
    }

    if (changed) {
        QList<QLandmarkId> ids;
        ids << landmark->id();
        emit landmarksChanged(ids);
    }

    return result;
}

bool QLandmarkManagerEngineSqlite::saveLandmarks(QList<QLandmark> * landmark,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkId> addedIds;
    QList<QLandmarkId> changedIds;

    bool noErrors = true;
    for (int i = 0; i < landmark->size(); ++i) {
        QLandmarkManager::Error loopError;
        bool added = false;
        bool changed = false;
        bool result = saveLandmarkInternal(&(landmark->operator [](i)), &loopError, 0, &added, &changed);
        if (!result && errorMap) {
            noErrors = false;
            errorMap->insert(i, loopError);
        }
        if (added)
            addedIds << landmark->at(i).id();
        if (changed)
            changedIds << landmark->at(i).id();
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = "Errors occured while saving landmarks.";
    }

    if (addedIds.size() != 0)
        emit landmarksAdded(addedIds);

    if (changedIds.size() != 0)
        emit landmarksChanged(changedIds);

    return noErrors;
}

bool QLandmarkManagerEngineSqlite::removeLandmarkInternal(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString,
        bool *removed)
{
    if (removed)
        *removed = false;

    if (landmarkId.managerUri() != managerUri()) {
        // TODO set error
        return false;
    }

    bool transacting = m_db.transaction();

    QString q0 = QString("SELECT 1 FROM landmark WHERE id = %1;").arg(landmarkId.id());
    QSqlQuery query0(q0, m_db);
    // invalid id - not an error, but we need to detect this to avoid sending the signals
    if (!query0.next()) {
        if (transacting)
            m_db.commit();
        return true;
    }

    QString q1 = QString("DELETE FROM landmark WHERE id = %1;").arg(landmarkId.id());
    QSqlQuery query1(q1, m_db);
    if (!query1.exec()) {
        if (transacting)
            m_db.rollback();
        return false;
    }

    QString q2 = QString("DELETE FROM landmark_category WHERE landmark_id = %1;").arg(landmarkId.id());
    QSqlQuery query2(q2, m_db);
    if (!query2.exec()) {
        if (transacting)
            m_db.rollback();
        return false;
    }

    if (transacting)
        m_db.commit();

    if (removed)
        *removed = true;

    return true;
}

bool QLandmarkManagerEngineSqlite::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    bool removed = false;
    bool result = removeLandmarkInternal(landmarkId, error, errorString, &removed);

    if (removed) {
        QList<QLandmarkId> ids;
        ids << landmarkId;
        emit landmarksRemoved(ids);
    }

    return result;
}

bool QLandmarkManagerEngineSqlite::removeLandmarks(const QList<QLandmarkId> &landmarkId,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkId> removedIds;

    bool noErrors = true;
    for (int i = 0; i < landmarkId.size(); ++i) {
        QLandmarkManager::Error loopError;
        bool removed = false;
        bool result = removeLandmarkInternal(landmarkId.at(i), &loopError, 0, &removed);
        if (!result && errorMap) {
            noErrors = false;
            errorMap->insert(i, loopError);
        }

        if (removed)
            removedIds << landmarkId.at(i);
    }

    if (noErrors) {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    } else {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = "Errors occured while removing landmarks.";
    }

    if (removedIds.size() != 0)
        emit landmarksRemoved(removedIds);

    return noErrors;
}

bool QLandmarkManagerEngineSqlite::saveCategory(QLandmarkCategory* category,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    bool update = (category->id().isValid() && category->id().managerUri() == managerUri());

    QStringList columns;
    QStringList values;

    if (update) {
        columns << "id";
        values << category->id().id();
    }

    columns << "name";
    if (!category->name().isEmpty())
        values << quoteString(category->name());
    else
        values << "null";

    columns << "description";
    if (!category->description().isEmpty())
        values << quoteString(category->description());
    else
        values << "null";

    columns << "icon_url";
    if (!category->iconUrl().isEmpty())
        values << quoteString(category->iconUrl().toString());
    else
        values << "null";

    bool transacting = m_db.transaction();
    QString q1 = QString("REPLACE INTO category (%1) VALUES (%2);").arg(columns.join(",")).arg(values.join(","));
    QSqlQuery query(m_db);
    if (!query.exec(q1)) {
        // TODO set error
        qWarning() << query.lastError().databaseText();
        if (transacting)
            m_db.rollback();
        return false;
    }

    if (!update) {
        QLandmarkCategoryId id;
        id.setManagerUri(managerUri());
        id.setId(query.lastInsertId().toString());
        category->setId(id);
    }

    if (transacting)
        m_db.commit();

    if (!update) {
        QList<QLandmarkCategoryId> ids;
        ids << category->id();
        emit categoriesAdded(ids);
    } else {
        QList<QLandmarkCategoryId> ids;
        ids << category->id();
        emit categoriesChanged(ids);
    }

    /*
    // grab keys from attributes tables for current id
    // delete those we no longer have
    // use replace for the rest

    // loop through attributes

    */

    return true;
}

bool QLandmarkManagerEngineSqlite::removeCategory(const QLandmarkCategoryId &categoryId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    if (categoryId.managerUri() != managerUri()) {
        // TODO set error
        return false;
    }

    bool transacting = m_db.transaction();

    QString q0 = QString("SELECT 1 FROM category WHERE id = %1;").arg(categoryId.id());
    QSqlQuery query0(q0, m_db);
    // invalid id - not an error, but we need to detect this to avoid sending the signals
    if (!query0.next()) {
        if (transacting)
            m_db.commit();
        return true;
    }

    QString q1 = QString("DELETE FROM category WHERE id = %1;").arg(categoryId.id());
    QSqlQuery query1(q1, m_db);
    if (!query1.exec()) {
        if (transacting)
            m_db.rollback();
        return false;
    }

    QString q2 = QString("DELETE FROM landmark_category WHERE category_id = %1;").arg(categoryId.id());
    QSqlQuery query2(q2, m_db);
    if (!query2.exec()) {
        if (transacting)
            m_db.rollback();
        return false;
    }

    if (transacting)
        m_db.commit();

    QList<QLandmarkCategoryId> ids;
    ids << categoryId;
    emit categoriesRemoved(ids);

    return true;
}

bool QLandmarkManagerEngineSqlite::importLandmarks(QIODevice *device,
        QLandmarkManager::Format format,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::exportLandmarks(QIODevice *device,
        QLandmarkManager::Format format,
        QList<QLandmarkId> landmarkIds,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::isFilterSupported(QLandmarkFilter::FilterType filterType) const
{
    switch (filterType) {
            // fall through for supported filters
        case QLandmarkFilter::DefaultFilter:
        case QLandmarkFilter::NameFilter:
        case QLandmarkFilter::ProximityFilter:
        case QLandmarkFilter::NearestFilter:
        case QLandmarkFilter::CategoryFilter:
        case QLandmarkFilter::BoxFilter:
        case QLandmarkFilter::IntersectionFilter:
        case QLandmarkFilter::UnionFilter:
        case QLandmarkFilter::CustomFilter:
            return true;

            // fall through for unsupported filters
        case QLandmarkFilter::InvalidFilter:
        default:
            return false;
    }

    return false;
}

/* Asynchronous Request Support */
void QLandmarkManagerEngineSqlite::requestDestroyed(QLandmarkAbstractRequest* request)
{
}

bool QLandmarkManagerEngineSqlite::startRequest(QLandmarkAbstractRequest* request)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::cancelRequest(QLandmarkAbstractRequest* request)
{
    return false;
}

bool QLandmarkManagerEngineSqlite::waitForRequestFinished(QLandmarkAbstractRequest* request,
        int msecs)
{
    return false;
}

#include "moc_qlandmarkmanagerengine_sqlite_p.cpp"

QTM_END_NAMESPACE


