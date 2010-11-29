#include "mapbox.h"
#include "statswidget.h"
#include "boxgraphicsscene.h"

#include <qgraphicsgeomap.h>
#include <qgeoserviceprovider.h>
#include <qgeomappingmanager.h>
#include <qgeocoordinate.h>
#include <qgeomaprectangleobject.h>
#include <qgeomapcircleobject.h>
#include <qgeomappolylineobject.h>
#include <qgeomappolygonobject.h>
#include <qgeomaprouteobject.h>

#include <qgeoroutingmanager.h>
#include <qgeoroutereply.h>

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>

#include "../../src/location/maps/qgraphicsgeomap_p.h"
#include <qgeomapdata.h>

MapBox::MapBox(QWidget *parent) :
    QWidget(parent),
    m_qgv(0),
    m_scene(0),
    m_mapWidget(0),
    m_serviceProvider(0),
    m_mapManager(0)
{
    m_statistics = new StatsWidget(this);

    m_scene = new BoxGraphicsScene(m_statistics, this);

    m_qgv = new QGraphicsView(m_scene, this);
    m_qgv->setOptimizationFlag(QGraphicsView::IndirectPainting, true);
    m_qgv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_qgv->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QVBoxLayout * layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_statistics);
    layout->addWidget(m_qgv);
    setLayout(layout);

    // determine the order of the stat entries
    m_statistics->stat("FPS", -1);
    m_statistics->stat("Render time", -1);
    m_statistics->stat("mem", -1);
    m_statistics->stat("map objects", 0);
    startTimer(1000);
}

void MapBox::timerEvent(QTimerEvent * event)
{
    m_statistics->stat("mem", perf_currentMemUsage());

    m_statistics->stat("map objects", m_mapWidget->mapObjects().size());
}

MapBox::~MapBox()
{
    m_scene->removeItem(m_mapWidget); // EVIL workaround!
}

void MapBox::setProvider(const QString & providerId)
{
    delete m_serviceProvider;

    m_serviceProvider = new QGeoServiceProvider(providerId);

    if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {
        QMessageBox::information(this, tr("MapViewer Example"), tr(
            "Unable to find the %1 geoservices plugin.").arg(providerId));
        qApp->quit();
    }

    m_mapManager = m_serviceProvider->mappingManager();

    createMapWidget();
}
void MapBox::createMapWidget()
{
    // delete m_mapWidget; // TODO: uncomment, since this is an EVIL workaround

    m_mapWidget = new QGraphicsGeoMap(m_mapManager);

    m_mapWidget->setCenter(QGeoCoordinate(52.5, 13.0));
    m_mapWidget->setZoomLevel(5);

    m_scene->addItem(m_mapWidget);
}

MapBox * MapBox::createOnlineMap(QWidget * parent)
{
    MapBox * mapBox = new MapBox(parent);
    mapBox->setProvider("nokia");
    return mapBox;
}

MapBox * MapBox::createOfflineMap(QWidget * parent)
{
    MapBox * mapBox = new MapBox(parent);
    mapBox->setProvider("nokia_mos");
    return mapBox;
}

void MapBox::resizeEvent(QResizeEvent * event)
{
    m_qgv->setSceneRect(QRectF(QPointF(0.0, 0.0), m_qgv->size()));
    if (m_mapWidget)
        m_mapWidget->resize(m_qgv->size());

    QWidget::resizeEvent(event);
}

static QPen defaultPen()
{
    QPen pen(Qt::white);
    pen.setWidth(2);
    pen.setCosmetic(true);
    return pen;
}

static QBrush defaultBrush()
{
    return QBrush(QColor(0,0,0,65));
}

QGeoMapRectangleObject * MapBox::addRectangle(qreal top, qreal left, qreal bottom, qreal right)
{
    return addRectangle(QGeoCoordinate(top, left), QGeoCoordinate(bottom, right));
}

QGeoMapRectangleObject * MapBox::addRectangle(const QGeoCoordinate & topLeft, const QGeoCoordinate & bottomRight)
{
    QGeoMapRectangleObject *rectangle = new QGeoMapRectangleObject(topLeft, bottomRight);

    rectangle->setPen(defaultPen());
    rectangle->setBrush(defaultBrush());

    m_mapWidget->addMapObject(rectangle);

    return rectangle;
}

QGeoMapPolylineObject * MapBox::addPolyline(const QList<QGeoCoordinate> & path)
{
    QGeoMapPolylineObject *polyline = new QGeoMapPolylineObject();

    polyline->setPen(defaultPen());
    polyline->setPath(path);

    m_mapWidget->addMapObject(polyline);

    return polyline;
}

QGeoMapPolygonObject * MapBox::addPolygon(const QList<QGeoCoordinate> & path)
{
    QGeoMapPolygonObject *polygon = new QGeoMapPolygonObject();

    polygon->setPen(defaultPen());
    polygon->setBrush(defaultBrush());
    polygon->setPath(path);

    m_mapWidget->addMapObject(polygon);

    return polygon;
}

QGeoMapCircleObject * MapBox::addCircle(const QGeoCoordinate & center, qreal radius)
{
    QGeoMapCircleObject *circle = new QGeoMapCircleObject(center, radius);

    circle->setPen(defaultPen());
    circle->setBrush(defaultBrush());

    m_mapWidget->addMapObject(circle);

    return circle;
}

void MapBox::addRoute(const QGeoCoordinate & start, const QGeoCoordinate & end)
{
    QList<QGeoCoordinate> waypoints;
    waypoints << start << end;
    addRoute(waypoints);
}

void MapBox::addRoute(const QList<QGeoCoordinate> & waypoints)
{
    QGeoRoutingManager * routingManager = m_serviceProvider->routingManager();

    QGeoRouteRequest req(waypoints);
    QGeoRouteReply *reply = routingManager->calculateRoute(req);

    QObject::connect(reply, SIGNAL(finished()),
                     this, SLOT(routeFinished()));
}

void MapBox::routeFinished()
{
    QGeoRouteReply *reply = static_cast<QGeoRouteReply*>(sender());

    if (!reply)
        return;

    if (reply->routes().size() < 1)
        return;

    QPen pen(QColor(0, 0, 255, 127)); // blue, semi-transparent
    pen.setWidth(7);
    pen.setCosmetic(true);
    pen.setCapStyle(Qt::RoundCap);

    QGeoMapRouteObject *route = new QGeoMapRouteObject(reply->routes().at(0));

    route->setPen(pen);

    m_mapWidget->addMapObject(route);
}

/* TODO
    network session!!!

    move proxy stuff to mapbox

    addRoute doesn't work and freezes on exit if used

    statistics panel
        - mem
            more plattforms
        - network traffic
            - maybe from the session?
        - render mode
            - sw/hw, but that's pretty static, no?

    remove EVIL workarounds after the "crash on deleting QGraphicsGeoMap" bug is fixed
*/
