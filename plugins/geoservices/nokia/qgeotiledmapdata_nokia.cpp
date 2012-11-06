/****************************************************************************
 **
 ** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the Qt Mobility Components.
 **
 ** $QT_BEGIN_LICENSE:LGPL$
 ** GNU Lesser General Public License Usage
 ** This file may be used under the terms of the GNU Lesser General Public
 ** License version 2.1 as published by the Free Software Foundation and
 ** appearing in the file LICENSE.LGPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU Lesser
 ** General Public License version 2.1 requirements will be met:
 ** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 **
 ** In addition, as a special exception, Nokia gives you certain additional
 ** rights. These rights are described in the Nokia Qt LGPL Exception
 ** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
 **
 ** GNU General Public License Usage
 ** Alternatively, this file may be used under the terms of the GNU General
 ** Public License version 3.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of this
 ** file. Please review the following information to ensure the GNU General
 ** Public License version 3.0 requirements will be met:
 ** http://www.gnu.org/copyleft/gpl.html.
 **
 ** Other Usage
 ** Alternatively, this file may be used in accordance with the terms and
 ** conditions contained in a signed written agreement between you and Nokia.
 **
 **
 **
 **
 **
 ** $QT_END_LICENSE$
 **
 ** This file is part of the Ovi services plugin for the Maps and
 ** Navigation API.  The use of these services, whether by use of the
 ** plugin or by other means, is governed by the terms and conditions
 ** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
 ** this package, located in the directory containing the Ovi services
 ** plugin source code.
 **
 ****************************************************************************/

#include "qgeotiledmapdata_nokia.h"
#include "jsonparser.h"
#include "qgeomappingmanagerengine_nokia.h"
#include "qgeoboundingbox.h"
#include "qgeocoordinate.h"
#include "qlocationnetworkaccessmanagerfactory.h"

#include <QNetworkAccessManager>
#include <QNetworkProxy>

QTM_USE_NAMESPACE

namespace
{
    void AdjustLogo(const QRect &windowRect, QRect &logoRect, int position)
    {
        logoRect.moveTo(windowRect.topLeft());
        if (position & ShiftedRight)
        {
            logoRect.moveRight(windowRect.x() + windowRect.width());
        }

        if (position & ShiftedDown)
        {
            logoRect.moveBottom(windowRect.y() + windowRect.height());
        }
    }
}

/*!
 Constructs a new tiled map data object, which stores the map data required by
 \a geoMap and makes use of the functionality provided by \a engine.
 */
QGeoTiledMapDataNokia::QGeoTiledMapDataNokia(QGeoMappingManagerEngineNokia *engine) :
    QGeoTiledMapData(engine),
    watermark(":/images/watermark.png"),
    m_logoPosition(engine->logoPosition())
{
    // Get manager from declarative factory or create a new one
    m_networkManager = QLocationNetworkAccessManagerFactory::instance()->create(this);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(copyrightReplyFinished(QNetworkReply*)));

    QString copyrightUrl = "http://";
    if (!engine->firstSubdomain().isNull()) {
        copyrightUrl += engine->firstSubdomain();
        copyrightUrl += ".";
    }
    copyrightUrl += engine->host();
    copyrightUrl += "/maptiler/v2/copyright/newest?output=json";

    if (!engine->token().isEmpty()) {
        copyrightUrl += "&token=";
        copyrightUrl += engine->token();
    }

    if (!engine->applicationId().isEmpty()) {
        copyrightUrl += "&app_id=";
        copyrightUrl += engine->applicationId();
    }

    m_networkManager->get(QNetworkRequest(QUrl(copyrightUrl)));
}

QGeoTiledMapDataNokia::~QGeoTiledMapDataNokia()
{
}

static QGeoBoundingBox variantListToBoundingBox(const QVariantList & list) {
    if (list.size() < 4) return QGeoBoundingBox();

    qreal latTop = list[0].toReal();
    qreal latBottom = list[2].toReal();
    if (latTop < latBottom)
        qSwap(latTop, latBottom);

    return QGeoBoundingBox(QGeoCoordinate(latTop, list[1].toReal()), QGeoCoordinate(latBottom, list[3].toReal()));
}

void QGeoTiledMapDataNokia::copyrightReplyFinished(QNetworkReply * reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << __FUNCTION__ << reply->errorString();
        return;
    }

    JSONParser jp(reply->readAll());
    QVariant root = jp.parse();

    if (!root.isValid())
        return;

    copyrights.clear();

    QVariantHash rootHash = root.toHash();

    foreach (QString key, rootHash.keys()) {
        QList<CopyrightDescriptor> copyrightDescriptorList;
        foreach (QVariant copyrightSource, rootHash[key].toList()) {
            QVariantHash copyrightSourceHash = copyrightSource.toHash();
            CopyrightDescriptor copyrightDescriptor;
            copyrightDescriptor.minLevel = copyrightSourceHash["minLevel"].toReal();
            copyrightDescriptor.maxLevel = copyrightSourceHash["maxLevel"].toReal();
            copyrightDescriptor.label    = copyrightSourceHash["label"].toString();
            copyrightDescriptor.alt      = copyrightSourceHash["alt"].toString();

            foreach (QVariant box, copyrightSourceHash["boxes"].toList()) {
                copyrightDescriptor.boxes << variantListToBoundingBox(box.toList());
            }

            copyrightDescriptorList << copyrightDescriptor;
        }

        copyrights[key] = copyrightDescriptorList;
    }
}

