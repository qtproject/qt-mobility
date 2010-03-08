/*
* ==============================================================================
*  Name        : QMapTileReply.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPTILEREPLY_H
#define QLOCATION_MAPTILEREPLY_H

#include <QObject>
#include <QByteArray>
#include <QPointF>
#include <QString>

#include "qmaptilerequest.h"
#include "qgeoreply.h"

namespace QLocation
{
/*!
* This class contains the requested map tile.
*/
class QMapTileReply : public QGeoReply
{
    Q_OBJECT

    friend class QGeoNetworkManager;

public:
    /*!
    * Constructor.
    * @param request The associated \ref QMapTileRequest
    */
    QMapTileReply(const QMapTileRequest& request);

    /*!
    * @return The raw byte array from which a QPixmap, etc. can then be constructed.
    */
    QByteArray& rawData() {
        return data;
    }
    /*!
    * @return A reference to the associated \ref QMapTileRequest.
    */
    const QMapTileRequest& request() const {
        return req;
    }

private:
    Q_DISABLE_COPY(QMapTileReply)

    QByteArray data;
    QMapTileRequest req;
};

}

#endif
