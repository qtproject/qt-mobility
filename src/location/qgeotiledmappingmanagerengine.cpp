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

#include "qgeotiledmappingmanagerengine.h"
#include "qgeotiledmappingmanagerengine_p.h"

#include "qgeotiledmapdata.h"
#include "qgeotiledmaprequest.h"

#include <QPainter>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QMutableListIterator>
#include <QDebug>


QTM_BEGIN_NAMESPACE

/*!
    Constructs a QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::QGeoTiledMappingManagerEngine(const QMap<QString, QString> &parameters, QObject *parent)
        : QGeoMappingManagerEngine(new QGeoTiledMappingManagerEnginePrivate(parameters), parent)
{
    qRegisterMetaType<QGeoTiledMapReply::Error>("QGeoTiledMapReply::Error");

    setTileSize(QSize(128, 128));
    //QTimer::singleShot(0, this, SLOT(init()));
}

//void QGeoTiledMappingManagerEngine::init()
//{
//    this->initialize();
//}

//void QGeoTiledMappingManagerEngine::initialize() {}

/*!
    Destroys this QGeoTiledMappingManagerEngine object.
*/
QGeoTiledMappingManagerEngine::~QGeoTiledMappingManagerEngine()
{
    // deleted in superclass destructor
}

QGeoMapData* QGeoTiledMappingManagerEngine::createMapData(QGeoMapWidget *widget)
{
    return new QGeoTiledMapData(this, widget);
}

void QGeoTiledMappingManagerEngine::removeMapData(QGeoMapData* mapData) {}

QPoint QGeoTiledMappingManagerEngine::screenPositionToTilePosition(const QGeoMapData *mapData, const QPointF &screenPosition) const
{
    // TODO checking mechanism for viewport type?
    const QGeoTiledMapData *tiledMapData = static_cast<const QGeoTiledMapData*>(mapData);
    return tiledMapData->screenPositionToTileIndices(screenPosition);
}

/*!
    Returns a list of the image formats supported by this QGeoTiledMappingManagerEngine
    instance.

    \sa QGeoTiledMappingManagerEngine::setSupportedImageFormats()
*/
QList<QString> QGeoTiledMappingManagerEngine::supportedImageFormats() const
{
    Q_D(const QGeoTiledMappingManagerEngine);
    return d->supportedImageFormats;
}

QSize QGeoTiledMappingManagerEngine::tileSize() const
{
    Q_D(const QGeoTiledMappingManagerEngine);
    return d->tileSize;
}

/*!
    Sets the list of image formats supported by this QGeoTiledMappingManagerEngine
    instance to \a imageFormats.

    Subclasses of QGeoCodingService should use this function to ensure that
    supportedImageFormats() provides accurate information.

    \sa QGeoTiledMappingManagerEngine::supportedImageFormats()
*/
void QGeoTiledMappingManagerEngine::setSupportedImageFormats(const QList<QString> &imageFormats)
{
    Q_D(QGeoTiledMappingManagerEngine);
    d->supportedImageFormats = imageFormats;
}

void QGeoTiledMappingManagerEngine::setTileSize(const QSize &tileSize)
{
    Q_D(QGeoTiledMappingManagerEngine);
    d->tileSize = tileSize;
}

/*!
    \fn void QGeoTiledMappingManagerEngine::replyFinished(QGeoMapReply *reply)

    Indicates that a request handled by this QGeoTiledMappingManagerEngine object has
    finished successfully.  The result of the request will be in \a reply.

    Note that \a reply will be the same object returned by this
    QGeoTiledMappingManagerEngine instance when the request was issued, and that the
    QGeoMapReply::finished() signal can be used instead of this signal if it
    is more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::finished()
*/

/*!
    \fn void QGeoTiledMappingManagerEngine::replyError(QGeoMapReply *reply,
                                            QGeoTiledMappingManagerEngine::ErrorCode errorCode,
                                            QString errorString);

    Indicates that a request handled by this QGeoTiledMappingManagerEngine object has
    failed.  The error is described by \a errorCode and \a errorString, and \a
    reply is the QGeoMapReply object which was managing the result of the
    corresponding service request.

    Note that \a reply will be the same object returned by this
    QGeoTiledMappingManagerEngine instance when the request was issued, and that the
    QGeoMapReply::error() signal can be used instead of this signal if it is
    more convinient to do so.

    Do not delete the QGeoMapReply object in a slot connected to this signal
    - use deleteLater() if it is necessary to do so.

    \sa QGeoMapReply::error()
*/

/*******************************************************************************
*******************************************************************************/

QGeoTiledMappingManagerEnginePrivate::QGeoTiledMappingManagerEnginePrivate(const QMap<QString, QString> &parameters)
        : QGeoMappingManagerEnginePrivate(parameters) {}

QGeoTiledMappingManagerEnginePrivate::QGeoTiledMappingManagerEnginePrivate(const QGeoTiledMappingManagerEnginePrivate &other)
        : QGeoMappingManagerEnginePrivate(other),
        supportedImageFormats(other.supportedImageFormats),
        tileSize(other.tileSize) {}

QGeoTiledMappingManagerEnginePrivate::~QGeoTiledMappingManagerEnginePrivate() {}

QGeoTiledMappingManagerEnginePrivate& QGeoTiledMappingManagerEnginePrivate::operator= (const QGeoTiledMappingManagerEnginePrivate & other)
{
    QGeoMappingManagerEnginePrivate::operator =(other);

    supportedImageFormats = other.supportedImageFormats;
    tileSize = other.tileSize;

    return *this;
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qgeotiledmappingmanagerengine.cpp"

QTM_END_NAMESPACE
