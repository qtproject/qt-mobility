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

#include "qgeoroutexmlparser_p.h"

#include <QGeoNavigationInstruction>
#include <QGeoDistance>
#include <QXmlStreamReader>
#include <QIODevice>
#include <QStringList>
#include <QString>

QGeoRouteXmlParser::QGeoRouteXmlParser(const QGeoRouteRequest &request)
        : m_request(request),
        m_reader(0)
{
}

QGeoRouteXmlParser::~QGeoRouteXmlParser()
{
    if (m_reader)
        delete m_reader;
}

bool QGeoRouteXmlParser::parse(QIODevice* source)
{
    if (m_reader)
        delete m_reader;
    m_reader = new QXmlStreamReader(source);

    if (!parseRootElement()) {
        m_errorString = m_reader->errorString();
        return false;
    }

    m_errorString = "";

    return true;
}

QList<QGeoRoute> QGeoRouteXmlParser::results() const
{
    return m_results;
}

QString QGeoRouteXmlParser::errorString() const
{
    return m_errorString;
}

bool QGeoRouteXmlParser::parseRootElement()
{
    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("Expected a root element named \"CalculateRoute\" (no root element found).");
        return false;
    }

    if (m_reader->name() != "CalculateRoute") {
        m_reader->raiseError(QString("The root element is expected to have the name \"CalculateRoute\" (root element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        if (m_reader->name() != "Response") {
            m_reader->raiseError(QString("Expected a element named \"Response\" (element was named \"%1\").").arg(m_reader->name().toString()));
            return false;
        }
    }

    if (m_reader->readNextStartElement()) {
        if (m_reader->name() != "MetaInfo") {
            m_reader->raiseError(QString(
                "Expected a element named \"MetaInfo\" (element was named \"%1\").").arg(
                m_reader->name().toString()));
            return false;
        }
        else {
            while (!(m_reader->tokenType() == QXmlStreamReader::EndElement
                     && m_reader->name() == "MetaInfo")) {
                if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
                    if (m_reader->name() == "RequestId") {
                        // TODO:  MetaInfo RequestId
                    }
                    else if (m_reader->name() == "Timestamp") {
                        // TODO:  MetaInfo Timestamp
                    }
                }
                m_reader->readNext();
            }
        }
    }

    while (m_reader->readNextStartElement()) {
        QString str = m_reader->name().toString();
        if (m_reader->name() == "Route") {
            QGeoRoute route;
            route.setRequest(m_request);

            if (!parseRoute(&route))
                return false;

            m_results.append(route);
        } else {
            m_reader->raiseError(QString("Did not expect a child element named \"%1\".").arg(m_reader->name().toString()));
            return false;
        }
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("A single root element named \"routes\" was expected (second root element was named \"%1\")").arg(m_reader->name().toString()));
        return false;
    }

    return true;
}

bool QGeoRouteXmlParser::parseRoute(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Route");
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Route")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "RouteId") {
                m_reader->readNext();
                if(m_reader->tokenType() == QXmlStreamReader::Characters)
                    route->setRouteId(m_reader->text().toString());
            }
            else if (m_reader->name() == "Waypoint") {
                if(!parseWaypoint(route))
                    return false;
            }
            else if (m_reader->name() == "Mode") {
                bool success = parseMode(route); //TODO: Check return
            }
            else if (m_reader->name() == "Shape") {
                // TODO:  Route Shape
            }
            else if (m_reader->name() == "Maneuver") {
                // TODO:  Route Maneuver
            }
            else if (m_reader->name() == "Link") {
                // TODO:  Route Link
            }
            else if (m_reader->name() == "Summary") {
                if(!parseSummary(route))
                    return false;
            }
        }
        m_reader->readNext();
    }

    return true;
}

bool QGeoRouteXmlParser::parseWaypoint(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Waypoint");
    m_reader->readNext();
    QList<QGeoCoordinate> path(route->pathSummary());

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Waypoint")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "MappedPosition") {
                QGeoCoordinate coordinates;
                if(!parseCoordinates(coordinates))
                    return false;
                path.append(coordinates);
            }
            else if (m_reader->name() == "OriginalPosition") {
            // TODO: Waypoint OriginalPosition
                QGeoCoordinate coordinates;
                parseCoordinates(coordinates);
            }
            else {
                return false;
            }
        }
        m_reader->readNext();
    }
    route->setPathSummary(path);
    return true;
}

