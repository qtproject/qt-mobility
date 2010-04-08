#ifndef QGEOCODINGXMLPARSER_H
#define QGEOCODINGXMLPARSER_H

#include "qmobilityglobal.h"

#include <QString>

class QXmlStreamReader;
class QIODevice;
class QRectF;

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeocodingReplyNokia;
class QGeoLocation;
class QGeoAddress;

class QGeocodingXmlParserNokia
{
public:
    QGeocodingXmlParserNokia();
    ~QGeocodingXmlParserNokia();

    bool parse(QIODevice* source, QGeocodingReplyNokia *output);
    QString errorString() const;

private:
    bool parseRootElement(QGeocodingReplyNokia *output);
    bool parsePlace(QGeoLocation *location);
    bool parseLocation(QGeoLocation *location);
    bool parseAddress(QGeoAddress *address);
    bool parseBoundingBox(QRectF *rect);
    bool parseCoordinate(QGeoCoordinate *coordinate, const QString &elementName);

    QXmlStreamReader *m_reader;
    QString m_errorString;
};

QTM_END_NAMESPACE

#endif
