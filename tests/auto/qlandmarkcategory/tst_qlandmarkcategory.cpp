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

//TESTED_COMPONENT=src/location

#include <qlandmark.h>
#include <qlandmarkcategory.h>

#include <qtest.h>
#include <QMetaType>

QTM_USE_NAMESPACE

class tst_QLandmarkCategory : public QObject
{
    Q_OBJECT

private slots:
  void settersAndGetters() {
        QFETCH(QString, name);
        QFETCH(QUrl, iconUrl);

        QLandmarkCategory cat;

        cat.setName(name);
        cat.setIconUrl(iconUrl);

        QCOMPARE(cat.name(), name);
        QCOMPARE(cat.iconUrl(), iconUrl);
    }

    void settersAndGetters_data() {
        genericData();
    }

    void copyConstructor() {
        QFETCH(QString, name);
        QFETCH(QUrl, iconUrl);

        QLandmarkCategory cat1;

        cat1.setName(name);
        cat1.setIconUrl(iconUrl);

        QLandmarkCategory cat2(cat1);

        QCOMPARE(cat2.name(), name);
        QCOMPARE(cat2.iconUrl(), iconUrl);
    }

    void copyConstructor_data() {
        genericData();
    }

    void assignment() {
        QFETCH(QString, name);
        QFETCH(QUrl, iconUrl);

        QLandmarkCategory cat1;
        QLandmarkCategory cat2;

        cat2.setName("test");

        cat1.setName(name);
        cat1.setIconUrl(iconUrl);
        cat2 = cat1;

        QCOMPARE(cat2.name(), name);
        QCOMPARE(cat2.iconUrl(), iconUrl);
    }

    void assignment_data() {
        genericData();
    }

    void equality() {
        QFETCH(QLandmarkCategory, lop);
        QFETCH(QLandmarkCategory, rop);
        QFETCH(bool, isEqual);

        QCOMPARE((lop == rop), isEqual);
    }

    void equality_data() {
        QTest::addColumn<QLandmarkCategory>("lop");
        QTest::addColumn<QLandmarkCategory>("rop");
        QTest::addColumn<bool>("isEqual");

        QLandmarkCategory cat0;
        cat0.setName("equalName");
        cat0.setIconUrl(QUrl("equalIcon"));

        QLandmarkCategory cat1;
        cat1.setName("unequalName");
        cat1.setIconUrl(QUrl("unequalIcon"));
        QTest::newRow("allUnequal") << cat0 << cat1 << false;

        QLandmarkCategory cat2(cat0);
        cat2.setName("unequalName");
        QTest::newRow("nameUnequal") << cat0 << cat2 << false;

        QLandmarkCategory cat3(cat0);
        cat3.setIconUrl(QUrl("unequalIcon"));
        QTest::newRow("iconUnequal") << cat0 << cat3 << false;

        QLandmarkCategory cat5(cat0);
        QTest::newRow("allEqual") << cat0 << cat5 << true;
    }

private:
    void genericData() {
        QTest::addColumn<QString>("name");
        QTest::addColumn<QUrl>("iconUrl");

        QTest::newRow("Row0") << "" << QUrl("");
        QTest::newRow("Row1") << "categoryName" << QUrl("");
        QTest::newRow("Row2") << "" << QUrl("categoryIcon");
        QTest::newRow("Row3") << "categoryName" << QUrl("categoryIcon");
        QTest::newRow("Row4") << "" << QUrl("");
        QTest::newRow("Row5") << "categoryName" << QUrl("") ;
        QTest::newRow("Row6") << "" << QUrl("categoryIcon");
        QTest::newRow("Row7") << "categoryName" << QUrl("categoryIcon");
    }
};

QTEST_MAIN(tst_QLandmarkCategory)
#include "tst_qlandmarkcategory.moc"