bool QGeoRouteXmlParser::parseMode(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Mode");
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Mode")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Type") {
                m_reader->readNext();
                if (m_reader->tokenType() == QXmlStreamReader::Characters) {
                    QString value = m_reader->text().toString();
                    if(value=="shortest")
                        route->setOptimization(QGeoRouteRequest::ShortestRoute);
                    else if(value=="fastestNow")
                        route->setOptimization(QGeoRouteRequest::FastestRoute);
                    else if(value=="economic")
                        route->setOptimization(QGeoRouteRequest::MostEconomicRoute);
                    else if(value=="scenic")
                        route->setOptimization(QGeoRouteRequest::MostScenicRoute);
                    else if(value=="directDrive")
                        route->setOptimization(QGeoRouteRequest::DirectRoute);
                    else
                        return false;
                }
            }
            else if (m_reader->name() == "TransportModes") {
                m_reader->readNext();
                if (m_reader->tokenType() == QXmlStreamReader::Characters) {
                    QString value = m_reader->text().toString();
                    if(value=="car")
                        route->setTravelMode(QGeoRouteRequest::CarTravel);
                    else if(value=="pedestrian")
                        route->setTravelMode(QGeoRouteRequest::PedestrianTravel);
                    else if(value=="publicTransport")
                        route->setTravelMode(QGeoRouteRequest::PublicTransitTravel);
                    else if(value=="bicycle")
                        route->setTravelMode(QGeoRouteRequest::BicycleTravel);
                    else if(value=="truck")
                        route->setTravelMode(QGeoRouteRequest::TruckTravel);
                    else
                        return false;
                }
            }
        }
        m_reader->readNext();
    }
    return true;
}

bool QGeoRouteXmlParser::parseSummary(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Summary");
    m_reader->readNext();
    QList<QGeoCoordinate> path(route->pathSummary());

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Summary")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Distance") {
                m_reader->readNext();
                if (m_reader->tokenType() == QXmlStreamReader::Characters) {
                    QGeoDistance dist(m_reader->text().toString().toDouble());
                    route->setDistance(dist);
                }
            }
            else if (m_reader->name() == "TrafficTime") {
                m_reader->readNext();
                if (m_reader->tokenType() == QXmlStreamReader::Characters) {
                    route->setTravelTime(m_reader->text().toString().toInt());
                }
            }
            else if (m_reader->name() == "BaseTime") {
            // TODO: Summary BaseTime
            }
            else {
                return false;
            }
        }
        m_reader->readNext();
    }
    route->setPathSummary(path);
    return true;
}

bool QGeoRouteXmlParser::parseCoordinates(QGeoCoordinate &coord)
{
    QString currentElement = m_reader->name().toString();
    m_reader->readNext();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == currentElement)) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            QString name = m_reader->name().toString();
            m_reader->readNext();
            if (m_reader->tokenType() == QXmlStreamReader::Characters) {
                QString value = m_reader->text().toString();
                if(name == "Latitude")
                    coord.setLatitude(value.toDouble());
                else if(name == "Longitude")
                    coord.setLongitude(value.toDouble());
                else
                    return false;
            }
        }
        m_reader->readNext();
    }
    return true;
}

