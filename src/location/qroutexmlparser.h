
#ifndef QROUTEXMLPARSER_H
#define QROUTEXMLPARSER_H

#include "qmobilityglobal.h"
#include <QList>

class QXmlStreamReader;
class QIODevice;
class QRectF;
class QDateTime;

QTM_BEGIN_NAMESPACE

class QRouteReply;
class QRoute;
class QGeoCoordinate;
class QManeuver;

class QRouteXmlParser
{

public:

    QRouteXmlParser();
    ~QRouteXmlParser();

    bool parse(QIODevice* source, QRouteReply *output);

private:
    bool readRootElement(QRouteReply *output);
    bool readRoute(QRoute *route);
    bool readXsdDateTime(const QString& strDateTime, QDateTime *dateTime, const QString &attributeName);
    bool readXsdDuration(const QString& strDuration, qint32 *durationSeconds, const QString &attributeName);
    bool readManeuver(QManeuver *maneuver);
    bool readGeoPoints(const QString& strPoints, QList<QGeoCoordinate> *geoPoints, const QString &elementName);
    bool readBoundingBox(QRectF *rect);
    bool readCoordinate(QGeoCoordinate *coordinate, const QString &elementName);

    QXmlStreamReader *m_reader;
};

QTM_END_NAMESPACE

#endif
