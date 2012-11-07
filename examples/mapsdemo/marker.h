/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MARKER_H
#define MARKER_H

#include <QSignalMapper>

#include "qgeomappixmapobject.h"
#include "qgeosearchmanager.h"
#include "qgeocoordinate.h"
#include "qgraphicsgeomap.h"
#include "qgeoaddress.h"
#include "qgeosearchreply.h"

using namespace QtMobility;

class StatusBarItem;

class MarkerPrivate;
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

    MarkerType markerType() const;
    void setMarkerType(MarkerType type);

    QString name() const;
    QGeoAddress address() const;
    bool moveable() const;

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
    MarkerPrivate *d;
};

class MarkerManagerPrivate;
class MarkerManager : public QObject
{
    Q_OBJECT
public:
    explicit MarkerManager(QGeoSearchManager *sm, QObject *parent=0);
    ~MarkerManager();

    QGeoCoordinate myLocation() const;

public slots:
    void setMap(QGraphicsGeoMap *map);
    void setStatusBar(StatusBarItem *bar);
    void setMyLocation(QGeoCoordinate coord);
    void search(QString query, qreal radius=-1);
    void removeSearchMarkers();

signals:
    void searchError(QGeoSearchReply::Error error, QString errorString);
    void searchFinished();

private:
    MarkerManagerPrivate *d;

private slots:
    void replyFinished(QGeoSearchReply *reply);
    void myLocationChanged(QGeoCoordinate location);
    void reverseReplyFinished(QGeoSearchReply *reply);
};

#endif // MARKER_H
