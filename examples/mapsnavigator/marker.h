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

#ifndef MARKER_H
#define MARKER_H

#include <QGeoMapPixmapObject>
#include <QGeoSearchManager>
#include <QGraphicsGeoMap>
#include <QGeoCoordinate>
#include <QSignalMapper>
#include <QGeoSearchReply>
#include <QGeoAddress>

using namespace QtMobility;

class Marker : public QGeoMapPixmapObject
{
    Q_OBJECT

    Q_PROPERTY(MarkerType markerType READ markerType WRITE setMarkerType NOTIFY markerTypeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QGeoAddress address READ address WRITE setAddress NOTIFY addressChanged)
    Q_PROPERTY(bool moveable READ moveable WRITE setMoveable NOTIFY moveableChanged)
public:
    enum MarkerType {
        MyLocationMarker,
        SearchMarker,
        WaypointMarker,
        StartMarker,
        EndMarker,
        PathMarker
    };

    explicit Marker(MarkerType type);

    inline MarkerType markerType() const { return m_type; }
    void setMarkerType(MarkerType type);

    inline QString name() const { return m_name; }
    inline QGeoAddress address() const { return m_address; }
    inline bool moveable() const { return m_moveable; }

public slots:
    void setName(QString name);
    void setAddress(QGeoAddress addr);
    void setMoveable(bool moveable);

signals:
    void markerTypeChanged(const MarkerType &type);
    void nameChanged(const QString &name);
    void addressChanged(const QGeoAddress &address);
    void moveableChanged(const bool &moveable);

private:
    MarkerType m_type;
    QString m_name;
    bool m_moveable;
    QGeoAddress m_address;
};

class MarkerManager : public QObject
{
    Q_OBJECT
public:
    explicit MarkerManager(QGeoSearchManager *sm, QObject *parent=0);
    ~MarkerManager();

    QGeoCoordinate myLocation() const;

public slots:
    void setMap(QGraphicsGeoMap *map);
    void setMyLocation(QGeoCoordinate coord);
    void search(QString query, qreal radius=-1);
    void removeSearchMarkers();

signals:
    void searchError(QGeoSearchReply::Error error, QString errorString);
    void searchFinished();

private:
    Marker *m_myLocation;
    QList<Marker*> searchMarkers;

    QGraphicsGeoMap *m_map;
    QGeoSearchManager *m_searchManager;
    QSignalMapper sigMap;

private slots:
    void replyFinished(QObject *reply);
};

#endif // MARKER_H
