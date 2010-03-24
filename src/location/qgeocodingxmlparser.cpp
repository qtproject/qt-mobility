
#include "qgeocodingxmlparser.h"

#include "qgeocoordinate.h"
#include "qgeocodingreply.h"
#include "qgeolocation.h"
#include "qgeoaddress.h"

#include <QXmlStreamReader>
#include <QIODevice>
#include <QRectF>

#include <QDebug>

QTM_BEGIN_NAMESPACE

QGeocodingXmlParser::QGeocodingXmlParser()
        : m_reader(0)
{
}

QGeocodingXmlParser::~QGeocodingXmlParser()
{
    if (m_reader)
        delete m_reader;
}

bool QGeocodingXmlParser::parse(QIODevice* source, QGeocodingReply *output)
{
    if (m_reader)
        delete m_reader;
    m_reader = new QXmlStreamReader(source);

    if (!readRootElement(output)) {
        // set errors appropriately
        qWarning() << m_reader->errorString();
        return false;
    }

    return true;
}

bool QGeocodingXmlParser::readRootElement(QGeocodingReply *output)
{
    qWarning() << "root";
    /*
    <xsd:element name="places">
        <xsd:complexType>
            <xsd:sequence>
                <xsd:element minOccurs="0" maxOccurs="unbounded" name="place" type="gc:Place"/>
            </xsd:sequence>
            <xsd:attribute name="resultCode" type="gc:ResultCodes"/>
            <xsd:attribute name="resultDescription" type="xsd:string"/>
            <xsd:attribute name="resultsTotal" type="xsd:nonNegativeInteger"/>
        </xsd:complexType>
    </xsd:element>

    <xsd:simpleType name="ResultCodes">
        <xsd:restriction base="xsd:string">
            <xsd:enumeration value="OK"/>
            <xsd:enumeration value="FAILED"/>
        </xsd:restriction>
    </xsd:simpleType>
    */

    if (m_reader->readNextStartElement()) {
        if (m_reader->name() == "places") {
            if (m_reader->attributes().hasAttribute("resultCode")) {
                QStringRef result = m_reader->attributes().value("resultCode");
                if (result == "OK") {
                    output->setResultCode(QGeocodingReply::OK);
                } else if (result == "FAILED") {
                    output->setResultCode(QGeocodingReply::Failed);
                } else {
                    m_reader->raiseError(QString("The attribute \"resultCode\" of the element \"places\" has an unknown value (value was %1).").arg(result.toString()));
                    return false;
                }
            }
            if (m_reader->attributes().hasAttribute("resultDescription")) {
                output->setResultDescription(m_reader->attributes().value("resultDescription").toString());
            }

            while (m_reader->readNextStartElement()) {
                if (m_reader->name() == "place") {
                    QGeoLocation location;

                    if (!readPlace(&location))
                        return false;

                    output->addPlace(location);
                } else {
                    m_reader->raiseError(QString("The element \"places\" did not expect a child element named \"%1\".").arg(m_reader->name().toString()));
                    return false;
                }
            }
        } else {
            m_reader->raiseError(QString("The root element is expected to have the name \"places\" (root element was named \"%1\").").arg(m_reader->name().toString()));
            return false;
        }
    } else {
        m_reader->raiseError("Expected a root element named \"places\" (no root element found).");
        return false;
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("A single root element named \"places\" was expected (second root element was named \"%1\")").arg(m_reader->name().toString()));
        return false;
    }

    return true;
}

