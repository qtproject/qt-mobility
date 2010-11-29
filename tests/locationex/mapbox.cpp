#include "mapbox.h"
#include "statswidget.h"
#include "boxgraphicsscene.h"

#include <qgraphicsgeomap.h>
#include <qgeoserviceprovider.h>
#include <qgeomappingmanager.h>
#include <qgeocoordinate.h>

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QMessageBox>
#include <QApplication>
#include <QPushButton>

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
    //m_statistics->stat("map objects", 0);
    startTimer(1000);
}

void MapBox::timerEvent(QTimerEvent * event)
{
    m_statistics->stat("mem", perf_currentMemUsage());
}

void MapBox::resetProvider()
{
    setProvider("nokia");
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

/* TODO
    network session!!!

    statistics panel
        - Render time: (nach_rendering-vor_rendering)" und "FPS: 1/(nach_rendering-nach_letztem_rendering)".
            - derive scene, override paint, measure time there
        - mem
            - wird eine plattformspezifische Lösung
        - objects on map
            - alles private, komme an nix ran, könnte mich höchstens durchhacken
        - network traffic
            - evtl session?
        - render mode
            - was ist gemeint?

    remove EVIL workarounds after the "crash on deleting QGraphicsGeoMap" bug is fixed
*/