/*
bool QGeoRouteXmlParser::parseXsdDateTime(const QString& strDateTime, QDateTime *dateTime, const QString &attributeName)
{
    QTime utcDiff(0, 0, 0, 0);
    QString dt;
    int milliseconds = 0;
    int change = 0;

    QString errorString = QString("The attribute \"%1\" has a bady formatted xsd:datetime string (string is \"%2\")").arg(attributeName).arg(strDateTime);

    if (!strDateTime.endsWith("Z")) {
        int plusIndex = strDateTime.lastIndexOf("+");
        int minusIndex = strDateTime.lastIndexOf("-");

        if ((plusIndex == -1) && (minusIndex == -1)) {
            m_reader->raiseError(errorString);
            return false;
        }

        if ((plusIndex != -1) && (minusIndex != -1)) {
            m_reader->raiseError(errorString);
            return false;
        }

        QString diff;
        int index = 0;

        if (plusIndex != -1) {
            index = plusIndex;
            change = 1;
        }

        if (minusIndex != -1) {
            index = minusIndex;
            change = -1;
        }

        dt = strDateTime.left(plusIndex);
        diff = strDateTime.right(strDateTime.length() - plusIndex - 1);

        // deal with milliseconds in utc offset datetime string here
        int dotIndex = diff.indexOf(".");
        if (dotIndex != -1) {
            QString fraction = diff.right(diff.length() - dotIndex);
            fraction.insert(0, '0');

            bool ok = false;
            milliseconds = qRound(1000.0 * fraction.toDouble(&ok));

            if (!ok) {
                m_reader->raiseError(errorString);
                return false;
            }
        }

        utcDiff = QTime::fromString(diff, "hh::mm::ss");

        if (!utcDiff.isValid()) {
            m_reader->raiseError(errorString);
            return false;
        }

        utcDiff.addMSecs(milliseconds);
    } else {
        dt = strDateTime.left(strDateTime.length() - 1);
    }

    QTime zeroTime(0, 0, 0, 0);
    milliseconds = change * zeroTime.msecsTo(utcDiff);

    // deal with milliseconds in main datetime string here
    int dotIndex = dt.indexOf(".");
    if (dotIndex != -1) {
        QString fraction = dt.right(dt.length() - dotIndex);
        fraction.insert(0, '0');

        bool ok = false;
        milliseconds += qRound(1000.0 * fraction.toDouble(&ok));

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }
    }

    *dateTime = QDateTime::fromString(dt, "yyyy-MM-ddThh:mm:ss");

    if (!dateTime->isValid()) {
        m_reader->raiseError(errorString);
        return false;
    }

    dateTime->addMSecs(milliseconds);
    dateTime->setTimeSpec(Qt::UTC);

    return true;
}
*/

bool QGeoRouteXmlParser::parseXsdDuration(const QString& strDuration, qint32 *durationSeconds, const QString &attributeName)
{
    QString dur = strDuration;
    QString errorString = QString("The attribute \"%1\" has a bady formatted xsd:duration string (string is \"%2\")").arg(attributeName).arg(strDuration);

    bool ok = false;
    int years = 0;
    int months = 0;
    int days = 0;
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    int pIndex = dur.indexOf("P");
    if (pIndex != 0) {
        m_reader->raiseError(errorString);
        return false;
    }

    dur.remove(0, 1);

    int yearIndex = dur.indexOf("Y");

    if (yearIndex != -1) {
        years = dur.left(yearIndex).toUInt(&ok);

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }

        dur.remove(0, yearIndex + 1);
    }

    int monthIndex = dur.indexOf("M");

    if (monthIndex != -1) {
        months = dur.left(monthIndex).toUInt(&ok);

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }

        dur.remove(0, monthIndex + 1);
    }

    int dayIndex = dur.indexOf("D");

    if (dayIndex != -1) {
        days = dur.left(dayIndex).toUInt(&ok);

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }

        dur.remove(0, dayIndex + 1);
    }

    double doubleDays = years * 365.25 ;
    doubleDays += (months * 365.25) / 12.0;
    doubleDays += days * 1.0;

    if (dur.length() == 0) {
        *durationSeconds = qRound(doubleDays * 24 * 60 * 60);
        return true;
    }

    int timeIndex = dur.indexOf("T");

    if (timeIndex != 0) {
        m_reader->raiseError(errorString);
        return false;
    }

    dur.remove(0, 1);

    int hourIndex = dur.indexOf("H");

    if (hourIndex != -1) {
        hours = dur.left(hourIndex).toUInt(&ok);

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }

        dur.remove(0, hourIndex + 1);
    }

    int minuteIndex = dur.indexOf("M");

    if (minuteIndex != -1) {
        minutes = dur.left(minuteIndex).toUInt(&ok);

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }

        dur.remove(0, minuteIndex + 1);
    }

    int secondIndex = dur.indexOf("S");

    if (secondIndex != -1) {
        seconds = dur.left(secondIndex).toUInt(&ok);

        if (!ok) {
            m_reader->raiseError(errorString);
            return false;
        }

        dur.remove(0, secondIndex + 1);
    }

    if (dur.length() != 0) {
        m_reader->raiseError(errorString);
        return false;
    }

    minutes += 60 * hours;
    seconds += 60 * minutes ;
    seconds += qRound(doubleDays * 24 * 60 * 60);

    *durationSeconds = seconds;

    return true;
}

