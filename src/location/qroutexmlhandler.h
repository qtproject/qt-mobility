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

#ifndef QLOCATION_ROUTEXMLHANDLER_H
#define QLOCATION_ROUTEXMLHANDLER_H

#include "qbasexmlhandler.h"
#include "qroutereply.h"
#include "qroute.h"
#include "qmaneuver.h"

QTM_BEGIN_NAMESPACE

class QRouteXmlHandler : public QBaseXmlHandler
{
public:
    QRouteXmlHandler(QRouteReply* routeReply);

    virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& atts);
    virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    virtual bool characters(const QString& ch);

private:
    Q_DISABLE_COPY(QRouteXmlHandler)

    virtual bool startBoundingBox();

    void parseRouteReply(const QXmlAttributes& atts);
    void parseRoute(const QXmlAttributes& atts);
    void parseManeuver(const QXmlAttributes& atts);
    bool parseGeoPoints(const QString& strPoints, QList<QGeoCoordinate>& points);

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

QTM_END_NAMESPACE

#endif
