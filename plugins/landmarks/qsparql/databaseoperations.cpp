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

#include <QDateTime>
#include <QDebug>
#include <QFile>
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
static QHash<QString, QString> countryCodes;

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
    if ((min < value || qFuzzyCompare(value,min)) && (value < max || qFuzzyCompare(value,max)))
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

void shiftCoordinate(QGeoCoordinate *coord, double bearing, double distance)
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
            int comparison = compareDistance(sorted->at(i).coordinate, point.coordinate, center);
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
                                       QueryRun * queryRun =0) {
    QList<QLandmarkId> result;
    QList<LandmarkPoint>  sortedPoints;
    LandmarkPoint point;

    qreal radius = proximityFilter.radius();
    QGeoCoordinate center = proximityFilter.center();

    while(qsparqlResult->next()) {
        if (queryRun && queryRun->isCanceled) {
            if (error)
                *error = QLandmarkManager::CancelError;
            if (errorString)
                *errorString = "Fetch operation canceled";
            return QList<QLandmarkId>();
        }

        point.coordinate.setLatitude(qsparqlResult->value(1).toDouble());
        point.coordinate.setLongitude(qsparqlResult->value(2).toDouble());

        point.landmarkId.setManagerUri(managerUri);
        point.landmarkId.setLocalId(qsparqlResult->value(0).toString());

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

QString createRegex(QLandmarkAttributeFilter attributeFilter, QString key)
{
    QString regexVariable = attributeFilter.attribute(key).toString();
    if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchStartsWith)
        regexVariable.prepend("^");
    if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchEndsWith)
        regexVariable.append("$");
    if (attributeFilter.matchFlags(key) == QLandmarkFilter::MatchExactly ||
        attributeFilter.matchFlags(key) == QLandmarkFilter::MatchFixedString) {
        regexVariable.prepend("^");
        regexVariable.append("$");
    }
    return regexVariable;
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
    if (queryResult != 0) {
        queryResult->waitForFinished();
        if (!queryResult->hasError()) {
            if (error)
                *error = QLandmarkManager::NoError;
           if (errorString)
                *errorString ="";
        } else {
            if (error)
                *error = QLandmarkManager::UnknownError;
            if (errorString)
                *errorString ="";
        }
    }
    return queryResult;
}

QString queryStringForRadius(const QGeoCoordinate &coord, qreal radius)
{
    Q_UNUSED(radius);
    Q_UNUSED(coord);
    return "select ?u ?latitude ?longitude {?g a slo:GeoLocation . ?u slo:location ?g . "
        "OPTIONAL { ?g slo:latitude ?latitude } . "
        "OPTIONAL { ?g slo:longitude ?longitude }}";
}

QString landmarkIdsDefaultQueryString()
{
    return "select ?u ?latitude ?longitude ?name {?g a slo:GeoLocation . ?u slo:location ?g . "
        "OPTIONAL { ?u nie:title ?name } . "
        "OPTIONAL { ?g slo:latitude ?latitude } . "
        "OPTIONAL { ?g slo:longitude ?longitude }}";
 }

QString landmarkIdsQueryString(const QList<QLandmarkId> ids)
{
    QString queryString = "select ?u ?latitude ?longitude {?g a slo:GeoLocation . ?u slo:location ?g . "
        "OPTIONAL { ?g slo:latitude ?latitude } . "
        "OPTIONAL { ?g slo:longitude ?longitude } . FILTER ( ";

    foreach(const QLandmarkId &id, ids) {
        QString regex = QString("regex( ?u, '^%1$') || ").arg(id.localId());
        queryString.append(regex);
    }
    if (ids.count() > 0) {
        queryString.chop(3);
        queryString += ")";
    } else {
        queryString.append("regex( ?u, '^ $'))");
    }
    queryString += "}";
    return queryString;
}


