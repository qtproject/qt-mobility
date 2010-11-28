#ifndef MAPBOX_H
#define MAPBOX_H

#include <QWidget>

#include <qmobilityglobal.h>

class QGraphicsScene;
class QGraphicsView;
class StatsWidget;

QTM_BEGIN_NAMESPACE
    class QGraphicsGeoMap;
    class QGeoServiceProvider;
    class QGeoMappingManager;
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


protected:
    void resizeEvent(QResizeEvent * event);

private slots:
    void resetProvider();
};

#endif // MAPBOX_H
