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

#include "qlandmarkfilehandler_lmx_p.h"

#include "qlandmarkmanagerengine.h"
#include "qlandmarkcategory.h"
#include "qlandmarkcategoryid.h"
#include "qgeocoordinate.h"
#include "qgeoaddress.h"

#include <QFile>
#include <QStringList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <qnumeric.h>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QLandmarkFileHandlerLmx::QLandmarkFileHandlerLmx(QLandmarkManagerEngine *engine)
    : QObject(),
    m_engine(engine),
    m_writer(0),
    m_reader(0)
{
}

QLandmarkFileHandlerLmx::~QLandmarkFileHandlerLmx()
{
    if (m_reader != 0)
        delete m_reader;
    if (m_writer != 0)
        delete m_writer;
}

QList<QLandmark> QLandmarkFileHandlerLmx::landmarks() const
{
    return m_landmarks;
}

void QLandmarkFileHandlerLmx::setLandmarks(const QList<QLandmark> &landmarks)
{
    m_landmarks = landmarks;
}

bool QLandmarkFileHandlerLmx::importData(QIODevice *device)
{
    if (m_reader)
        delete m_reader;

    m_reader = new QXmlStreamReader(device);

    if (!readLmx()) {
        m_error = m_reader->errorString();
        emit error(m_error);
        return false;
    } else {
        if (m_reader->atEnd()) {
            m_reader->readNextStartElement();
            if (!m_reader->name().isEmpty()) {
                m_error = QString("A single root element named \"lmx\" was expected (second root element was named \"%1\").").arg(m_reader->name().toString());
                emit error(m_error);
                return false;
            }
        }
    }

    m_error = "";
    emit finishedImport();
    return true;
}

