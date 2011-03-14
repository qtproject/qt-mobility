/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdeclarativegeomappixmapobject_p.h"

#include <QBrush>
#include <QUrl>
#include <QFile>
#include <QIODevice>
#include <QImage>
#include <QImageReader>

#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecontext.h>

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass MapImage

    \brief The MapImage element displays an image on a map.
    \inherits QGeoMapPixmapObject

    \ingroup qml-location-maps

    The image loaded from \l source will be drawn \l offset.x and 
    \l offset.y pixels away from the on-screen position of \l coordinate.
    
    If \l source does not point to an image or \l coordinate is 
    invalid nothing will be displayed.

    The status of the image loading can be monitored via \l status.

    The MapImage element is part of the \bold{QtMobility.location 1.1} module.
*/

QDeclarativeGeoMapPixmapObject::QDeclarativeGeoMapPixmapObject()
    : m_status(QDeclarativeGeoMapPixmapObject::Null),
    m_reply(0)
{
    m_coordinate = new QDeclarativeCoordinate(this);

    connect(m_coordinate,
            SIGNAL(latitudeChanged(double)),
            this,
            SLOT(coordinateLatitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(longitudeChanged(double)),
            this,
            SLOT(coordinateLongitudeChanged(double)));

    connect(m_coordinate,
            SIGNAL(altitudeChanged(double)),
            this,
            SLOT(coordinateAltitudeChanged(double)));
}

QDeclarativeGeoMapPixmapObject::~QDeclarativeGeoMapPixmapObject()
{
}

/*!
    \qmlproperty Coordinate MapImage::coordinate

    This property holds the coordinate at which to anchor the image. 
*/

void QDeclarativeGeoMapPixmapObject::setDeclarativeCoordinate(const QDeclarativeCoordinate *coordinate)
{
    if (m_coordinate->coordinate() == coordinate->coordinate())
        return;

    m_coordinate->setCoordinate(coordinate->coordinate());
    setCoordinate(coordinate->coordinate());

    emit declarativeCoordinateChanged(m_coordinate);
}

QDeclarativeCoordinate* QDeclarativeGeoMapPixmapObject::declarativeCoordinate() 
{
    return m_coordinate;
}

void QDeclarativeGeoMapPixmapObject::coordinateLatitudeChanged(double /*latitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapPixmapObject::coordinateLongitudeChanged(double /*longitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

void QDeclarativeGeoMapPixmapObject::coordinateAltitudeChanged(double /*altitude*/)
{
    setCoordinate(m_coordinate->coordinate());
}

/*!
    \qmlproperty int MapImage::offset.x
    \qmlproperty int MapImage::offset.y

    These properties hold the offset from the on-screen position of 
    \l coordinate at which the image should be displayed.

    They both default to 0.
*/

/*!
    \qmlproperty url MapImage::source

    This property holds the URL describing the location of the image to 
    display.

    The URL can be absolute or relative to where the QML file 
    was loaded from, and can be a local file, a file embedded within 
    a Qt Resource bundle, or a file retrieved from the network.
*/

void QDeclarativeGeoMapPixmapObject::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;

    load();

    emit sourceChanged(source);
}

QUrl QDeclarativeGeoMapPixmapObject::source() const
{
    return m_source;
}

/*!
    \qmlproperty enumeration MapImage::status

    This property holds the status of image loading.  It can be one of:
    \list
    \o MapImage.Null - no image has been set
    \o MapImage.Ready - the image has been loaded
    \o MapImage.Loading - the image is currently being loaded
    \o MapImage.Error - an error occurred while loading the image
    \endlist
*/

QDeclarativeGeoMapPixmapObject::Status QDeclarativeGeoMapPixmapObject::status() const
{
    return m_status;
}


void QDeclarativeGeoMapPixmapObject::setStatus(const QDeclarativeGeoMapPixmapObject::Status status)
{
    if (m_status == status)
        return;

    m_status = status;

    emit statusChanged(m_status);
}

