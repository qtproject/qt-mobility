/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