bool QGeoRouteXmlParser::parseManeuver(QGeoRoute *route, QGeoRouteSegment *segment)
{
    /*
    <xsd:complexType name="Maneuver">
        <xsd:sequence>
            <xsd:element minOccurs="0" maxOccurs="1" name="wayPoints" type="rt:GeoPoints"/>
            <xsd:element minOccurs="1" maxOccurs="1" name="maneuverPoints" type="rt:GeoPoints"/>
        </xsd:sequence>
        <xsd:attribute name="description" type="xsd:string" use="required"/>
        <xsd:attribute name="action" type="xsd:string" use="required"/>
        <xsd:attribute name="distance" type="xsd:nonNegativeInteger" use="required"/>
        <xsd:attribute name="duration" type="xsd:duration" use="required"/>
        <xsd:attribute name="turn" type="xsd:string"/>
        <xsd:attribute name="streetName" type="xsd:string"/>
        <xsd:attribute name="routeName" type="xsd:string"/>
        <xsd:attribute name="nextStreetName" type="xsd:string"/>
        <xsd:attribute name="signPost" type="xsd:string"/>
        <xsd:attribute name="trafficDirection" type="xsd:long"/>
        <xsd:attribute name="icon" type="xsd:long"/>
    </xsd:complexType>

    <xsd:simpleType name="GeoPoints">
        <xsd:list itemType="rt:GeoPoint"/>
    </xsd:simpleType>

    <xsd:simpleType name="GeoPoint">
        <xsd:restriction base="xsd:string">
            <!--
                 Pattern for [-90.0,90.0]","[-180.0,180.0] (Lat/Long as String as there is no XSD alternative this short)
            -->
            <xsd:pattern value="^-?((90(\.0{1,})?)|(([1-8]\d)|(\d))(\.\d+)?),-?((180(\.0{1,})?)|((1[0-7]\d)|(\d{1,2}))(\.\d+)?)$"/>
        </xsd:restriction>
    </xsd:simpleType>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "maneuver");

    if (!m_reader->attributes().hasAttribute("description")) {
        m_reader->raiseError("The element \"maneuver\" did not have the required attribute \"description\".");
        return false;
    } else {
        QGeoNavigationInstruction *instruction = new QGeoNavigationInstruction();
        instruction->setInstructionText(m_reader->attributes().value("description").toString());
        segment->setInstruction(instruction);
    }

    /* // currently unused, might be used in a qgeoroutesegment subclass later
    if (!m_reader->attributes().hasAttribute("action")) {
        m_reader->raiseError("The element \"maneuver\" did not have the required attribute \"action\".");
        return false;
    } else {
        maneuver->setAction(m_reader->attributes().value("action").toString());
    }
    */

    if (!m_reader->attributes().hasAttribute("distance")) {
        m_reader->raiseError("The element \"maneuver\" did not have the required attribute \"distance\".");
        return false;
    } else {
        bool ok = false;
        QString s = m_reader->attributes().value("distance").toString();
        segment->setDistance(QGeoDistance(s.toUInt(&ok), QGeoDistance::Metres));

        if (!ok) {
            m_reader->raiseError(QString("The attribute \"distance\" was expected to have a value convertable to an unsigned int (value was \"%1\")").arg(s));
            return false;
        }
    }

    if (!m_reader->attributes().hasAttribute("duration")) {
        m_reader->raiseError("The element \"maneuver\" did not have the required attribute \"duration\".");
        return false;
    } else {
        QString duration = m_reader->attributes().value("duration").toString();
        qint32 seconds;
        if (!duration.isEmpty() && !parseXsdDuration(duration, &seconds, "duration"))
            return false;
        segment->setTravelTime(seconds);
    }

    /* // currently unused, might be used in a qgeoroutesegment subclass later
    if (m_reader->attributes().hasAttribute("turn")) {
        maneuver->setTurn(m_reader->attributes().value("turn").toString());
    }

    if (m_reader->attributes().hasAttribute("streetName")) {
        maneuver->setStreetName(m_reader->attributes().value("streetName").toString());
    }

    if (m_reader->attributes().hasAttribute("routeName")) {
        maneuver->setRouteName(m_reader->attributes().value("routeName").toString());
    }

    if (m_reader->attributes().hasAttribute("nextStreetName")) {
        maneuver->setNextStreetName(m_reader->attributes().value("nextStreetName").toString());
    }

    if (m_reader->attributes().hasAttribute("signPost")) {
        maneuver->setSignPost(m_reader->attributes().value("signPost").toString());
    }
    */

    /*
     // currently broken at the services side
    if (m_reader->attributes().hasAttribute("trafficDirection")) {
        bool ok = false;
        QString s = m_reader->attributes().value("trafficDirection").toString();
        maneuver->setTrafficDirection(s.toLong(&ok));

        if (!ok) {
            m_reader->raiseError(QString("The attribute \"trafficDirection\" was expected to have a value convertable to a long integer (value was \"%1\")").arg(s));
            return false;
        }
    }
    */

    /* // not likely to be used
    if (m_reader->attributes().hasAttribute("icon")) {
        bool ok = false;
        QString s = m_reader->attributes().value("icon").toString();
        maneuver->setIcon(s.toLong(&ok));

        if (!ok) {
            m_reader->raiseError(QString("The attribute \"icon\" was expected to have a value convertable to a long integer (value was \"%1\")").arg(s));
            return false;
        }
    }
    */

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"maneuever\" was expected to have a child element named \"maneuverPoints\"");
        return false;
    }

    if (m_reader->name() == "wayPoints") {
        QList<QGeoCoordinate> wPoints;
        if (!parseGeoPoints(m_reader->readElementText(), &wPoints, "wayPoints"))
            return false;

        QList<QGeoCoordinate> pathSummary = route->pathSummary();
        pathSummary.append(wPoints);
        route->setPathSummary(pathSummary);

        if (!m_reader->readNextStartElement()) {
            m_reader->raiseError(QString("The element \"maneuever\" expected this child element to be named \"maneuverPoints\"").arg(m_reader->name().toString()));
            return false;
        }
    }

    if (m_reader->name() == "maneuverPoints") {
        QList<QGeoCoordinate> mPoints;
        if (!parseGeoPoints(m_reader->readElementText(), &mPoints, "maneuverPoints"))
            return false;

        segment->setPath(mPoints);

    } else {
        m_reader->raiseError(QString("The element \"maneuever\" was expected to have a child element named \"maneuverPoints\" (found an element named \"%1\")").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("The element \"maneuver\" did not expect the child element \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
        return false;
    }

    return true;
}

bool QGeoRouteXmlParser::parseGeoPoints(const QString& strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName)
{
    QStringList rawPoints = strPoints.split(' ');

    for (int i = 0; i < rawPoints.length(); i++) {
        QStringList coords = rawPoints[i].split(',');

        if (coords.length() != 2) {
            m_reader->raiseError(QString("Each of the space separated values of \"%1\" is expected to be a comma separated pair of coordinates (value was \"%2\")").arg(elementName).arg(rawPoints[i]));
            return false;
        }

        bool ok = false;
        QString latString = coords[0];
        double lat = latString.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The latitude portions of \"%1\" are expected to have a value convertable to a double (value was \"%2\")").arg(elementName).arg(latString));
            return false;
        }

        QString lngString = coords[1];
        double lng = lngString.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The longitude portions of \"%1\" are expected to have a value convertable to a double (value was \"%2\")").arg(elementName).arg(lngString));
            return false;
        }

        QGeoCoordinate geoPoint(lat, lng);
        geoPoints->append(geoPoint);
    }

    return true;
}