QString landmarkIdsNameQueryString(const QLandmarkNameFilter &filter)
{
    if (filter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
        return QString("select ?u  {?g a slo:GeoLocation . ?u slo:location ?g . ?u nie:title ?name . FILTER regex( ?name, '%1') }").arg(filter.name());
    else
        return QString("select ?u  {?g a slo:GeoLocation . ?u slo:location ?g . ?u nie:title ?name . FILTER regex( ?name, '%1', 'i') }").arg(filter.name());
}

QString landmarkIdsCategoryQueryString(const QLandmarkCategoryFilter &filter)
{
    return QString("select ?u { ?u a slo:Landmark ; slo:belongsToCategory '%1' . }").arg(filter.categoryId().localId());
}

QString landmarkIdsBoxQueryString(const QLandmarkBoxFilter &filter)
{
    double tly = filter.boundingBox().topLeft().latitude();
    double bry = filter.boundingBox().bottomRight().latitude();
    double tlx = filter.boundingBox().topLeft().longitude();
    double brx = filter.boundingBox().bottomRight().longitude();

    //note: it is already assumed tly > bry
    bool lonWrap = (tlx > brx);

    QString queryString = "select ?u ?latitude ?longitude {?g a slo:GeoLocation . ?u slo:location ?g . "
        "OPTIONAL { ?g slo:latitude ?latitude } . "
        "OPTIONAL { ?g slo:longitude ?longitude } . FILTER ((";

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

bool categoryNameCompare(const QLandmarkCategory &cat1, const QLandmarkCategory &cat2) {
    return (cat1.name() < cat2.name());
}

void fillCountryCodesTable() {
    countryCodes.insertMulti("AFG","Afghanistan");
    countryCodes.insertMulti("ALA","land Islands");
    countryCodes.insertMulti("ALB","Albania");
    countryCodes.insertMulti("DZA","Algeria");
    countryCodes.insertMulti("ASM","American Samoa");
    countryCodes.insertMulti("AND","Andorra");
    countryCodes.insertMulti("AGO","Angola");
    countryCodes.insertMulti("AIA","Anguilla");
    countryCodes.insertMulti("ATA","Antarctica");
    countryCodes.insertMulti("ATG","Antigua and Barbuda");
    countryCodes.insertMulti("ARG","Argentina");
    countryCodes.insertMulti("ARM","Armenia");
    countryCodes.insertMulti("ABW","Aruba");
    countryCodes.insertMulti("AUS","Australia");
    countryCodes.insertMulti("AUT","Austria");
    countryCodes.insertMulti("AZE","Azerbaijan");
    countryCodes.insertMulti("BHS","Bahamas");
    countryCodes.insertMulti("BHR","Bahrain");
    countryCodes.insertMulti("BGD","Bangladesh");
    countryCodes.insertMulti("BRB","Barbados");
    countryCodes.insertMulti("BLR","Belarus");
    countryCodes.insertMulti("BEL","Belgium");
    countryCodes.insertMulti("BLZ","Belize");
    countryCodes.insertMulti("BEN","Benin");
    countryCodes.insertMulti("BMU","Bermuda");
    countryCodes.insertMulti("BTN","Bhutan");
    countryCodes.insertMulti("BOL","Bolivia");
    countryCodes.insertMulti("BIH","Bosnia and Herzegovina");
    countryCodes.insertMulti("BWA","Botswana");
    countryCodes.insertMulti("BVT","Bouvet Island");
    countryCodes.insertMulti("BRA","Brazil");
    countryCodes.insertMulti("IOT","British Indian Ocean Territory");
    countryCodes.insertMulti("BRN","Brunei Darussalam");
    countryCodes.insertMulti("BGR","Bulgaria");
    countryCodes.insertMulti("BFA","Burkina Faso");
    countryCodes.insertMulti("BDI","Burundi");
    countryCodes.insertMulti("KHM","Cambodia");
    countryCodes.insertMulti("CMR","Cameroon");
    countryCodes.insertMulti("CAN","Canada");
    countryCodes.insertMulti("CPV","Cape Verde");
    countryCodes.insertMulti("CYM","Cayman Islands");
    countryCodes.insertMulti("CAF","Central African Republic");
    countryCodes.insertMulti("TCD","Chad");
    countryCodes.insertMulti("CHL","Chile");
    countryCodes.insertMulti("CHN","China");
    countryCodes.insertMulti("CXR","Christmas Island");
    countryCodes.insertMulti("CCK","Cocos (Keeling) Islands");
    countryCodes.insertMulti("COL","Colombia");
    countryCodes.insertMulti("COM","Comoros");
    countryCodes.insertMulti("COG","Congo");
    countryCodes.insertMulti("COD","Congo, the Democratic Republic of the");
    countryCodes.insertMulti("COK","Cook Islands");
    countryCodes.insertMulti("CRI","Costa Rica");
    countryCodes.insertMulti("CIV","Cte d'Ivoire");
    countryCodes.insertMulti("HRV","Croatia");
    countryCodes.insertMulti("CUB","Cuba");
    countryCodes.insertMulti("CYP","Cyprus");
    countryCodes.insertMulti("CZE","Czech Republic");
    countryCodes.insertMulti("DNK","Denmark");
    countryCodes.insertMulti("DJI","Djibouti");
    countryCodes.insertMulti("DMA","Dominica");
    countryCodes.insertMulti("DOM","Dominican Republic");
    countryCodes.insertMulti("ECU","Ecuador");
    countryCodes.insertMulti("EGY","Egypt");
    countryCodes.insertMulti("SLV","El Salvador");
    countryCodes.insertMulti("GNQ","Equatorial Guinea");
    countryCodes.insertMulti("ERI","Eritrea");
    countryCodes.insertMulti("EST","Estonia");
    countryCodes.insertMulti("ETH","Ethiopia");
    countryCodes.insertMulti("FLK","Falkland Islands (Malvinas)");
    countryCodes.insertMulti("FRO","Faroe Islands");
    countryCodes.insertMulti("FJI","Fiji");
    countryCodes.insertMulti("FIN","Finland");
    countryCodes.insertMulti("FRA","France");
    countryCodes.insertMulti("GUF","French Guiana");
    countryCodes.insertMulti("PYF","French Polynesia");
    countryCodes.insertMulti("ATF","French Southern Territories");
    countryCodes.insertMulti("GAB","Gabon");
    countryCodes.insertMulti("GMB","Gambia");
    countryCodes.insertMulti("GEO","Georgia");
    countryCodes.insertMulti("DEU","Germany");
    countryCodes.insertMulti("GHA","Ghana");
    countryCodes.insertMulti("GIB","Gibraltar");
    countryCodes.insertMulti("GRC","Greece");
    countryCodes.insertMulti("GRL","Greenland");
    countryCodes.insertMulti("GRD","Grenada");
    countryCodes.insertMulti("GLP","Guadeloupe");
    countryCodes.insertMulti("GUM","Guam");
    countryCodes.insertMulti("GTM","Guatemala");
    countryCodes.insertMulti("GGY","Guernsey");
    countryCodes.insertMulti("GIN","Guinea");
    countryCodes.insertMulti("GNB","Guinea-Bissau");
    countryCodes.insertMulti("GUY","Guyana");
    countryCodes.insertMulti("HTI","Haiti");
    countryCodes.insertMulti("HMD","Heard Island and McDonald Islands");
    countryCodes.insertMulti("VAT","Holy See (Vatican City State)");
    countryCodes.insertMulti("HND","Honduras");
    countryCodes.insertMulti("HKG","Hong Kong");
    countryCodes.insertMulti("HUN","Hungary");
    countryCodes.insertMulti("ISL","Iceland");
    countryCodes.insertMulti("IND","India");
    countryCodes.insertMulti("IDN","Indonesia");
    countryCodes.insertMulti("*IW","International Waters (WOUDC assigned code)");
    countryCodes.insertMulti("IRN","Iran, Islamic Republic of");
    countryCodes.insertMulti("IRQ","Iraq");
    countryCodes.insertMulti("IRL","Ireland");
    countryCodes.insertMulti("IMN","Isle of Man");
    countryCodes.insertMulti("ISR","Israel");
    countryCodes.insertMulti("ITA","Italy");
    countryCodes.insertMulti("JAM","Jamaica");
    countryCodes.insertMulti("JPN","Japan");
    countryCodes.insertMulti("JEY","Jersey");
    countryCodes.insertMulti("JOR","Jordan");
    countryCodes.insertMulti("KAZ","Kazakhstan");
    countryCodes.insertMulti("KEN","Kenya");
    countryCodes.insertMulti("KIR","Kiribati");
    countryCodes.insertMulti("PRK","Korea, Democratic People's Republic of");
    countryCodes.insertMulti("PRK","Democratic People's Republic of Korea");
    countryCodes.insertMulti("PRK","North Korea");
    countryCodes.insertMulti("KOR","Korea, Republic of");
    countryCodes.insertMulti("KOR","South Korea");
    countryCodes.insertMulti("KWT","Kuwait");
    countryCodes.insertMulti("KGZ","Kyrgyzstan");
    countryCodes.insertMulti("LAO","Lao People's Democratic Republic");
    countryCodes.insertMulti("LVA","Latvia");
    countryCodes.insertMulti("LBN","Lebanon");
    countryCodes.insertMulti("LSO","Lesotho");
    countryCodes.insertMulti("LBR","Liberia");
    countryCodes.insertMulti("LBY","Libyan Arab Jamahiriya");
    countryCodes.insertMulti("LIE","Liechtenstein");
    countryCodes.insertMulti("LTU","Lithuania");
    countryCodes.insertMulti("LUX","Luxembourg");
    countryCodes.insertMulti("MAC","Macao");
    countryCodes.insertMulti("MKD","Macedonia, the former Yugoslav Republic of");
    countryCodes.insertMulti("MDG","Madagascar");
    countryCodes.insertMulti("MWI","Malawi");
    countryCodes.insertMulti("MYS","Malaysia");
    countryCodes.insertMulti("MDV","Maldives");
    countryCodes.insertMulti("MLI","Mali");
    countryCodes.insertMulti("MLT","Malta");
    countryCodes.insertMulti("MHL","Marshall Islands");
    countryCodes.insertMulti("MTQ","Martinique");
    countryCodes.insertMulti("MRT","Mauritania");
    countryCodes.insertMulti("MUS","Mauritius");
    countryCodes.insertMulti("MYT","Mayotte");
    countryCodes.insertMulti("MEX","Mexico");
    countryCodes.insertMulti("FSM","Micronesia, Federated States of");
    countryCodes.insertMulti("MDA","Moldova, Republic of");
    countryCodes.insertMulti("MCO","Monaco");
    countryCodes.insertMulti("MNG","Mongolia");
    countryCodes.insertMulti("MNE","Montenegro");
    countryCodes.insertMulti("MSR","Montserrat");
    countryCodes.insertMulti("MAR","Morocco");
    countryCodes.insertMulti("MOZ","Mozambique");
    countryCodes.insertMulti("MMR","Myanmar");
    countryCodes.insertMulti("NAM","Namibia");
    countryCodes.insertMulti("NRU","Nauru");
    countryCodes.insertMulti("NPL","Nepal");
    countryCodes.insertMulti("NLD","Netherlands");
    countryCodes.insertMulti("ANT","Netherlands Antilles");
    countryCodes.insertMulti("NCL","New Caledonia");
    countryCodes.insertMulti("NZL","New Zealand");
    countryCodes.insertMulti("NIC","Nicaragua");
    countryCodes.insertMulti("NER","Niger");
    countryCodes.insertMulti("NGA","Nigeria");
    countryCodes.insertMulti("NIU","Niue");
    countryCodes.insertMulti("NFK","Norfolk Island");
    countryCodes.insertMulti("MNP","Northern Mariana Islands");
    countryCodes.insertMulti("NOR","Norway");
    countryCodes.insertMulti("OMN","Oman");
    countryCodes.insertMulti("PAK","Pakistan");
    countryCodes.insertMulti("PLW","Palau");
    countryCodes.insertMulti("PSE","Palestinian Territory, Occupied");
    countryCodes.insertMulti("PAN","Panama");
    countryCodes.insertMulti("PNG","Papua New Guinea");
    countryCodes.insertMulti("PRY","Paraguay");
    countryCodes.insertMulti("PER","Peru");
    countryCodes.insertMulti("PHL","Philippines");
    countryCodes.insertMulti("PCN","Pitcairn");
    countryCodes.insertMulti("POL","Poland");
    countryCodes.insertMulti("PRT","Portugal");
    countryCodes.insertMulti("PRI","Puerto Rico");
    countryCodes.insertMulti("QAT","Qatar");
    countryCodes.insertMulti("REU","Runion");
    countryCodes.insertMulti("ROU","Romania");
    countryCodes.insertMulti("RUS","Russian Federation");
    countryCodes.insertMulti("RWA","Rwanda");
    countryCodes.insertMulti("BLM","Saint Barthlemy");
    countryCodes.insertMulti("SHN","Saint Helena");
    countryCodes.insertMulti("KNA","Saint Kitts and Nevis");
    countryCodes.insertMulti("LCA","Saint Lucia");
    countryCodes.insertMulti("MAF","Saint Martin (French part)");
    countryCodes.insertMulti("SPM","Saint Pierre and Miquelon");
    countryCodes.insertMulti("VCT","Saint Vincent and the Grenadines");
    countryCodes.insertMulti("WSM","Samoa");
    countryCodes.insertMulti("SMR","San Marino");
    countryCodes.insertMulti("STP","Sao Tome and Principe");
    countryCodes.insertMulti("SAU","Saudi Arabia");
    countryCodes.insertMulti("SEN","Senegal");
    countryCodes.insertMulti("SRB","Serbia");
    countryCodes.insertMulti("SYC","Seychelles");
    countryCodes.insertMulti("SLE","Sierra Leone");
    countryCodes.insertMulti("SGP","Singapore");
    countryCodes.insertMulti("SVK","Slovakia");
    countryCodes.insertMulti("SVN","Slovenia");
    countryCodes.insertMulti("SLB","Solomon Islands");
    countryCodes.insertMulti("SOM","Somalia");
    countryCodes.insertMulti("ZAF","South Africa");
    countryCodes.insertMulti("SGS","South Georgia and the South Sandwich Islands");
    countryCodes.insertMulti("ESP","Spain");
    countryCodes.insertMulti("LKA","Sri Lanka");
    countryCodes.insertMulti("SDN","Sudan");
    countryCodes.insertMulti("SUR","Suriname");
    countryCodes.insertMulti("SJM","Svalbard and Jan Mayen");
    countryCodes.insertMulti("SWZ","Swaziland");
    countryCodes.insertMulti("SWE","Sweden");
    countryCodes.insertMulti("CHE","Switzerland");
    countryCodes.insertMulti("SYR","Syrian Arab Republic");
    countryCodes.insertMulti("TWN","Taiwan, Province of China");
    countryCodes.insertMulti("TJK","Tajikistan");
    countryCodes.insertMulti("TZA","Tanzania, United Republic of");
    countryCodes.insertMulti("THA","Thailand");
    countryCodes.insertMulti("TLS","Timor-Leste");
    countryCodes.insertMulti("TGO","Togo");
    countryCodes.insertMulti("TKL","Tokelau");
    countryCodes.insertMulti("TON","Tonga");
    countryCodes.insertMulti("TTO","Trinidad and Tobago");
    countryCodes.insertMulti("TUN","Tunisia");
    countryCodes.insertMulti("TUR","Turkey");
    countryCodes.insertMulti("TKM","Turkmenistan");
    countryCodes.insertMulti("TCA","Turks and Caicos Islands");
    countryCodes.insertMulti("TUV","Tuvalu");
    countryCodes.insertMulti("UGA","Uganda");
    countryCodes.insertMulti("UKR","Ukraine");
    countryCodes.insertMulti("ARE","United Arab Emirates");
    countryCodes.insertMulti("GBR","United Kingdom");
    countryCodes.insertMulti("USA","United States");
    countryCodes.insertMulti("UMI","United States Minor Outlying Islands");
    countryCodes.insertMulti("URY","Uruguay");
    countryCodes.insertMulti("UZB","Uzbekistan");
    countryCodes.insertMulti("VUT","Vanuatu");
    countryCodes.insertMulti("VEN","Venezuela");
    countryCodes.insertMulti("VNM","Viet Nam");
    countryCodes.insertMulti("VGB","Virgin Islands, British");
    countryCodes.insertMulti("VIR","Virgin Islands, U.S.");
    countryCodes.insertMulti("WLF","Wallis and Futuna");
    countryCodes.insertMulti("ESH","Western Sahara");
    countryCodes.insertMulti("YEM","Yemen");
    countryCodes.insertMulti("ZMB","Zambia");
    countryCodes.insertMulti("ZWE","Zimbabwe");
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
    if (countryCodes.size() == 0) {
        fillCountryCodesTable();
    }
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
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
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
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
            *errorString = "Landmark local id is empty";
        return QLandmark();
    }
    QLandmark lm;
    if (queryRun && queryRun->isCanceled) {
        return QLandmark();
    }
    QSparqlConnection conn("QTRACKER");
    QString queryString = QString("select ?u nie:title(?u) slo:latitude(?g) slo:longitude(?g) slo:altitude(?g) "
        "slo:iconUrl(?u) nie:description(?u) slo:radius(?g) "
        "nco:country(?pa) nco:region(?pa) nco:locality(?pa) nco:streetAddress(?pa) nco:postalcode(?pa) "
        "nco:url(?c) nco:hasPhoneNumber(?c) "
        "{?u a slo:Landmark ; slo:location ?g ; slo:hasContact ?c . "
        "?g a slo:GeoLocation ; slo:postalAddress ?pa . ?c a nco:PersonContact "
        "FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());

    QSparqlQuery qsparqlQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);
    QSparqlResult* qsparqlResult = conn.exec(qsparqlQuery);
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

    bool found = false;
    bool lmHasPhoneNumber = false;
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
            coord.setLatitude(qsparqlResult->value(2).toString().toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted. Latitude is not a double for landmark id: %1")
                        .arg(landmarkId.localId());
                return QLandmark();
            }
        }
        if (!qsparqlResult->value(3).toString().isEmpty()) {
            coord.setLongitude(qsparqlResult->value(3).toString().toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted. Longitude is not a double for landmark id: %1")
                        .arg(landmarkId.localId());
                return QLandmark();
            }
        }
        if (!qsparqlResult->value(4).toString().isEmpty()) {
            coord.setAltitude(qsparqlResult->value(4).toString().toDouble(&ok));
            if (!ok) {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Landmark database is corrupted. Altitude is not a double for landmark id: %1")
                        .arg(landmarkId.localId());
                return QLandmark();
            }
        }
        lm.setCoordinate(coord);
        if (!qsparqlResult->value(5).toString().isEmpty())
            lm.setIconUrl(QUrl(qsparqlResult->value(5).toString()));
        if (!qsparqlResult->value(6).toString().isEmpty())
            lm.setDescription(qsparqlResult->value(6).toString());
        else
            lm.setDescription("");
        if (!qsparqlResult->value(7).toString().isEmpty()) {
            if (qsparqlResult->value(7).toString().toFloat() != 0)
                lm.setRadius(qsparqlResult->value(7).toString().toFloat());
            else
                lm.setRadius(0.0);
        } else {
            lm.setRadius(0.0);
        }
        QGeoAddress address = lm.address();
        if (!qsparqlResult->value(8).toString().isEmpty()) {
            address.setCountry(qsparqlResult->value(8).toString());
            QString countryCode = countryCodes.key(qsparqlResult->value(8).toString(), QString());
            if (!countryCode.isEmpty())
                address.setCountryCode(countryCode);
        }
        if (!qsparqlResult->value(9).toString().isEmpty())
            address.setState(qsparqlResult->value(9).toString());
        if (!qsparqlResult->value(10).toString().isEmpty())
            address.setCity(qsparqlResult->value(10).toString());
        if (!qsparqlResult->value(11).toString().isEmpty())
            address.setStreet(qsparqlResult->value(11).toString());
        if (!qsparqlResult->value(12).toString().isEmpty())
            address.setPostcode(qsparqlResult->value(12).toString());
        lm.setAddress(address);
        if (!qsparqlResult->value(13).toString().isEmpty())
            lm.setUrl(QUrl(qsparqlResult->value(13).toString()));
        if (!qsparqlResult->value(14).toString().isEmpty())
            lmHasPhoneNumber = true;
    }
    delete qsparqlResult;
    if (found) {
        lm.setLandmarkId(landmarkId);
        queryString = QString("select ?u {?u a slo:LandmarkCategory . "
            "?landmarkId slo:belongsToCategory ?u . FILTER regex( ?landmarkId, '^%1$') }").arg(landmarkId.localId());
        qsparqlQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);
        QSparqlResult* qsparqlResult2 = conn.exec(qsparqlQuery);
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
        delete qsparqlResult2;
        if (lmHasPhoneNumber) {
            queryString = QString("select ?p { ?u a slo:Landmark ; slo:hasContact ?c . ?c a nco:PersonContact ; nco:hasPhoneNumber ?pn"
                " . ?pn a nco:PhoneNumber ; nco:phoneNumber ?p . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
            qsparqlQuery = QSparqlQuery(queryString, QSparqlQuery::SelectStatement);
            QSparqlResult* qsparqlResult3 = conn.exec(qsparqlQuery);
            qsparqlResult3->waitForFinished();
            if(!qsparqlResult3->hasError()) {
                qsparqlResult3->next();
                if (!qsparqlResult3->value(0).toString().isEmpty())
                    lm.setPhoneNumber(qsparqlResult3->value(0).toString());
            }
            delete qsparqlResult3;
        }
    } else {
        Q_ASSERT(error);
        Q_ASSERT(errorString);
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
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
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString ="";

    QList<QLandmarkId> result;
    bool alreadySorted = false;

    for (int i=0; i < sortOrders.count(); ++i) {
        if (sortOrders.at(i).type() == QLandmarkSortOrder::NameSort) {
            QLandmarkNameSort nameSort = sortOrders.at(i);
            if (nameSort.caseSensitivity() == Qt::CaseSensitive) {
                if (error)
                    *error = QLandmarkManager::NotSupportedError;
                if (errorString)
                    *errorString = "Case sensitive sorting is not supported";
                return QList<QLandmarkId>();
            }
        }
    }
    if (queryRun && queryRun->isCanceled) {
        if (error)
            *error = QLandmarkManager::CancelError;
        if (errorString)
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
                    if (error)
                        *error = QLandmarkManager::NotSupportedError;
                    if (errorString)
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
                queryString = QString("select ?u ?name {?g a slo:GeoLocation . ?u slo:location ?g . "
                        "OPTIONAL { ?u nie:title ?name }");
                break;
            } else {
                queryString = QString("select ?u {?u a slo:Landmark . ?u slo:hasContact ?pc . ?u slo:location ?g . "
                    "OPTIONAL { ?pc nco:hasPhoneNumber ?pn } . OPTIONAL { ?pn nco:phoneNumber ?phoneNumber } . "
                    "OPTIONAL { ?g slo:postalAddress ?pa } . "
                    "OPTIONAL { ?u nie:title ?name } . OPTIONAL { ?g slo:latitude ?latitude } . "
                    "OPTIONAL { ?g slo:longitude ?longitude } . OPTIONAL { ?g slo:altitude ?altitude } . "
                    "OPTIONAL { ?g slo:radius ?radius } . OPTIONAL { ?u nie:description ?description } . "
                    "OPTIONAL { ?pa nco:country ?country } . OPTIONAL { ?pa nco:region ?region } . "
                    "OPTIONAL { ?pa nco:locality ?locality } . OPTIONAL { ?pa nco:streetAddress ?street } . "
                    "OPTIONAL { ?pa nco:postalcode ?postcode }. FILTER ( ");
                if (attributeFilter.operationType() == QLandmarkAttributeFilter::AndOperation) {
                    QStringList lmLocalIds;
                    bool filterAdded = false;
                    if (attributeFilter.attributeKeys().contains("name")) {
                        QString regexVariable = createRegex(attributeFilter, "name");
                        QString nameString;
                        if (attributeFilter.matchFlags("name") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("name") == QLandmarkFilter::MatchExactly)
                            nameString = QString("regex( ?name, '%1' ) && ").arg(regexVariable);
                        else
                            nameString = QString("regex( ?name, '%1', 'i' ) && ").arg(regexVariable);
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
                        QString regexVariable = createRegex(attributeFilter, "description");
                        QString descriptionString;
                        if (attributeFilter.matchFlags("description") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("description") == QLandmarkFilter::MatchExactly)
                            descriptionString = QString("regex( ?description, '%1' ) && ").arg(regexVariable);
                        else
                            descriptionString = QString("regex( ?description, '%1', 'i' ) && ").arg(regexVariable);
                        queryString.append(descriptionString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("state")) {
                        QString stateString = QString("regex( ?region, '%1') && ").arg(attributeFilter.attribute("state").toString());
                        queryString.append(stateString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("country")) {
                        QString regexVariable = createRegex(attributeFilter, "country");
                        QString countryString;
                        if (attributeFilter.matchFlags("country") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("country") == QLandmarkFilter::MatchExactly)
                            countryString = QString("regex( ?country, '%1' ) && ").arg(regexVariable);
                        else
                            countryString = QString("regex( ?country, '%1', 'i' ) && ").arg(regexVariable);
                        queryString.append(countryString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("city")) {
                        QString regexVariable = createRegex(attributeFilter, "city");
                        QString cityString;
                        if (attributeFilter.matchFlags("city") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("city") == QLandmarkFilter::MatchExactly)
                            cityString = QString("regex( ?locality, '%1' ) && ").arg(regexVariable);
                        else
                            cityString = QString("regex( ?locality, '%1', 'i' ) && ").arg(regexVariable);
                        queryString.append(cityString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("street")) {
                        QString regexVariable = createRegex(attributeFilter, "street");
                        QString streetString;
                        if (attributeFilter.matchFlags("street") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("street") == QLandmarkFilter::MatchExactly)
                            streetString = QString("regex( ?street, '%1' ) && ").arg(regexVariable);
                        else
                            streetString = QString("regex( ?street, '%1', 'i' ) && ").arg(regexVariable);
                        queryString.append(streetString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("postcode")) {
                        QString regexVariable = createRegex(attributeFilter, "postcode");
                        QString postcodeString;
                        if (attributeFilter.matchFlags("postcode") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("postcode") == QLandmarkFilter::MatchExactly)
                            postcodeString = QString("regex( ?postcode, '%1' ) && ").arg(regexVariable);
                        else
                            postcodeString = QString("regex( ?postcode, '%1', 'i' ) && ").arg(regexVariable);
                        queryString.append(postcodeString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("phoneNumber")) {
                        QString regexVariable = createRegex(attributeFilter, "phoneNumber");
                        QString phoneNumberString;
                        if (attributeFilter.matchFlags("phoneNumber") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("phoneNumber") == QLandmarkFilter::MatchExactly)
                            phoneNumberString = QString("regex( ?phoneNumber, '%1' ) && ").arg(regexVariable);
                        else
                            phoneNumberString = QString("regex( ?phoneNumber, '%1', 'i' ) && ").arg(regexVariable);
                        queryString.append(phoneNumberString);
                        if (!attributeFilter.attribute("phoneNumber").toString().isEmpty()) {
                            queryString.replace( "OPTIONAL { ?pc nco:hasPhoneNumber ?pn } . "
                                "OPTIONAL { ?pn nco:phoneNumber ?phoneNumber } . ", "?pc nco:hasPhoneNumber ?pn . "
                                "?pn nco:phoneNumber ?phoneNumber . ");
                        }
                        filterAdded = true;
                    }
                    if (filterAdded) {
                        queryString.chop(3);
                        queryString.append(" )");
                    } else {
                        queryString.chop(9);
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
                            if(!lmLocalIds.contains(qsparqlResult->value(0).toString()))
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
                    if (attributeFilter.attributeKeys().contains("name")) {
                        QString regexVariable = createRegex(attributeFilter, "name");
                        QString nameString;
                        if (attributeFilter.matchFlags("name") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("name") == QLandmarkFilter::MatchExactly)
                            nameString = QString("regex( ?name, '%1' ) || ").arg(regexVariable);
                        else
                            nameString = QString("regex( ?name, '%1', 'i' ) || ").arg(regexVariable);
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
                        QString regexVariable = createRegex(attributeFilter, "description");
                        QString descriptionString;
                        if (attributeFilter.matchFlags("description") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("description") == QLandmarkFilter::MatchExactly)
                            descriptionString = QString("regex( ?description, '%1' ) || ").arg(regexVariable);
                        else
                            descriptionString = QString("regex( ?description, '%1', 'i' ) || ").arg(regexVariable);
                        queryString.append(descriptionString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("state")) {
                        QString stateString = QString("regex( ?region, '%1') || ").arg(attributeFilter.attribute("state").toString());
                        queryString.append(stateString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("country")) {
                        QString regexVariable = createRegex(attributeFilter, "country");
                        QString countryString;
                        if (attributeFilter.matchFlags("country") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("country") == QLandmarkFilter::MatchExactly)
                            countryString = QString("regex( ?country, '%1' ) || ").arg(regexVariable);
                        else
                            countryString = QString("regex( ?country, '%1', 'i' ) || ").arg(regexVariable);
                        queryString.append(countryString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("city")) {
                        QString regexVariable = createRegex(attributeFilter, "city");
                        QString cityString;
                        if (attributeFilter.matchFlags("city") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("city") == QLandmarkFilter::MatchExactly)
                            cityString = QString("regex( ?locality, '%1' ) || ").arg(regexVariable);
                        else
                            cityString = QString("regex( ?locality, '%1', 'i' ) || ").arg(regexVariable);
                        queryString.append(cityString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("street")) {
                        QString regexVariable = createRegex(attributeFilter, "street");
                        QString streetString;
                        if (attributeFilter.matchFlags("street") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("street") == QLandmarkFilter::MatchExactly)
                            streetString = QString("regex( ?street, '%1' ) || ").arg(regexVariable);
                        else
                            streetString = QString("regex( ?street, '%1', 'i' ) || ").arg(regexVariable);
                        queryString.append(streetString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("postcode")) {
                        QString regexVariable = createRegex(attributeFilter, "postcode");
                        QString postcodeString;
                        if (attributeFilter.matchFlags("postcode") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("postcode") == QLandmarkFilter::MatchExactly)
                            postcodeString = QString("regex( ?postcode, '%1' ) || ").arg(regexVariable);
                        else
                            postcodeString = QString("regex( ?postcode, '%1', 'i' ) || ").arg(regexVariable);
                        queryString.append(postcodeString);
                        filterAdded = true;
                    }
                    if (attributeFilter.attributeKeys().contains("phoneNumber")) {
                        QString regexVariable = createRegex(attributeFilter, "phoneNumber");
                        QString phoneNumberString;
                        if (attributeFilter.matchFlags("phoneNumber") & QLandmarkFilter::MatchCaseSensitive ||
                            attributeFilter.matchFlags("phoneNumber") == QLandmarkFilter::MatchExactly)
                            phoneNumberString = QString("regex( ?phoneNumber, '%1' ) || ").arg(regexVariable);
                        else
                            phoneNumberString = QString("regex( ?phoneNumber, '%1', 'i' ) || ").arg(regexVariable);
                        queryString.append(phoneNumberString);
                        if (!attributeFilter.attribute("phoneNumber").toString().isEmpty()) {
                            queryString.replace( "OPTIONAL { ?pc nco:hasPhoneNumber ?pn } . "
                                "OPTIONAL { ?pn nco:phoneNumber ?phoneNumber } . ", "?pc nco:hasPhoneNumber ?pn . "
                                "?pn nco:phoneNumber ?phoneNumber . ");
                        }
                        filterAdded = true;
                    }
                    if (filterAdded) {
                        queryString.chop(3);
                        queryString.append(" )");
                    } else {
                        queryString.chop(9);
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
                            if(!lmLocalIds.contains(qsparqlResult->value(0).toString()))
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
                if (error)
                    *error = QLandmarkManager::NotSupportedError;
                if (errorString)
                    *errorString = "The name filter's configuration is not supported";
                return result;
            }
            queryString = QString("select ?u { ?g a slo:GeoLocation . ?u slo:location ?g . OPTIONAL { ?u nie:title ?name } . FILTER ");
            QString nameKey = "?name";
            QString nameValue = nameFilter.name();
            QString regex;
            if (nameValue.isEmpty()) {
                queryString = QString("select ?u ?name {?g a slo:GeoLocation . ?u slo:location ?g . "
                        "OPTIONAL { ?u nie:title ?name }}");
            } else if (nameFilter.matchFlags() == QLandmarkFilter::MatchExactly) {
                if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                    regex = QString("regex( ?name, '^%1$' ) }").arg(nameValue);
                else
                    regex = QString("regex( ?name, '^%1$', 'i') }").arg(nameValue);
                queryString.append(regex);
            } else {
                if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchEndsWith) {
                    if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                       regex = QString("regex( ?name, '%1$' ) }").arg(nameValue);
                    else
                        regex = QString("regex( ?name, '%1$', 'i') }").arg(nameValue);
                    queryString.append(regex);
                } else if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchStartsWith) {
                    if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                        regex = QString("regex( ?name, '^%1' ) }").arg(nameValue);
                    else
                        regex = QString("regex( ?name, '^%1', 'i') }").arg(nameValue);
                    queryString.append(regex);
                } else if ((nameFilter.matchFlags() & 3) == QLandmarkFilter::MatchContains) {
                    if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                        regex = QString("regex( ?name, '%1' ) }").arg(nameValue);
                    else
                         regex = QString("regex( ?name, '%1', 'i') }").arg(nameValue);
                    queryString.append(regex);
                } else if (nameFilter.matchFlags() == QLandmarkFilter::MatchFixedString) {
                    if (nameFilter.matchFlags() & QLandmarkFilter::MatchCaseSensitive)
                        regex = QString("regex( ?name, '^%1$' ) }").arg(nameValue);
                    else
                        regex = QString("regex( ?name, '^%1$', 'i') }").arg(nameValue);
                    queryString.append(regex);
                }
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
            if (error)
                *error = QLandmarkManager::NoError;
            if (errorString)
                *errorString = "";
            return result;
        } else if (categoryId.localId().isEmpty()) {
            if (error)
                *error = QLandmarkManager::NoError;
            if (errorString)
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
            if (error)
                *error = QLandmarkManager::NoError;
            if (errorString)
                *errorString = "";
            delete qsparqlResult;
            return result;
        }
        delete qsparqlResult;
        queryString = landmarkIdsCategoryQueryString(categoryFilter);
        break;
        }
    case QLandmarkFilter::ProximityFilter: {
            QLandmarkProximityFilter proximityFilter = filter;
            QGeoCoordinate center = proximityFilter.center();
            if ( (qIsNaN(center.latitude()) || qIsNaN(center.longitude())
                || !isValidLat(center.latitude()) || !isValidLong(center.longitude()))
                ) {
                if (error)
                    *error = QLandmarkManager::BadArgumentError;
                if (errorString)
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
                if (filter.type() == QLandmarkFilter::ProximityFilter) {
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
                        if (error)
                            *error = QLandmarkManager::BadArgumentError;
                        if (errorString)
                            *errorString = "The proximity filter covers the north pole which is not allowed";
                        result.clear();
                        return result;
                    }
                } else { //coordinate must be in southern hemisphere so "shift" south first
                    QGeoCoordinate coord = center;
                    shiftCoordinate(&coord,180, radius);
                    shiftCoordinate(&coord, 90, radius);
                    maxLongAbs = qAbs(center.longitude() - coord.longitude());

                    double minLat = center.latitude() - radius / ( 2.0 * M_PI * EARTH_MEAN_RADIUS *1000) *360;
                    if (minLat < -90.0 || qFuzzyCompare(minLat, -90.0)) {
                        if (error)
                            *error = QLandmarkManager::BadArgumentError;
                        if (errorString)
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
            }

            if (!boxFilter.boundingBox().isValid()) {
                if (error)
                    *error =  QLandmarkManager::BadArgumentError;
                if (errorString)
                    *errorString = QString("Invalid bounding box for landmark box filter");
                return result;
            }
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

            int subOffset = 0;
            int subLimit = -1;

            if (filters.size() == 0) {
                //do nothing
            } else if (filters.size() == 1) {
                result = landmarkIds( filters.at(0), QList<QLandmarkSortOrder>(), subLimit, subOffset, error, errorString);
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
                                                QList<QLandmarkSortOrder>(), subLimit, subOffset, error, errorString);
                ids = firstResult.toSet();

                for (int i = 1; i < filters.size(); ++i) {
                    if (queryRun && queryRun->isCanceled) {
                        if (error)
                            *error = QLandmarkManager::CancelError;
                        if (errorString)
                            *errorString = "Fetch operation canceled";
                        result.clear();
                        return result;
                    }

                    QList<QLandmarkId> subResult = landmarkIds(filters.at(i),
                                                QList<QLandmarkSortOrder>(), subLimit, subOffset, error, errorString);

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
                    delete qsparqlResult;
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

            int subOffset = 0;
            int subLimit = -1;

            if (filters.size() == 0) {
                //do nothing
            } else if (filters.size() == 1) {
                result =  landmarkIds(filters.at(0),
                                        QList<QLandmarkSortOrder>(), subLimit, subOffset, error, errorString);
                if (*error != QLandmarkManager::NoError) {
                    result.clear();
                    return result;
                }
            } else {
                for (int i = 0; i < filters.size(); ++i) {
                    if (queryRun && queryRun->isCanceled) {
                        if (error)
                            *error = QLandmarkManager::CancelError;
                        if (errorString)
                            *errorString = "Fetch operation canceled";
                        result.clear();
                        return result;
                    }
                    QList<QLandmarkId> subResult = landmarkIds(filters.at(i),
                                                               QList<QLandmarkSortOrder>(),
                                                               subLimit, subOffset,
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
            queryString.append(" ORDER BY ");
            if (nameSort.direction() == Qt::AscendingOrder)
                queryString.append("ASC(?name)");
            else
                queryString.append("DESC(?name)");
            alreadySorted = true;
        }
        QSparqlResult* qsparqlResult = executeQuery(m_conn, queryString,bindValues,error,errorString);

        if (qsparqlResult->hasError()) {
            if (error)
                *error = QLandmarkManager::UnknownError;
            delete qsparqlResult;
            return result;
        }
        QLandmarkId id;
        while (qsparqlResult->next()) {
            if (queryRun && queryRun->isCanceled) {
                if (error)
                    *error = QLandmarkManager::CancelError;
                if (errorString)
                    *errorString = "Fetch operation canceled";
                delete qsparqlResult;
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
                        if (error)
                            *error = QLandmarkManager::CancelError;
                        if (errorString)
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
        delete qsparqlResult;
    }
    if (offset  >= result.count()) {
        result.clear();
        return result;
    }

    if (!alreadySorted && sortOrders.count() > 0 ) {
        QList<QLandmark> landmarks;
        QLandmark landmark;
        for (int i=0; i < result.count(); ++i) {
            if (queryRun && queryRun->isCanceled) {
                if (error)
                    *error = QLandmarkManager::CancelError;
                if (errorString)
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
            if (error)
                *error = QLandmarkManager::CancelError;
            if (errorString)
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
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
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
            return false;
        }
    }
    QSparqlConnection conn("QTRACKER");
    QString queryString;

    queryString.append("insert { ?:uri_value a slo:Landmark ");

    for (int i = landmark->categoryIds().size() - 1; i >= 0; --i) {
        QLandmarkCategoryId catId = landmark->categoryIds().at(i);
        QString categoryQueryString = QString("select ?u {?u a slo:LandmarkCategory "
                                              ". FILTER regex( ?u, '^%1$') }").arg(catId.localId());
        QSparqlQuery qsparqlCategoryQuery = QSparqlQuery(categoryQueryString, QSparqlQuery::SelectStatement);
        QSparqlResult* categoryResult = conn.exec(qsparqlCategoryQuery);
        categoryResult->waitForFinished();
        if (categoryResult->hasError()) {
            if (error)
                *error = QLandmarkManager::CategoryDoesNotExistError;
            if (errorString)
                *errorString = "Landmark contains a category id that does not exist";
            return false;
        }
        categoryResult->next();
        if (categoryResult->value(0).toString().isEmpty()) {
            if (error)
                *error = QLandmarkManager::CategoryDoesNotExistError;
            if (errorString)
                *errorString = "Category does not exist in database.";
            return false;
        }
        if (catId.managerUri() == managerUri) {
            queryString.append("; slo:belongsToCategory \"");
            queryString.append(catId.localId());
            queryString.append("\" ");
        } else {
            if (error)
                *error = QLandmarkManager::CategoryDoesNotExistError;
            if (errorString)
                *errorString = "Landmark contains category that belongs to another manager";
            return false;
        }        
        delete categoryResult;
    }
    queryString.append("; nie:identifier \"");
    QDateTime dateTime = QDateTime::currentDateTime();
    qint64 timeStamp = (qint64)dateTime.toTime_t() *1000 + dateTime.time().msec();
    QString timeStampString;
    queryString.append(timeStampString.setNum(timeStamp));
    queryString.append("\" ");

    if (!landmark->name().isEmpty()) {
        queryString.append("; nie:title \"");
        queryString.append(landmark->name());
        queryString.append("\" ");
    }
    if (!landmark->description().isEmpty()) {
        queryString.append("; nie:description \"");
        queryString.append(landmark->description());
        queryString.append("\" ");
    }
    if (!landmark->iconUrl().toString().isEmpty()) {
        queryString.append("; slo:iconUrl \"");
        queryString.append(landmark->iconUrl().toString());
        queryString.append("\" ");
    }
    queryString.append("; slo:hasContact _:c ");

    queryString.append("; slo:location _:x . ");
    queryString.append("_:x a slo:GeoLocation ");

    QGeoCoordinate geoCoord;
    geoCoord = landmark->coordinate();

    if (qIsNaN(geoCoord.latitude()) && qIsNaN(geoCoord.longitude())) {
        // both fields are left empty
    } else if (!qIsNaN(geoCoord.latitude()) && !qIsNaN(geoCoord.longitude())
        && isValidLat(geoCoord.latitude()) && isValidLong(geoCoord.longitude())) {
        queryString.append("; slo:latitude ");
        queryString.append(QString::number(geoCoord.latitude(),'g',8));
        queryString.append(" ");
        queryString.append("; slo:longitude ");
        queryString.append(QString::number(geoCoord.longitude(),'g',8));
        queryString.append(" ");
    } else {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark coordinate is not valid, latitude must between -90 and 90 and longitude must be between -180 and 180, or both "
                "latitude and longitude are NaN";
        return false;
    }
    if (!qIsNaN(geoCoord.altitude())) {
        queryString.append("; slo:altitude ");
        queryString.append(QString::number(geoCoord.altitude(),'g',8));
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
        queryString.append("; nco:country \"");
        queryString.append(landmark->address().country());
        queryString.append("\" ");
    } else {
        QString countryCode = landmark->address().countryCode();
        if (!countryCode.isEmpty()) {
            QString country = countryCodes.value(countryCode, QString());
            if (!country.isEmpty()) {
                queryString.append("; nco:country \"");
                queryString.append(country);
                queryString.append("\" ");
            }
        }
    }
    if (!landmark->address().city().isEmpty()) {
        queryString.append("; nco:locality \"");
        queryString.append(landmark->address().city());
        queryString.append("\" ");
    }
    if (!landmark->address().street().isEmpty()) {
        queryString.append("; nco:streetAddress \"");
        queryString.append(landmark->address().street());
        queryString.append("\" ");
    }
    if (!landmark->address().postcode().isEmpty()) {
        queryString.append("; nco:postalcode \"");
        queryString.append(landmark->address().postcode());
        queryString.append("\" ");
    }
    queryString.append(". _:c a nco:PersonContact ");
    if (!landmark->url().toString().isEmpty()) {
        queryString.append(" ; nco:url \"");
        queryString.append(landmark->url().toString());
        queryString.append("\" ");
    }
    if (!landmark->phoneNumber().isEmpty()) {

        QString phoneQueryString = QString("select ?pn { ?u a slo:Landmark ; slo:hasContact ?c . "
            "?c a nco:PersonContact ; nco:hasPhoneNumber ?pn"
            " . ?pn a nco:PhoneNumber ; nco:phoneNumber ?p . FILTER regex( ?p, '^%1$') }").arg(landmark->phoneNumber());
        QSparqlQuery qsparqlPhoneQuery = QSparqlQuery(phoneQueryString, QSparqlQuery::SelectStatement);
        QSparqlResult* phoneResult = conn.exec(qsparqlPhoneQuery);
        phoneResult->waitForFinished();
        if (!(phoneResult->hasError())) {
            phoneResult->next();
            if (phoneResult->value(0).toString().isEmpty()) {
                queryString.append("; nco:hasPhoneNumber _:pn . ");
                queryString.append("_:pn a nco:PhoneNumber ; ");
                queryString.append("nco:phoneNumber \"");
                queryString.append(landmark->phoneNumber());
                queryString.append("\" ");
            } else {
                queryString.append("; nco:hasPhoneNumber \"");
                queryString.append(phoneResult->value(0).toString());
                queryString.append("\" ");
            }
        }
        delete phoneResult;
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
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = "Unable to execute insert statement. ";
        delete insertResult;
        return false;
    }
    delete insertResult;
if (error)
    *error = QLandmarkManager::NoError;
if (errorString)
    *errorString="";
return true;
}

bool DatabaseOperations::saveLandmark(QLandmark* landmark,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkId> addedIds;
    QList<QLandmarkId> changedIds;
    bool result = saveLandmarkHelper(landmark, error, errorString);

    return result;
}

bool DatabaseOperations::saveLandmarks(QList<QLandmark> * landmark,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString,
        QList<QLandmarkId> *addedIds,
        QList<QLandmarkId> *changedIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();

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
                for (i = 0; i < landmark->size(); ++i)
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
              changedIds->append(landmark->at(i).landmarkId());
          else
              addedIds->append(landmark->at(i).landmarkId());
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
    return noErrors;
}

bool DatabaseOperations::removeLandmarkHelper(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString,
        const QString &managerUri)
{
    QMap<QString,QVariant> bindValues;
    QString q0 = QString("select ?u {?u a slo:Landmark . "
         "FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());
    QSparqlResult *queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);

    if (queryResult->hasError()) {
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
            *errorString = QString("Landmark with local id %1, does not exist in database.")
                .arg(landmarkId.localId());
        delete queryResult;
        return false;
    }
    if (!queryResult->next()) {
        if (error)
            *error = QLandmarkManager::LandmarkDoesNotExistError;
        if (errorString)
            *errorString = QString("Landmark with local id %1, does not exist in database")
                .arg(landmarkId.localId());
        delete queryResult;
        return false;
    }
    if (landmarkId.managerUri() != managerUri) {
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
            *errorString = "Landmark id comes from different landmark manager.";
        delete queryResult;
        return false;
    }   
    delete queryResult;
    QSparqlConnection conn("QTRACKER");
    bool otherContactHasSameNumber = false;
    QString phoneQueryString = QString("select ?c { ?u a slo:Landmark ; slo:hasContact ?pc . "
        "?pc a nco:PersonContact ; nco:hasPhoneNumber ?pn"
        " . ?pn a nco:PhoneNumber ; nco:phoneNumber ?p"
        " . ?c a nco:Contact ; nco:hasPhoneNumber ?pn"
        " . FILTER regex( ?u, '^%1$') }").arg(landmarkId.localId());

    QSparqlQuery qsparqlPhoneQuery = QSparqlQuery(phoneQueryString, QSparqlQuery::SelectStatement);
    QSparqlResult* phoneResult = conn.exec(qsparqlPhoneQuery);
    phoneResult->waitForFinished();
    if (!(phoneResult->hasError())) {
        phoneResult->next();
        if (!phoneResult->value(0).toString().isEmpty()) {
            phoneResult->next();
            if (!phoneResult->value(0).toString().isEmpty()) {
                otherContactHasSameNumber = true;
            }
        }
    }
    delete phoneResult;
    QString deleteQuery = QString( "delete { ?c a nco:PersonContact . } "
               "WHERE { ?:landmark_uri slo:hasContact ?c . } "
               "delete { ?pa a nco:PostalAddress . } "
               "WHERE { ?g slo:postalAddress ?pa . "
               "?:landmark_uri slo:location ?g . } "
               "delete { ?g a slo:GeoLocation . } "
               "WHERE { ?:landmark_uri slo:location ?g . } "
               "delete { ?:landmark_uri nie:description ?des . } "
               "WHERE { ?:landmark_uri nie:description ?des . } "
               "delete { ?:landmark_uri nie:identifier ?ide . } "
               "WHERE { ?:landmark_uri nie:identifier ?ide . } "
               "delete { ?:landmark_uri nie:title ?name . } "
               "WHERE { ?:landmark_uri nie:title ?name . } "
               "delete { ?:landmark_uri a slo:Landmark . }");

    if (!otherContactHasSameNumber) {
        deleteQuery.prepend(QString("delete { ?pn a rdfs:Resource . } "
                  "WHERE { ?c nco:hasPhoneNumber ?pn . "
                  "?c a nco:PersonContact . "
                  "?:landmark_uri slo:hasContact ?c . } "));
    } else {
        deleteQuery.prepend(QString("delete { ?c nco:hasPhoneNumber ?pn . } "
                  "WHERE { ?c a nco:PersonContact . ?pn a nco:PhoneNumber . "
                  "?:landmark_uri slo:hasContact ?c . } "));
    }

    QSparqlQuery qsparqlDeleteQuery = QSparqlQuery(deleteQuery,
            QSparqlQuery::DeleteStatement);

    qsparqlDeleteQuery.unbindValues();
    qsparqlDeleteQuery.bindValue("landmark_uri", QUrl(landmarkId.localId()));
    QSparqlResult* deleteResult = conn.exec(qsparqlDeleteQuery);
    deleteResult->waitForFinished();
    if (deleteResult->hasError()) {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = QString("Unable to execute remove landmark statement.");
        delete deleteResult;
        return false;
    }
    delete deleteResult;
    return true;
}

bool DatabaseOperations::removeLandmark(const QLandmarkId &landmarkId,
        QLandmarkManager::Error *error,
        QString *errorString)
{
    QList<QLandmarkId> removedIds;
    bool result = removeLandmarkHelper(landmarkId, error, errorString, managerUri);

    return result;
}

bool DatabaseOperations::removeLandmarks(const QList<QLandmarkId> &landmarkIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString, QList<QLandmarkId> *removedIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();

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
                for (i=0; i < landmarkIds.size(); ++i)
                    errorMap->insert(i, lastError);
            }
            noErrors = false;
            break;
        }
       result = removeLandmarkHelper(landmarkIds.at(i), &loopError, &loopErrorString, managerUri);
       if (result) {
            removedIds->append(landmarkIds.at(i));
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
    return noErrors;
}

QList<QLandmarkCategoryId> DatabaseOperations::categoryIds(const QLandmarkNameSort &nameSort,
                                       int limit, int offset,
                                       QLandmarkManager::Error *error, QString *errorString) const
{
    QList<QLandmarkCategoryId> result;

    QString uri = managerUri;
    QString queryString = QString("select ?u {?u a slo:LandmarkCategory ; nie:title ?name . } ORDER BY");

    if (nameSort.caseSensitivity() == Qt::CaseSensitive) {
        if (error)
            *error = QLandmarkManager::NotSupportedError;
        if (errorString)
            *errorString = "Case sensitive name sorting of categories is not supported";
        return QList<QLandmarkCategoryId>();
    }

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
            if (error)
                *error = QLandmarkManager::CancelError;
            if (errorString)
                *errorString = "Fetch operation was canceled";
            result.clear();
            return result;
        }
        QLandmarkCategoryId id;
        id.setManagerUri(managerUri);
        id.setLocalId(queryResult->value(0).toString());
        result << id;
    }

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
        if (error)
            *error = QLandmarkManager::UnknownError;
        return QLandmarkCategory();
    }

    bool found = false;
    while (queryResult->next()) {
        if (found) {
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

        if (!queryResult->value(2).toString().isEmpty())
            cat.setIconUrl(queryResult->value(2).toString());

        cat.setCategoryId(landmarkCategoryId);
    }

    if (!found) {
        if (error)
            *error = QLandmarkManager::CategoryDoesNotExistError;
        if (errorString)
            *errorString = "None of the existing categories match the given category id.";
    } else {
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            *errorString = "";
    }
    delete queryResult;
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

    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
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
        if (error)
            *error = QLandmarkManager::NoError;
        if (errorString)
            (*errorString).clear();

        if (queryRun && queryRun->isCanceled) {
            if (error)
                *error = QLandmarkManager::CancelError;
            if (errorString)
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
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
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

    QSparqlConnection conn("QTRACKER");
    QString checkString = QString("select ?c { ?c a slo:LandmarkCategory ; nie:title ?name . FILTER regex( ?name, '^%1$') }").arg(category->name());
    QSparqlQuery query = QSparqlQuery(checkString, QSparqlQuery::SelectStatement);
    QSparqlResult* queryResult = conn.exec(query);
    queryResult->waitForFinished();
    if (queryResult->hasError()) {
        if (error)
            *error =  QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = QString("Category saving failed.");
        return false;
    }
    if (queryResult->next()) {
        if (!update || (update && (queryResult->value(0).toString() != category->categoryId().localId()))) {
            if (error)
                *error = QLandmarkManager::AlreadyExistsError;
            if (errorString)
                *errorString = QString("Category with name: %1 already exists").arg(category->name());
           return false;
        }
    }
    delete queryResult;

    if (update) {
        bool removeResult = removeCategoryHelper(category->categoryId(), error, errorString);
        if(!removeResult) {
            return false;
        }
    }

    QString queryString = QString("insert { ?:uri_value a slo:LandmarkCategory ");

    if (!category->name().isEmpty()) {
        queryString.append("; nie:title \'");
        queryString.append(category->name());
        queryString.append("\' ");
    }
    queryString.append("; nie:identifier \"");
    QDateTime dateTime = QDateTime::currentDateTime();
    qint64 timeStamp = (qint64)dateTime.toTime_t() *1000 + dateTime.time().msec();
    QString timeStampString;
    queryString.append(timeStampString.setNum(timeStamp));
    queryString.append("\" ");

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

    insertResult->waitForFinished();
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
    delete insertResult;
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString="";
    return true;
}

bool DatabaseOperations::saveCategory(QLandmarkCategory *category,
                                      QLandmarkManager::Error *error, QString *errorString)
{
    QList<QLandmarkCategoryId> addedIds;
    QList<QLandmarkCategoryId> changedIds;
    bool result = saveCategoryHelper(category, error, errorString);

   return result;
}

bool DatabaseOperations::saveCategories(QList<QLandmarkCategory> * categories,
        QMap<int, QLandmarkManager::Error> *errorMap,
        QLandmarkManager::Error *error,
        QString *errorString,
        QList<QLandmarkCategoryId> *addedIds,
        QList<QLandmarkCategoryId> *changedIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();

    bool noErrors = true;
    QLandmarkManager::Error lastError = QLandmarkManager::NoError;
    QString lastErrorString;
    QLandmarkManager::Error loopError;
    QString loopErrorString;

    for (int i = 0; i < categories->size(); ++i) {
        loopError = QLandmarkManager::NoError;
        loopErrorString = "";
        bool update = categories->at(i).categoryId().isValid();
        bool result = saveCategoryHelper(&(categories->operator [](i)), &loopError, &loopErrorString);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
            if (errorMap)
                errorMap->insert(i, loopError);
        } else {
          if(update)
              changedIds->append(categories->at(i).categoryId());
          else
              addedIds->append(categories->at(i).categoryId());
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
    return noErrors;
}

bool DatabaseOperations::removeCategory(const QLandmarkCategoryId &categoryId,
    QLandmarkManager::Error *error, QString *errorString)
{
    QList<QLandmarkCategoryId> removedIds;
    bool result = removeCategoryHelper(categoryId, error, errorString);

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
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = QString("Unable to remove category.");
        return false;
    }

    if (!queryResult->next()) {
        if (error)
            *error = QLandmarkManager::CategoryDoesNotExistError;
        if (errorString)
            *errorString = QString("Category with local id %1, does not exist in database")
                        .arg(categoryId.localId());
        return false;
    }

    if((queryResult->value(1).toString().compare("false", Qt::CaseSensitive)) == 0) {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = QString("Category is not removable.");
        return false;
    }
    delete queryResult;

    QSparqlQuery qsparqlDeleteQuery = QSparqlQuery(
            "delete { ?:category_uri nie:identifier ?ide . } "
            "WHERE { ?:category_uri nie:identifier ?ide . } "
            "delete { ?:category_uri nie:title ?title . } "
            "WHERE { ?:category_uri nie:title ?title . } "
            "delete { ?:category_uri a slo:LandmarkCategory . }",
             QSparqlQuery::DeleteStatement);

    qsparqlDeleteQuery.unbindValues();
    qsparqlDeleteQuery.bindValue("category_uri", QUrl(categoryId.localId()));
    QSparqlResult* deleteResult = m_conn->exec(qsparqlDeleteQuery);

    deleteResult->waitForFinished();
    if (deleteResult->hasError()) {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = QString("Unable to execute remove category statement.");
        return false;
    }
    delete deleteResult;
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";
    return true;
}

bool DatabaseOperations::removeCategories(const QList<QLandmarkCategoryId> &categoryIds,
                    QMap<int, QLandmarkManager::Error> *errorMap,
                    QLandmarkManager::Error *error,
                    QString *errorString, QList<QLandmarkCategoryId> *removedIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);
    if (errorMap)
        errorMap->clear();
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

        if (errorMap && (loopError != QLandmarkManager::NoError))
            errorMap->insert(i, loopError);

        if (!result) {
            noErrors = false;
            lastError = loopError;
            lastErrorString = loopErrorString;
        }
        if (result)
            removedIds->append(categoryIds.at(i));
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
    return noErrors;
}

bool DatabaseOperations::importLandmarks(QIODevice *device,
                     const QString &format,
                     QLandmarkManager::TransferOption option,
                     const QLandmarkCategoryId &categoryId,
                     QLandmarkManager::Error *error,
                     QString *errorString,
                     QList<QLandmarkId> *addedLandmarkIds,
                     QList<QLandmarkCategoryId> *addedCategoryIds,
                     QueryRun *queryRun,
                     QList<QLandmarkId> *landmarkIds)
{
    Q_ASSERT(error);
    Q_ASSERT(errorString);

    if (!device) {
        if (error)
             *error = QLandmarkManager::BadArgumentError;
       if (errorString)
            *errorString = "Invalid io device pointer";
       return false;
    }

    QFile *file = qobject_cast<QFile *>(device);
    if (file)
    {
        if (!file->exists()) {
            if (error)
                *error = QLandmarkManager::DoesNotExistError;
            if (errorString)
                *errorString = QString("Import operation failed, file does not exist: %1").arg(file->fileName());
            return false;
        }
        if (!file->open(QIODevice::ReadOnly)) {
            if (file->error() == QFile::OpenError) {
                if (error)
                    *error = QLandmarkManager::PermissionsError;
                if (errorString)
                    *errorString = QString("Insufficient permissions to open file");
                return false;
            } else {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Unable to open file for importing landmarks");
                return false;
            }
        }
    } else if(!device->open(QIODevice::ReadOnly)) {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
            *errorString = "Unable to open io device for importing landmarks";
        return false;
    }

    QString detectedFormat = format;
    if (detectedFormat == "") {
        QXmlStreamReader reader(device);
        if (!reader.readNextStartElement()) {
            if (error)
                *error = QLandmarkManager::ParsingError;
            if (errorString)
                *errorString = "Could not read root element of io device";
            device->reset();
            return false;
        } else {
            if (reader.name() == "lmx")
                detectedFormat = (const QLatin1String)QLandmarkManager::Lmx;
            else if (reader.name() == "gpx")
                detectedFormat = (const QLatin1String)QLandmarkManager::Gpx;
            else {
                if (error)
                    *error = QLandmarkManager::NotSupportedError;
                if (errorString)
                    *errorString = "Unknown import file format";
                device->reset();
                return false;
            }
        }
        device->reset();
    }

    bool result = false;
    if (detectedFormat ==  QLandmarkManager::Lmx) {
        result = importLandmarksLmx(device, option, categoryId, error, errorString, queryRun, landmarkIds,
                                    addedLandmarkIds, addedCategoryIds);
        device->close();
        return result;
    } else if (detectedFormat == QLandmarkManager::Gpx) {
        result = importLandmarksGpx(device, option, categoryId, error, errorString, queryRun, landmarkIds,
                                    addedLandmarkIds);
        if (file)
            file->close();
        else
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
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
             *errorString = "Invalid io device pointer";
       return false;
    }

    QFile *file = qobject_cast<QFile *>(device);
    if (file) {
        if (!file->open(QIODevice::WriteOnly)) {
            if (file->error() == QFile::OpenError) {
                if (error)
                    *error = QLandmarkManager::PermissionsError;
                if (errorString)
                    *errorString = QString("Insufficient permissions to open file");
                return false;
            } else {
                if (error)
                    *error = QLandmarkManager::UnknownError;
                if (errorString)
                    *errorString = QString("Unable to open file for importing landmarks");
                return false;
            }
        }
    } else if (!device->open(QIODevice::WriteOnly)) {
        if (error)
            *error = QLandmarkManager::UnknownError;
        if (errorString)
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
        if (error)
            *error = QLandmarkManager::BadArgumentError;
        if (errorString)
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
                        QList<QLandmarkId>  *landmarkIds,
                        QList<QLandmarkId>* addedLandmarkIds,
                        QList<QLandmarkCategoryId>* addedCategoryIds)
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
        if (error)
            *error = lmxHandler.errorCode();
        if (errorString)
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
                        if (addedCategoryIds)
                            addedCategoryIds->clear();
                        return false;
                    } else {
                        addedCategoryIds->append(category.categoryId());
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
            if (error)
                *error = QLandmarkManager::CancelError;
            if (errorString)
                *errorString = "Import of lmx file canceled";
        } else {
            saveLandmarkHelper(&(landmarks[i]),error, errorString);
        }
        if (*error != QLandmarkManager::NoError) {
            for(int j=0; j < i; ++j) {
                QLandmarkManager::Error *removeError = new QLandmarkManager::Error();
                QString *removeErrorString = new QString();
                removeLandmarkHelper(landmarks[j].landmarkId(), removeError, removeErrorString, managerUri);
                delete removeError;
                delete removeErrorString;
            }
            for(int j=0; j<addedCategoryIds->count(); ++j) {
                QLandmarkManager::Error *removeError = new QLandmarkManager::Error();
                QString *removeErrorString = new QString();
                removeCategoryHelper(addedCategoryIds->at(j), removeError, removeErrorString);
                delete removeError;
                delete removeErrorString;
            }
            if(landmarkIds)
                landmarkIds->clear();
            if (addedLandmarkIds)
                addedLandmarkIds->clear();
            if (addedCategoryIds)
                addedCategoryIds->clear();
            return false;
        } else {
            addedLandmarkIds->append(landmarks[i].landmarkId());
        }

        if (landmarkIds)
            landmarkIds->append(landmarks[i].landmarkId());
    }
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";
    return true;
}

bool DatabaseOperations::importLandmarksGpx(QIODevice *device,
                        QLandmarkManager::TransferOption option,
                        const QLandmarkCategoryId &categoryId,
                        QLandmarkManager::Error *error,
                        QString *errorString,
                        QueryRun *queryRun,
                        QList<QLandmarkId> *landmarkIds,
                        QList<QLandmarkId>*)
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
        if (error)
            *error = gpxHandler.error();
        if (errorString)
            *errorString = gpxHandler.errorString();
        return false;
    }

    QList<QLandmark> landmarks = gpxHandler.waypoints();
    for(int i =0; i < landmarks.count(); ++i) {
        if (option == QLandmarkManager::AttachSingleCategory)
            landmarks[i].addCategoryId(categoryId);

        if (queryRun && queryRun->isCanceled) {
            if (error)
                *error = QLandmarkManager::CancelError;
            if (errorString)
                *errorString = "Import of gpx file canceled";
        } else {
            saveLandmarkHelper(&(landmarks[i]),error, errorString);
        }
        if (*error != QLandmarkManager::NoError) {
            for(int j=0; j < i; ++j) {
                QLandmarkManager::Error *removeError = new QLandmarkManager::Error();
                QString *removeErrorString = new QString();
                removeLandmarkHelper(landmarks[j].landmarkId(), removeError, removeErrorString, managerUri);
                delete removeError;
                delete removeErrorString;
            }
            if (landmarkIds)
                landmarkIds->clear();
            return false;
        }
        if (landmarkIds)
            landmarkIds->append(landmarks[i].landmarkId());
    }
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
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

    if (error && *error != QLandmarkManager::NoError) {
        if (*error == QLandmarkManager::CancelError) {
            if (errorString)
                *errorString = "Export of lmx file was canceled";
        }
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
        if (error)
            *error = lmxHandler.errorCode();
        if (errorString)    
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
        if (error)
            *error = gpxHandler.error();
        if (errorString)
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
    if (error)
        *error = QLandmarkManager::NoError;
    if (errorString)
        *errorString = "";
    QMap<QString,QVariant> bindValues;
    QString q0 = QString("select ?removable {?u a slo:LandmarkCategory ; slo:isRemovable ?removable . "
         "FILTER regex( ?u, '^%1$') }").arg(categoryId.localId());
    QSparqlResult *queryResult = executeQuery(m_conn, q0,bindValues,error,errorString);

    if (queryResult->hasError()) {
        delete queryResult;
        return false;
    }
    if (!queryResult->next()) {
        delete queryResult;
        return false;
    }
    if((queryResult->value(0).toString().compare("true", Qt::CaseSensitive)) == 0) {
        delete queryResult;
        return false;
    } else {
        delete queryResult;
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
                QList<QLandmarkId> addedIds;
                QList<QLandmarkId> changedIds;
                databaseOperations.saveLandmarks(&lms, &errorMap, &error, &errorString, &addedIds, &changedIds);
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
                QList<QLandmarkId> removedIds;
                databaseOperations.removeLandmarks(lmIds, &errorMap, &error, &errorString, &removedIds);
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
                QList<QLandmarkCategoryId> addedIds;
                QList<QLandmarkCategoryId> changedIds;
                databaseOperations.saveCategories( &categories, &errorMap, &error, &errorString, &addedIds, &changedIds);
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
                    QList<QLandmarkCategoryId> removedIds;
                    databaseOperations.removeCategories(categoryIds,
                                                        &errorMap, &error,
                                                        &errorString, &removedIds);
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
                   QList<QLandmarkId> addedLandmarkIds;
                   QList<QLandmarkCategoryId> addedCategoryIds;
                   databaseOperations.importLandmarks(device,
                                                      format, transferOption,
                                                      categoryId,
                                                      &error, &errorString,
                                                      &addedLandmarkIds, &addedCategoryIds,
                                                      this, &landmarkIds);
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
