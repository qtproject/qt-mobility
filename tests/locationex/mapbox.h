#ifndef MAPBOX_H
#define MAPBOX_H

#include <QWidget>

#include <qmobilityglobal.h>

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
    void createMapWidget();

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

protected:
    virtual void resizeEvent(QResizeEvent * event);
    virtual void timerEvent(QTimerEvent * event);

private slots:
    void routeFinished();
};

#endif // MAPBOX_H
