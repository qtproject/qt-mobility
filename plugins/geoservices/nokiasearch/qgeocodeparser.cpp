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

#include "qgeocodeparser.h"

#include <QXmlStreamReader>
#include <QIODevice>

QGeoCodeParser::QGeoCodeParser()
        : m_reader(0)
{
}

QGeoCodeParser::~QGeoCodeParser()
{
    if (m_reader)
        delete m_reader;
}

bool QGeoCodeParser::parse(QIODevice* source)
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

QList<QGeoPlace> QGeoCodeParser::results() const
{
    return m_results;
}

QString QGeoCodeParser::errorString() const
{
    return m_errorString;
}

bool QGeoCodeParser::parseRootElement()
{
    if (m_reader->readNextStartElement()) {
        if (m_reader->name() == "searchresponse") {
            if (m_reader->readNextStartElement()) {
                if (m_reader->name() == "results") {
                    while (m_reader->readNextStartElement()) {
                        QString ggg = m_reader->name().toString();
                        if (m_reader->name().toString() == "item") {
                            QGeoPlace place;

                            if (!parsePlace(&place))
                                return false;

                            m_results.append(place);
                        } else {
                            m_reader->raiseError(QString("The element \"places\" did not expect a child element named \"%1\".").arg(m_reader->name().toString()));
                            return false;
                        }
                    }
                }
                else {
                    m_reader->raiseError("The place not found, request needs more attributes or there is error in request.");
                    return false;
                }
            }
        }
    }
    return true;
}

bool QGeoCodeParser::parsePlace(QGeoPlace *place)
{
    /*
    <?xml version="1.0" encoding="ISO-8859-1" ?>
    <searchresponse version="1.0" id="a7:20100701133742794:0000050">
        <results>
            <item id="19220387" relevance="163108">
                <view id="15" />
                <tag id="9000284" />
                <property name="GEO_LATITUDE">61.49242995726158</property>
                <property name="ADDR_COUNTY_NAME">Satakunta</property>
                <property name="ADDR_POPULATION">0</property>
                <property name="ADDR_HOUSE_NUMBER">11</property>
                <property name="ADDR_POSTAL_CODE">28100</property>
                <property name="TYPE">Street</property>
                <property name="ADDR_TOWNSHIP_NAME">Pori</property>
                <property name="ADDR_CITY_NAME">Pori</property>
                <property name="ADDR_COUNTRY_CODE">FIN</property>
                <property name="ADDR_STATE_NAME">Länsi-Suomi</property>
                <property name="GEO_LONGITUDE">21.79834065543428</property>
                <property name="ADDR_COUNTRY_NAME">Finland</property>
                <property name="ADDR_HOUSE_ALPHA" />
                <property name="TITLE">Pohjoisranta 11, 28100 Pori, Finland</property>
                <property name="ADDR_STREET_NAME">Pohjoisranta</property>
            </item>
        </results>
    </searchresponse>
    */

    Q_ASSERT(m_reader->isStartElement() && m_reader->name() == "item");

    if (!m_reader->attributes().hasAttribute("id")) {
        m_reader->raiseError("The element \"item\" did not have the required attribute \"id\".");
        return false;
    }

    if (!m_reader->attributes().hasAttribute("relevance")) {
        m_reader->raiseError("The element \"item\" did not have the required attribute \"relevance\".");
        return false;
    } else {
        QString id = m_reader->attributes().value("id").toString();
        QString relevance = m_reader->attributes().value("relevance").toString();
    }

    m_reader->readNext();
    QGeoAddress address;
    QGeoCoordinate coordinate;

    QGeoCoordinate bBoxTopLeft;
    QGeoCoordinate bBoxBottomRight;

    while (!(m_reader->tokenType() == QXmlStreamReader::EndElement && m_reader->name() == "item")) {
        QString hoo3 = m_reader->name().toString();

        if (m_reader->tokenType() == QXmlStreamReader::StartElement) {

            if (m_reader->name() == "property") {
                if (m_reader->attributes().value("name").toString() == "ADDR_CITY_NAME") {
                    address.setCity(m_reader->readElementText());
                }
                else if (m_reader->attributes().value("name").toString() == "ADDR_COUNTRY_NAME") {
                    address.setCountry(m_reader->readElementText());
                }
                else if (m_reader->attributes().value("name").toString() == "ADDR_STREET_NAME") {
                    address.setThoroughfareName(m_reader->readElementText());
                }
                else if (m_reader->attributes().value("name").toString() == "ADDR_HOUSE_NUMBER") {
                    address.setThoroughfareNumber(m_reader->readElementText());
                }
                else if (m_reader->attributes().value("name").toString() == "ADDR_POSTAL_CODE") {
                    address.setPostCode(m_reader->readElementText());
                }
                else if (m_reader->attributes().value("name").toString() == "ADDR_STATE_NAME") {
                    address.setState(m_reader->readElementText());
                }
                else if (m_reader->attributes().value("name").toString() == "ADDR_COUNTY_NAME") {
                    address.setCounty(m_reader->readElementText());
                }


                else if (m_reader->attributes().value("name").toString() == "GEO_BBX_LATITUDE_2") {
                    bBoxTopLeft.setLatitude((m_reader->readElementText()).toDouble());
                }
                else if (m_reader->attributes().value("name").toString() == "GEO_BBX_LONGITUDE_1") {
                    bBoxTopLeft.setLongitude((m_reader->readElementText()).toDouble());
                }
                else if (m_reader->attributes().value("name").toString() == "GEO_BBX_LATITUDE_1") {
                    bBoxBottomRight.setLatitude((m_reader->readElementText()).toDouble());
                }
                else if (m_reader->attributes().value("name").toString() == "GEO_BBX_LONGITUDE_2") {
                    bBoxBottomRight.setLongitude((m_reader->readElementText()).toDouble());
                }


                else if (m_reader->attributes().value("name").toString() == "GEO_LATITUDE") {
                    coordinate.setLatitude((m_reader->readElementText()).toDouble());
                }
                else if (m_reader->attributes().value("name").toString() == "GEO_LONGITUDE") {
                    coordinate.setLongitude((m_reader->readElementText()).toDouble());
                }
                else {
                    m_reader->skipCurrentElement();
                }
            }
            else {
                m_reader->skipCurrentElement();
            }
        }
        m_reader->readNext();
    }

    place->setAddress(address);
    place->setCoordinate(coordinate);

    if(bBoxTopLeft.isValid() && bBoxBottomRight.isValid()) {
        place->setBoundingBox(QGeoBoundingBox(bBoxTopLeft,bBoxBottomRight));
    }
    return true;
}
