#include <QtTest/QtTest>
#include <QString>
#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>
#include "testhelper.h"
#include <qgraphicsgeomap.h>
#include <qgeocoordinate.h>

QTM_USE_NAMESPACE

TestHelper::TestHelper() :
    m_serviceProvider("geomapobject.test.plugin"),
    m_map(0),
    m_view(&m_scene)
{
    if (m_serviceProvider.error() != QGeoServiceProvider::NoError) {
        qDebug("Unable to find geoservices plugin.");
        qDebug() << m_serviceProvider.errorString();
        return;
    }

    QGeoMappingManager* mapManager = m_serviceProvider.mappingManager();
    if (mapManager)
        m_map = new QGraphicsGeoMap(mapManager);

    if (m_map == 0) {
        qDebug("Unable to create QGraphicsGeoMap.");
        return;
    }

    m_view.scene()->addItem(m_map);
    m_view.setGeometry(QRect(0, 0, 600, 600));
    m_map->setGeometry(QRect(0, 0, 600, 600));
    m_view.show();

    //set dummy data
    m_map->setCenter(QGeoCoordinate(52.5, 13.4, 0));
    m_map->setZoomLevel(10);
}

TestHelper::~TestHelper()
{
    if (m_map)
        delete m_map;
}

QGraphicsGeoMap* TestHelper::map()
{
    return m_map;
}

