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

#ifndef QROUTEXMLPARSER_P_H
#define QROUTEXMLPARSER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QList>
#include <QString>

class QXmlStreamReader;
class QIODevice;

#include <QGeoRoute>
#include <QGeoRouteSegment>
#include <QGeoCoordinate>
#include <QGeoBoundingBox>

#include <QGeoRouteRequest>

QTM_USE_NAMESPACE

class QGeoRouteXmlParser
{
public:
    QGeoRouteXmlParser(const QGeoRouteRequest &request);
    ~QGeoRouteXmlParser();

    bool parse(QIODevice* source);
    QList<QGeoRoute> results() const;
    QString errorString() const;

private:
    bool parseRootElement();
    bool parseRoute(QGeoRoute *route);
    bool parseWaypoint(QGeoRoute *route);
    bool parseCoordinates(QGeoCoordinate &coord);
    bool parseMode(QGeoRoute *route);
    bool parseSummary(QGeoRoute *route);
    bool parseGeoPoints(const QString& strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName);
    bool parseManeuver(QGeoRoute *route);

    bool parseBoundingBox(QGeoBoundingBox *bounds);
    bool parseCoordinate(QGeoCoordinate *coordinate, const QString &elementName);

    QGeoRouteRequest m_request;
    QXmlStreamReader *m_reader;
    QList<QGeoRoute> m_results;
    QString m_errorString;
};

#endif
