/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
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
#include "qgeocoordinate.h"
#include <QGraphicsItem>
#include <QSignalSpy>
#include <qgeoboundingbox.h>
#include "tst_qgeomapobjectinfoinherit.h"
#include "qgeomappingmanagerengine.h"
#include "qgeomappingmanager.h"
#include "qgeomapdata.h"

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QGeoCoordinate)

class tst_QGeoMapObjectInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void initTest();
    void initTest_data();
    void boundingBox();
    void boundingBox_data();
    void contains();
    void contains_data();
    void windowSizeChanged();
    void windowSizeChanged_data();
    void zoomLevelChanged();
    void zoomLevelChanged_data();
    void centerChanged();
    void centerChanged_data();
    void zValueChanged();
    void zValueChanged_data();
    void visibleChanged();
    void visibleChanged_data();
    void selectedChanged();
    void selectedChanged_data();
    void originChanged();
    void originChanged_data();
    void unitsChanged();
    void unitsChanged_data();
    void transformTypeChanged();
    void transformTypeChanged_data();
    void mapData();
    void mapData_data();
    void mapObject();
    void mapObject_data();
private :
    QGeoMapObjectInfoInherit* object;
};


void tst_QGeoMapObjectInfo::initTestCase()
{
    object = new QGeoMapObjectInfoInherit;
}

void tst_QGeoMapObjectInfo::init()
{
}

void tst_QGeoMapObjectInfo::cleanup()
{

}

void tst_QGeoMapObjectInfo::cleanupTestCase()
{
        delete object;
}

void tst_QGeoMapObjectInfo::initTest()
{
    QFETCH(QString, init);

    QBENCHMARK{
       object->init();
    }
}

void tst_QGeoMapObjectInfo::initTest_data()
{
    QTest::addColumn<QString>("init");
    QTest::newRow("init") << "init";
}

void tst_QGeoMapObjectInfo::boundingBox()
{
    QFETCH(QString, boundingBox);

    QBENCHMARK{
       object->boundingBox();
    }
}

void tst_QGeoMapObjectInfo::boundingBox_data()
{
    QTest::addColumn<QString>("boundingBox");
    QTest::newRow("boundingBox") << "boundingBox";
}

void tst_QGeoMapObjectInfo::contains()
{
    QFETCH(QGeoCoordinate, contains);

    QBENCHMARK{
        object->contains(contains);
    }
}

void tst_QGeoMapObjectInfo::contains_data()
{
    QTest::addColumn<QGeoCoordinate>("contains");
    QTest::newRow("contains") << QGeoCoordinate(10, 10, 0);
}

void tst_QGeoMapObjectInfo::windowSizeChanged()
{
    QFETCH(QString, windowSizeChanged);

    object->emitWindowSizeChanged();
}

void tst_QGeoMapObjectInfo::windowSizeChanged_data()
{
    QTest::addColumn<QString>("windowSizeChanged");
    QTest::newRow("windowSizeChanged") << "windowSizeChanged";
}

void tst_QGeoMapObjectInfo::zoomLevelChanged()
{
    QFETCH(QString, zoomLevelChanged);

    object->emitZoomLevelChanged();
}

void tst_QGeoMapObjectInfo::zoomLevelChanged_data()
{
    QTest::addColumn<QString>("zoomLevelChanged");
    QTest::newRow("zoomLevelChanged") << "zoomLevelChanged";
}

void tst_QGeoMapObjectInfo::centerChanged()
{
    QFETCH(QString, centerChanged);

    object->emitCenterChanged();
}

void tst_QGeoMapObjectInfo::centerChanged_data()
{
    QTest::addColumn<QString>("centerChanged");
    QTest::newRow("centerChanged") << "centerChanged";
}

void tst_QGeoMapObjectInfo::zValueChanged()
{
    QFETCH(QString, zValueChanged);

    object->emitZValueChanged();
}

void tst_QGeoMapObjectInfo::zValueChanged_data()
{
    QTest::addColumn<QString>("zValueChanged");
    QTest::newRow("zValueChanged") << "zValueChanged";
}

void tst_QGeoMapObjectInfo::visibleChanged()
{
    QFETCH(QString, visibleChanged);

    object->emitVisibleChanged();
}

void tst_QGeoMapObjectInfo::visibleChanged_data()
{
    QTest::addColumn<QString>("visibleChanged");
    QTest::newRow("visibleChanged") << "visibleChanged";
}

void tst_QGeoMapObjectInfo::selectedChanged()
{
    QFETCH(QString, selectedChanged);

    object->emitSelectedChanged();
}

void tst_QGeoMapObjectInfo::selectedChanged_data()
{
    QTest::addColumn<QString>("selectedChanged");
    QTest::newRow("selectedChanged") << "selectedChanged";
}

void tst_QGeoMapObjectInfo::originChanged()
{
    QFETCH(QString, originChanged);

    object->emitOriginChanged();
}

void tst_QGeoMapObjectInfo::originChanged_data()
{
    QTest::addColumn<QString>("originChanged");
    QTest::newRow("originChanged") << "originChanged";
}

void tst_QGeoMapObjectInfo::unitsChanged()
{
    QFETCH(QString, unitsChanged);

    object->emitUnitsChanged();
}

void tst_QGeoMapObjectInfo::unitsChanged_data()
{
    QTest::addColumn<QString>("unitsChanged");
    QTest::newRow("unitsChanged") << "unitsChanged";
}

void tst_QGeoMapObjectInfo::transformTypeChanged()
{
    QFETCH(QString, transformTypeChanged);

    object->emitTransformTypeChanged();
}

void tst_QGeoMapObjectInfo::transformTypeChanged_data()
{
    QTest::addColumn<QString>("transformTypeChanged");
    QTest::newRow("transformTypeChanged") << "transformTypeChanged";
}

void tst_QGeoMapObjectInfo::mapData()
{
    QFETCH(QString, mapData);

    object->callMapData();
}

void tst_QGeoMapObjectInfo::mapData_data()
{
    QTest::addColumn<QString>("mapData");
    QTest::newRow("mapData") << "mapData";
}

void tst_QGeoMapObjectInfo::mapObject()
{
    QFETCH(QString, mapObject);

    object->callMapObject();
}

void tst_QGeoMapObjectInfo::mapObject_data()
{
    QTest::addColumn<QString>("mapObject");
    QTest::newRow("mapObject") << "mapObject";
}

QTEST_MAIN(tst_QGeoMapObjectInfo)
#include "tst_qgeomapobjectinfo.moc"
