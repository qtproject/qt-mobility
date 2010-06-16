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

    if ( m_reader->name() != "CalculateRoute" && m_reader->name() != "GetRoute" )  {
        m_reader->raiseError(QString("The root element is expected to have the name \"CalculateRoute\" or \"GetRoute\" (root element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        if (m_reader->name() != "Response") {
            m_reader->raiseError(QString("Expected a element named \"Response\" (element was named \"%1\").").arg(m_reader->name().toString()));
            return false;
        }
    }

    while (m_reader->readNextStartElement()) {
        if (m_reader->name() == "MetaInfo") {
            m_reader->skipCurrentElement();
        }
        else if (m_reader->name() == "Route") {
            QGeoRoute route;
            route.setRequest(m_request);

            if (!parseRoute(&route))
                continue; //route parsing failed move on to the next

            m_results.append(route);
        }
        else if (m_reader->name() == "Progress") {
            //TODO: updated route progress
            m_reader->skipCurrentElement();
        }
        else {
            m_reader->raiseError(QString("Did not expect a child element named \"%1\".").arg(
                m_reader->name().toString()));
            return false;
        }
    }

    return true;
}

bool QGeoRouteXmlParser::parseRoute(QGeoRoute *route)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Route");
    instructions.clear();
    segments.clear();

    m_reader->readNext();
    bool succeeded = true;
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Route")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement && succeeded) {
            if (m_reader->name() == "RouteId") {
                route->setRouteId(m_reader->readElementText());
            }
            else if (m_reader->name() == "Waypoint") {
                succeeded = parseWaypoint(route);
            }
            else if (m_reader->name() == "Mode") {
                succeeded = parseMode(route);
            }
            else if (m_reader->name() == "Shape") {
                QString elementName = m_reader->name().toString();
                QList<QGeoCoordinate> path;
                succeeded = parseGeoPoints(m_reader->readElementText(), &path, elementName);
                if(succeeded)
                    route->setPath(path);
            }
            else if (m_reader->name() == "BoundingBox") {
                QGeoBoundingBox bounds;
                succeeded = parseBoundingBox(bounds);
                if(succeeded)
                    route->setBounds(bounds);
            }
            else if (m_reader->name() == "Maneuver") {
                succeeded = parseManeuver();
            }
            else if (m_reader->name() == "Link") {
                succeeded = parseLink();
            }
            else if (m_reader->name() == "Summary") {
                succeeded = parseSummary(route);
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    if(succeeded)
        succeeded = postProcessRoute(route);

    return succeeded;
}

bool QGeoRouteXmlParser::postProcessRoute(QGeoRoute *route)
{
    QList<QGeoRouteSegment*> routesegments;

    for (int i=0;i<segments.length();++i) {
        QGeoRouteSegment* segment = segments[i].segment;
        for (int j=0;j<instructions.length();++j) {
            if(instructions[j].id==segments[i].instructionId) {
                segment->setInstruction(instructions[j].instruction);
                break;
                }
        }
        routesegments.append(segment);
    }

    for (int i=0;i<instructions.length();++i) {
        QGeoNavigationInstruction* instruction = instructions[i].instruction;
        bool found=false;
        for (int j=0;j<segments.length();++j) {
            if(instructions[i].toId==segments[j].id) {
                QGeoRouteSegment* segment=segments[j].segment;
                if (segment->instruction()) {
                    if (segment->instruction() != instruction) {
                        QGeoRouteSegment* segment = new QGeoRouteSegment();
                        segment->setInstruction(instruction);
                        routesegments.append(segment);
                    }
                }
                else {
                    segment->setInstruction(instruction);
                }
                found=true;
                break;
            }
        }
        if (!found) {
            if(instructions[i].toId != 0) {
                QGeoRouteSegment* segment = new QGeoRouteSegment();
                segment->setInstruction(instruction);
                routesegments.append(segment);
            }
            else {
                bool found = false;
                for (int j=0;j<segments.length();++j) {
                    if (segments[j].instructionId == instructions[i].id) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    //Add orphan instruction into new empty segment
                    QGeoRouteSegment* segment = new QGeoRouteSegment();
                    segment->setInstruction(instruction);
                    routesegments.append(segment);
                }
            }
        }

    }

    for (int i = 0; i < routesegments.length(); ++i) {
        route->appendRouteSegment(routesegments[i]);
    }

    instructions.clear();
    segments.clear();
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
            else {
                m_reader->skipCurrentElement();
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
                QString value = m_reader->readElementText();
                if (value == "directDrive") // TODO: check difference to "shortest" once data available
                    route->setOptimization(QGeoRouteRequest::ShortestRoute);
                else if (value == "fastestNow")
                    route->setOptimization(QGeoRouteRequest::FastestRoute);
                else if (value == "economic")
                    route->setOptimization(QGeoRouteRequest::MostEconomicRoute);
                else if (value == "scenic")
                    route->setOptimization(QGeoRouteRequest::MostScenicRoute);
                else // unsupported optimization
                    return false;
            }
            else if (m_reader->name() == "TransportModes") {
                QString value = m_reader->readElementText();
                if (value == "car")
                    route->setTravelMode(QGeoRouteRequest::CarTravel);
                else if (value == "pedestrian")
                    route->setTravelMode(QGeoRouteRequest::PedestrianTravel);
                else if (value == "publicTransport")
                    route->setTravelMode(QGeoRouteRequest::PublicTransitTravel);
                else if (value == "bicycle")
                    route->setTravelMode(QGeoRouteRequest::BicycleTravel);
                else if (value == "truck")
                    route->setTravelMode(QGeoRouteRequest::TruckTravel);
                else // unsupported optimization
                    return false;
            }
            else {
                m_reader->skipCurrentElement();
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
                QString value = m_reader->readElementText();
                route->setDistance(QGeoDistance(value.toDouble()));
            }
            else if (m_reader->name() == "TrafficTime") {
                QString value = m_reader->readElementText();
                route->setTravelTime(value.toDouble());
            }
            else {
                m_reader->skipCurrentElement();
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
            QString value = m_reader->readElementText();
            if(name == "Latitude")
                coord.setLatitude(value.toDouble());
            else if(name == "Longitude")
                coord.setLongitude(value.toDouble());
        }
        m_reader->readNext();
    }

    return true;
}

