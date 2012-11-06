/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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
#include "qlocationnetworkaccessmanagerfactory.h"

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
        if (registeredTypes.empty()){
            registeredTypes["terrain.day"] = QGraphicsGeoMap::TerrainMap;
            registeredTypes["normal.day"] = QGraphicsGeoMap::StreetMap;
            registeredTypes["satellite.day"] = QGraphicsGeoMap::SatelliteMapDay;
            registeredTypes["hybrid.day"] = QGraphicsGeoMap::HybridMap;
            registeredTypes["normal.day.transit"] = QGraphicsGeoMap::TransitMap;
            registeredTypes["normal.day.grey"] = QGraphicsGeoMap::GrayStreetMap;
            registeredTypes["normal.day.mobile"] = QGraphicsGeoMap::MobileStreetMap;
            registeredTypes["terrain.day.mobile"] = QGraphicsGeoMap::MobileTerrainMap;
            registeredTypes["hybrid.day.mobile"] = QGraphicsGeoMap::MobileHybridMap;
            registeredTypes["normal.day.transit.mobile"] = QGraphicsGeoMap::MobileTransitMap;
            registeredTypes["normal.day.grey.mobile"] = QGraphicsGeoMap::MobileGrayStreetMap;
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
        if (knownPositions.empty()) {
            knownPositions["top.left"] = TopLeft;
            knownPositions["left.top"] = TopLeft;
            knownPositions["top.right"] = ShiftedRight;
            knownPositions["right.top"] = ShiftedRight;
            knownPositions["bottom.left"] = ShiftedDown;
            knownPositions["left.bottom"] = ShiftedDown;
            knownPositions["bottom.right"] = ShiftedRight | ShiftedDown;
            knownPositions["right.bottom"] = ShiftedRight | ShiftedDown;
        }

        KnownPositionsType::const_iterator found = knownPositions.find(positionName);
        return found == knownPositions.end() ? ShiftedDown : found->second;
    }
    
    const char* MAPTILES_HOST = "1-4.maptile.lbs.ovi.com";
    const char* MAPTILES_HOST_CN = "a-k.maptile.maps.svc.nokia.com.cn";
}

