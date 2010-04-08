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

#ifndef QLOCATION_BASE_H
#define QLOCATION_BASE_H

#include "qmobilityglobal.h"

#include <QString>
#include <QSize>
#include <QPointF>
#include <QPixmap>

QTM_BEGIN_NAMESPACE

/*!
* This struct represents a map version.
*/
struct MapVersion {
    //! Default map versions.
    enum DefaultVersion {
        Newest //!< The newest (latest) map version.
    };

    /*!
    * Constructor
    */
    MapVersion() {}
    /*!
    * Constructs a default map version.
    * @param The value indicating the desired default map version.
    */
    MapVersion(const DefaultVersion /*version*/) {
        id = "newest";
        newest = true;
        region = "all";
    }

    QString id; //!< The id of this map version.
    bool newest; //!< Flag indicating whether this is the newest version.
    QString region; //!< The region where the map is valid (currently: all or china).
};

/*!
* This struct represents a concrete map resolution.
* The map resolution defines the size of the individual map tiles
* that comprise the map.
*/
struct MapResolution {
    //! Default map resolutions.
    enum DefaultResolution {
        Res_128_128, //!< 128x128 pixel map tiles.
        Res_256_256 //!< 256x256 pixel map tiles.
    };

    /*!
    * Constructor.
    */
    MapResolution() {}
    /*!
    * Constructs a default map resolution.
    * @param resolution The value indicating the desired default map resolution.
    */
    MapResolution(const DefaultResolution resolution) {
        if (resolution == Res_128_128) {
            id = "128";
            size = QSize(128, 128);
        } else if (resolution == Res_256_256) {
            id = "256";
            size = QSize(256, 256);
        }
    }

    QString id; //!< The id of this map resolution.
    QSize size; //!< The size of a map tile (in pixels).
};

/*!
* This struct represents a concrete map format.
* The map format defines the format of the individual map tiles
* that comprise the map.
*/
struct MapFormat {
    //! Default map formats.
    enum DefaultMapFormat {
        Png, //!< PNG
        Png8 //<! PNG8
    };

    /*!
    * Constructor.
    */
    MapFormat() {}
    /*!
    * Constructs a default map format.
    * @param format The value indicating the desired default map format.
    */
    MapFormat(const DefaultMapFormat format) {
        if (format == Png) {
            id = "png";
            bbp = 24;
        } else if (format == Png8) {
            id = "png8";
            bbp = 8;
        }

        encoding = "png";
    }

    QString id; //!< The id of this map format.
    quint16 bbp; //!< Bytes per pixel.
    QString encoding; //!< The image encoding format.
};

/*!
* This struct represents a concrete map scheme.
* The map scheme defines the scheme of the individual map tiles
* that comprise the map.
*/
struct MapScheme {
    enum DefaultScheme {
        Normal_Day, //!< Normal daylight map images.
        Satellite_Day, //!< daylight satellite images.
        Terrain_Day //!< Daylight terrain map images.
    };

    /*!
    * Constructor.
    */
    MapScheme() {}
    /*!
    * Constructs a default map scheme.
    * @param scheme The value indicating the desired default map scheme.
    */
    MapScheme(const DefaultScheme scheme) {
        if (scheme == Normal_Day)
            id = "normal.day";
        else if (scheme == Satellite_Day)
            id = "satellite.day";
        else if (scheme == Terrain_Day)
            id = "terrain.day";
    }

    QString id; //!< The id of this map scheme.
};

QTM_END_NAMESPACE

#endif
