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
    virtual ~TestHelper();
    QGraphicsGeoMap* map()
    {
        return m_map;
    }
    ;
    static void loadProvider(const QString& provider);
    static void unloadProvider();
private:
    static QGeoServiceProvider* m_serviceProvider;
    QGraphicsGeoMap* m_map;
    QGraphicsScene m_scene;
    QGraphicsView m_view;

};

#endif /* TESTHELPER_H_ */
