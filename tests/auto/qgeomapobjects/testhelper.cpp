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

QGeoServiceProvider* TestHelper::m_serviceProvider = 0;

TestHelper::TestHelper() :
    m_map(0), m_view(&m_scene)
{

    if (!m_serviceProvider) {

        qDebug("Geoservices plugin not loaded.");

        return;
    }

    QGeoMappingManager* mapManager = m_serviceProvider->mappingManager();

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

    if (m_map) {
        delete m_map;
    }
}

void TestHelper::loadProvider(const QString& provider)
{

    if (!m_serviceProvider) {

        m_serviceProvider = new QGeoServiceProvider(provider);

        if (m_serviceProvider->error() != QGeoServiceProvider::NoError) {

            qDebug("Unable to find geoservices plugin.");

            m_serviceProvider = 0;

        }
    }
}

void TestHelper::unloadProvider()
{

    if (m_serviceProvider) {

        delete m_serviceProvider;

        m_serviceProvider = 0;

    }

}

