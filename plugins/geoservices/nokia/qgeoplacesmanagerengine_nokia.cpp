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

#include "qgeoplacesmanagerengine_nokia_p.h"
#include "qgeoplacesreply_nokia_p.h"

#include <QGeoAddress>
#include <QGeoCoordinate>
#include <QNetworkProxy>
#include <QMap>

QGeoPlacesManagerEngineNokia::QGeoPlacesManagerEngineNokia(const QMap<QString, QString> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoPlacesManagerEngine(parameters),
        m_host("loc.desktop.maps.svc.ovi.com")
{
    m_networkManager = new QNetworkAccessManager(this);

    QList<QString> keys = parameters.keys();

    if (keys.contains("places.proxy")) {
        QString proxy = parameters.value("places.proxy");
        if (!proxy.isEmpty())
            m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy, proxy, 8080));
    }

    if (keys.contains("places.host")) {
        QString host = parameters.value("places.host");
        if (!host.isEmpty())
            m_host = host;
    }

    setSupportsGeocoding(true);
    setSupportsViewportBiasing(false);

    QGeoPlacesManager::SearchTypes supportedSearchTypes;
    supportedSearchTypes |= QGeoPlacesManager::SearchGeocode;
    setSupportedSearchTypes(supportedSearchTypes);

    if (error)
        *error = QGeoServiceProvider::NoError;

    if (errorString)
        *errorString = "";
}

QGeoPlacesManagerEngineNokia::~QGeoPlacesManagerEngineNokia() {}

QGeoPlacesReply* QGeoPlacesManagerEngineNokia::geocode(const QGeoAddress &address, const QGeoBoundingBox &bounds)
{
    Q_UNUSED(bounds)

    if (!supportsGeocoding()) {
        QGeoPlacesReply *reply = new QGeoPlacesReply(QGeoPlacesReply::UnsupportedOptionError, "Geocoding is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/gc/1.0?referer=localhost";

    // TODO locale / language handling
    //requestString += "&lg=";
    //requestString += request->locale().language();

    requestString += "&country=";
    requestString += address.country();

    if (!address.state().isEmpty()) {
        requestString += "&state=";
        requestString += address.state();
    }

    if (!address.city().isEmpty()) {
        requestString += "&city=";
        requestString += address.city();
    }

    if (!address.postCode().isEmpty()) {
        requestString += "&zip=";
        requestString += address.postCode();
    }

    if (!address.thoroughfareName().isEmpty()) {
        requestString += "&street=";
        requestString += address.thoroughfareName();
    }

    if (!address.thoroughfareNumber().isEmpty()) {
        requestString += "&number=";
        requestString += address.thoroughfareNumber();
    }

    return search(requestString);
}

QGeoPlacesReply* QGeoPlacesManagerEngineNokia::geocode(const QGeoCoordinate &coordinate, const QGeoBoundingBox &bounds)
{
    Q_UNUSED(bounds)

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/rgc/1.0?referer=localhost";
    requestString += "&long=";
    requestString += trimDouble(coordinate.longitude());
    requestString += "&lat=";
    requestString += trimDouble(coordinate.latitude());

    // TODO locale / language handling
    //requestString += "&lg=";
    //requestString += request->locale().language();

    return search(requestString);
}

QGeoPlacesReply* QGeoPlacesManagerEngineNokia::placesSearch(const QString &searchString, QGeoPlacesManager::SearchTypes searchTypes, const QGeoBoundingBox &bounds)
{
    // NOTE this will eventually replaced by a much improved implementation
    // which will make use of the additionLandmarkManagers()
    if ((searchTypes != QGeoPlacesManager::SearchTypes(QGeoPlacesManager::SearchAll))
            && ((searchTypes & supportedSearchTypes()) != searchTypes)) {

        QGeoPlacesReply *reply = new QGeoPlacesReply(QGeoPlacesReply::UnsupportedOptionError, "The selected search type is not supported by this service provider.", this);
        emit error(reply, reply->error(), reply->errorString());
        return reply;
    }

    Q_UNUSED(bounds)

    QString requestString = "http://";
    requestString += m_host;
    requestString += "/geocoder/gc/1.0?referer=localhost";

    // TODO locale / language handling
    //requestString += "&lg=";
    //requestString += request->locale().language();

    requestString += "&obloc=";
    requestString += addressString;

    return search(requestString);
}

QGeoPlacesReply* QGeoPlacesManagerEngineNokia::search(QString requestString)
{
    QNetworkReply *networkReply = m_networkManager->get(QNetworkRequest(QUrl(requestString)));
    QGeoPlacesReplyNokia *reply = new QGeoPlacesReplyNokia(networkReply, this);

    connect(reply,
            SIGNAL(finished()),
            this,
            SLOT(placesFinished()));

    connect(reply,
            SIGNAL(error(QGeoPlacesReply::Error, QString)),
            this,
            SLOT(placesError(QGeoPlacesReply::Error, QString)));

    return reply;
}

QString QGeoPlacesManagerEngineNokia::trimDouble(qreal degree, int decimalDigits)
{
    QString sDegree = QString::number(degree, 'g', decimalDigits);

    int index = sDegree.indexOf('.');

    if (index == -1)
        return sDegree;
    else
        return QString::number(degree, 'g', decimalDigits + index);
}

void QGeoPlacesManagerEngineNokia::placesFinished()
{
    QGeoPlacesReply *reply = qobject_cast<QGeoPlacesReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(finished(QGeoPlacesReply*))) == 0) {
        reply->deleteLater();
        return;
    }

    emit finished(reply);
}

void QGeoPlacesManagerEngineNokia::placesError(QGeoPlacesReply::Error error, const QString &errorString)
{
    QGeoPlacesReply *reply = qobject_cast<QGeoPlacesReply*>(sender());

    if (!reply)
        return;

    if (receivers(SIGNAL(error(QGeoPlacesReply*, QGeoPlacesReply::Error, QString))) == 0) {
        reply->deleteLater();
        return;
    }

    emit this->error(reply, error, errorString);
}