QGeoMappingManagerEngineNokia::QGeoMappingManagerEngineNokia(const QMap<QString, QVariant> &parameters, QGeoServiceProvider::Error *error, QString *errorString)
        : QGeoTiledMappingManagerEngine(parameters),
        m_cache(0),
        m_token(QGeoServiceProviderFactoryNokia::defaultToken),
        m_referer(QGeoServiceProviderFactoryNokia::defaultReferer),
        m_logoPosition(ShiftedDown),
        m_firstSubdomain(QChar::Null),
        m_maxSubdomains(0)
{
    Q_UNUSED(error)
    Q_UNUSED(errorString)

    setHost(MAPTILES_HOST);

    QList<QGraphicsGeoMap::ConnectivityMode> modes;
    modes << QGraphicsGeoMap::OnlineMode;
    setSupportedConnectivityModes(modes);

    // Get manager from declarative factory or create a new one
    m_networkManager = QLocationNetworkAccessManagerFactory::instance()->create(this);

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
            setHost(host);
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

    if (parameters.contains("mapping.app_id")) {
        m_applicationId = parameters.value("mapping.app_id").toString();
    }
    else if (parameters.contains("app_id")) {
        m_applicationId = parameters.value("app_id").toString();
    }

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

    if (!isValidParameter(m_applicationId) || !isValidParameter(m_token)) {
        qWarning() << "********************************************************************************";
        qWarning() << "Qt Location requires usage of app_id and token parameters obtained from:";
        qWarning() << "https://api.forum.nokia.com/ovi-api/ui/registration";
        qWarning() << "For more information about app_id and token please consult:";
        qWarning() << "http://doc.qt.nokia.com/qtmobility-latest/location-overview.html#the-nokia-plugin";
        qWarning() << "********************************************************************************";
    }

    setupServiceInfo();
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
    const char subdomain = m_maxSubdomains ? m_firstSubdomain.toAscii() + (request.row() + request.column()) % m_maxSubdomains : 0;

    static const QString http("http://");
    static const QString path("/maptiler/v2/maptile/newest/");
    static const QChar dot('.');
    static const QChar slash('/');

    QString requestString = http;

    if (subdomain != 0) {
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

    if (!m_applicationId.isEmpty()) {
        requestString += "&app_id=";
        requestString += m_applicationId;
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
    typedef std::map<QGraphicsGeoMap::MapType, QString> MapTypeRegistry;
    static MapTypeRegistry registeredTypes;

    if (registeredTypes.empty()) {
        registeredTypes[QGraphicsGeoMap::NoMap] = "normal.day";
        registeredTypes[QGraphicsGeoMap::TerrainMap] = "terrain.day";
        registeredTypes[QGraphicsGeoMap::StreetMap] = "normal.day";
        registeredTypes[QGraphicsGeoMap::SatelliteMapDay] = "satellite.day";
        registeredTypes[QGraphicsGeoMap::SatelliteMapNight] = "satellite.day";
        registeredTypes[QGraphicsGeoMap::HybridMap] = "hybrid.day";
        registeredTypes[QGraphicsGeoMap::TransitMap] = "normal.day.transit";
        registeredTypes[QGraphicsGeoMap::GrayStreetMap] = "normal.day.grey";
        registeredTypes[QGraphicsGeoMap::MobileStreetMap] = "normal.day.mobile";
        registeredTypes[QGraphicsGeoMap::MobileTerrainMap] = "terrain.day.mobile";
        registeredTypes[QGraphicsGeoMap::MobileHybridMap] = "hybrid.day.mobile";
        registeredTypes[QGraphicsGeoMap::MobileTransitMap] = "normal.day.transit.mobile";
        registeredTypes[QGraphicsGeoMap::MobileGrayStreetMap] = "normal.day.grey.mobile";
    }

    MapTypeRegistry::const_iterator it = registeredTypes.find(type);
    if (it != registeredTypes.end()) {
        return it->second;
    }

    qWarning() << "Unknown QGraphicsGeoMap::MapType: " << type;
    return "normal.day";
}

void QGeoMappingManagerEngineNokia::setupServiceInfo()
{
    QSize maxResolution(256, 256);
    double minZoomLevel(0.0), maxZoomLevel(20.0);

    QList<QGraphicsGeoMap::MapType> types;
    types << QGraphicsGeoMap::StreetMap;
    types << QGraphicsGeoMap::SatelliteMapDay;
    types << QGraphicsGeoMap::TerrainMap;
    types << QGraphicsGeoMap::HybridMap;
    types << QGraphicsGeoMap::TransitMap;
    types << QGraphicsGeoMap::GrayStreetMap;
    types << QGraphicsGeoMap::MobileStreetMap;
    types << QGraphicsGeoMap::MobileTerrainMap;
    types << QGraphicsGeoMap::MobileHybridMap;
    types << QGraphicsGeoMap::MobileTransitMap;
    types << QGraphicsGeoMap::MobileGrayStreetMap;

    QString path = "http://";
    if (!m_firstSubdomain.isNull()) {
        path += m_firstSubdomain;
        path += ".";
    }
    path += m_host;
    path += "/maptiler/v2/info?output=xml";

    if (!m_token.isEmpty()) {
        path += "&token=";
        path += m_token;
    }

    if (!m_applicationId.isEmpty()) {
        path += "&app_id=";
        path += m_applicationId;
    }

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
    } else {
        qWarning() << __FUNCTION__ << reply->errorString();
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

const QString & QGeoMappingManagerEngineNokia::applicationId() const
{
    return m_applicationId;
}

const QString & QGeoMappingManagerEngineNokia::referer() const
{
    return m_referer;
}

const QString & QGeoMappingManagerEngineNokia::token() const
{
    return m_token;
}

QChar QGeoMappingManagerEngineNokia::firstSubdomain() const
{
    return m_firstSubdomain;
}

unsigned char QGeoMappingManagerEngineNokia::maxSubdomains() const
{
    return m_maxSubdomains;
}

void QGeoMappingManagerEngineNokia::setHost(const QString &host)
{
    if (host.length() > 4 && host.at(1) == QChar('-') && host.at(3) == QChar('.')) {
        QString realHost = host.right(host.length() - 4);
        m_firstSubdomain = host.at(0);
        m_maxSubdomains = host.at(2).toAscii() - host.at(0).toAscii();
        m_host = realHost;
    } else {
        m_host = host;
        m_firstSubdomain = QChar::Null;
        m_maxSubdomains = 0;
    }
}

void QGeoMappingManagerEngineNokia::currentMobileCountryCodeChanged(const QString & mcc)
{
    if (mcc == "460" || mcc == "461" || mcc == "454" || mcc == "455") {
        setHost(MAPTILES_HOST_CN);
    }
}

bool QGeoMappingManagerEngineNokia::isValidParameter(const QString &param)
{
    if (param.isEmpty())
        return false;

    if (param.length() > 512)
        return false;

    foreach (QChar c, param) {
        if (!c.isLetterOrNumber() && c.toAscii() != '%' && c.toAscii() != '-' &&
            c.toAscii() != '+' && c.toAscii() != '_') {
            return false;
        }
    }

    return true;
}