QString QGeoTiledMapDataNokia::getViewCopyright()
{
    QGeoBoundingBox viewport = this->viewport();

    QString terraintype = "normal";

    typedef std::map<QGraphicsGeoMap::MapType, QString> MapTypeRegistry;
    static MapTypeRegistry mapTypes;

    if (mapTypes.empty()) {
        mapTypes[QGraphicsGeoMap::NoMap]                 = "normal";
        mapTypes[QGraphicsGeoMap::TerrainMap]            = "terrain";
        mapTypes[QGraphicsGeoMap::StreetMap]             = "normal";
        mapTypes[QGraphicsGeoMap::SatelliteMapDay]       = "satellite";
        mapTypes[QGraphicsGeoMap::SatelliteMapNight]     = "satellite";
        mapTypes[QGraphicsGeoMap::HybridMap]             = "hybrid";
        mapTypes[QGraphicsGeoMap::TransitMap]            = "normal";
        mapTypes[QGraphicsGeoMap::GrayStreetMap]         = "normal";
        mapTypes[QGraphicsGeoMap::MobileStreetMap]       = "normal";
        mapTypes[QGraphicsGeoMap::MobileTerrainMap]      = "terrain";
        mapTypes[QGraphicsGeoMap::MobileHybridMap]       = "hybrid";
        mapTypes[QGraphicsGeoMap::MobileTransitMap]      = "normal";
        mapTypes[QGraphicsGeoMap::MobileGrayStreetMap]   = "normal";
    }

    MapTypeRegistry::const_iterator it = mapTypes.find(mapType());
    if (it != mapTypes.end()) {
        terraintype = it->second;
    }

    CopyrightDescriptor fallback;

    QStringList copyrightStrings;
    bool contained = false;
    foreach (CopyrightDescriptor copyrightDescriptor, copyrights[terraintype]) {
        if (zoomLevel() < copyrightDescriptor.minLevel) continue;
        if (zoomLevel() > copyrightDescriptor.maxLevel) continue;

        if (copyrightDescriptor.boxes.isEmpty()) {
            fallback = copyrightDescriptor;
        }
        else {
            foreach (QGeoBoundingBox box, copyrightDescriptor.boxes) {
                if (box.intersects(viewport)) {
                    copyrightStrings << copyrightDescriptor.label;
                    if (box.contains(viewport)) {
                        contained = true;
                        break;
                    }
                    // TODO: consider the case where the viewport is fully contained by the combined bounding boxes, but not by one individual bounding box
                }
            }
        }
    }

    if (copyrightStrings.isEmpty() || !contained) {
        if (!fallback.label.isEmpty()) copyrightStrings << fallback.label;
    }

    copyrightStrings.removeDuplicates();

    QString ret = copyrightStrings.join(", ");

    return ret;
}

/*!
 \reimp
 */
void QGeoTiledMapDataNokia::paintProviderNotices(QPainter *painter, const QStyleOptionGraphicsItem *)
{
    QColor fontColor(Qt::white);
    if (mapType() == QGraphicsGeoMap::StreetMap || mapType() == QGraphicsGeoMap::NoMap)
    {
       fontColor = Qt::black;
       fontColor.setAlphaF(0.5);
    }

    QFont font("Arial");
    font.setPixelSize(10);
    font.setStyleHint(QFont::SansSerif);

    painter->save();
    painter->setFont(font);
    painter->setPen(fontColor);

    QRect viewport = painter->viewport();
    const int offset = 5;
    viewport.adjust(offset, offset, -offset, -offset);

    QString copyrightText = getViewCopyright();
    QRect copyrightRect = painter->boundingRect(viewport, Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap, copyrightText);

    QRect watermarkViewRect(viewport), copyrightViewRect(viewport);
    watermarkViewRect.setHeight(watermarkViewRect.height() - copyrightRect.height());
    copyrightViewRect.adjust(0, watermark.height(), 0, 0);

    QRect watermarkRect(watermark.rect());
    AdjustLogo(watermarkViewRect, watermarkRect, m_logoPosition);
    AdjustLogo(copyrightViewRect, copyrightRect, m_logoPosition);

    painter->drawPixmap(watermarkRect, watermark);
    painter->drawText(copyrightRect, Qt::TextWordWrap, copyrightText);

    painter->restore();
}
