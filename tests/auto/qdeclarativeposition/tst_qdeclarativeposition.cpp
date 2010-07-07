/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtDeclarative/qdeclarativeengine.h>
#include <QtDeclarative/qdeclarativecomponent.h>
#include "qdeclarativeposition.h"
#include <QString>

// Eventually these will make it into qtestcase.h
// but we might need to tweak the timeout values here.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

#ifndef QTRY_WAIT
#define QTRY_WAIT(code, __expr) \
        do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            do { code } while(0); \
            QTest::qWait(__step); \
        } \
    } while(0)
#endif

#ifdef Q_OS_SYMBIAN
// In Symbian OS test data is located in applications private dir
#define SRCDIR "."
#endif

QTM_USE_NAMESPACE

class tst_QDeclarativePosition : public QObject
{
Q_OBJECT

public:
    tst_QDeclarativePosition();
    virtual ~tst_QDeclarativePosition();

public slots:
    void initTestCase();
    void cleanupTestCase();
    
private slots:
    void construction();
    
private:
    // Engine is needed for instantiating declarative components
    QDeclarativeEngine engine;
};

tst_QDeclarativePosition::tst_QDeclarativePosition()
{
}

tst_QDeclarativePosition::~tst_QDeclarativePosition()
{
}

void tst_QDeclarativePosition::initTestCase()
{ 
    qDebug("tst_QDeclarativePosition initTestCase");
}

void tst_QDeclarativePosition::cleanupTestCase()
{
    qDebug("tst_QDeclarativePosition cleanupTestCase");
}

/*
Tests Position element instantiating from plugin and as a c++ class.
*/

void tst_QDeclarativePosition::construction()
{
    qDebug("tst_QDeclarativePosition construction");
    QString componentStr("import Qt 4.7 \n import QtMobility.location 1.0 \n Position {id: positionId }");
    // Component encapsulates one component description
    QDeclarativeComponent component(&engine);
    component.setData(componentStr.toLatin1(), QUrl::fromLocalFile(""));

    QObject* obj = component.create();
    QVERIFY(obj != 0);
    qDebug() << "Class name: " << obj->metaObject()->className();
    QCOMPARE(obj->metaObject()->className(), "QDeclarativePosition");
    
    // The above check is a bit clumsy, but testing with qobject_cast<> will not work,
    // because the obj is instantiated from a aplugin _but_ also the header has been compiled in this
    // test binary --> static meta object addresses do not match. Test below indicates why the 
    // qobject_cast would fail, although the types are actually same. (This could probably be worked around by
    // declaring the interface of QDeclarativePosition into the meta-system).
    // qDebug() << "Address of the plugin-instantiated metaobject of the object: " << QString::number(uint(obj->metaObject()));
    // qDebug() << "Whereas the address of the metaobject compiled in this binary is: " << QString::number(uint(&((QDeclarativePosition*)0)->staticMetaObject));
    // QDeclarativePosition* position = qobject_cast<QDeclarativePosition*>(obj);
    // QVERIFY(position != 0);

    // Check the default values: TODO
    // TODO create _data function that creates with diffrent kind of strings
    
    delete obj;
}

QTEST_MAIN(tst_QDeclarativePosition)
#include "tst_qdeclarativeposition.moc"
