#ifndef QGEOCODINGXMLPARSER_H
#define QGEOCODINGXMLPARSER_H

#include "qmobilityglobal.h"

class QXmlStreamReader;
class QIODevice;
class QRectF;

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;
class QGeocodingReply;
class QGeoLocation;
class QAddress;

class QGeocodingXmlParser
{

public:

    QGeocodingXmlParser();
    ~QGeocodingXmlParser();

    bool parse(QIODevice* source, QGeocodingReply *output);

private:
    bool readRootElement(QGeocodingReply *output);
    bool readPlace(QGeoLocation *location);
    bool readLocation(QGeoLocation *location);
    bool readAddress(QAddress *address);
    bool readBoundingBox(QRectF *rect);
    bool readCoordinate(QGeoCoordinate *coordinate, const QString &elementName);

    QXmlStreamReader *m_reader;
};

QTM_END_NAMESPACE

#endif
