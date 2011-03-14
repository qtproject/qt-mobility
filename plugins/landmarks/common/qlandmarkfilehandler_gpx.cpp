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
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
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
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qlandmarkfilehandler_gpx_p.h"

#include <qlandmarkmanagerengine.h>
#include <qlandmarkcategory.h>
#include <qlandmarkcategoryid.h>
#include <qgeocoordinate.h>

#include <math.h>

#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <qnumeric.h>
#include <QCoreApplication>

#include <QDebug>

QTM_USE_NAMESPACE

QLandmarkFileHandlerGpx::QLandmarkFileHandlerGpx(const volatile bool *cancel)
    : QObject(),
    m_reader(0),
    m_writer(0),
    m_behavior(QLandmarkFileHandlerGpx::ExportSubset),
    m_cancel(cancel)
{
}

QLandmarkFileHandlerGpx::~QLandmarkFileHandlerGpx()
{
    if (m_reader != 0)
        delete m_reader;
    if (m_writer != 0)
        delete m_writer;
}

QList<QLandmark> QLandmarkFileHandlerGpx::waypoints() const
{
    return m_waypoints;
}

void QLandmarkFileHandlerGpx::setWaypoints(const QList<QLandmark> &waypoints)
{
    m_waypoints = waypoints;
}

QList<QList<QLandmark> > QLandmarkFileHandlerGpx::tracks() const
{
    return m_tracks;
}

void QLandmarkFileHandlerGpx::setTracks(const QList<QList<QLandmark> > &tracks)
{
    m_tracks = tracks;
}

QList<QList<QLandmark> > QLandmarkFileHandlerGpx::routes() const
{
    return m_routes;
}

void QLandmarkFileHandlerGpx::setRoutes(const QList<QList<QLandmark> > &routes)
{
    m_routes = routes;
}

bool QLandmarkFileHandlerGpx::importData(QIODevice *device)
{
    // rejects GPX 1.0, need add to docs that 1.1 is the only version supported

    if (m_reader)
        delete m_reader;

    m_reader = new QXmlStreamReader(device);

    if (!readGpx()) {
        if (m_errorCode != QLandmarkManager::CancelError) {
            m_errorCode = QLandmarkManager::ParsingError;
            m_errorString = m_reader->errorString();
        }
        return false;
    } else {
        if(m_reader->atEnd()) {
            m_reader->readNextStartElement();
            if (!m_reader->name().isEmpty()) {
                m_errorString = QString("A single root element named \"gpx\" was expected (second root element was named \"%1\")").arg(m_reader->name().toString());
                m_errorCode = QLandmarkManager::ParsingError;
                return false;
            }
        }
    }

    m_errorCode = QLandmarkManager::NoError;
    m_errorString = "";
    return true;
}

