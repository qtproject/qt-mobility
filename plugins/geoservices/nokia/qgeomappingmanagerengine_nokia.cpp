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
** This file is part of the Ovi services plugin for the Maps and
** Navigation API.  The use of these services, whether by use of the
** plugin or by other means, is governed by the terms and conditions
** described by the file OVI_SERVICES_TERMS_AND_CONDITIONS.txt in
** this package, located in the directory containing the Ovi services
** plugin source code.
**
****************************************************************************/

#include "qgeomappingmanagerengine_nokia.h"
#include "qgeomapreply_nokia.h"
#include "qgeotiledmapdata_nokia.h"
#include "marclanguagecodes.h"

#include <qgeotiledmaprequest.h>

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QNetworkProxy>
#include <QSize>
#include <QDir>
#include <QUrl>
#include <QEventLoop>
#include <QDomDocument>
#include <QScopedPointer>

#include <algorithm>

#define LARGE_TILE_DIMENSION 256

#define DISK_CACHE_MAX_SIZE 50*1024*1024  //50MB

#if defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
#undef DISK_CACHE_ENABLED
#else
#define DISK_CACHE_ENABLED 1
#endif

#if defined(Q_OS_SYMBIAN)
#include <f32file.h>
QChar QGeoMappingManagerEngineNokia::findFirstInternalFlashDrive()
{
    QChar flashDrive;
    RFs fsSession;
    // if something leaves just return an empty QChar
    TRAP_IGNORE(
        User::LeaveIfError(fsSession.Connect());
        CleanupClosePushL(fsSession);
        TDriveList drivelist;
        User::LeaveIfError(fsSession.DriveList(drivelist));
        for (int i = 0; i < KMaxDrives; ++i) {
            if (drivelist[i] != 0) {
                TChar driveChar;
                User::LeaveIfError(RFs::DriveToChar(i, driveChar));
                TDriveInfo driveInfo;
                if (fsSession.Drive(driveInfo, i) != KErrNone)
                    continue;
                if ((driveInfo.iDriveAtt & KDriveAttInternal) && driveInfo.iType == EMediaHardDisk) {
                    flashDrive = QChar(driveChar);
                    break;
                }
            }
        }
        CleanupStack::PopAndDestroy(&fsSession);
    )
    return flashDrive;
}
#endif //Q_OS_SYMBIAN

namespace
{
    void getMapTypes(const QDomDocument &doc, QList<QGraphicsGeoMap::MapType> &mapTypes)
    {
        typedef std::map<QString, QGraphicsGeoMap::MapType> MapTypeRegistry;
        static MapTypeRegistry registeredTypes;
        if (registeredTypes.empty())
        {

            registeredTypes.insert(std::make_pair("terrain.day", QGraphicsGeoMap::TerrainMap));
            registeredTypes.insert(std::make_pair("normal.day", QGraphicsGeoMap::StreetMap));
            registeredTypes.insert(std::make_pair("satellite.day", QGraphicsGeoMap::SatelliteMapDay));

            //registeredTypes.insert(std::make_pair("hybrid.day", QGraphicsGeoMap::));
            //registeredTypes.insert(std::make_pair("normal.day.transit", QGraphicsGeoMap::));
            //registeredTypes.insert(std::make_pair("normal.day.grey", QGraphicsGeoMap::));
        }

        QDomElement response  = doc.firstChildElement("response");
        QDomNodeList schemes = response.firstChildElement("schemes").childNodes();
        for (int i = 0; i < schemes.count(); ++i)
        {
            QString id = schemes.at(i).toElement().attribute("id");
            MapTypeRegistry::const_iterator found = registeredTypes.find(id);
            if (found != registeredTypes.end())
            {
                mapTypes << found->second;
            }
        }

        if (mapTypes.empty())
        {
            mapTypes << QGraphicsGeoMap::NoMap;
        }
    }

    void getResolutions(const QDomDocument &doc, QList<QSize> &sizes)
    {
        QDomElement response  = doc.firstChildElement("response");
        QDomNodeList resolutions = response.firstChildElement("resolutions").childNodes();

        for (int i = 0; i < resolutions.count(); ++i)
        {
           QDomElement size = resolutions.at(i).toElement();
           sizes << QSize(size.attribute("width").toInt(), size.attribute("height").toInt());
        }
    }

    void getZoomLevels(const QDomDocument &doc, double &minZoomLevel, double &maxZoomLevel)
    {
        QDomElement response  = doc.firstChildElement("response");

        QDomElement zoomLevels = response.firstChildElement("zoomLevels");
        minZoomLevel = zoomLevels.attribute("min").toDouble();
        maxZoomLevel = zoomLevels.attribute("max").toDouble();
    }

    bool less(const QSize &first, const QSize &second)
    {
        return first.width() * first.height() < second.width() * second.height();
    }