bool QLandmarkFileHandlerLmx::readLmx()
{
    /*
    <xsd:element name="lmx">
        <xsd:complexType>
            <xsd:choice>
                <xsd:element name="landmark" type="landmarkType" />
                <xsd:element name="landmarkCollection" type="landmarkCollectionType" />
            </xsd:choice>
        </xsd:complexType>
    </xsd:element>
    */

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("Expected a root element named \"lmx\" (no root element found).");
        return false;
    }

    if (m_reader->name() != "lmx") {
        m_reader->raiseError(QString("The root element is expected to have the name \"lmx\" (root element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"lmx\" expected a single child element named either \"landmark\" or \"landmarkCollection\" (no child element found).");
        return false;
    }

    if (m_reader->name() == "landmark") {
        QLandmark lm;
        if (!readLandmark(lm))
            return false;
        m_landmarks << lm;
    } else if (m_reader->name() == "landmarkCollection") {
        if (!readLandmarkCollection(m_landmarks))
            return false;
    } else {
        m_reader->raiseError(QString("The element \"lmx\" expected a single child element named either \"landmark\" or \"landmarkCollection\" (child element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }

    if (!m_reader->readNextStartElement()) {
        m_reader->skipCurrentElement();
        return true;
    } else {
        m_reader->raiseError(QString("The element \"lmx\" expected a single child element (second child element was named \"%1\").").arg(m_reader->name().toString()));
        return false;
    }
}

bool QLandmarkFileHandlerLmx::readLandmarkCollection(QList<QLandmark> &landmarkCollection)
{
    /*
    <xsd:complexType name="landmarkCollectionType">
        <xsd:sequence>
            <xsd:element name="name" type="xsd:string" minOccurs="0" />
            <xsd:element name="description" type="xsd:string" minOccurs="0" />
            <xsd:element name="landmark" type="landmarkType" maxOccurs="unbounded" />
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "landmarkCollection"));

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"landmarkCollection\" expected 1 or more child elements named \"landmark\" (0 found).");
        return false;
    }

    if (m_reader->name() == "name") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement()) {
            m_reader->raiseError("The element \"landmarkCollection\" expected 1 or more child elements named \"landmark\" (0 found).");
            return false;
        }
    }

    if (m_reader->name() == "description") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement()) {
            m_reader->raiseError("The element \"landmarkCollection\" expected 1 or more child elements named \"landmark\" (0 found).");
            return false;
        }
    }

    while (m_reader->name() == "landmark") {
        QLandmark lm;
        if (!readLandmark(lm))
            return false;
        landmarkCollection << lm;

        if (!m_reader->readNextStartElement())
            return true;
    }

    m_reader->raiseError(QString("The element \"landmarkCollection\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerLmx::readLandmark(QLandmark &landmark)
{
    /*
    <xsd:complexType name="landmarkType">
        <xsd:sequence>
            <xsd:element name="name" type="xsd:string" minOccurs="0" />
            <xsd:element name="description" type="xsd:string" minOccurs="0" />
            <xsd:element name="coordinates" type="coordinatesType" minOccurs="0" />
            <xsd:element name="coverageRadius" minOccurs="0">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:float">
                        <xsd:minInclusive value="0"/>
                    </xsd:restriction>
                </xsd:simpleType>
            </xsd:element>
            <xsd:element name="addressInfo" type="addressInfoType" minOccurs="0" />
            <xsd:element name="mediaLink" type="mediaLinkType" minOccurs="0" maxOccurs="unbounded" />
            <xsd:element name="category" type="categoryType" minOccurs="0" maxOccurs="unbounded" />
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement() &&
             (m_reader->name() == "landmark"));

    if (!m_reader->readNextStartElement())
        return true;

    if (m_reader->name() == "name") {
        landmark.setName(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "description") {
        landmark.setDescription(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "coordinates") {
        if (!readCoordinates(landmark))
            return false;

        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "coverageRadius") {
        bool ok = false;
        QString s = m_reader->readElementText();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The element \"coverageRadius\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double rad = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"coverageRadius\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        if (rad < 0.0) {
            m_reader->raiseError(QString("The element \"coverageRadius\" is expected to have a non-negative value (value was \"%1\").").arg(s));
            return false;
        }

        landmark.setRadius(rad);

        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "addressInfo") {
        if (!readAddressInfo(landmark))
            return false;

        if (!m_reader->readNextStartElement())
            return true;
    }

    // TODO need to document the fact that only the first link is read
    // and the others are ignored
    bool mediaLinkRead = false;

    while (m_reader->name() == "mediaLink") {
        if (!mediaLinkRead) {
            mediaLinkRead = true;
            if (!readMediaLink(landmark))
                return false;
        }

        if (!m_reader->readNextStartElement())
            return true;
    }

    QList<QLandmarkCategoryId> categoryIds;

    while (m_reader->name() == "category") {
        QLandmarkCategoryId id;
        if (!readCategory(id))
            return false;
        categoryIds << id;

        if (!m_reader->readNextStartElement()) {
            landmark.setCategoryIds(categoryIds);
            return true;
        }
    }

    m_reader->raiseError(QString("The element \"landmark\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerLmx::readCoordinates(QLandmark &landmark)
{
    /*
    <xsd:complexType name="coordinatesType">
        <xsd:sequence>
            <xsd:element name="latitude">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:double">
                        <xsd:minInclusive value="-90"/>
                        <xsd:maxInclusive value="90"/>
                    </xsd:restriction>
                </xsd:simpleType>
            </xsd:element>
            <xsd:element name="longitude">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:double">
                        <xsd:minInclusive value="-180"/>
                        <xsd:maxExclusive value="180"/>
                    </xsd:restriction>
                </xsd:simpleType>
            </xsd:element>
            <xsd:element name="altitude" type="xsd:float" minOccurs="0" />
            <xsd:element name="horizontalAccuracy" minOccurs="0">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:float">
                        <xsd:minInclusive value="0"/>
                    </xsd:restriction>
                </xsd:simpleType>
            </xsd:element>
            <xsd:element name="verticalAccuracy" minOccurs="0">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:float">
                        <xsd:minInclusive value="0"/>
                    </xsd:restriction>
                </xsd:simpleType>
            </xsd:element>
            <xsd:element name="timeStamp" type="xsd:dateTime" minOccurs="0" />
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement() &&
             (m_reader->name() == "coordinates"));

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"coordinates\" did not have the required child element \"latitude\".");
        return false;
    }

    if (m_reader->name() == "latitude") {
        bool ok = false;
        QString s = m_reader->readElementText();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The element \"latitude\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double lat = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"latitude\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        if (lat < -90.0 || 90.0 < lat) {
            m_reader->raiseError(QString("The element \"latitude\" fell outside of the bounds -90.0 <= latitude <= 90.0 (value was \"%1\").").arg(s));
            return false;
        }

        QGeoCoordinate coord = landmark.coordinate();
        coord.setLatitude(lat);
        landmark.setCoordinate(coord);

        if (!m_reader->readNextStartElement()) {
            m_reader->raiseError("The element \"coordinates\" did not have the required child element \"longitude\".");
            return false;
        }
    } else {
        m_reader->raiseError("The element \"coordinates\" did not have the required child element \"latitude\".");
        return false;
    }

    if (m_reader->name() == "longitude") {
        bool ok = false;
        QString s = m_reader->readElementText();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The element \"longitude\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double lon = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"longitude\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        if (lon < -180.0 || 180.0 <= lon) {
            m_reader->raiseError(QString("The element \"longitude\" fell outside of the bounds -180.0 <= longitude < 180.0 (value was \"%1\").").arg(s));
            return false;
        }

        QGeoCoordinate coord = landmark.coordinate();
        coord.setLongitude(lon);
        landmark.setCoordinate(coord);

        if (!m_reader->readNextStartElement())
            return true;
    } else {
        m_reader->raiseError("The element \"coordinates\" did not have the required child element \"longitude\".");
        return false;
    }

    if (m_reader->name() == "altitude") {
        bool ok = false;
        QString s = m_reader->readElementText();

        if ((s == "INF") || (s == "-INF") || (s == "NaN")) {
            m_reader->raiseError(QString("The element \"altitude\" expected a value convertable to type double (value was \"%1\").").arg(s));
            return false;
        }

        double alt = s.toDouble(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"altitude\" expected a value convertable to type float (value was \"%1\").").arg(s));
            return false;
        }

        QGeoCoordinate coord = landmark.coordinate();
        coord.setAltitude(alt);
        landmark.setCoordinate(coord);

        if (!m_reader->readNextStartElement())
            return true;
    }

    QList<QString> names;
    names << "horizontalAccuracy";
    names << "verticalAccuracy";
    names << "timeStamp";

    for (int i = 0; i < names.size(); ++i) {
        // Not used outside of schema compliance check
        if (m_reader->name() == names.at(i)) {
           m_reader->skipCurrentElement();
           if (!m_reader->readNextStartElement())
               return true;
        }
    }

    m_reader->raiseError(QString("The element \"coordinate\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerLmx::readAddressInfo(QLandmark &landmark)
{
    /*
    <xsd:complexType name="addressInfoType">
        <xsd:all>
            <xsd:element name="country" type="xsd:string" minOccurs="0" />
            <xsd:element name="countryCode" minOccurs="0">
                <xsd:simpleType>
                    <xsd:restriction base="xsd:token">
                        <xsd:length value="2"/>
                    </xsd:restriction>
                </xsd:simpleType>
            </xsd:element>
            <xsd:element name="state" type="xsd:string" minOccurs="0" />
            <xsd:element name="county" type="xsd:string" minOccurs="0" />
            <xsd:element name="city" type="xsd:string" minOccurs="0" />
            <xsd:element name="district" type="xsd:string" minOccurs="0" />
            <xsd:element name="postalCode" type="xsd:string" minOccurs="0" />
            <xsd:element name="crossing1" type="xsd:string" minOccurs="0" />
            <xsd:element name="crossing2" type="xsd:string" minOccurs="0" />
            <xsd:element name="street" type="xsd:string" minOccurs="0" />
            <xsd:element name="buildingName" type="xsd:string" minOccurs="0" />
            <xsd:element name="buildingZone" type="xsd:string" minOccurs="0" />
            <xsd:element name="buildingFloor" type="xsd:string" minOccurs="0" />
            <xsd:element name="buildingRoom" type="xsd:string" minOccurs="0" />
            <xsd:element name="extension" type="xsd:string" minOccurs="0" />
            <xsd:element name="phoneNumber" type="xsd:string" minOccurs="0" />
        </xsd:all>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "addressInfo"));

    QHash<QString, int> counts;
    counts["country"] = 0;
    counts["countryCode"] = 0;
    counts["state"] = 0;
    counts["county"] = 0;
    counts["city"] = 0;
    counts["district"] = 0;
    counts["postalCode"] = 0;
    counts["crossing1"] = 0;
    counts["crossing2"] = 0;
    counts["street"] = 0;
    counts["buildingName"] = 0;
    counts["buildingZone"] = 0;
    counts["buildingFloor"] = 0;
    counts["buildingRoom"] = 0;
    counts["extension"] = 0;
    counts["phoneNumber"] = 0;

    QGeoAddress address;

    while (m_reader->readNextStartElement()) {
        QString name = m_reader->name().toString();
        if (counts.keys().contains(name)) {
            if (counts.value(name) == 1) {
                m_reader->raiseError(QString("The element \"addressInfo\" did not expect more then one occurrence of the child element named \"%1\".").arg(m_reader->name().toString()));
                return false;
            }
            counts[name] = 1;

            if (name == "country") {
                address.setCountry(m_reader->readElementText());
            } else if (name == "state") {
                address.setState(m_reader->readElementText());
            } else if (name == "city") {
                address.setCity(m_reader->readElementText());
            } else if (name == "postalCode") {
                address.setPostCode(m_reader->readElementText());
            } else if (name == "street") {
                QStringList street = m_reader->readElementText().split(' ');
                address.setThoroughfareNumber(street.takeFirst());
                address.setThoroughfareName(street.join(" "));
            } else if (name == "phoneNumber") {
                landmark.setPhone(m_reader->readElementText());
            } else {
                m_reader->skipCurrentElement();
            }
        } else {
            m_reader->raiseError(QString("The element \"addressInfo\" did not expect a child element named \"%1\".").arg(m_reader->name().toString()));
            return false;
        }
    }

    landmark.setAddress(address);

    return true;
}

bool QLandmarkFileHandlerLmx::readMediaLink(QLandmark &landmark)
{
    /*
    <xsd:complexType name="mediaLinkType">
        <xsd:sequence>
            <xsd:element name="name" type="xsd:string" minOccurs="0" />
            <xsd:element name="mime" type="xsd:string" minOccurs="0" />
            <xsd:element name="url" type="xsd:anyURI" />
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "mediaLink"));

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"mediaLink\" did not have the required child element \"url\".");
        return false;
    }

    if (m_reader->name() == "name") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"mediaLink\" did not have the required child element \"url\".");
            return false;
        }
    }

    if (m_reader->name() == "mime") {
        // Not used outside of schema compliance check
        m_reader->skipCurrentElement();
        if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"mediaLink\" did not have the required child element \"url\".");
            return false;
        }
    }

    if (m_reader->name() == "url") {
        landmark.setUrl(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    m_reader->raiseError(QString("The element \"url\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerLmx::readCategory(QLandmarkCategoryId &categoryId)
{
    /*
    <xsd:complexType name="categoryType">
        <xsd:sequence>
            <xsd:element name="id" type="xsd:unsignedShort" minOccurs="0" />
            <xsd:element name="name" type="xsd:string" />
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement()
             && (m_reader->name() == "category"));

    if (!m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"category\" did not have the required child element \"name\".");
        return false;
    }

    QString idString;

    if (m_reader->name() == "id") {
        bool ok = false;
        idString = m_reader->readElementText();
        unsigned int id = idString.toUShort(&ok);

        if (!ok) {
            m_reader->raiseError(QString("The element \"id\" expected a value convertable to type unsigned short (value was \"%1\").").arg(idString));
            return false;
        }

        if (!m_reader->readNextStartElement()) {
            m_reader->raiseError("The element \"category\" did not have the required child element \"name\".");
            return false;
        }
    }

    if (m_reader->name() == "name") {
        QString name = m_reader->readElementText();
        if (!m_reader->readNextStartElement()) {

            QLandmarkCategory cat;

            if (!idString.isEmpty()) {
                QLandmarkCategoryId id;
                id.setManagerUri(m_engine->managerUri());
                id.setId(idString);

                QLandmarkManager::Error error;
                cat = m_engine->category(id, &error, &m_error);

                if (error != QLandmarkManager::NoError) {
                    m_reader->raiseError(m_error);
                    return false;
                }
            }

            cat.setName(name);
            if(!m_engine->saveCategory(&cat, 0, &m_error)) {
                m_reader->raiseError(m_error);
                return false;
            }

            categoryId = cat.id();
            return true;
        }
    }

    m_reader->raiseError(QString("The element \"category\" did not expect a child element named \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QLandmarkFileHandlerLmx::exportData(QIODevice *device, const QString &nsPrefix)
{
    if (m_writer != 0)
        delete m_writer;

    m_writer = new QXmlStreamWriter(device);
    m_writer->setAutoFormatting(true);

    m_nsPrefix = nsPrefix;

    bool result = writeLmx();

    if(!result) {
        emit error(m_error);
        return false;
    }

    m_error = "";
    emit finishedExport();
    return true;
}

bool QLandmarkFileHandlerLmx::writeLmx()
{
    // LMX files must contain at least one landmark
    if (m_landmarks.size() == 0)
        return true;

    QString nsLmx = "http://www.nokia.com/schemas/location/landmarks/1/0/";
    QString nsXsi = "http://www.w3.org/2001/XMLSchema-instance";

    if (!m_nsPrefix.isEmpty())
        m_ns = nsLmx;
    else
        m_ns = "";

    m_writer->writeStartDocument();

    if (m_nsPrefix.isEmpty())
        m_writer->writeDefaultNamespace(nsLmx);
    else {
        m_writer->writeNamespace(nsLmx, m_nsPrefix);
    }

    m_writer->writeStartElement(m_ns, "lmx");
    m_writer->writeNamespace(nsXsi, "xsi");
    m_writer->writeAttribute(nsXsi, "schemaLocation", "http://www.nokia.com/schemas/location/landmarks/1/0/ lmx.xsd");

    if (m_landmarks.size() > 1) {
        if(!writeLandmarkCollection(m_landmarks))
            return false;
    } else {
        if(!writeLandmark(m_landmarks.at(0)))
            return false;
    }

    m_writer->writeEndElement();
    m_writer->writeEndDocument();

    return true;
}

bool QLandmarkFileHandlerLmx::writeLandmarkCollection(const QList<QLandmark> &landmarkCollection)
{
    m_writer->writeStartElement(m_ns, "landmarkCollection");

    for (int i = 0; i < m_landmarks.size(); ++i) {
        if (!writeLandmark(m_landmarks.at(i)))
            return false;
    }

    m_writer->writeEndElement();

    return true;
}

bool QLandmarkFileHandlerLmx::writeLandmark(const QLandmark &landmark)
{
    m_writer->writeStartElement(m_ns, "landmark");

    if (!landmark.name().isEmpty())
        m_writer->writeTextElement(m_ns, "name", landmark.name());

    if (!landmark.description().isEmpty())
        m_writer->writeTextElement(m_ns, "description", landmark.description());

    if (landmark.coordinate().isValid())
        if (!writeCoordinates(landmark))
            return false;

    if (landmark.radius() != -1.0)
        m_writer->writeTextElement(m_ns, "coverageRadius", QString::number(landmark.radius()));

    if (!writeAddressInfo(landmark))
        return false;

    if (!landmark.url().isEmpty())
        if (!writeMediaLink(landmark))
            return false;

    for (int i = 0; i < landmark.categoryIds().size(); ++i) {
        if (!writeCategory(landmark.categoryIds().at(i)))
            return false;
    }

    m_writer->writeEndElement();

    return true;
}

bool QLandmarkFileHandlerLmx::writeCoordinates(const QLandmark &landmark)
{
    m_writer->writeStartElement(m_ns, "coordinates");

    double lat = landmark.coordinate().latitude();
    double lon = landmark.coordinate().longitude();
    double alt = landmark.coordinate().altitude();

    if (qIsNaN(lat))
        m_writer->writeTextElement(m_ns, "latitude", "NaN");
    else
        m_writer->writeTextElement(m_ns, "latitude", QString::number(lat));

    if (qIsNaN(lon))
        m_writer->writeTextElement(m_ns, "longitude", "NaN");
    else
        m_writer->writeTextElement(m_ns, "longitude", QString::number(lon));

    if (!qIsNaN(alt))
        m_writer->writeTextElement(m_ns, "altitude", QString::number(alt));

    m_writer->writeEndElement();

    return true;
}

bool QLandmarkFileHandlerLmx::writeAddressInfo(const QLandmark &landmark)
{
    QGeoAddress address = landmark.address();

    if (address.thoroughfareNumber().isEmpty()
            && address.thoroughfareName().isEmpty()
            && address.city().isEmpty()
            && address.state().isEmpty()
            && address.country().isEmpty()
            && address.postCode().isEmpty()
            && landmark.phone().isEmpty())
        return true;

    m_writer->writeStartElement(m_ns, "addressInfo");

    if (!address.country().isEmpty())
        m_writer->writeTextElement(m_ns, "country", address.country());

    if (!address.state().isEmpty())
        m_writer->writeTextElement(m_ns, "state", address.state());

    if (!address.city().isEmpty())
        m_writer->writeTextElement(m_ns, "city", address.city());

    if (!address.postCode().isEmpty())
        m_writer->writeTextElement(m_ns, "postalCode", address.postCode());

    QString street;
    if (!address.thoroughfareNumber().isEmpty())
        street.append(address.thoroughfareNumber());

    if (!address.thoroughfareNumber().isEmpty() && !address.thoroughfareName().isEmpty())
        street.append(" ");

    if (!address.thoroughfareName().isEmpty())
        street.append(address.thoroughfareName());

    if (!street.isEmpty())
        m_writer->writeTextElement(m_ns, "street", street);

    if (!landmark.phone().isEmpty())
        m_writer->writeTextElement(m_ns, "phoneNumber", landmark.phone());

    m_writer->writeEndElement();

    return true;
}

bool QLandmarkFileHandlerLmx::writeMediaLink(const QLandmark &landmark)
{
    m_writer->writeStartElement(m_ns, "mediaLink");
    m_writer->writeTextElement(m_ns, "url", landmark.url().toString());
    m_writer->writeEndElement();

    return true;
}

bool QLandmarkFileHandlerLmx::writeCategory(const QLandmarkCategoryId &id)
{
    if (!id.isValid()) {
        m_error = QString("The category with id \"%1\" from manager \"%2\" is invalid.").arg(id.id()).arg(id.managerUri());
        return false;
    }

    QLandmarkManager::Error error;
    QLandmarkCategory cat = m_engine->category(id, &error, &m_error);
    if (error != QLandmarkManager::NoError) {
        return false;
    }

    m_writer->writeStartElement(m_ns, "category");
    m_writer->writeTextElement(m_ns, "id", cat.id().id());
    m_writer->writeTextElement(m_ns, "name", cat.name());
    m_writer->writeEndElement();

    return true;
}

QString QLandmarkFileHandlerLmx::errorString() const
{
    return m_error;
}

#include "moc_qlandmarkfilehandler_lmx_p.cpp"

QTM_END_NAMESPACE