bool QGeoRouteXmlParser::parseManeuver()
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Maneuver");

    if (!m_reader->attributes().hasAttribute("id")) {
        m_reader->raiseError("The element \"Maneuver\" did not have the required attribute \"id\".");
        return false;
    }
    QGeoNavigationInstructionContainer instructionContainer;
    QGeoNavigationInstruction* instruction = new QGeoNavigationInstruction();

    instructionContainer.id = m_reader->attributes().value("id").toString();
    instructionContainer.instruction = instruction;

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Maneuver")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "Position") {
                QGeoCoordinate coordinates;
                if(parseCoordinates(coordinates))
                    instruction->setPosition(coordinates);
            }
            else if (m_reader->name() == "Instruction") {
                instruction->setInstructionText(m_reader->readElementText());
            }
            else if (m_reader->name() == "ToLink") {
                instructionContainer.toId = m_reader->readElementText();
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    instructions.append(instructionContainer);
    return true;
}

bool QGeoRouteXmlParser::parseLink()
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "Link");
    m_reader->readNext();

    QGeoRouteSegmentContainer segmentContainer;
    segmentContainer.segment = new QGeoRouteSegment();

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "Link")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "LinkId") {
                segmentContainer.id = m_reader->readElementText();
            }
            else if (m_reader->name() == "Shape") {
                QString elementName = m_reader->name().toString();
                QList<QGeoCoordinate> path;
                parseGeoPoints(m_reader->readElementText(), &path, elementName);
                segmentContainer.segment->setPath(path);
            }
            else if (m_reader->name() == "Length") {

                segmentContainer.segment->setDistance(QGeoDistance(m_reader->readElementText().toDouble()));
            }
            else if (m_reader->name() == "NextLink") {
                segmentContainer.nextId = m_reader->readElementText();
            }
            else if (m_reader->name() == "Maneuver") {
                segmentContainer.instructionId = m_reader->readElementText();
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }
    segments.append(segmentContainer);
    return true;
}

bool QGeoRouteXmlParser::parseGeoPoints(const QString& strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName)
{
    QStringList rawPoints = strPoints.split(' ');

    for (int i = 0; i < rawPoints.length(); ++i) {
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

bool QGeoRouteXmlParser::parseBoundingBox(QGeoBoundingBox &bounds)
{
    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "BoundingBox");

    QGeoCoordinate tl;
    QGeoCoordinate br;

    m_reader->readNext();
    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "BoundingBox")) {
        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {
            if (m_reader->name() == "TopLeft") {
                QGeoCoordinate coordinates;
                if(parseCoordinates(coordinates))
                    tl = coordinates;
            }
            else if (m_reader->name() == "BottomRight") {
                QGeoCoordinate coordinates;
                if(parseCoordinates(coordinates))
                    br = coordinates;
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    if(tl.isValid() && br.isValid()) {
        bounds = QGeoBoundingBox(tl, br);
        return true;
    }

    return false;
}
