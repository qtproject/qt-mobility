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

#include <QtTest/QtTest>

#include "testhelper.h"

#include "qgeomapobject.h"
#include "qgraphicsgeomap.h"
#include "qgeocoordinate.h"
#include <QGraphicsRectItem>
#include <QPointer>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QSignalSpy>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoMapObject : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void init();
    void cleanup();

    // basic property tests
    void holdsGraphicsItem();
    void ownsGraphicsItem();
    void type();
    void holdsVisible();
    void holdsOrigin();

private:
    TestHelper *m_helper;
};

void tst_QGeoMapObject::initTestCase()
{
    qRegisterMetaType<QGeoCoordinate>();
}

void tst_QGeoMapObject::init()
{
    m_helper = new TestHelper();
    QGraphicsGeoMap *map = m_helper->map();

    if (!map)
        QFAIL("Could not create map!");
}

void tst_QGeoMapObject::cleanup()
{
    delete m_helper;
    m_helper = 0;
}

void tst_QGeoMapObject::ownsGraphicsItem()
{
    QPointer<QGeoMapObject> obj = new QGeoMapObject;
    QPointer<QGraphicsTextItem> ri = new QGraphicsTextItem;
    obj->setGraphicsItem(ri);

    delete obj;
    QVERIFY(!ri);
}

void tst_QGeoMapObject::holdsGraphicsItem()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QVERIFY(!obj->graphicsItem());

    QSignalSpy spy(obj, SIGNAL(graphicsItemChanged(QGraphicsItem*)));
    QGraphicsRectItem *ri = new QGraphicsRectItem;
    obj->setGraphicsItem(ri);

    QVERIFY(spy.count() == 1);
    QVERIFY(obj->graphicsItem() == ri);

    obj->setGraphicsItem(0);
    delete ri;
    delete obj;
}

void tst_QGeoMapObject::type()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QCOMPARE(obj->type(), QGeoMapObject::NullType);

    QGraphicsRectItem *ri = new QGraphicsRectItem;

    obj->setGraphicsItem(ri);
    QCOMPARE(obj->type(), QGeoMapObject::CustomType);

    obj->setGraphicsItem(0);
    delete ri;
    delete obj;
}

void tst_QGeoMapObject::holdsVisible()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QVERIFY(obj->isVisible());
    QSignalSpy spy(obj, SIGNAL(visibleChanged(bool)));

    obj->setVisible(true);
    QCOMPARE(spy.count(), 0);

    obj->setVisible(false);
    QCOMPARE(spy.count(), 1);
    QVERIFY(!obj->isVisible());

    delete obj;
}

void tst_QGeoMapObject::holdsOrigin()
{
    QGeoMapObject *obj = new QGeoMapObject;

    QCOMPARE(obj->origin(), QGeoCoordinate());

    QSignalSpy spy(obj, SIGNAL(originChanged(QGeoCoordinate)));

    QGeoCoordinate c(10, 5);
    obj->setOrigin(c);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(obj->origin(), c);

    delete obj;
}

QTEST_MAIN(tst_QGeoMapObject)
#include "tst_qgeomapobject.moc"
