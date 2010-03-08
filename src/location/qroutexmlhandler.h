/*
* ==============================================================================
*  Name        : QRouteXmlHandler.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_ROUTEXMLHANDLER_H
#define QLOCATION_ROUTEXMLHANDLER_H

#include "qbasexmlhandler.h"
#include "qroutereply.h"
#include "qroute.h"
#include "qmaneuver.h"

namespace QLocation
{
/*!
* This class parses a returned raw Qt::QNetworkReply to populate a
* corresponding \ref QRouteReply.
*/
class QRouteXmlHandler : public QBaseXmlHandler
{
public:
    /*!
    * Cosntructor.
    * @param routeReply The QRouteReply to populate.
    */
    QRouteXmlHandler(QRouteReply* routeReply);

    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool characters(const QString& ch);

private:
    Q_DISABLE_COPY(QRouteXmlHandler)

    virtual bool startBoundingBox();
    /*!
    * Parses a route reply.
    * @param atts The XML attributes to parse.
    */
    void parseRouteReply(const QXmlAttributes& atts);
    /*!
    * Parses a route.
    * @param atts The XML attributes to parse.
    */
    void parseRoute(const QXmlAttributes& atts);
    /*!
    * Parses a maneuver.
    * @param atts The XML attributes to parse.
    */
    void parseManeuver(const QXmlAttributes& atts);
    /*!
    * Parses a string of concatened geo coordinates.
    * E.g. "37.593874,55.627041 37.594056,55.627352 37.595215,55.628854"
    * @param strPoints The string containing all geo coordinates.
    * @param points This list will contain all parsed QGeoCoordinateMapss.
    * @return Whether the parsing succeeded.
    */
    bool parseGeoPoints(const QString& strPoints, QList<QGeoCoordinateMaps>& points);

private:
    //! Internal parsing states.
    enum RouteParseState {
        Route = QBaseXmlHandler::Last,
        BoundingBox,
        BoundingBox_Lat,
        BoundingBox_Long,
        Maneuver,
        WayPoints,
        ManeuverPoints
    };

    QRouteReply* reply; //!< Pointer to the QRouteReply object being populated.
    QRoute currRoute; //!< The current route for which XML data is being parsed.
    QManeuver currManeuver; //!< The current maneuver for which XML data is being parsed.
};

}

#endif
