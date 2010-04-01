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

#ifndef QLOCATION_MAPTILEREQUEST_H
#define QLOCATION_MAPTILEREQUEST_H

#include <QString>

#include "qmaptile.h"

QTM_BEGIN_NAMESPACE
class QMapTileRequestPrivate;
/*!
* This class represents a request for a specific map tile. By definition, a map is
* divided into 2^currZoomLevel columns and 2^currZoomLevel rows of tiles.
*/
class Q_LOCATION_EXPORT QMapTileRequest
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
    QMapTileRequest();
    /*!
    * Destructor
    */
    ~QMapTileRequest();
    /*!
    * Assignment operator.
    * @param request The request to assign from.
    * @return A reference to this request.
    */
    QMapTileRequest& operator=(const QMapTileRequest& request); //disallow

    /*!
    * @return The col index of the requested map tile.
    */
    quint32 col() const;
    /*!
    * Sets the col index of the requested map tile.
    * Valid values are 0 ... 2^currZoomLevel - 1.
    * @param c The col index.
    */
    void setCol(quint32 c);
    
    /*!
    * @return The row index of the requested map tile.
    */
    quint32 row() const;
    
    /*!
    * Sets the row index of the requested map tile.
    * Valid values are 0 ... 2^currZoomLevel - 1.
    * @param r The row index.
    */
    void setRow(quint32 r);
    
    /*!
    * @return The version of the requested map tile.
    */
    MapVersion version() const;
    
    /*!
    * Sets the version of the requested map tile.
    * @param version The version.
    */
    void setVersion(const MapVersion& version);

    /*!
    * @return The resolution of the requested map tile.
    */
    MapResolution resolution() const;
    
    /*!
    * Sets the resolution of the requested map tile.
    * @param resolution The resolution.
    */
    void setResolution(const MapResolution& resolution);

    /*!
    * @return The format of the requested map tile.
    */
    MapFormat format() const;
    
    /*!
    * Sets the format of the requested map tile.
    * @param format The format.
    */
    void setFormat(const MapFormat& format);

    /*!
    * @return The scheme of the requested map tile.
    */
    MapScheme scheme() const;
    
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
    QMapTileRequestPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QMapTileRequest)
};

QTM_END_NAMESPACE

#endif
