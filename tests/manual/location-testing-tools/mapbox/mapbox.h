/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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

#ifndef MAPBOX_H
#define MAPBOX_H

#include <qmobilityglobal.h>

#include <QWidget>
#include <qnetworksession.h>

class QGraphicsScene;
class QGraphicsView;
class StatsWidget;

QTM_BEGIN_NAMESPACE
    class QGeoCoordinate;
    class QGraphicsGeoMap;
    class QGeoServiceProvider;
    class QGeoMappingManager;
    class QGeoMapRectangleObject;
    class QGeoMapCircleObject;
    class QGeoMapPolylineObject;
    class QGeoMapPolygonObject;
    class QGeoMapRouteObject;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class MapBox : public QWidget
{
    Q_OBJECT
private:
    QGraphicsView * m_qgv;
    QGraphicsScene * m_scene;
    QGraphicsGeoMap * m_mapWidget;

    QGeoServiceProvider *m_serviceProvider;
    QGeoMappingManager *m_mapManager;
    void createProvider();
    void createMapWidget();

    static QNetworkSession * m_session;

    QString m_providerId;
    QMap<QString,QVariant> m_parameters;

public:
    StatsWidget * m_statistics;
    explicit MapBox(QWidget *parent = 0);
    ~MapBox();

    QGraphicsGeoMap * map() { return m_mapWidget; }
    void setProvider(const QString & providerId);
    static MapBox * createOnlineMap(QWidget * parent = 0);
    static MapBox * createOfflineMap(QWidget * parent = 0);

    QGeoMapRectangleObject * addRectangle(qreal top, qreal left, qreal bottom, qreal right);
    QGeoMapRectangleObject * addRectangle(const QGeoCoordinate & topLeft, const QGeoCoordinate & bottomRight);
    QGeoMapPolylineObject * addPolyline(const QList<QGeoCoordinate> &path);
    QGeoMapPolygonObject * addPolygon(const QList<QGeoCoordinate> &path);
    QGeoMapCircleObject * addCircle(const QGeoCoordinate & center, qreal radius);
    void addRoute(const QGeoCoordinate & start, const QGeoCoordinate & end);
    void addRoute(const QList<QGeoCoordinate> & waypoints);

    static QNetworkSession * session();

    void setParameter(const QString & parameter, const QVariant & value);

    QPixmap grab();
    qreal squareError(MapBox * other);
    qreal squareError(const QPixmap & other);
    qreal squareError(const QImage & other);
    static qreal squareError(const QImage & image1, const QImage & image2);

    int countErrors(MapBox * other);
    int countErrors(const QPixmap & other);
    int countErrors(const QImage & other);
    static int countErrors(const QImage & image1, const QImage & image2);

public slots:
    void clearCache();

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void timerEvent(QTimerEvent * event);

private slots:
    void routeFinished();
};

#endif // MAPBOX_H