bool QGeoRouteXmlParser::parseBoundingBox(QGeoBoundingBox *bounds)
{
    /*
    <xsd:complexType name="GeoBox">
        <xsd:sequence>
            <xsd:element name="northWest" type="gc:GeoCoord"/>
            <xsd:element name="southEast" type="gc:GeoCoord"/>
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "boundingBox");

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"boundingBox\" was expected to have 2 child elements (0 found)");
        return false;
    }

    QGeoCoordinate nw;

    if (m_reader->name() == "northWest") {
        if (!parseCoordinate(&nw, "northWest"))
            return false;
    } else {
        m_reader->raiseError(QString("The element \"boundingBox\" expected this child element to be named \"northWest\" (found an element named \"%1\")").arg(m_reader->name().toString()));
        return false;
    }

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"boundingBox\" was expected to have 2 child elements (1 found)");
        return false;
    }

    QGeoCoordinate se;

    if (m_reader->name() == "southEast") {
        if (!parseCoordinate(&se, "southEast"))
            return false;
    } else {
        m_reader->raiseError(QString("The element \"boundingBox\" expected this child element to be named \"southEast\" (found an element named \"%1\")").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"boundingBox\" was expected to have 2 child elements (more than 2 found)");
        return false;
    }

    *bounds = QGeoBoundingBox(nw, se);

    return true;
}

bool QGeoRouteXmlParser::parseCoordinate(QGeoCoordinate *coordinate, const QString &elementName)
{
    /*
    <xsd:complexType name="GeoCoord">
        <xsd:sequence>
            <xsd:element name="latitude" type="gc:Latitude"/>
            <xsd:element name="longitude" type="gc:Longitude"/>
        </xsd:sequence>
    </xsd:complexType>

    <xsd:simpleType name="Latitude">
        <xsd:restriction base="xsd:float">
            <xsd:minInclusive value="-90.0"/>
            <xsd:maxInclusive value="90.0"/>
        </xsd:restriction>
    </xsd:simpleType>

    <xsd:simpleType name="Longitude">
        <xsd:restriction base="xsd:float">
            <xsd:minInclusive value="-180.0"/>
            <xsd:maxInclusive value="180.0"/>
        </xsd:restriction>
    </xsd:simpleType>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == elementName);

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("The element \"%1\" was expected to have 2 child elements (0 found)").arg(elementName));
        return false;
    }

    if (m_reader->name() == "latitude") {
        bool ok = false;
        QString s = m_reader->readElementText();
        double lat = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"latitude\" expected a value convertable to type float (value was \"%1\")").arg(s));
            return false;
        }

        if (lat < -90.0 || 90.0 < lat) {
            m_reader->raiseError(QString("The element \"latitude\" expected a value between -90.0 and 90.0 inclusive (value was %1)").arg(lat));
            return false;
        }

        coordinate->setLatitude(lat);
    } else {
        m_reader->raiseError(QString("The element \"%1\" expected this child element to be named \"latitude\" (found an element named \"%2\")").arg(elementName).arg(m_reader->name().toString()));
        return false;
    }

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("The element \"%1\" was expected to have 2 child elements (1 found)").arg(elementName));
        return false;
    }

    if (m_reader->name() == "longitude") {
        bool ok = false;
        QString s = m_reader->readElementText();
        double lng = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"longitude\" expected a value convertable to type float (value was \"%1\")").arg(s));
            return false;
        }

        if (lng < -180.0 || 180.0 < lng) {
            m_reader->raiseError(QString("The element \"longitude\" expected a value between -180.0 and 180.0 inclusive (value was %1)").arg(lng));
            return false;
        }

        coordinate->setLongitude(lng);
    } else {
        m_reader->raiseError(QString("The element \"%1\" expected this child element to be named \"longitude\" (found an element named \"%2\")").arg(elementName).arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("The element \"%1\" was expected to have 2 child elements (more than 2 found)").arg(elementName));
        return false;
    }

    return true;
}

