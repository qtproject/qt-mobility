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

#ifndef QDECLARATIVEGEOMAPOBJECT_H
#define QDECLARATIVEGEOMAPOBJECT_H

#include "qgeomapobject.h"
#include "qdeclarativegeomapmouseevent_p.h"
#include "qgeomapgroupobject.h"
#include "QModelIndex"

#include <QtDeclarative/qdeclarativeitem.h>
class QAbstractItemModel;

QTM_BEGIN_NAMESPACE

class QDeclarativeGraphicsGeoMap;
class QDeclarativeGeoMapMouseArea;

class QDeclarativeGeoMapObject : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)

public:
    QDeclarativeGeoMapObject(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapObject();

    virtual void componentComplete();

    virtual void setMap(QDeclarativeGraphicsGeoMap *map);
    QDeclarativeGraphicsGeoMap* map() const;

    void setMapObject(QGeoMapObject *object);
    QGeoMapObject* mapObject();

    void setVisible(bool visible);
    bool isVisible() const;

    virtual void doubleClickEvent(QDeclarativeGeoMapMouseEvent *event);
    virtual void pressEvent(QDeclarativeGeoMapMouseEvent *event);
    virtual void releaseEvent(QDeclarativeGeoMapMouseEvent *event);
    virtual void enterEvent();
    virtual void exitEvent();
    virtual void moveEvent(QDeclarativeGeoMapMouseEvent *event);

Q_SIGNALS:
    void visibleChanged(bool visible);

private Q_SLOTS:
    void parentZChanged();

private:
    QGeoMapObject *object_;
    bool visible_;
    QDeclarativeGraphicsGeoMap* map_;
    QList<QDeclarativeGeoMapMouseArea*> mouseAreas_;
};


class QDeclarativeGeoMapObjectView : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
    Q_PROPERTY(qreal z READ zValue WRITE setZValue NOTIFY zChanged)
    Q_INTERFACES(QDeclarativeParserStatus)

public:
    QDeclarativeGeoMapObjectView(QDeclarativeItem *parent = 0);
    ~QDeclarativeGeoMapObjectView();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent*);

    void setMapData(QDeclarativeGraphicsGeoMap*);
    void repopulate();
    void removeInstantiatedItems();

    qreal zValue();
    void setZValue(qreal zValue);

    void setVisible(bool visible);
    bool isVisible() const;

    QDeclarativeGeoMapObject* createItem(int modelRow);
    // From QDeclarativeParserStatus
    virtual void componentComplete();
    void classBegin() {}

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void visibleChanged();
    void zChanged();

private Q_SLOTS:
    void modelReset();
    void modelRowsInserted(QModelIndex, int start, int end);
    void modelRowsRemoved(QModelIndex, int start, int end);

private:
    bool visible_;
    bool componentCompleted_;
    QDeclarativeComponent *delegate_;
    QVariant modelVariant_;
    QAbstractItemModel* model_;
    QDeclarativeGraphicsGeoMap *map_;
    QGeoMapGroupObject group_;
    QList<QDeclarativeGeoMapObject*> declarativeObjectList_;
};

QTM_END_NAMESPACE
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapObject));
QML_DECLARE_TYPE(QTM_PREPEND_NAMESPACE(QDeclarativeGeoMapObjectView));

#endif
