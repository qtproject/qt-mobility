/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QApplication>

#include <qlandmarkmanager.h>

QTM_USE_NAMESPACE

class tst_QLandmarkManagerPlugins : public QObject
{
Q_OBJECT
public:
    tst_QLandmarkManagerPlugins();
    virtual ~tst_QLandmarkManagerPlugins();

public slots:
    void init();
    void cleanup();
private slots:
    void testDummy();
};

tst_QLandmarkManagerPlugins::tst_QLandmarkManagerPlugins()
{
}

tst_QLandmarkManagerPlugins::~tst_QLandmarkManagerPlugins()
{
}

void tst_QLandmarkManagerPlugins::init()
{
}

void tst_QLandmarkManagerPlugins::cleanup()
{
}

Q_IMPORT_PLUGIN(landmarks_testdummystatic)

void tst_QLandmarkManagerPlugins::testDummy()
{
#ifndef Q_OS_SYMBIAN
    QVERIFY(QLandmarkManager::availableManagers().contains("LandmarkManagerFactoryDummyStatic"));
#endif
    QVERIFY(QLandmarkManager::availableManagers().contains("LandmarkManagerFactoryDummyDynamic"));
#if !defined (Q_OS_SYMBIAN) && !defined(Q_OS_WINCE) && !defined(Q_WS_MAEMO_5) && !defined(Q_WS_MAEMO_6)

    QVERIFY(QLandmarkManager::availableManagers().contains("com.nokia.qt.landmarks.engines.sqlite"));
    QLandmarkManager lm("com.nokia.qt.landmarks.engines.sqlite");
    QVERIFY(lm.managerName() == "com.nokia.qt.landmarks.engines.sqlite");
#endif
}

QTEST_MAIN(tst_QLandmarkManagerPlugins)
#include "tst_qlandmarkmanagerplugins.moc"
