#include "boxgraphicsscene.h"
#include "mapbox.h"
#include "statswidget.h"
#include "performance.h"

#include <QVariant>

BoxGraphicsScene::BoxGraphicsScene(StatsWidget * statistics, MapBox *parent) :
    QGraphicsScene(parent),
    m_statistics(statistics)
{
}

void BoxGraphicsScene::drawItems(QPainter *painter,
                               int numItems,
                               QGraphicsItem *items[],
                               const QStyleOptionGraphicsItem options[], QWidget *widget)
{
    perf_t before_rendering = perf_currentTime();
    QGraphicsScene::drawItems(painter, numItems, items, options, widget);

    perf_t after_rendering = perf_currentTime();

    m_statistics->stat("Render time", perf_diffTime(before_rendering, after_rendering));
    m_statistics->stat("FPS", 1.0/perf_diffTime(last_after_rendering, after_rendering));

    last_after_rendering = after_rendering;
}
