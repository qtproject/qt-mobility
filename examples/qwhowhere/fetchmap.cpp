/*
 * Copyright (c) 2010 Nokia Corporation.
 */

#include "fetchmap.h"
#include <QDesktopServices>
#include <QImage>

// Uses same Google Map API key than qtmobility\examples\fetchgooglemaps
const QString
GMAPS_STATICMAP_URL_TEMPLATE_You =
    "http://maps.google.com/maps/api/staticmap?center=%1,%2&zoom=14&size=%3x%4&maptype=mobile&markers=color:blue|label:Y|%1,%2&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";

const QString
GMAPS_STATICMAP_URL_TEMPLATE_You_and_Friend =
    "http://maps.google.com/maps/api/staticmap?center=%1,%2&size=%3x%4&maptype=mobile&markers=color:blue|label:Y|%1,%2&markers=color:red|label:F|%5,%6&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";

FetchMap::FetchMap(QObject *parent) :
        QObject(parent)
{
    QObject::connect(&m_manager, SIGNAL(finished(QNetworkReply*)),
                     this, SLOT(handleNetworkData(QNetworkReply*)));
}

FetchMap::~FetchMap()
{
}

void FetchMap::fetchMapImage(const QSize& size, qreal yourLong, qreal yourLat, qreal friendLong,
                             qreal friendLat)
{
    QString url;
    if (friendLong == 0 || friendLat == 0) {
        url = GMAPS_STATICMAP_URL_TEMPLATE_You .arg(QString::number(yourLat)) .arg(QString::number(
                    yourLong)) .arg(QString::number(size.width())) .arg(QString::number(size.height()));
    } else {
        url = GMAPS_STATICMAP_URL_TEMPLATE_You_and_Friend .arg(QString::number(yourLat)) .arg(
                  QString::number(yourLong)) .arg(QString::number(size.width())) .arg(QString::number(
                              size.height())) .arg(QString::number(friendLat)) .arg(QString::number(friendLong));
    }

    m_url = QUrl(url);
    QNetworkRequest request;
    request.setUrl(m_url);
    m_manager.get(request);
}

void FetchMap::handleNetworkData(QNetworkReply *reply)
{
    // Get image from the reply
    QImage img;
    if (reply->error() == QNetworkReply::NoError) {
        img.load(reply, 0);
    }

    // Delete reply
    reply->deleteLater();

    // Emit image
    if (!img.isNull()) {
        QPixmap p = QPixmap::fromImage(img);
        emit mapImageLoaded(p);
    }
}