bool QGeocodingXmlParser::readPlace(QGeoLocation *location)
{
    qWarning() << "place";
    /*
    <xsd:complexType name="Place">
        <xsd:all>
            <xsd:element name="location" type="gc:Location"/>
            <xsd:element minOccurs="0" name="address" type="gc:Address"/>
            <xsd:element minOccurs="0" name="alternatives" type="gc:Alternatives"/>
        </xsd:all>
        <xsd:attribute name="title" type="xsd:string" use="required"/>
        <xsd:attribute name="language" type="gc:LanguageCode" use="required"/>
    </xsd:complexType>

    <xsd:simpleType name="LanguageCode">
        <xsd:restriction base="xsd:string">
            <xsd:length value="3"/>
        </xsd:restriction>
    </xsd:simpleType>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "place");

    if (!m_reader->attributes().hasAttribute("title")) {
        m_reader->raiseError("The element \"place\" did not have the required attribute \"title\".");
        return false;
    }

    if (!m_reader->attributes().hasAttribute("language")) {
        //m_reader->raiseError("The element \"place\" did not have the required attribute \"language\".");
        //return false;
    } else {
        QString lang = m_reader->attributes().value("language").toString();

        if (lang.length() != 3) {
            m_reader->raiseError(QString("The attribute \"language\" of the element \"place\" was not of length 3 (length was %1).").arg(lang.length()));
            return false;
        }
    }

    bool parsedLocation = false;
    bool parsedAddress = false;
    bool parsedAlternatives = false;

    while (m_reader->readNextStartElement()) {
        QString name = m_reader->name().toString();
        if (name == "location") {
            if (parsedLocation) {
                m_reader->raiseError("The element \"place\" has multiple child elements named \"location\" (exactly one expected)");
                return false;
            }

            if (!readLocation(location))
                return false;

            parsedLocation = true;
        } else if (name == "address") {
            if (parsedAddress) {
                m_reader->raiseError("The element \"place\" has multiple child elements named \"address\" (at most one expected)");
                return false;
            }

            QGeoAddress address;
            if (!readAddress(&address))
                return false;
            else
                location->setAddress(address);

            parsedAddress = true;
        } else if (name == "alternatives") {
            if (parsedAlternatives) {
                m_reader->raiseError("The element \"place\" has multiple child elements named \"alternatives\" (at most one expected)");
                return false;
            }

            // skip alternatives for now
            // need to work out if we have a use for them at all
            // and how to store them if we get them
            m_reader->skipCurrentElement();

            parsedAlternatives = true;
        } else {
            m_reader->raiseError(QString("The element \"place\" did not expect a child element named \"%1\".").arg(m_reader->name().toString()));
            return false;
        }
    }

    if (!parsedLocation) {
        m_reader->raiseError("The element \"place\" has no child elements named \"location\" (exactly one expected)");
        return false;
    }

    return true;
}

bool QGeocodingXmlParser::readLocation(QGeoLocation *location)
{
    qWarning() << "location";
    /*
    <xsd:complexType name="Location">
        <xsd:all>
            <xsd:element name="position" type="gc:GeoCoord"/>
            <xsd:element minOccurs="0" name="boundingBox" type="gc:GeoBox"/>
        </xsd:all>
    </xsd:complexType>

    <xsd:complexType name="GeoBox">
        <xsd:sequence>
            <xsd:element name="northWest" type="gc:GeoCoord"/>
            <xsd:element name="southEast" type="gc:GeoCoord"/>
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "location");

    bool parsedPosition = false;
    bool parsedBox = false;

    while (m_reader->readNextStartElement()) {
        QString name = m_reader->name().toString();
        if (name == "position") {
            if (parsedPosition) {
                m_reader->raiseError("The element \"location\" has multiple child elements named \"position\" (exactly one expected)");
                return false;
            }

            QGeoCoordinate coordinate;
            if (!readCoordinate(&coordinate, "position"))
                return false;
            else
                location->setCoordinate(coordinate);

            parsedPosition = true;
        } else if (name == "boundingBox") {
            if (parsedBox) {
                m_reader->raiseError("The element \"location\" has multiple child elements named \"boundingBox\" (at most one expected)");
                return false;
            }

            QRectF box;
            if (!readBoundingBox(&box))
                return false;
            location->setBoundingBox(box);

            parsedBox = true;
        } else {
            m_reader->raiseError(QString("The element \"location\" did not expect a child element named \"%1\".").arg(m_reader->name().toString()));
            return false;
        }
    }

    if (!parsedPosition) {
        m_reader->raiseError("The element \"location\" has no child elements named \"position\" (exactly one expected)");
        return false;
    }

    return true;
}

bool QGeocodingXmlParser::readAddress(QGeoAddress *address)
{
    qWarning() << "address";
    /*
    <xsd:complexType name="Address">
        <xsd:sequence>
            <xsd:element minOccurs="0" maxOccurs="1" name="country" type="xsd:string"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="countryCode" type="gc:CountryCode"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="state" type="xsd:string"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="county" type="xsd:string"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="city" type="xsd:string"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="district" type="xsd:string"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="thoroughfare" type="gc:Thoroughfare"/>
            <xsd:element minOccurs="0" maxOccurs="1" name="postCode" type="xsd:string"/>
        </xsd:sequence>
        <xsd:attribute name="type" type="xsd:string"/>
    </xsd:complexType>

    <xsd:simpleType name="CountryCode">
        <xsd:restriction base="xsd:string">
            <xsd:length value="3" fixed="true"/>
        </xsd:restriction>
    </xsd:simpleType>

    <xsd:complexType name="Thoroughfare">
        <xsd:sequence>
            <xsd:element minOccurs="0" name="name" type="xsd:string"/>
            <xsd:element minOccurs="0" name="number" type="xsd:string"/>
        </xsd:sequence>
    </xsd:complexType>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "address");

    // currently ignoring the type of the address

    if (!m_reader->readNextStartElement())
        return true;

    if (m_reader->name() == "country") {
        address->setCountry(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "countryCode") {
        address->setCountryCode(m_reader->readElementText());

        if (address->countryCode().length() != 3) {
            m_reader->raiseError(QString("The text of the element \"countryCode\" was not of length 3 (length was %1).").arg(address->countryCode().length()));
            return false;
        }

        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "state") {
        address->setState(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "county") {
        address->setCounty(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "city") {
        address->setCity (m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "district") {
        address->setDistrict(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    bool inThoroughfare = false;

    if (m_reader->name() == "thoroughfare") {
        inThoroughfare = m_reader->readNextStartElement();

        if (inThoroughfare && (m_reader->name() == "name")) {
            address->setThoroughfareName(m_reader->readElementText());
            if (!m_reader->readNextStartElement())
                inThoroughfare = false;
        }

        if (inThoroughfare && (m_reader->name() == "number")) {
            address->setThoroughfareNumber(m_reader->readElementText());
            if (!m_reader->readNextStartElement())
                inThoroughfare = false;
        }

        if (inThoroughfare) {
            m_reader->raiseError(QString("The element \"thoroughFare\" did not expect the child element \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
            return false;
        }

        if (!m_reader->readNextStartElement())
            return true;
    }

    if (m_reader->name() == "postCode") {
        address->setPostCode(m_reader->readElementText());
        if (!m_reader->readNextStartElement())
            return true;
    }

    m_reader->raiseError(QString("The element \"address\" did not expect the child element \"%1\" at this point (unknown child element or child element out of order).").arg(m_reader->name().toString()));
    return false;
}

bool QGeocodingXmlParser::readBoundingBox(QRectF *rect)
{
    qWarning() << "boundingBox";
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
        if (!readCoordinate(&nw, "northWest"))
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
        if (!readCoordinate(&se, "southEast"))
            return false;
    } else {
        m_reader->raiseError(QString("The element \"boundingBox\" expected this child element to be named \"southEast\" (found an element named \"%1\")").arg(m_reader->name().toString()));
        return false;
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError("The element \"boundingBox\" was expected to have 2 child elements (more than 2 found)");
        return false;
    }

    rect->setTopLeft(QPointF(nw.longitude(), nw.latitude()));
    rect->setBottomRight(QPointF(se.longitude(), se.latitude()));

    return true;
}

bool QGeocodingXmlParser::readCoordinate(QGeoCoordinate *coordinate, const QString &elementName)
{
    qWarning() << "coord";
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
    }

    if (m_reader->readNextStartElement()) {
        m_reader->raiseError(QString("The element \"%1\" was expected to have 2 child elements (more than 2 found)").arg(elementName));
        return false;
    }

    return true;
}

QTM_END_NAMESPACE

