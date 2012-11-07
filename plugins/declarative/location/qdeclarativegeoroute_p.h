/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
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

#ifndef QDECLARATIVEGEOROUTE_H
#define QDECLARATIVEGEOROUTE_H

#include "qdeclarativegeoboundingbox_p.h"
#include "qdeclarativecoordinate_p.h"
#include "qdeclarativegeoroutesegment_p.h"

#include <qgeoroute.h>

#include <QObject>

QTM_BEGIN_NAMESPACE

class QDeclarativeGeoRoute : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeGeoBoundingBox *bounds READ bounds CONSTANT)
    Q_PROPERTY(int travelTime READ travelTime CONSTANT)
    Q_PROPERTY(qreal distance READ distance CONSTANT)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeCoordinate> path READ path)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> segments READ segments)

public:
    QDeclarativeGeoRoute(QObject *parent = 0);
    QDeclarativeGeoRoute(const QGeoRoute &route, QObject *parent = 0);
    ~QDeclarativeGeoRoute();

    QDeclarativeGeoBoundingBox* bounds() const;
    int travelTime() const;
    qreal distance() const;
    QDeclarativeListProperty<QDeclarativeCoordinate> path();
    QDeclarativeListProperty<QDeclarativeGeoRouteSegment> segments();

private:
    static void path_append(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, QDeclarativeCoordinate *coordinate);
    static int path_count(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);
    static QDeclarativeCoordinate* path_at(QDeclarativeListProperty<QDeclarativeCoordinate> *prop, int index);
    static void path_clear(QDeclarativeListProperty<QDeclarativeCoordinate> *prop);

    static void segments_append(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop, QDeclarativeGeoRouteSegment *segment);
    static int segments_count(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop);
    static QDeclarativeGeoRouteSegment* segments_at(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop, int index);
    static void segments_clear(QDeclarativeListProperty<QDeclarativeGeoRouteSegment> *prop);

    QGeoRoute route_;
    QDeclarativeGeoBoundingBox* bounds_;
    QList<QDeclarativeCoordinate*> path_;
    QList<QDeclarativeGeoRouteSegment*> segments_;
};


QTM_END_NAMESPACE

#endif

