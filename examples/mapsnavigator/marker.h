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

using namespace QtMobility;

class Marker : public QGeoMapPixmapObject
{
    Q_OBJECT
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

private:
    MarkerType m_type;
};

class MarkerManager : public QObject
{
    Q_OBJECT
public:
    explicit MarkerManager(QGeoSearchManager *sm, QObject *parent=0);
    ~MarkerManager();

public slots:
    void setMap(QGraphicsGeoMap *map);
    void setMyLocation(QGeoCoordinate coord);
    void search(QString query);
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