void QDeclarativeGeoMapPixmapObject::load()
{
    // need to deal with absolute / relative local / remote files

    QUrl url = QDeclarativeEngine::contextForObject(this)->resolvedUrl(m_source);

    QString path;

    if (url.scheme().compare(QLatin1String("qrc"), Qt::CaseInsensitive) == 0) {
        if (url.authority().isEmpty())
            path = QLatin1Char(':') + url.path();
    } else if (url.scheme().compare(QLatin1String("file"), Qt::CaseInsensitive) == 0) {
        path = url.toLocalFile();
    }

    if (!path.isEmpty()) {
        QFile f(path);
        if (f.open(QIODevice::ReadOnly)) {
            QImage image;
            QImageReader imageReader(&f);
            if (imageReader.read(&image)) {
                setPixmap(QPixmap::fromImage(image));
                setStatus(QDeclarativeGeoMapPixmapObject::Ready);
            } else {
                setPixmap(QPixmap());
                setStatus(QDeclarativeGeoMapPixmapObject::Error);
                //qWarning() << "image read fail";
            }
        } else {
            setPixmap(QPixmap());
            setStatus(QDeclarativeGeoMapPixmapObject::Error);
            //qWarning() << "file open fail";
        }
    } else {
        if (m_reply) {
            m_reply->abort();
            m_reply->deleteLater();
            m_reply = 0;
        }

        QDeclarativeEngine *engine = QDeclarativeEngine::contextForObject(this)->engine();
        if (engine) {
            QNetworkAccessManager *nam = engine->networkAccessManager();
            m_reply = nam->get(QNetworkRequest(url));

            if (m_reply->isFinished()) {
                if (m_reply->error() == QNetworkReply::NoError) {
                    finished();
                } else {
                    error(m_reply->error());
                }
                delete m_reply;
                m_reply = 0;
                return;
            }

            setStatus(QDeclarativeGeoMapPixmapObject::Loading);

            connect(m_reply,
                    SIGNAL(finished()),
                    this,
                    SLOT(finished()));
            connect(m_reply,
                    SIGNAL(error(QNetworkReply::NetworkError)),
                    this,
                    SLOT(error(QNetworkReply::NetworkError)));

        } else {
            setPixmap(QPixmap());
            setStatus(QDeclarativeGeoMapPixmapObject::Error);
            //qWarning() << "null engine fail";
        }
    }
}

void QDeclarativeGeoMapPixmapObject::finished()
{
    if (m_reply->error() != QNetworkReply::NoError) {
        m_reply->deleteLater();
        m_reply = 0;
        return;
    }

    QImage image;
    QImageReader imageReader(m_reply);
    if (imageReader.read(&image)) {
        setPixmap(QPixmap::fromImage(image));
        setStatus(QDeclarativeGeoMapPixmapObject::Ready);
    } else {
        setPixmap(QPixmap());
        setStatus(QDeclarativeGeoMapPixmapObject::Error);
        //qWarning() << "image read fail";
    }

    m_reply->deleteLater();
    m_reply = 0;
}

void QDeclarativeGeoMapPixmapObject::error(QNetworkReply::NetworkError error)
{
    m_reply->deleteLater();
    m_reply = 0;

    setPixmap(QPixmap());
    setStatus(QDeclarativeGeoMapPixmapObject::Error);
    //qWarning() << "network error fail";
}

/*!
    \qmlproperty int MapImage::zValue

    This property holds the z-value of the image.

    Map objects are drawn in z-value order, and objects with the 
    same z-value will be drawn in insertion order.
*/

/*!
    \qmlproperty bool MapImage::visible

    This property holds a boolean corresponding to whether or not the 
    image is visible.
*/

/*!
    \qmlproperty bool MapImage::selected

    This property holds a boolean corresponding to whether or not the 
    image is selected.
*/

#include "moc_qdeclarativegeomappixmapobject_p.cpp"

QTM_END_NAMESPACE