bool QLandmarkFileHandlerGpx::readGpx()
{
    /*
    <xsd:complexType name="gpxType">
        <xsd:sequence>
             <xsd:element name="metadata" type="metadataType" minOccurs="0" />
             <xsd:element name="wpt" type="wptType" minOccurs="0" maxOccurs="unbounded" />
             <xsd:element name="rte" type="rteType" minOccurs="0" maxOccurs="unbounded" />
             <xsd:element name="trk" type="trkType"	minOccurs="0" maxOccurs="unbounded" />
             <xsd:element name="extensions" type="extensionsType" minOccurs="0" />
        </xsd:sequence>
        <xsd:attribute name="version" type="xsd:string" use="required" fixed="1.1" />
        <xsd:attribute name="creator" type="xsd:string" use="required" />
    </xsd:complexType>
    */

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("Expected a root element named \"gpx\" (no root element found).");
        return false;
    }

    if (m_reader->name() != "gpx") {
        m_reader->raiseError(QString("The root element is expected to have the name \"gpx\" (root element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->attributes().hasAttribute("version")) {
        QString version = m_reader->attributes().value("version").toString();
        if (version != "1.1") {
            m_reader->raiseError(QString("Only version 1.1. of the GPX schema is supported (version found was \"%1\").").arg(version));
            return false;
        }
    } else {
        m_reader->raiseError("The element \"gpx\" did not have the required attribute \"version\".");
        return false;
    }

    if (m_reader->attributes().hasAttribute("creator")) {
        // Not used outside of schema compliance check
    } else {
        m_reader->raiseError("The element \"gpx\" did not have the required attribute \"creator\".");
        return false;
    }

    if (!m_reader->readNextStartElement()) {
        m_reader->skipCurrentElement();
        return true;
    }

    if (m_reader->name() == "metadata") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement()) {
            m_reader->skipCurrentElement();
            return true;
        }
    }

    while (m_reader->name() == "wpt") {
        if (m_cancel && (*m_cancel==true)) {
            m_errorCode = QLandmarkManager::CancelError;
            m_errorString = "Import of gpx file was canceled";
            return false;
        }

        QLandmark landmark;

        if (!readWaypoint(landmark, "wpt")) {
            return false;
        }

        m_waypoints.append(landmark);

        if(!m_reader->readNextStartElement()) {
            m_reader->skipCurrentElement();
            return true;
        }
    }

    while (m_reader->name() == "rte") {
        if (m_cancel && (*m_cancel==true)) {
            m_errorCode = QLandmarkManager::CancelError;
            m_errorString = "Import of gpx file was canceled";
            return false;
        }

        QList<QLandmark> route;
        if (!readRoute(route))
            return false;

        m_routes.append(route);

        if(!m_reader->readNextStartElement()) {
            m_reader->skipCurrentElement();
            return true;
        }
    }

    while (m_reader->name() == "trk") {
        if (m_cancel && (*m_cancel==true)) {
            m_errorCode = QLandmarkManager::CancelError;
            m_errorString = "Import of gpx file was canceled";
            return false;
        }

        QList<QLandmark> track;
        if (!readTrack(track))
            return false;

        m_tracks.append(track);

        if(!m_reader->readNextStartElement()) {
            m_reader->skipCurrentElement();
            return true;
        }
    }

    if (m_reader->name() == "extensions") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement()) {
            m_reader->skipCurrentElement();
            return true;
        }
    }

    m_reader->raiseError(QString("The element \"gpx\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerGpx::readWaypoint(QLandmark &landmark, const QString &elementName)
{
    /*
    <xsd:complexType name="wptType">
        <xsd:sequence>
            <xsd:element name="ele" type="xsd:decimal" minOccurs="0" />
            <xsd:element name="time" type="xsd:dateTime" minOccurs="0" />
            <xsd:element name="magvar" type="degreesType" minOccurs="0" />
            <xsd:element name="geoidheight" type="xsd:decimal" minOccurs="0" />
            <xsd:element name="name" type="xsd:string" minOccurs="0" />
            <xsd:element name="cmt" type="xsd:string" minOccurs="0" />
            <xsd:element name="desc" type="xsd:string" minOccurs="0" />
            <xsd:element name="src" type="xsd:string" minOccurs="0" />
            <xsd:element name="link" type="linkType" minOccurs="0" maxOccurs="unbounded" />
            <xsd:element name="sym" type="xsd:string" minOccurs="0" />
            <xsd:element name="type" type="xsd:string" minOccurs="0" />
            <xsd:element name="fix" type="fixType" minOccurs="0" />
            <xsd:element name="sat" type="xsd:nonNegativeInteger" minOccurs="0" />
            <xsd:element name="hdop" type="xsd:decimal" minOccurs="0" />
            <xsd:element name="vdop" type="xsd:decimal" minOccurs="0" />
            <xsd:element name="pdop" type="xsd:decimal" minOccurs="0" />
            <xsd:element name="ageofdgpsdata" type="xsd:decimal" minOccurs="0" />
            <xsd:element name="dgpsid" type="dgpsStationType" minOccurs="0" />
            <xsd:element name="extensions" type="extensionsType" minOccurs="0" />
        </xsd:sequence>
        <xsd:attribute name="lat" type="latitudeType" use="required" />
        <xsd:attribute name="lon" type="longitudeType" use="required" />
    </xsd:complexType>

    <xsd:simpleType name="latitudeType">
        <xsd:restriction base="xsd:decimal">
            <xsd:minInclusive value="-90.0"/>
            <xsd:maxInclusive value="90.0"/>
        </xsd:restriction>
    </xsd:simpleType>

    <xsd:simpleType name="longitudeType">
        <xsd:restriction base="xsd:decimal">
            <xsd:minInclusive value="-180.0"/>
            <xsd:maxExclusive value="180.0"/>
        </xsd:restriction>
    </xsd:simpleType>
    */
    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == elementName));

    QGeoCoordinate coord;

    if (m_reader->attributes().hasAttribute("lat")) {
        bool ok = false;
        QString s = m_reader->attributes().value("lat").toString();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The attribute \"lat\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double lat = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The attribute \"lat\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        if (lat < -90.0 || 90.0 < lat) {
            m_reader->raiseError(QString("The attribute \"lat\" fell outside of the bounds -90.0 <= lat <= 90.0 (value was \"%1\").").arg(s));
            return false;
        }

        coord.setLatitude(lat);
    } else {
        m_reader->raiseError(QString("The element \"%1\" did not have the required attribute \"lat\".").arg(elementName));
        return false;
    }

    if (m_reader->attributes().hasAttribute("lon")) {
        bool ok = false;
        QString s = m_reader->attributes().value("lon").toString();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The attribute \"lon\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double lon = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The attribute \"lon\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        if (lon < -180.0 || 180.0 <= lon) {
            m_reader->raiseError(QString("The attribute \"lon\" fell outside of the bounds -180.0 <= lat < 180.0 (value was \"%1\").").arg(s));
            return false;
        }

        coord.setLongitude(lon);
    } else {
        m_reader->raiseError(QString("The element \"%1\" did not have the required attribute \"lon\".").arg(elementName));
        return false;
    }

    landmark.setCoordinate(coord);

    if (!m_reader->readNextStartElement())
        return true;

    if (m_reader->name() == "ele") {
        bool ok = false;
        QString s = m_reader->readElementText();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The element \"ele\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double alt = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"ele\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        coord.setAltitude(alt);
        landmark.setCoordinate(coord);

        if (!m_reader->readNextStartElement())
            return true;
    }

    QList<QString> unusedNames1;
    unusedNames1 << "time";
    unusedNames1 << "magvar";
    unusedNames1 << "geoidheight";

    for (int i = 0; i < unusedNames1.size(); ++i) {
        if (m_reader->name() == unusedNames1.at(i)) {
            // Not used outside of schema compliance check
            m_reader->skipCurrentElement();
            if (!m_reader->readNextStartElement())
                return true;
        }
    }

    if (m_reader->name() == "name") {
        landmark.setName(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "cmt") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "desc") {
        landmark.setDescription(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "src") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement())
            return true;
    }

    while (m_reader->name() == "link") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement())
            return true;
    }

    QList<QString> unusedNames2;
    unusedNames2 << "sym";
    unusedNames2 << "type";
    unusedNames2 << "fix";
    unusedNames2 << "sat";
    unusedNames2 << "hdop";
    unusedNames2 << "vdop";
    unusedNames2 << "pdop";
    unusedNames2 << "ageofdgpsdata";
    unusedNames2 << "dgpsid";
    unusedNames2 << "extensions";

    for (int i = 0; i < unusedNames2.size(); ++i) {
        if (m_reader->name() == unusedNames2.at(i)) {
            // Not used outside of schema compliance check
            m_reader->skipCurrentElement();
            if (!m_reader->readNextStartElement())
                return true;
        }
    }

    m_reader->raiseError(QString("The element \"%1\" did not expect a child element named \"%2\" at this point (unknown child element or child element out of order).").arg(elementName).arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerGpx::readRoute(QList<QLandmark> &route)
{
    /*
    <xsd:complexType name="rteType">
        <xsd:sequence>
            <xsd:element name="name" type="xsd:string" minOccurs="0" />
            <xsd:element name="cmt" type="xsd:string" minOccurs="0" />
            <xsd:element name="desc" type="xsd:string" minOccurs="0" />
            <xsd:element name="src" type="xsd:string" minOccurs="0" />
            <xsd:element name="link" type="linkType" minOccurs="0" maxOccurs="unbounded" />
            <xsd:element name="number" type="xsd:nonNegativeInteger" minOccurs="0" />
            <xsd:element name="type" type="xsd:string" minOccurs="0" />
            <xsd:element name="extensions" type="extensionsType" minOccurs="0" />
            <xsd:element name="rtept" type="wptType" minOccurs="0" maxOccurs="unbounded" />
        </xsd:sequence>
    </xsd:complexType>
    */
    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "rte"));

    if (!m_reader->readNextStartElement())
        return true;

    QList<QString> unusedNames1;
    unusedNames1 << "name";
    unusedNames1 << "cmt";
    unusedNames1 << "desc";
    unusedNames1 << "src";

    for (int i = 0; i < unusedNames1.size(); ++i) {
        if (m_reader->name() == unusedNames1.at(i)) {
            // Not used outside of schema compliance check
            m_reader->skipCurrentElement();
            if (!m_reader->readNextStartElement())
                return true;
        }
    }

    while (m_reader->name() == "link") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement())
            return true;
    }

    QList<QString> unusedNames2;
    unusedNames2 << "number";
    unusedNames2 << "type";
    unusedNames2 << "extensions";

    for (int i = 0; i < unusedNames2.size(); ++i) {
        if (m_reader->name() == unusedNames2.at(i)) {
            // Not used outside of schema compliance check
            m_reader->skipCurrentElement();
            if (!m_reader->readNextStartElement())
                return true;
        }
    }

    while (m_reader->name() == "rtept") {
        QLandmark landmark;

        if (!readWaypoint(landmark, "rtept"))
            return false;

        route << landmark;

        if (!m_reader->readNextStartElement())
            return true;
    }

    m_reader->raiseError(QString("The element \"rte\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerGpx::readTrack(QList<QLandmark> &track)
{
    /*
    <xsd:complexType name="trkType">
        <xsd:sequence>
            <xsd:element name="name" type="xsd:string" minOccurs="0" />
            <xsd:element name="cmt" type="xsd:string" minOccurs="0" />
            <xsd:element name="desc" type="xsd:string" minOccurs="0" />
            <xsd:element name="src" type="xsd:string" minOccurs="0" />
            <xsd:element name="link" type="linkType" minOccurs="0" maxOccurs="unbounded" />
            <xsd:element name="number" type="xsd:nonNegativeInteger" minOccurs="0" />
            <xsd:element name="type" type="xsd:string" minOccurs="0" />
            <xsd:element name="extensions" type="extensionsType" minOccurs="0" />
            <xsd:element name="trkseg" type="trksegType" minOccurs="0" maxOccurs="unbounded" />
        </xsd:sequence>
    </xsd:complexType>

    <xsd:complexType name="trksegType">
        <xsd:sequence>
            <xsd:element name="trkpt" type="wptType" minOccurs="0" maxOccurs="unbounded" />
            <xsd:element name="extensions" type="extensionsType" minOccurs="0" />
        </xsd:sequence>
    </xsd:complexType>
    */
    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "trk"));

    if (!m_reader->readNextStartElement())
        return true;

    QList<QString> unusedNames1;
    unusedNames1 << "name";
    unusedNames1 << "cmt";
    unusedNames1 << "desc";
    unusedNames1 << "src";

    for (int i = 0; i < unusedNames1.size(); ++i) {
        if (m_reader->name() == unusedNames1.at(i)) {
            // Not used outside of schema compliance check
            m_reader->skipCurrentElement();
            if (!m_reader->readNextStartElement())
                return true;
        }
    }

    while (m_reader->name() == "link") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement())
            return true;
    }

    QList<QString> unusedNames2;
    unusedNames2 << "number";
    unusedNames2 << "type";
    unusedNames2 << "extensions";

    for (int i = 0; i < unusedNames2.size(); ++i) {
        if (m_reader->name() == unusedNames2.at(i)) {
            // Not used outside of schema compliance check
            m_reader->skipCurrentElement();
            if (!m_reader->readNextStartElement())
                return true;
        }
    }

    while (m_reader->name() == "trkseg") {

        if (!readTrackSegment(track))
            return false;

        if (!m_reader->readNextStartElement())
            return true;
    }

    m_reader->raiseError(QString("The element \"trk\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}
bool QLandmarkFileHandlerGpx::readTrackSegment(QList<QLandmark> &track)
{
    /*
    <xsd:complexType name="trksegType">
        <xsd:sequence>
            <xsd:element name="trkpt" type="wptType" minOccurs="0" maxOccurs="unbounded" />
            <xsd:element name="extensions" type="extensionsType" minOccurs="0" />
        </xsd:sequence>
    </xsd:complexType>
    */
    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "trkseg"));

    if (!m_reader->readNextStartElement())
        return true;

    while (m_reader->name() == "trkpt") {
        QLandmark landmark;

        if (!readWaypoint(landmark, "trkpt"))
            return false;

        track << landmark;

        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "extensions") {
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement())
            return true;
    }

    m_reader->raiseError(QString("The element \"trkseg\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerGpx::exportData(QIODevice *device, const QString &nsPrefix)
{
    if (m_writer != 0)
        delete m_writer;

    m_writer = new QXmlStreamWriter(device);
    m_writer->setAutoFormatting(true);

    m_nsPrefix = nsPrefix;

    bool result = writeGpx();

    if (!result) {
        //errors should already have been set
        return false;
    }

    m_errorCode = QLandmarkManager::NoError;;
    m_errorString = "";
    return true;
}

bool QLandmarkFileHandlerGpx::writeGpx()
{
    QString nsGpx = "http://www.topografix.com/GPX/1/1";
    QString nsXsi = "http://www.w3.org/2001/XMLSchema-instance";

    if (!m_nsPrefix.isEmpty())
        m_ns = nsGpx;
    else
        m_ns = "";

    m_writer->writeStartDocument();

    if (m_nsPrefix.isEmpty())
        m_writer->writeDefaultNamespace(nsGpx);
    else {
        m_writer->writeNamespace(nsGpx, m_nsPrefix);
    }

    m_writer->writeStartElement(m_ns, "gpx");
    m_writer->writeAttribute("version", "1.1");
    m_writer->writeAttribute("creator", "Qt Mobility Location API");

    m_writer->writeNamespace(nsXsi, "xsi");
    m_writer->writeAttribute(nsXsi, "schemaLocation", "http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");


    for (int i = 0; i < m_waypoints.size(); ++i) {
        if (m_cancel && (*m_cancel == true)) {
            m_errorCode = QLandmarkManager::CancelError;
            m_errorString = "Export of gpx file was canceled";
            return false;
        }

       if (!writeWaypoint(m_waypoints.at(i), "wpt"))
        return false;
    }

    for (int i = 0; i < m_routes.size(); ++i) {
        if (!writeRoute(m_routes.at(i)))
            return false;
    }

    for (int i = 0; i < m_tracks.size(); ++i) {
        if (!writeTrack(m_tracks.at(i)))
            return false;
    }
    m_writer->writeEndElement();
    m_writer->writeEndDocument();
    return true;
}

bool QLandmarkFileHandlerGpx::writeWaypoint(const QLandmark &landmark, const QString &elementName)
{
    double lat = landmark.coordinate().latitude();
    double lon = landmark.coordinate().longitude();

    QString latString;
    QString lonString;
    bool isInvalid = false;

    if (!qIsNaN(lat)) {
        if ((lat > 90.0 )| (lat < -90.0))
            isInvalid = true;
        latString = QString::number(lat);
    } else {
        latString = "NaN";
        isInvalid = true;
    }

    if (!qIsNaN(lon)) {
        if ((lon > 180.0) | (lon < -180.0))
            isInvalid = true;
        lonString = QString::number(lon);
    } else {
        lonString = "NaN";
        isInvalid = true;
    }

    if (isInvalid) {
        if(m_behavior == QLandmarkFileHandlerGpx::ExportAll){
            m_errorString = QString("Landmarks cannot be exported with invalid coordinates (latitude is %1, longitude is %2)").arg(latString).arg(lonString);
            m_errorCode = QLandmarkManager::BadArgumentError; //TODO: should be invalid error code?
            return false;
        } else { //m_behavior == QLandmarkFileHandlerGpx::ExportSome
            return true;//ignore landmarks with invalid coordinates.
        }
    }

    m_writer->writeStartElement(m_ns, elementName);

    m_writer->writeAttribute("lat", latString);
    m_writer->writeAttribute("lon", lonString);

    if (!qIsNaN(landmark.coordinate().altitude()))
        m_writer->writeTextElement(m_ns, "ele", QString::number(landmark.coordinate().altitude()));

    if (!landmark.name().isEmpty())
        m_writer->writeTextElement(m_ns, "name", landmark.name());

    if (!landmark.description().isEmpty())
        m_writer->writeTextElement(m_ns, "desc", landmark.description());
    m_writer->writeEndElement();
    return true;
}

bool QLandmarkFileHandlerGpx::writeRoute(const QList<QLandmark> &route)
{
    m_writer->writeStartElement(m_ns, "rte");
    for (int i = 0; i < route.size(); ++i) {
        if (!writeWaypoint(route.at(i), "rtept"))
            return false;
    }
    m_writer->writeEndElement();
    return true;
}

bool QLandmarkFileHandlerGpx::writeTrack(const QList<QLandmark> &track)
{
    m_writer->writeStartElement(m_ns, "trk");
    m_writer->writeStartElement(m_ns, "trkseg");

    for (int i = 0; i < track.size(); ++i) {
        if(!writeWaypoint(track.at(i), "trkpt"))
            return false;
    }

    m_writer->writeEndElement();
    m_writer->writeEndElement();

    return true;
}

void QLandmarkFileHandlerGpx::setBehavior(Behavior behavior) {
    m_behavior = behavior;
}

QLandmarkManager::Error QLandmarkFileHandlerGpx::error()
{
    return m_errorCode;
}

QString QLandmarkFileHandlerGpx::errorString() const
{
    return m_errorString;
}

#include "moc_qlandmarkfilehandler_gpx_p.cpp"