    int GetLogoPosition(QString positionName)
    {
        typedef std::map<QString, int> KnownPositionsType;
        static KnownPositionsType knownPositions;
        if (knownPositions.empty())
        {
            knownPositions.insert(std::make_pair("top.left", TopLeft));
            knownPositions.insert(std::make_pair("left.top", TopLeft));
            knownPositions.insert(std::make_pair("top.right", ShiftedRight));
            knownPositions.insert(std::make_pair("right.top", ShiftedRight));
            knownPositions.insert(std::make_pair("bottom.left", ShiftedRight | ShiftedDown));
            knownPositions.insert(std::make_pair("left.bottom", ShiftedRight | ShiftedDown));
            knownPositions.insert(std::make_pair("bottom.right", ShiftedDown));
            knownPositions.insert(std::make_pair("right.bottom", ShiftedDown));
        }

        KnownPositionsType::const_iterator found = knownPositions.find(positionName);
        return found == knownPositions.end() ? ShiftedDown : found->second;
    }
}

QGeoMappingManagerEngineNokia::QGeoMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoTiledMappingManagerEngine(parameters),
        m_cache(0),
        m_host("maptile.maps.svc.ovi.com"),
        m_token(QGeoServiceProviderFactoryNokia::defaultToken),
        m_referer(QGeoServiceProviderFactoryNokia::defaultReferer),
        m_logoPosition(ShiftedDown)
{
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    QList<QGraphicsGeoMap::ConnectivityMode> modes;
    modes << QGraphicsGeoMap::OnlineMode;
    setSupportedConnectivityModes(modes);

    m_networkManager = new QNetworkAccessManager(this);

    if (parameters.contains("mapping.proxy")) {
        QString proxy = parameters.value("mapping.proxy").toString();
        if (!proxy.isEmpty()) {
            QUrl proxyUrl(proxy);
            if (proxyUrl.isValid()) {
                m_networkManager->setProxy(QNetworkProxy(QNetworkProxy::HttpProxy,
                    proxyUrl.host(),
                    proxyUrl.port(8080),
                    proxyUrl.userName(),
                    proxyUrl.password()));
            }
        }
    }

    if (parameters.contains("mapping.host")) {
        QString host = parameters.value("mapping.host").toString();
        if (!host.isEmpty())
            m_host = host;
    }

    if (parameters.contains("mapping.referer")) {
        m_referer = parameters.value("mapping.referer").toString();
    }

    if (parameters.contains("mapping.token")) {
        m_token = parameters.value("mapping.token").toString();
    }
    else if (parameters.contains("token")) {
        m_token = parameters.value("token").toString();
    }

    if (parameters.contains("logo.position"))
    {
        m_logoPosition =  GetLogoPosition(parameters.value("logo.position").toString());
    }

     setupServiceInfo();
#ifdef DISK_CACHE_ENABLED
    QString cacheDir;
    if (parameters.contains("mapping.cache.directory"))
        cacheDir = parameters.value("mapping.cache.directory").toString();

    if (cacheDir.isEmpty()) {
#if defined(Q_OS_SYMBIAN)
        QChar driveLetter(findFirstInternalFlashDrive());
        if (!driveLetter.isNull()) {
            cacheDir = driveLetter;
            cacheDir += ":/data/nokia/maptiles";
        }
#else
        cacheDir = QDir::temp().path()+"/maptiles";
#endif
    }
    if (!cacheDir.isEmpty()) {
        m_cache = new QNetworkDiskCache(this);
        QDir dir;
        dir.mkpath(cacheDir);
        dir.setPath(cacheDir);

        m_cache->setCacheDirectory(dir.path());

        if (parameters.contains("mapping.cache.size")) {
            bool ok = false;
            qint64 cacheSize = parameters.value("mapping.cache.size").toString().toLongLong(&ok);
            if (ok)
                m_cache->setMaximumCacheSize(cacheSize);
        }

        if (m_cache->maximumCacheSize() > DISK_CACHE_MAX_SIZE)
            m_cache->setMaximumCacheSize(DISK_CACHE_MAX_SIZE);

        m_networkManager->setCache(m_cache);
    }
#endif

#ifdef USE_CHINA_NETWORK_REGISTRATION
    connect(&m_networkInfo, SIGNAL(currentMobileCountryCodeChanged(const QString&)), SLOT(currentMobileCountryCodeChanged(const QString&)));
    currentMobileCountryCodeChanged(m_networkInfo.currentMobileCountryCode());
#endif
}

QGeoMappingManagerEngineNokia::~QGeoMappingManagerEngineNokia() {}

QGeoMapData* QGeoMappingManagerEngineNokia::createMapData()
{
    QGeoMapData *data = new QGeoTiledMapDataNokia(this);
    if (!data)
        return 0;

    data->setConnectivityMode(QGraphicsGeoMap::OnlineMode);
    return data;
}

