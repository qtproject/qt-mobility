#ifndef BOXGRAPHICSSCENE_H
#define BOXGRAPHICSSCENE_H

#include <QGraphicsScene>
#include "performance.h"

class StatsWidget;
class MapBox;

class BoxGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
private:
    StatsWidget * m_statistics;
    perf_t last_after_rendering;

public:
    explicit BoxGraphicsScene(StatsWidget * statistics, MapBox * parent = 0);

protected:
    virtual void drawItems(QPainter *painter, int numItems,
                           QGraphicsItem *items[],
                           const QStyleOptionGraphicsItem options[],
                           QWidget *widget = 0);

};

#endif // BOXGRAPHICSSCENE_H
