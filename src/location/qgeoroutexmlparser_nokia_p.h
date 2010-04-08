
#ifndef QROUTEXMLPARSER_H
#define QROUTEXMLPARSER_H

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
