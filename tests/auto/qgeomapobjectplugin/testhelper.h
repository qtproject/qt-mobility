#ifndef TESTHELPER_H_
#define TESTHELPER_H_

#include <qgeoserviceprovider.h>
#include <QGraphicsScene>
#include <QGraphicsView>
class QString;

QTM_BEGIN_NAMESPACE
class QGraphicsGeoMap;
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

class TestHelper
{
public:
    TestHelper();
    ~TestHelper();
    QGraphicsGeoMap* map();

private:
    QGeoServiceProvider m_serviceProvider;
    QGraphicsGeoMap* m_map;
    QGraphicsScene m_scene;
    QGraphicsView m_view;

};

#endif /* TESTHELPER_H_ */
