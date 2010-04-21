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

#ifndef QROUTEXMLPARSER_H
#define QROUTEXMLPARSER_H

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

#include "qmobilityglobal.h"
#include <QList>
#include <QString>

class QXmlStreamReader;
class QIODevice;
class QRectF;
class QDateTime;

QTM_BEGIN_NAMESPACE

class QGeoRouteReplyNokia;
class QGeoRoute;
class QGeoCoordinate;
class QManeuver;

class QGeoRouteXmlParserNokia
{

public:

    QGeoRouteXmlParserNokia();
    ~QGeoRouteXmlParserNokia();

    bool parse(QIODevice* source, QGeoRouteReplyNokia *output);
    QString errorString() const;

private:
    bool parseRootElement(QGeoRouteReplyNokia *output);
    bool parseRoute(QGeoRoute *route);
    bool parseXsdDateTime(const QString& strDateTime, QDateTime *dateTime, const QString &attributeName);
    bool parseXsdDuration(const QString& strDuration, qint32 *durationSeconds, const QString &attributeName);
    bool parseManeuver(QManeuver *maneuver);
    bool parseGeoPoints(const QString& strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName);
    bool parseBoundingBox(QRectF *rect);
    bool parseCoordinate(QGeoCoordinate *coordinate, const QString &elementName);

    QXmlStreamReader *m_reader;
    QString m_errorString;
};

QTM_END_NAMESPACE

#endif
