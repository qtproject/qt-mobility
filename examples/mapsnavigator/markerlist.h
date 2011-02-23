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

#ifndef MARKERLIST_H
#define MARKERLIST_H

#include <QWidget>

#include <qgeosearchmanager.h>
#include <qgeosearchreply.h>
#include <qgeocoordinate.h>
#include <qgeomapobject.h>
#include <qgeomappixmapobject.h>
#include <qgeoplace.h>

QTM_USE_NAMESPACE

class MarkerObject : public QGeoMapPixmapObject
{
    Q_OBJECT
public:
    enum MarkerState {
        Valid,
        Held,
        Address,
        Searching,
        Error
    };

    enum MarkerType {
        MyLocation,
        Search,
        Waypoint,
        Startpoint,
        Endpoint,
        Path
    };

    MarkerObject(MarkerType type)
        : m_state(Valid), m_type(type), m_searchReply(0), m_center(false) {}

    ~MarkerObject() {}

    void setMarkerState(MarkerState state) {m_state = state;}
    MarkerState markerState() const {return m_state;}

    void setIconPath(const QString &filename) {m_filename = filename; normalizeIcon();}
    QString iconPath() const {return m_filename;}

    void setName(const QString &name) {m_name = name;}
    QString name() const {return m_name;}

    void setMarkerType(MarkerType type) {m_type = type;}
    MarkerType markerType() const {return m_type;}

    void setPlace(const QGeoPlace& place) {m_place = place;}
    QGeoPlace place() const {return m_place;}

    void setCenter(bool center) {m_center = center;}
    bool center() const {return m_center;}

    void setSearchReply(QGeoSearchReply *reply);
    QGeoSearchReply* searchReply() {return m_searchReply;}

public slots:
    void normalizeIcon();
    void expandIcon(const QGeoCoordinate &pos);

private slots:
    void searchFinished();
    void searchError(QGeoSearchReply::Error error, const QString &errorString);

signals:
    void searchReplyError(const QString &errorString);

private:
    MarkerState m_state;
    MarkerType m_type;
    QString m_name;
    QString m_filename;
    QGeoPlace m_place;
    QGeoSearchReply *m_searchReply;
    bool m_center;
};


class MarkerList : public QWidget
{
    Q_OBJECT
public:
    MarkerList(QWidget *parent = 0);
    ~MarkerList();

public slots:
    void initialize(QGeoSearchManager *m_searchManager);
    void addMarker(MarkerObject *marker);
    void removeMarker(MarkerObject *marker);
    void moveMarker(const QGeoCoordinate &last, const QGeoCoordinate &curr);
    void markerPressed(MarkerObject *marker);
    void markerReleased();
    void mapMoved();
    void getMarkerLocation(MarkerObject *marker);

signals:
    void showMarkerInfo(MarkerObject *marker);

private:

    QGeoSearchManager *m_searchManager;
    MarkerObject *selectedMarker;

    QList<MarkerObject*> markerList;

    bool moved;
    int waypointCount;
    int markerCount;
    int pathCount;
};

#endif // MARKERLIST_H
