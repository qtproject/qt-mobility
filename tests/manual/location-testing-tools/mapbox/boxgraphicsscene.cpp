/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
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
