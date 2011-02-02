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

#include "mapswidget.h"
#include "marker.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneWheelEvent>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QTimer>

#include "qgeocoordinate.h"

GeoMap::GeoMap(QGeoMappingManager *manager, MapsWidget *mapsWidget) :
    QGraphicsGeoMap(manager), mapsWidget(mapsWidget)
{
}

GeoMap::~GeoMap()
{
}

double GeoMap::centerLatitude() const
{
    return center().latitude();
}

double GeoMap::centerLongitude() const
{
    return center().longitude();
}

void GeoMap::setCenterLatitude(double lat)
{
    QGeoCoordinate c = center();
    c.setLatitude(lat);
    setCenter(c);
}

void GeoMap::setCenterLongitude(double lon)
{
    QGeoCoordinate c = center();
    c.setLongitude(lon);
    setCenter(c);
}

void GeoMap::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    panActive = true;

    markerPressed = false;
    QList<QGeoMapObject*> objects = mapObjectsAtScreenPosition(event->pos());
    if (objects.size() > 0) {
        pressed = objects.first();
        markerPressed = true;
    }

    event->accept();
}

void GeoMap::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    panActive = false;

    if (markerPressed) {
        // check if we're still over the object
        QList<QGeoMapObject*> objects = mapObjectsAtScreenPosition(event->pos());
        if (objects.contains(pressed)) {
            Marker *m = dynamic_cast<Marker*>(pressed);
            if (m)
                emit clicked(m);
        }

        markerPressed = false;
    }

    event->accept();
}

void GeoMap::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (panActive) {
        QPointF delta = event->lastPos() - event->pos();
        pan(delta.x(), delta.y());
        emit panned();
    }
    event->accept();
}

void GeoMap::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    qreal panx = event->pos().x() - size().width() / 2.0;
    qreal pany = event->pos().y() - size().height() / 2.0;
    pan(panx, pany);
    if (event->delta() > 0) {   // zoom in
        if (zoomLevel() < maximumZoomLevel()) {
            setZoomLevel(zoomLevel() + 1);
        }
    } else {                    // zoom out
        if (zoomLevel() > minimumZoomLevel()) {
            setZoomLevel(zoomLevel() - 1);
        }
    }
    pan(-panx, -pany);
    event->accept();
}

ZoomButtonItem::ZoomButtonItem(GeoMap *map) :
    map(map),
    pressedOverTopHalf(false),
    pressedOverBottomHalf(false)
{
    setPen(QPen(QBrush(), 0));
    setBrush(QBrush(QColor(0,0,0,150)));

    plusText = new QGraphicsSimpleTextItem(this);
    plusText->setText("+");
    plusText->setBrush(QBrush(Qt::white));

    minusText = new QGraphicsSimpleTextItem(this);
    minusText->setText("-");
    minusText->setBrush(QBrush(Qt::white));
}

void ZoomButtonItem::setRect(qreal x, qreal y, qreal w, qreal h)
{
    QGraphicsRectItem::setRect(x, y, w, h);

    QFont f;
    f.setFixedPitch(true);
    f.setPixelSize(h/3.0);
    plusText->setFont(f);
    minusText->setFont(f);

    QRectF plusBound = plusText->boundingRect();
    QPointF plusCenter(x+w/2.0, y+h/4.0);
    QPointF plusDelta = plusCenter - plusBound.center();
    plusText->setPos(plusDelta);

    QRectF minusBound = minusText->boundingRect();
    QPointF minusCenter(x+w/2.0, y+3.0*h/4.0);
    QPointF minusDelta = minusCenter - minusBound.center();
    minusText->setPos(minusDelta);
}

void ZoomButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    const QPointF pos = event->pos();
    if (!pressedOverTopHalf && !pressedOverBottomHalf) {
        if (isTopHalf(pos)) {
            pressedOverTopHalf = true;
        } else if (isBottomHalf(pos)) {
            pressedOverBottomHalf = true;
        }
    }
    event->accept();
}

bool ZoomButtonItem::isTopHalf(const QPointF &point)
{
    return QRectF(rect().x(), rect().y(),
                  rect().width(), rect().height()/2).contains(point);
}

bool ZoomButtonItem::isBottomHalf(const QPointF &point)
{
    return QRectF(rect().x(), rect().y() + rect().height()/2,
                  rect().width(), rect().height()/2).contains(point);
}

void ZoomButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    const QPointF pos = event->pos();
    if (isTopHalf(pos) && pressedOverTopHalf) {
        map->setZoomLevel(map->zoomLevel() + 1.0);
    } else if (isBottomHalf(pos) && pressedOverBottomHalf) {
        map->setZoomLevel(map->zoomLevel() - 1.0);
    }
    pressedOverBottomHalf = false;
    pressedOverTopHalf = false;
    event->accept();
}

void ZoomButtonItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    event->accept();
}


class StatusBarItemPrivate
{
public:
    int offset;
    QGraphicsSimpleTextItem *textItem;
};

