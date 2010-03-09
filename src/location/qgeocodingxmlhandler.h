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

#ifndef QLOCATION_GEOCODINGXMLHANDLER_H
#define QLOCATION_GEOCODINGXMLHANDLER_H

#include "qbasexmlhandler.h"
#include "qgeocodingreply.h"

QTM_BEGIN_NAMESPACE

class QGeocodingXmlHandler : public QBaseXmlHandler
{
public:
    QGeocodingXmlHandler(QGeocodingReply* codingReply);

    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool characters(const QString& ch);

private:
    Q_DISABLE_COPY(QGeocodingXmlHandler)

    void parseCodingReply(const QXmlAttributes& atts);
    void parsePlace(const QXmlAttributes& atts);
    virtual bool startBoundingBox();

private:
    //! Internal parsing states.
    enum GeocodingParseState {
        Place = QBaseXmlHandler::Last,
        Location,
        Position,
        Address,
        Address_Country,
        Address_CountryCode,
        Address_State,
        Address_County,
        Address_City,
        Address_District,
        Address_Thoroughfare,
        Address_ThoroughfareName,
        Address_ThoroughfareNumber,
        Address_PostCode,
        Alternative,
        Alternative_Country,
        Alternative_CountryCode,
        Alternative_State,
        Alternative_County,
        Alternative_City,
        Alternative_District,
        Alternative_Thoroughfare,
        Alternative_ThoroughfareName,
        Alternative_ThoroughfareNumber,
        Alternative_PostCode

    };

    QGeocodingReply* reply; //!< Pointer to the QRouteReply object being populated.
    QGeoLocation* currPlace; //!< The current geo location for which XML data is being parsed.
};

QTM_END_NAMESPACE

#endif
