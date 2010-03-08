/*
* ==============================================================================
*  Name        : QMapTileCache.cpp
*  Version     : 0.1
*  Author      : Thomas Zahn
*
*  Copyright (c) 2010 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation.
* ==============================================================================
*/

#include <QStringBuilder>
#include <QDateTime>
#include <QMap>

#include "qmaptilecache.h"

#define SECONDS_PER_DAY 86400
#define THIRTY_DAYS (30 * SECONDS_PER_DAY)
#define QLOCATION_CACHE_DIR ".qlocation"
#define DEFAULT_HIGHWATERMARK 500000000 //in bytes
#define DEFAULT_LOWWATERMARK 400000000 //in bytes

QTM_BEGIN_NAMESPACE

//TODO: thoroughly check whether LRU correctly works

QMapTileCache::QMapTileCache()
{
    cacheDir = QDir::home();
    expire = THIRTY_DAYS;
    highWm = DEFAULT_HIGHWATERMARK;
    lowWm = DEFAULT_LOWWATERMARK;

    init();
}

QMapTileCache::QMapTileCache(QDir& directory)
{
    cacheDir = directory;
    expire = THIRTY_DAYS;
    highWm = DEFAULT_HIGHWATERMARK;
    lowWm = DEFAULT_LOWWATERMARK;

    init();
}

void QMapTileCache::init()
{
    if (!cacheDir.exists(QLOCATION_CACHE_DIR)) {
        cacheDir.mkdir(QLOCATION_CACHE_DIR);
    }

    cacheDir.cd(QLOCATION_CACHE_DIR);
    sz = 0;

    for (int i = 0; i < 16; i++)

        for (int j = 0; j < 16; j++) {
            QDir dir(cacheDir.path() % '/' % QString::number(i, 16)
                     % '/' % QString::number(j, 16));

            if (dir.exists()) {
                QFileInfoList files = dir.entryInfoList();

                for (int n = 0; n < files.count(); n++) {
                    QFileInfo info(files[n]);
                    sz += info.size();
                }
            }
        }
}

QMapTileReply* QMapTileCache::get(const QMapTileRequest& request)
{
    QString fileName = constrFileName(request);
    int hashKey = constrHashKey(fileName);
    int level1 = hashKey >> 4;
    int level2 = hashKey & 15;
    QString dir = QString::number(level1, 16)
                  % '/' % QString::number(level2, 16);
    QFile tile(cacheDir.path() % '/' % dir % '/' % fileName);

    if (!tile.exists())
        return NULL;

    QFileInfo info(tile);

    if (hasExpired(info.lastRead())) {
        //no need to remove here
        //will be overwritten in subsquent call to cache()
        return NULL;
    }

    if (!tile.open(QIODevice::ReadOnly))
        return NULL;

    QMapTileReply* reply = new QMapTileReply(request);
    reply->rawData() = tile.readAll();
    tile.close();

    return reply;
}

bool QMapTileCache::hasExpired(const QDateTime& timestamp)
{
    uint lastAccessed = timestamp.toTime_t();
    uint currTime = QDateTime::currentDateTime().toTime_t();

    if ((currTime - lastAccessed) > expire) {
        return true;
    }

    return false;
}

void QMapTileCache::cache(QMapTileReply& reply)
{
    quint64 oldFileSize = 0;
    //construct proper file name and subfolder
    QString fileNm = constrFileName(reply.request());
    int hashKey = constrHashKey(fileNm);
    int level1 = hashKey >> 4;
    int level2 = hashKey & 15;
    QString dir = QString::number(level1, 16)
                  % '/' % QString::number(level2, 16);

    if (!cacheDir.exists(dir)) {
        if (!cacheDir.mkpath(dir))
            return;
    }

    //Open/create cache file
    QFile tile(cacheDir.path() % '/' % dir % '/' % fileNm);

    if (tile.exists())
        oldFileSize = QFileInfo(tile).size();

    if (!tile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    tile.write(reply.rawData());
    tile.close();

    QFileInfo info(tile);
    sz += info.size() - oldFileSize;

    //Is cache now full?
    if (sz > highWm)
        cleanup();
}

void QMapTileCache::setExpireAfter(const quint32 seconds)
{
    expire = seconds;
}

void QMapTileCache::setExpireAfter(const quint16 days)
{
    expire = days * SECONDS_PER_DAY;
}

int QMapTileCache::constrHashKey(const QString& fileName)
{
    unsigned int hashKey = 0;

    for (int i = 0; i < fileName.length(); i++) {
        hashKey += fileName[i].toAscii();
    }

    return hashKey % 256;
}

QString QMapTileCache::constrFileName(const QMapTileRequest& request)
{
    return request.version().id % '_' %
           request.scheme().id % '_' %
           QString::number(request.zoomLevel()) % '_' %
           request.resolution().id % '_' %
           QString::number(request.col()) % '_' %
           QString::number(request.row()) % '.' %
           request.format().id;
}

void QMapTileCache::cleanup(bool force)
{
    //Contains all (timestamp, filename) pairs
    QMap<QDateTime, QFileInfo> lru;

    for (int i = 0; i < 16; i++)

        for (int j = 0; j < 16; j++) {
            QDir dir(cacheDir.path() % '/' % QString::number(i, 16) %
                     '/' % QString::number(j, 16));

            if (dir.exists()) {
                QFileInfoList files = dir.entryInfoList();

                for (int n = 0; n < files.count(); n++) {
                    QFileInfo info(files[n]);
                    QFile tile(info.filePath());

                    if (hasExpired(info.lastRead())) {
                        if (tile.remove())
                            sz -= info.size();
                    } else
                        lru.insertMulti(info.lastRead() , info);
                }
            }

            if (sz <= lowWm && !force)
                return; //we're done
        }


    //first remove all expired entries
    QMapIterator<QDateTime, QFileInfo> it(lru);

    while (it.hasNext()) {
        it.next();
        QFileInfo file = it.value();
        quint64 fSz = file.size();

        if (QFile::remove(file.filePath()))
            sz -= fSz;

        if (sz <= lowWm)
            break;
    }
}

QTM_END_NAMESPACE
