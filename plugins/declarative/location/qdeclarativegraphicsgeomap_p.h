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

#ifndef QDECLARATIVEGRAPHICSGEOMAP_H
#define QDECLARATIVEGRAPHICSGEOMAP_H

#include "qdeclarativecoordinate_p.h"
#include "qgraphicsgeomap.h"
#include "qgeomapobject.h"

class QGraphicsItem;

QTM_BEGIN_NAMESPACE

class QGeoCoordinate;

class QDeclarativeGraphicsGeoMap : public QGraphicsGeoMap
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeCoordinate* center READ declarativeCenter WRITE setDeclarativeCenter NOTIFY declarativeCenterChanged)
    Q_PROPERTY(QDeclarativeListProperty<QGeoMapObject> objects READ objects)

    Q_CLASSINFO("DefaultProperty", "objects")

public:
    QDeclarativeGraphicsGeoMap(QGraphicsItem *parent = 0);
    ~QDeclarativeGraphicsGeoMap();

    QDeclarativeListProperty<QGeoMapObject> objects();

    void setDeclarativeCenter(const QDeclarativeCoordinate *center);
    QDeclarativeCoordinate* declarativeCenter() const;

    Q_INVOKABLE QDeclarativeCoordinate* toCoordinate(QPointF screenPosition) const;
    Q_INVOKABLE QPointF toScreenPosition(QDeclarativeCoordinate* coordinate) const;

private slots:
    void memberCenterChanged(const QGeoCoordinate &coordinate);

signals:
    void declarativeCenterChanged(const QDeclarativeCoordinate *coordinate);

private:
    static void object_append(QDeclarativeListProperty<QGeoMapObject> *prop, QGeoMapObject *mapObject);
    static int object_count(QDeclarativeListProperty<QGeoMapObject> *prop);
    static QGeoMapObject* object_at(QDeclarativeListProperty<QGeoMapObject> *prop, int index);
    static void object_clear(QDeclarativeListProperty<QGeoMapObject> *prop);

    QDeclarativeCoordinate* m_center;
    Q_DISABLE_COPY(QDeclarativeGraphicsGeoMap)
};

QTM_END_NAMESPACE

QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGraphicsGeoMap));

#endif