QGeoTiledMapReply* QGeoMappingManagerEngineNokia::getTileImage(const QGeoTiledMapRequest &request)
{
    // TODO add error detection for if request.connectivityMode() != QGraphicsGeoMap::OnlineMode
    QString rawRequest = getRequestString(request);

    QNetworkRequest netRequest((QUrl(rawRequest))); // The extra pair of parens disambiguates this from a function declaration
    netRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

#ifdef DISK_CACHE_ENABLED
    netRequest.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
#endif

    QNetworkReply* netReply = m_networkManager->get(netRequest);

    QGeoTiledMapReply* mapReply = new QGeoMapReplyNokia(netReply, request);

    // TODO goes badly on linux
    //qDebug() << "request: " << QString::number(reinterpret_cast<int>(mapReply), 16) << " " << request.row() << "," << request.column();
    // this one might work better. It follows defined behaviour, unlike reinterpret_cast
    //qDebug("request: %p %i,%i @ %i", mapReply, request.row(), request.column(), request.zoomLevel());
    return mapReply;
}

QString QGeoMappingManagerEngineNokia::getRequestString(const QGeoTiledMapRequest &request) const
{
    const int maxDomains = 11; // TODO: hmmm....
    const char subdomain = 'a' + (request.row() + request.column()) % maxDomains; // a...k

    static const QString http("http://");
    static const QString path("/maptiler/maptile/newest/");
    static const QChar dot('.');
    static const QChar slash('/');

    QString requestString = http;
    if ("maptile.maps.svc.ovi.com" == m_host) // TODO: temporay solution while china DNS does not work
    {
        requestString += subdomain;
        requestString += dot;
    }

    requestString += m_host;
    requestString += path;
    requestString += mapTypeToStr(request.mapType());
    requestString += slash;
    requestString += QString::number(request.zoomLevel());
    requestString += slash;
    requestString += QString::number(request.column());
    requestString += slash;
    requestString += QString::number(request.row());
    requestString += slash;
    requestString += sizeToStr(tileSize());
//#if defined(Q_OS_SYMBIAN) || defined(Q_OS_WINCE_WM) || defined(Q_WS_MAEMO_5) || defined(Q_WS_MAEMO_6)
    static const QString slashpng("/png8");
//#else
//    static const QString slashpng("/png");
//#endif
    requestString += slashpng;

    requestString += "?lg=";
    requestString += languageToMarc(locale().language());

    if (!m_token.isEmpty()) {
        requestString += "&token=";
        requestString += m_token;
    }

    if (!m_referer.isEmpty()) {

        requestString += "&referer=";
        requestString += m_referer;
    }

    return requestString;
}

QString QGeoMappingManagerEngineNokia::sizeToStr(const QSize &size)
{
    static const QString s256("256");
    static const QString s128("128");
    if (size.height() >= LARGE_TILE_DIMENSION ||
            size.width() >= LARGE_TILE_DIMENSION)
        return s256;
    else
        return s128;
}

QString QGeoMappingManagerEngineNokia::mapTypeToStr(QGraphicsGeoMap::MapType type)
{
    if (type == QGraphicsGeoMap::StreetMap)
        return "normal.day";
    else if (type == QGraphicsGeoMap::SatelliteMapDay ||
             type == QGraphicsGeoMap::SatelliteMapNight) {
        return "satellite.day";
    } else if (type == QGraphicsGeoMap::TerrainMap)
        return "terrain.day";
    else
        return "normal.day";
}

void QGeoMappingManagerEngineNokia::setupServiceInfo()
{
    QSize maxResolution(256, 256);
    double minZoomLevel(0.0), maxZoomLevel(18.0);
    QList<QGraphicsGeoMap::MapType> types;
    types << QGraphicsGeoMap::StreetMap;
    types << QGraphicsGeoMap::SatelliteMapDay;
    types << QGraphicsGeoMap::TerrainMap;

    QString path = "http://";
    path += m_host;
    path += "/maptiler/info";

    QScopedPointer<QNetworkReply> reply(m_networkManager->get(QNetworkRequest((QUrl(path)))));

    QEventLoop loop;
    connect(reply.data(), SIGNAL(finished()), &loop, SLOT(quit()));
    connect(reply.data(), SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));
    loop.exec();

    if (QNetworkReply::NoError == reply->error())
    {
        QDomDocument doc;
        if (doc.setContent(reply->readAll()))
        {           
            getZoomLevels(doc, minZoomLevel, maxZoomLevel);

            types.clear();
            getMapTypes(doc, types);

            QList<QSize> resolutions;
            getResolutions(doc, resolutions);
            QList<QSize>::const_iterator found = std::max_element(resolutions.begin(), resolutions.end(), &less);
            if (found != resolutions.end())
            {
                maxResolution = *found;
            }
        }
    }

    setTileSize(maxResolution);
    setMinimumZoomLevel(minZoomLevel);
    setMaximumZoomLevel(maxZoomLevel);
    setSupportedMapTypes(types);
}

int QGeoMappingManagerEngineNokia::logoPosition() const
{
    return m_logoPosition;
}

const QString & QGeoMappingManagerEngineNokia::host() const
{
    return m_host;
}

void QGeoMappingManagerEngineNokia::currentMobileCountryCodeChanged(const QString & mcc)
{
    if (mcc == "460" || mcc == "461") {
        m_host ="211.151.53.179";
    } else {
        m_host ="maptile.maps.svc.ovi.com";
    }
}
