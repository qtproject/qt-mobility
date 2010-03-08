/*
* ==============================================================================
*  Name        : QMapTileRequest.h
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#ifndef QLOCATION_MAPTILEREQUEST_H
#define QLOCATION_MAPTILEREQUEST_H

#include <QString>

#include "qmaptile.h"

namespace QLocation
{
/*!
* This class represents a request for a specific map tile. By definition, a map is
* divided into 2^currZoomLevel columns and 2^currZoomLevel rows of tiles.
*/
class QMapTileRequest
{
public:
    QMapTileRequest(const MapVersion& mapVersion,
                    const MapScheme& mapScheme,
                    const MapResolution& mapResolution,
                    const MapFormat& mapFormat);
    /*!
    * Copy constructor.
    * @param request The request to be copied.
    */
    QMapTileRequest(const QMapTileRequest& request);
    /*!
    * Default constructor
    */
    QMapTileRequest() {}
    /*!
    * Assignment operator.
    * @param request The request to assign from.
    * @return A reference to this request.
    */
    QMapTileRequest& operator=(const QMapTileRequest& request); //disallow

    /*!
    * @return The col index of the requested map tile.
    */
    quint32 col() const {
        return cl;
    }
    /*!
    * Sets the col index of the requested map tile.
    * Valid values are 0 ... 2^currZoomLevel - 1.
    * @param c The col index.
    */
    void setCol(quint32 c) {
        cl = c;
    }

    /*!
    * @return The row index of the requested map tile.
    */
    quint32 row() const {
        return rw;
    }
    /*!
    * Sets the row index of the requested map tile.
    * Valid values are 0 ... 2^currZoomLevel - 1.
    * @param r The row index.
    */
    void setRow(quint32 r) {
        rw = r;
    }

    /*!
    * @return The version of the requested map tile.
    */
    MapVersion version() const {
        return ver;
    }
    /*!
    * Sets the version of the requested map tile.
    * @param version The version.
    */
    void setVersion(const MapVersion& version);

    /*!
    * @return The resolution of the requested map tile.
    */
    MapResolution resolution() const {
        return res;
    }
    /*!
    * Sets the resolution of the requested map tile.
    * @param resolution The resolution.
    */
    void setResolution(const MapResolution& resolution);

    /*!
    * @return The format of the requested map tile.
    */
    MapFormat format() const {
        return frmt;
    }
    /*!
    * Sets the format of the requested map tile.
    * @param format The format.
    */
    void setFormat(const MapFormat& format);

    /*!
    * @return The scheme of the requested map tile.
    */
    MapScheme scheme() const {
        return schm;
    }
    /*!
    * Sets the scheme of the requested map tile.
    * @param scheme The scheme.
    */
    void setScheme(const MapScheme& scheme);

    /*!
    * @return The zoom level of the requested map tile.
    */
    quint16 zoomLevel() const;
    /*!
    * Sets the zoom level of the requested map tile.
    * @param level The zoom level.
    */
    void setZoomLevel(quint16 level);

private:
    quint32 cl;
    quint32 rw;
    MapVersion ver;
    quint16 zoom;
    MapScheme schm;
    MapResolution res;
    MapFormat frmt;
};

}

#endif