StatusBarItem::StatusBarItem() :
    d(new StatusBarItemPrivate)
{
    d->offset = 0;

    setPen(QPen(QBrush(), 0));
    setBrush(QBrush(QColor(0,0,0,120)));

    d->textItem = new QGraphicsSimpleTextItem(this);
    d->textItem->setBrush(QBrush(Qt::white));

    setText("");
}

StatusBarItem::~StatusBarItem()
{
}

void StatusBarItem::setText(QString text)
{
    d->textItem->setText(text);
    QRectF rect = d->textItem->boundingRect();
    QPointF delta = this->rect().center() - rect.center();
    d->textItem->setPos(delta.x(), delta.y());
}

int StatusBarItem::offset() const
{
    return d->offset;
}

void StatusBarItem::setRect(qreal x, qreal y, qreal w, qreal h)
{
    QGraphicsRectItem::setRect(x, y + d->offset, w, h);
    setText(d->textItem->text());
}

void StatusBarItem::setOffset(int offset)
{
    this->setY(this->y() - d->offset + offset);
    d->offset = offset;
}

void StatusBarItem::showText(QString text, quint32 timeout)
{
    setText(text);
    show();
    QTimer::singleShot(timeout, this, SLOT(hide()));
}

void StatusBarItem::show()
{
    QPropertyAnimation *anim = new QPropertyAnimation(this, "offset");
    anim->setStartValue(0);
    anim->setEndValue(-1 * rect().height());
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void StatusBarItem::hide()
{
    QPropertyAnimation *anim = new QPropertyAnimation(this, "offset");
    anim->setStartValue(d->offset);
    anim->setEndValue(0);
    anim->setDuration(500);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}



class MapsWidgetPrivate
{
public:
    GeoMap *map;
    QGraphicsView *view;
    MarkerManager *markerManager;
    StatusBarItem *statusBarItem;
    ZoomButtonItem *zoomButtonItem;
};

MapsWidget::MapsWidget(QWidget *parent) :
    QWidget(parent),
    d(new MapsWidgetPrivate)
{
    d->map = 0;
    d->view = 0;
    d->markerManager = 0;
}

MapsWidget::~MapsWidget()
{
}

void MapsWidget::initialize(QGeoMappingManager *manager)
{
    d->map = new GeoMap(manager, this);
    if (d->markerManager)
        d->markerManager->setMap(d->map);

    connect(d->map, SIGNAL(clicked(Marker*)),
            this, SIGNAL(markerClicked(Marker*)));
    connect(d->map, SIGNAL(panned()),
            this, SIGNAL(mapPanned()));

    QGraphicsScene *sc = new QGraphicsScene;
    sc->addItem(d->map);

    d->map->setPos(0, 0);
    d->map->resize(this->size());

    d->view = new QGraphicsView(sc, this);
    d->view->setVisible(true);
    d->view->setInteractive(true);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    d->statusBarItem = new StatusBarItem;
    sc->addItem(d->statusBarItem);

    d->zoomButtonItem = new ZoomButtonItem(d->map);
    sc->addItem(d->zoomButtonItem);

    resizeEvent(0);

    d->view->resize(this->size());
    d->view->centerOn(d->map);

    d->map->setCenter(QGeoCoordinate(-27.5796, 153.1));
    d->map->setZoomLevel(15);
}

void MapsWidget::setMyLocation(QGeoCoordinate location, bool center)
{
    if (d->markerManager)
        d->markerManager->setMyLocation(location);
    if (d->map && center)
        d->map->setCenter(location);
}

QGraphicsGeoMap *MapsWidget::map() const
{
    return d->map;
}

StatusBarItem *MapsWidget::statusBar() const
{
    return d->statusBarItem;
}

void MapsWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    if (d->view && d->map) {
        d->view->resize(size());
        d->map->resize(size());
        d->view->centerOn(d->map);

        d->statusBarItem->setRect(0, height(), width(), 20);
        d->zoomButtonItem->setRect(width()-30, height()/2.0 - 35, 25, 70);
    }
}

void MapsWidget::animatedPanTo(QGeoCoordinate center)
{
    if (!d->map)
        return;

    QPropertyAnimation *latAnim = new QPropertyAnimation(d->map, "centerLatitude");
    latAnim->setEndValue(center.latitude());
    latAnim->setDuration(200);
    QPropertyAnimation *lonAnim = new QPropertyAnimation(d->map, "centerLongitude");
    lonAnim->setEndValue(center.longitude());
    lonAnim->setDuration(200);

    QParallelAnimationGroup *group = new QParallelAnimationGroup;
    group->addAnimation(latAnim);
    group->addAnimation(lonAnim);
    group->start(QAbstractAnimation::DeleteWhenStopped);
}

void MapsWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    resizeEvent(0);
}

MarkerManager *MapsWidget::markerManager() const
{
    return d->markerManager;
}

void MapsWidget::setMarkerManager(MarkerManager *markerManager)
{
    d->markerManager = markerManager;
    if (d->map)
        d->markerManager->setMap(d->map);
    d->markerManager->setStatusBar(d->statusBarItem);
}
