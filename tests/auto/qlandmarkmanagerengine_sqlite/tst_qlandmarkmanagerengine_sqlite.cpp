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
#include <QtTest/QSignalSpy>

#include "qlandmark.h"
#include "qlandmarkid.h"
#include "qlandmarkcategory.h"
#include "qlandmarkcategoryid.h"
#include "qlandmarkmanager.h"
#include "qgeolocation.h"
#include "qgeocoordinate.h"

#include "qlandmarkfilter.h"
#include "qlandmarknamefilter.h"
#include "qlandmarkproximityfilter.h"
#include "qlandmarknearestfilter.h"
#include "qlandmarkcategoryfilter.h"
#include "qlandmarkboxfilter.h"
#include "qlandmarkintersectionfilter.h"
#include "qlandmarkunionfilter.h"

#include "qlandmarksortorder.h"
#include "qlandmarknamesort.h"
#include "qlandmarkdistancesort.h"

#include <QMetaType>
#include <QDebug>

#include <QFile>
#include <QList>
#include <QPair>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <float.h>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QList<QLandmarkCategoryId>);
Q_DECLARE_METATYPE(QList<QLandmarkId>);

class tst_QLandmarkManagerEngineSqlite : public QObject
{
    Q_OBJECT

public:
    tst_QLandmarkManagerEngineSqlite() {
        qRegisterMetaType<QList<QLandmarkCategoryId> >();
        qRegisterMetaType<QList<QLandmarkId> >();
    }

private:
    QLandmarkManager *m_manager;

    void createDb() {
        QMap<QString, QString> map;
        map["filename"] = "test.db";
        m_manager = new QLandmarkManager("com.nokia.qt.landmarks.engines.sqlite", map);
    }

    void deleteDb() {
        {
            QSqlDatabase db = QSqlDatabase::database("landmarks");
            QSqlQuery q1("delete from landmark;", db);
            q1.exec();
            QSqlQuery q2("delete from category;", db);
            q2.exec();
            QSqlQuery q3("delete from landmark_category;", db);
            q3.exec();
        }

        delete m_manager;
        QFile file("test.db");
        file.remove();
    }

    bool tablesExist() {
        QStringList tables = QSqlDatabase::database("landmarks").tables();
        tables.sort();

        QStringList knownTables;
        knownTables << "category";
        knownTables << "category_attribute";
        knownTables << "landmark";
        knownTables << "landmark_attribute";
        knownTables << "landmark_category";

        return (tables == knownTables);
    }

private slots:

    void init() {
        createDb();
    }

    void cleanup() {
        deleteDb();
    }

    void createDbNew() {
        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QVERIFY(tablesExist());

        deleteDb();
        createDb();

        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QVERIFY(tablesExist());
    }

    void createDbExists() {
        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QVERIFY(tablesExist());

        createDb();

        QCOMPARE(m_manager->error(), QLandmarkManager::NoError);
        QVERIFY(tablesExist());
    }

    void retrieveCategory() {
        QLandmarkCategoryId id1;
        id1.setManagerUri(m_manager->managerUri());
        id1.setId("1");
        QCOMPARE(m_manager->category(id1).id().isValid(), false);

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategoryId id2;
        id2.setManagerUri(m_manager->managerUri());
        id2.setId(cat2.id().id());
        QCOMPARE(cat2, m_manager->category(id2));

        // add  - with attributes
        // get present
    }

    void retrieveLandmark() {
        QLandmarkId id1;
        id1.setManagerUri(m_manager->managerUri());
        id1.setId("1");
        QCOMPARE(m_manager->landmark(id1).id().isValid(), false);

        QLandmark lm2;
        lm2.setName("LM2");
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmarkId id2;
        id2.setManagerUri(m_manager->managerUri());
        id2.setId(lm2.id().id());
        QCOMPARE(lm2, m_manager->landmark(id2));

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QLandmark lm3;
        lm3.setName("LM3");
        lm3.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm3));

        QLandmarkId id3;
        id3.setManagerUri(m_manager->managerUri());
        id3.setId(lm3.id().id());
        QCOMPARE(lm3, m_manager->landmark(id3));
    }

    void addCategory() {
        QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));

        // add - no attributes
        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));
        QCOMPARE(cat1, m_manager->category(cat1.id()));

        QCOMPARE(spyAdd.count(), 1);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.id());

        // add - with attributes
    }

    void updateCategory() {
        QSignalSpy spyAdd(m_manager, SIGNAL(categoriesAdded(QList<QLandmarkCategoryId>)));
        QSignalSpy spyChange(m_manager, SIGNAL(categoriesChanged(QList<QLandmarkCategoryId>)));

        // add - with attributes
        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        //cat1.setAttribute("Key1", "Value1");
        QVERIFY(m_manager->saveCategory(&cat1));
        QCOMPARE(cat1, m_manager->category(cat1.id()));

        QCOMPARE(spyAdd.count(), 1);
        QCOMPARE(spyChange.count(), 0);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.id());
        spyAdd.clear();

        // update core
        cat1.setName("CAT1New");
        QVERIFY(m_manager->saveCategory(&cat1));
        QCOMPARE(cat1, m_manager->category(cat1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat1.id());

        // update attributes - existing
        // update attributes - added
        // update attributes - removed
    }

    void addLandmark() {
        QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));

        // add - no attributes
        QLandmark lm1;
        lm1.setName("LM1");
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 1);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyAdd.clear();

        // add - with attributes

        // add - with categories

        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));
        QCOMPARE(cat1, m_manager->category(cat1.id()));

        QLandmark lm2;
        lm2.setName("LM2");
        lm2.addCategory(cat1.id());
        QVERIFY(m_manager->saveLandmark(&lm2));
        QCOMPARE(lm2, m_manager->landmark(lm2.id()));

        QCOMPARE(spyAdd.count(), 1);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm2.id());
    }

    void updateLandmark() {
        QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
        QSignalSpy spyChange(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)));

        // add - with attributes
        QLandmark lm1;
        lm1.setName("LM1");
        //lm1.setAttribute("Key1", "Value1");
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 1);
        QCOMPARE(spyChange.count(), 0);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyAdd.clear();

        // update core
        lm1.setName("LM1New");
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();

        // update attributes - existing
        // update attributes - added
        // update attributes - removed

        // add and removing categories to landmarks

        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        // adding categories

        lm1.addCategory(cat1.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();

        lm1.addCategory(cat2.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();

        lm1.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();

        // removing categories

        lm1.removeCategory(cat1.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();

        lm1.removeCategory(cat2.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();

        lm1.removeCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        spyChange.clear();
    }

    void removeCategory() {
        QSignalSpy spyRemove(m_manager, SIGNAL(categoriesRemoved(QList<QLandmarkCategoryId>)));

        QLandmarkCategoryId id1;
        id1.setManagerUri(m_manager->managerUri());
        id1.setId("1");

        QVERIFY(m_manager->removeCategory(id1));

        QCOMPARE(spyRemove.count(), 0);

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QVERIFY(m_manager->removeCategory(cat2.id()));

        QCOMPARE(spyRemove.count(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat2.id());
        spyRemove.clear();

        // with attributes

        // effect on landmarks

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QLandmark lm1;
        lm1.setName("LM1");
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        lm1.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm1));
        QCOMPARE(lm1, m_manager->landmark(lm1.id()));

        QVERIFY(m_manager->removeCategory(cat3.id()));

        QCOMPARE(spyRemove.count(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkCategoryId> >().at(0), cat3.id());
        spyRemove.clear();

        QLandmark lm2 = m_manager->landmark(lm1.id());
        QVERIFY(lm1 != lm2);
        QCOMPARE(lm1.categories().size(), 1);
        QCOMPARE(lm2.categories().size(), 0);
    }

    void removeLandmark() {
        QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));

        QLandmarkId id1;
        id1.setManagerUri(m_manager->managerUri());
        id1.setId("1");

        QVERIFY(m_manager->removeLandmark(id1));

        QCOMPARE(spyRemove.count(), 0);

        QLandmark lm2;
        lm2.setName("LM2");
        QVERIFY(m_manager->saveLandmark(&lm2));

        QVERIFY(m_manager->removeLandmark(lm2.id()));

        QCOMPARE(spyRemove.count(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm2.id());
        spyRemove.clear();

        // with attributes

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QLandmark lm3;
        lm3.setName("LM3");
        lm3.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm3));

        QVERIFY(m_manager->removeLandmark(lm3.id()));

        QCOMPARE(spyRemove.count(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm3.id());
        spyRemove.clear();
    }

    void retrieveMultipleCategories() {
        QList<QLandmarkCategoryId> catIds;

        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));
        catIds << cat1.id();

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));
        catIds << cat3.id();

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));
        catIds << cat2.id();

        QString uri = m_manager->managerUri();
        int i = 1;

        QList<QLandmarkCategoryId> invalidCatIds;

        while (invalidCatIds.size() < 3) {
            QLandmarkCategoryId id;
            id.setManagerUri(uri);
            id.setId(QString::number(i));
            QLandmarkCategory cat = m_manager->category(id);
            if (!cat.id().isValid()) {
                invalidCatIds << id;
            }
            ++i;
        }

        catIds.insert(1, invalidCatIds.at(0));
        catIds.insert(3, invalidCatIds.at(1));
        catIds.insert(5, invalidCatIds.at(2));

        QList<QLandmarkCategory> cats = m_manager->categories(catIds);

        QCOMPARE(cats.size(), 3);

        QCOMPARE(cats.at(0).name(), QString("CAT1"));
        QCOMPARE(cats.at(0).id().isValid(), true);
        QCOMPARE(cats.at(1).name(), QString("CAT2"));
        QCOMPARE(cats.at(1).id().isValid(), true);
        QCOMPARE(cats.at(2).name(), QString("CAT3"));
        QCOMPARE(cats.at(2).id().isValid(), true);
    }

    void retrieveMultipleLandmarks() {
        QList<QLandmarkId> lmIds;

        QLandmark lm1;
        lm1.setName("LM1");
        QVERIFY(m_manager->saveLandmark(&lm1));
        lmIds << lm1.id();

        QLandmark lm2;
        lm2.setName("LM2");
        QVERIFY(m_manager->saveLandmark(&lm2));
        lmIds << lm2.id();

        QLandmark lm3;
        lm3.setName("LM3");
        QVERIFY(m_manager->saveLandmark(&lm3));
        lmIds << lm3.id();

        QString uri = m_manager->managerUri();
        int i = 1;

        QList<QLandmarkId> invalidLmIds;

        while (invalidLmIds.size() < 3) {
            QLandmarkId id;
            id.setManagerUri(uri);
            id.setId(QString::number(i));
            QLandmark lm = m_manager->landmark(id);
            if (!lm.id().isValid()) {
                invalidLmIds << id;
            }
            ++i;
        }

        lmIds.insert(1, invalidLmIds.at(0));
        lmIds.insert(3, invalidLmIds.at(1));
        lmIds.insert(5, invalidLmIds.at(2));

        QList<QLandmark> lms = m_manager->landmarks(lmIds);

        QCOMPARE(lms.size(), 3);

        QCOMPARE(lms.at(0).name(), QString("LM1"));
        QCOMPARE(lms.at(0).id().isValid(), true);
        QCOMPARE(lms.at(1).name(), QString("LM2"));
        QCOMPARE(lms.at(1).id().isValid(), true);
        QCOMPARE(lms.at(2).name(), QString("LM3"));
        QCOMPARE(lms.at(2).id().isValid(), true);
    }

    void saveMultipleLandmarks() {
        QSignalSpy spyAdd(m_manager, SIGNAL(landmarksAdded(QList<QLandmarkId>)));
        QSignalSpy spyChange(m_manager, SIGNAL(landmarksChanged(QList<QLandmarkId>)));

        QList<QLandmark> lms1;

        QLandmark lm1;
        lm1.setName("LM1");
        lms1 << lm1;

        QLandmark lm2;
        lm2.setName("LM2");
        lms1 << lm2;

        QLandmark lm3;
        lm3.setName("LM3");
        lms1 << lm3;

        QVERIFY(m_manager->saveLandmarks(&lms1, 0));

        lm1 = lms1.at(0);
        lm2 = lms1.at(1);
        lm3 = lms1.at(2);

        QCOMPARE(lm1, m_manager->landmark(lm1.id()));
        QCOMPARE(lm2, m_manager->landmark(lm2.id()));
        QCOMPARE(lm3, m_manager->landmark(lm3.id()));

        QCOMPARE(spyAdd.count(), 1);
        QCOMPARE(spyChange.count(), 0);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().size(), 3);
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.id());
        QCOMPARE(spyAdd.at(0).at(0).value<QList<QLandmarkId> >().at(2), lm3.id());
        spyAdd.clear();

        QList<QLandmark> lms2;

        lm1.setName("LM1New");
        lms2 << lm1;

        lm2.setName("LM2New");
        lms2 << lm2;

        lm3.setName("LM3New");
        lms2 << lm3;

        QVERIFY(m_manager->saveLandmarks(&lms2, 0));

        lm1 = lms2.at(0);
        lm2 = lms2.at(1);
        lm3 = lms2.at(2);

        QCOMPARE(lm1, m_manager->landmark(lm1.id()));
        QCOMPARE(lm2, m_manager->landmark(lm2.id()));
        QCOMPARE(lm3, m_manager->landmark(lm3.id()));

        QCOMPARE(spyAdd.count(), 0);
        QCOMPARE(spyChange.count(), 1);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().size(), 3);
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.id());
        QCOMPARE(spyChange.at(0).at(0).value<QList<QLandmarkId> >().at(2), lm3.id());
        spyChange.clear();
    }

    void removeMultipleLandmarks() {
        QSignalSpy spyRemove(m_manager, SIGNAL(landmarksRemoved(QList<QLandmarkId>)));

        QList<QLandmarkId> lmIds1;
        QList<QLandmarkId> lmIds2;

        QLandmark lm1;
        lm1.setName("LM1");
        QVERIFY(m_manager->saveLandmark(&lm1));
        lmIds1 << lm1.id();

        QLandmark lm2;
        lm2.setName("LM2");
        QVERIFY(m_manager->saveLandmark(&lm2));
        lmIds1 << lm2.id();

        QLandmark lm3;
        lm3.setName("LM3");
        QVERIFY(m_manager->saveLandmark(&lm3));
        lmIds2 << lm3.id();

        QString uri = m_manager->managerUri();
        int i = 1;

        QList<QLandmarkId> invalidLmIds;

        while (invalidLmIds.size() < 3) {
            QLandmarkId id;
            id.setManagerUri(uri);
            id.setId(QString::number(i));
            QLandmark lm = m_manager->landmark(id);
            if (!lm.id().isValid()) {
                invalidLmIds << id;
            }
            ++i;
        }

        lmIds1.insert(1, invalidLmIds.at(0));
        lmIds2.insert(0, invalidLmIds.at(1));
        lmIds2.insert(2, invalidLmIds.at(2));

        QCOMPARE(m_manager->landmark(lmIds1.at(0)).id().isValid(), true);
        QCOMPARE(m_manager->landmark(lmIds1.at(1)).id().isValid(), false);
        QCOMPARE(m_manager->landmark(lmIds1.at(2)).id().isValid(), true);
        QCOMPARE(m_manager->landmark(lmIds2.at(0)).id().isValid(), false);
        QCOMPARE(m_manager->landmark(lmIds2.at(1)).id().isValid(), true);
        QCOMPARE(m_manager->landmark(lmIds2.at(2)).id().isValid(), false);

        QVERIFY(m_manager->removeLandmarks(lmIds1, 0));

        QCOMPARE(spyRemove.count(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().size(), 2);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm1.id());
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(1), lm2.id());
        spyRemove.clear();

        for (int i = 0; i < lmIds1.size(); ++i)
            QCOMPARE(m_manager->landmark(lmIds1.at(i)).id().isValid(), false);

        QCOMPARE(m_manager->landmark(lmIds2.at(0)).id().isValid(), false);
        QCOMPARE(m_manager->landmark(lmIds2.at(1)).id().isValid(), true);
        QCOMPARE(m_manager->landmark(lmIds2.at(2)).id().isValid(), false);

        QVERIFY(m_manager->removeLandmarks(lmIds2, 0));

        QCOMPARE(spyRemove.count(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().size(), 1);
        QCOMPARE(spyRemove.at(0).at(0).value<QList<QLandmarkId> >().at(0), lm3.id());
        spyRemove.clear();

        for (int i = 0; i < lmIds1.size(); ++i)
            QCOMPARE(m_manager->landmark(lmIds1.at(i)).id().isValid(), false);
        for (int i = 0; i < lmIds2.size(); ++i)
            QCOMPARE(m_manager->landmark(lmIds2.at(i)).id().isValid(), false);
    }

    void listCategoryIds() {
        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QList<QLandmarkCategoryId> ids = m_manager->categoryIds();

        QCOMPARE(ids.size(), 3);
        QCOMPARE(ids.at(0), cat1.id());
        QCOMPARE(ids.at(1), cat2.id());
        QCOMPARE(ids.at(2), cat3.id());
    }

    void filterLandmarksNull() {
        QList<QLandmark> lms = m_manager->landmarks(0, QList<const QLandmarkSortOrder*>());
        QCOMPARE(lms.size(), 0);
    }

    void filterLandmarksLimitMatches() {
        for (int i = 0; i < 50; ++i) {
            QLandmark lm;
            lm.setName(QString("LM%1").arg(i));
            QVERIFY(m_manager->saveLandmark(&lm));
        }

        QLandmarkFilter* filter = new QLandmarkFilter();
        filter->setMaximumMatches(-1);

        QList<QLandmarkId> ids = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(ids.size(), 50);

        filter->setMaximumMatches(100);

        ids = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(ids.size(), 50);

        filter->setMaximumMatches(25);

        ids = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());

        delete filter;
    }

    void filterLandmarksDefault() {
        QLandmark lm1;
        lm1.setName("LM1");
        QVERIFY(m_manager->saveLandmark(&lm1));

        QLandmark lm2;
        lm2.setName("LM2");
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmark lm3;
        lm3.setName("LM3");
        QVERIFY(m_manager->saveLandmark(&lm3));

        QLandmarkFilter* filter = new QLandmarkFilter();

        QList<QLandmarkId> ids = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(ids.size(), 3);
        QCOMPARE(ids.at(0), lm1.id());
        QCOMPARE(ids.at(1), lm2.id());
        QCOMPARE(ids.at(2), lm3.id());

        delete filter;
    }

    void filterLandmarksName() {
        QLandmark lm1;
        lm1.setName("test");
        QVERIFY(m_manager->saveLandmark(&lm1));

        QLandmark lm2;
        lm2.setName("junk1");
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmark lm3;
        lm3.setName("TEST");
        QVERIFY(m_manager->saveLandmark(&lm3));

        QLandmark lm4;
        lm4.setName("junk2");
        QVERIFY(m_manager->saveLandmark(&lm4));

        QLandmark lm5;
        lm5.setName("tEsT");
        QVERIFY(m_manager->saveLandmark(&lm5));

        QLandmark lm6;
        lm6.setName("junk3");
        QVERIFY(m_manager->saveLandmark(&lm6));

        QLandmarkNameFilter* filter = new QLandmarkNameFilter("TEST");
        filter->setCaseSensitivity(Qt::CaseInsensitive);

        QList<QLandmarkId> ids1 = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(ids1.size(), 3);
        QCOMPARE(ids1.at(0), lm1.id());
        QCOMPARE(ids1.at(1), lm3.id());
        QCOMPARE(ids1.at(2), lm5.id());

        filter->setCaseSensitivity(Qt::CaseSensitive);
        QList<QLandmarkId> ids2 = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(ids2.size(), 1);
        QCOMPARE(ids2.at(0), lm3.id());

        filter->setName("No match");
        QList<QLandmarkId> ids3 = m_manager->landmarkIds(filter, QList<const QLandmarkSortOrder*>());
        QCOMPARE(ids3.size(), 0);

        delete filter;
    }

    void filterLandmarksProximity() {
        QList<QGeoCoordinate> greenwhichFilterCoords;
        QList<QGeoCoordinate> datelineFilterCoords;
        QList<QGeoCoordinate> northPoleFilterCoords;
        QList<QGeoCoordinate> southPoleFilterCoords;
        QList<QGeoCoordinate> northFilterCoords;
        QList<QGeoCoordinate> eastFilterCoords;
        QList<QGeoCoordinate> northeastFilterCoords;

        QList<QGeoCoordinate> greenwhichLmCoords;
        QList<QGeoCoordinate> datelineLmCoords;
        QList<QGeoCoordinate> northPoleLmCoords;
        QList<QGeoCoordinate> southPoleLmCoords;
        QList<QGeoCoordinate> northLmCoords;
        QList<QGeoCoordinate> eastLmCoords;
        QList<QGeoCoordinate> northeastLmCoords;

        greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
        greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
        greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
        greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

        datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
        datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
        datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
        datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

        northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
        northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
        northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
        northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

        southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
        southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
        southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
        southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

        eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
        eastFilterCoords << QGeoCoordinate(0.1, 10.0);
        northFilterCoords << QGeoCoordinate(10.0, -0.1);
        northFilterCoords << QGeoCoordinate(10.0, 0.1);
        northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

        greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
        greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
        greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
        greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

        datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
        datelineLmCoords << QGeoCoordinate(1.0, -179.0);
        datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
        datelineLmCoords << QGeoCoordinate(1.0, 179.0);

        northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
        northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
        northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
        northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

        southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
        southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
        southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
        southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

        eastLmCoords << QGeoCoordinate(-1.0, 11.0);
        eastLmCoords << QGeoCoordinate(1.0, 11.0);
        northLmCoords << QGeoCoordinate(11.0, -1.0);
        northLmCoords << QGeoCoordinate(11.0, 1.0);
        northeastLmCoords << QGeoCoordinate(11.0, 11.0);

        QList<QList<QGeoCoordinate> > coords;
        coords << greenwhichLmCoords;
        coords << datelineLmCoords;
        coords << northPoleLmCoords;
        coords << southPoleLmCoords;
        coords << eastLmCoords;
        coords << northLmCoords;
        coords << northeastLmCoords;

        for (int i = 0; i < coords.size(); ++i) {
            QList<QGeoCoordinate> c = coords.at(i);
            for (int j = 0; j < c.size(); ++j) {
                QLandmark lm;
                QGeoLocation location;
                location.setCoordinate(c.at(j));
                lm.setLocation(location);
                QVERIFY(m_manager->saveLandmark(&lm));
            }
        }

        QList<QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> > > testSets;
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(greenwhichFilterCoords, greenwhichLmCoords);
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(datelineFilterCoords, datelineLmCoords);
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northPoleFilterCoords, northPoleLmCoords);
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(southPoleFilterCoords, southPoleLmCoords);
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northFilterCoords, northLmCoords);
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(eastFilterCoords, eastLmCoords);
        testSets << QPair<QList<QGeoCoordinate>, QList<QGeoCoordinate> >(northeastFilterCoords, northeastLmCoords);

        double dist = QGeoCoordinate(0.0, 0.0).distanceTo(QGeoCoordinate(5.0, 5.0));

        for (int i = 0; i < testSets.size(); ++i) {
            QList<QGeoCoordinate> filterCoords = testSets.at(i).first;
            QList<QGeoCoordinate> lmCoords = testSets.at(i).second;

            for (int j = 0; j < filterCoords.size(); ++j) {
                QLandmarkProximityFilter* filter = new QLandmarkProximityFilter(filterCoords.at(j), dist);

                QList<QLandmark> lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

                if (lms.size() != lmCoords.size()) {
                    for (int k = 0; k < lms.size(); ++k)
                        qWarning() << "lms" << lms.at(k).coordinate().toString();
                    for (int k = 0; k < lmCoords.size(); ++k)
                        qWarning() << "lmCoords" << lmCoords.at(k).toString();
                }

                QCOMPARE(lms.size(), lmCoords.size());

                for (int k = 0; k < lms.size(); ++k) {
                    QVERIFY(lmCoords.contains(lms.at(k).location().coordinate()));
                }

                delete filter;
            }
        }
    }

    void filterLandmarksNearest() {
        QList<QGeoCoordinate> greenwhichFilterCoords;
        QList<QGeoCoordinate> datelineFilterCoords;
        QList<QGeoCoordinate> northPoleFilterCoords;
        QList<QGeoCoordinate> southPoleFilterCoords;
        QList<QGeoCoordinate> northFilterCoords;
        QList<QGeoCoordinate> eastFilterCoords;
        QList<QGeoCoordinate> northeastFilterCoords;

        QList<QGeoCoordinate> greenwhichLmCoords;
        QList<QGeoCoordinate> datelineLmCoords;
        QList<QGeoCoordinate> northPoleLmCoords;
        QList<QGeoCoordinate> southPoleLmCoords;
        QList<QGeoCoordinate> northLmCoords;
        QList<QGeoCoordinate> eastLmCoords;
        QList<QGeoCoordinate> northeastLmCoords;

        greenwhichFilterCoords << QGeoCoordinate(-0.1, -0.1);
        greenwhichFilterCoords << QGeoCoordinate(0.1, -0.1);
        greenwhichFilterCoords << QGeoCoordinate(-0.1, 0.1);
        greenwhichFilterCoords << QGeoCoordinate(0.1, 0.1);

        datelineFilterCoords << QGeoCoordinate(-0.1, -179.9);
        datelineFilterCoords << QGeoCoordinate(0.1, -179.9);
        datelineFilterCoords << QGeoCoordinate(-0.1, 179.9);
        datelineFilterCoords << QGeoCoordinate(0.1, 179.9);

        northPoleFilterCoords << QGeoCoordinate(89.9, -179.9);
        northPoleFilterCoords << QGeoCoordinate(89.9, -0.1);
        northPoleFilterCoords << QGeoCoordinate(89.9, 0.1);
        northPoleFilterCoords << QGeoCoordinate(89.9, 179.9);

        southPoleFilterCoords << QGeoCoordinate(-89.9, -179.9);
        southPoleFilterCoords << QGeoCoordinate(-89.9, -0.1);
        southPoleFilterCoords << QGeoCoordinate(-89.9, 0.1);
        southPoleFilterCoords << QGeoCoordinate(-89.9, 179.9);

        eastFilterCoords << QGeoCoordinate(-0.1, 10.0);
        eastFilterCoords << QGeoCoordinate(0.1, 10.0);
        northFilterCoords << QGeoCoordinate(10.0, -0.1);
        northFilterCoords << QGeoCoordinate(10.0, 0.1);
        northeastFilterCoords << QGeoCoordinate(10.0, 10.0);

        greenwhichLmCoords << QGeoCoordinate(-1.0, -1.0);
        greenwhichLmCoords << QGeoCoordinate(1.0, -1.0);
        greenwhichLmCoords << QGeoCoordinate(-1.0, 1.0);
        greenwhichLmCoords << QGeoCoordinate(1.0, 1.0);

        datelineLmCoords << QGeoCoordinate(-1.0, -179.0);
        datelineLmCoords << QGeoCoordinate(1.0, -179.0);
        datelineLmCoords << QGeoCoordinate(-1.0, 179.0);
        datelineLmCoords << QGeoCoordinate(1.0, 179.0);

        northPoleLmCoords << QGeoCoordinate(89.0, -179.0);
        northPoleLmCoords << QGeoCoordinate(89.0, -1.0);
        northPoleLmCoords << QGeoCoordinate(89.0, 1.0);
        northPoleLmCoords << QGeoCoordinate(89.0, 179.0);

        southPoleLmCoords << QGeoCoordinate(-89.0, -179.0);
        southPoleLmCoords << QGeoCoordinate(-89.0, -1.0);
        southPoleLmCoords << QGeoCoordinate(-89.0, 1.0);
        southPoleLmCoords << QGeoCoordinate(-89.0, 179.0);

        eastLmCoords << QGeoCoordinate(-1.0, 11.0);
        eastLmCoords << QGeoCoordinate(1.0, 11.0);
        northLmCoords << QGeoCoordinate(11.0, -1.0);
        northLmCoords << QGeoCoordinate(11.0, 1.0);
        northeastLmCoords << QGeoCoordinate(11.0, 11.0);

        QList<QList<QGeoCoordinate> > coords;
        coords << greenwhichLmCoords;
        coords << datelineLmCoords;
        coords << northPoleLmCoords;
        coords << southPoleLmCoords;
        coords << eastLmCoords;
        coords << northLmCoords;
        coords << northeastLmCoords;

        for (int i = 0; i < coords.size(); ++i) {
            QList<QGeoCoordinate> c = coords.at(i);
            for (int j = 0; j < c.size(); ++j) {
                QLandmark lm;
                QGeoLocation location;
                location.setCoordinate(c.at(j));
                lm.setLocation(location);
                QVERIFY(m_manager->saveLandmark(&lm));
            }
        }

        QList<QPair<QGeoCoordinate, QGeoCoordinate> > testSets;

        for (int i = 0; i < greenwhichFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(greenwhichFilterCoords.at(i), greenwhichLmCoords.at(i));
        }

        for (int i = 0; i < datelineFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(datelineFilterCoords.at(i), datelineLmCoords.at(i));
        }

        for (int i = 0; i < northPoleFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northPoleFilterCoords.at(i), northPoleLmCoords.at(i));
        }

        for (int i = 0; i < southPoleFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(southPoleFilterCoords.at(i), southPoleLmCoords.at(i));
        }

        for (int i = 0; i < northFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northFilterCoords.at(i), northLmCoords.at(i));
        }

        for (int i = 0; i < eastFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(eastFilterCoords.at(i), eastLmCoords.at(i));
        }

        for (int i = 0; i < northeastFilterCoords.size(); ++i) {
            testSets << QPair<QGeoCoordinate, QGeoCoordinate>(northeastFilterCoords.at(i), northeastLmCoords.at(i));
        }

        for (int i = 0; i < testSets.size(); ++i) {
            QGeoCoordinate filterCoord = testSets.at(i).first;
            QGeoCoordinate lmCoord = testSets.at(i).second;

            QLandmarkNearestFilter* filter = new QLandmarkNearestFilter(filterCoord);

            QList<QLandmark> lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

            QCOMPARE(lms.size(), 1);

            QCOMPARE(lms.at(0).coordinate(), lmCoord);

            delete filter;
        }

        QLandmarkNearestFilter* filter = new QLandmarkNearestFilter(QGeoCoordinate(-10.0, -10.0), -1.0);

        QList<QLandmark> lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());
        QCOMPARE(lms.size(), 1);
        QCOMPARE(lms.at(0).coordinate(), QGeoCoordinate(-1.0, -1.0));

        delete filter;

        filter = new QLandmarkNearestFilter(QGeoCoordinate(-10.0, -10.0), 100.0);

        lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());
        QCOMPARE(lms.size(), 0);

        delete filter;
    }

    void filterLandmarksCategory() {
        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QLandmark lm1;
        lm1.setName("LM1");
        lm1.addCategory(cat1.id());
        QVERIFY(m_manager->saveLandmark(&lm1));

        QLandmark lm2;
        lm2.setName("LM2");
        lm2.addCategory(cat1.id());
        lm2.addCategory(cat2.id());
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmark lm3;
        lm3.setName("LM3");
        lm3.addCategory(cat1.id());
        lm3.addCategory(cat2.id());
        lm3.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm3));

        QLandmark lm4;
        lm4.setName("LM4");
        lm4.addCategory(cat2.id());
        lm4.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm4));

        QLandmark lm5;
        lm5.setName("LM5");
        lm5.addCategory(cat3.id());
        QVERIFY(m_manager->saveLandmark(&lm5));

        QLandmarkCategoryFilter *filter = new QLandmarkCategoryFilter(cat2.id());

        QList<QLandmark> lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 3);

        QSet<QString> names;
        for (int i = 0; i < lms.size(); ++i) {
            names.insert(lms.at(i).name());
        }

        QSet<QString> expectedNames;
        expectedNames.insert("LM2");
        expectedNames.insert("LM3");
        expectedNames.insert("LM4");

        QCOMPARE(names, expectedNames);

        delete filter;
    }

    void filterLandmarksBox() {
        QList<QGeoCoordinate> outBox;

        QList<QGeoCoordinate> inBox1;

        inBox1 << QGeoCoordinate(-5.0, -5.0);
        inBox1 << QGeoCoordinate(-5.0, 0.0);
        inBox1 << QGeoCoordinate(-5.0, 5.0);
        inBox1 << QGeoCoordinate(0.0, -5.0);
        inBox1 << QGeoCoordinate(0.0, 0.0);
        inBox1 << QGeoCoordinate(0.0, 5.0);
        inBox1 << QGeoCoordinate(5.0, -5.0);
        inBox1 << QGeoCoordinate(5.0, 0.0);
        inBox1 << QGeoCoordinate(5.0, 5.0);

        outBox << QGeoCoordinate(-5.0, -6.0);
        outBox << QGeoCoordinate(5.0, -6.0);
        outBox << QGeoCoordinate(-6.0, -5.0);
        outBox << QGeoCoordinate(6.0, -5.0);
        outBox << QGeoCoordinate(-6.0, 5.0);
        outBox << QGeoCoordinate(6.0, 5.0);
        outBox << QGeoCoordinate(-5.0, 6.0);
        outBox << QGeoCoordinate(5.0, 6.0);

        QList<QGeoCoordinate> inBox2;

        inBox2 << QGeoCoordinate(-5.0, 10.0);
        inBox2 << QGeoCoordinate(-5.0, 15.0);
        inBox2 << QGeoCoordinate(-5.0, 20.0);
        inBox2 << QGeoCoordinate(0.0, 10.0);
        inBox2 << QGeoCoordinate(0.0, 15.0);
        inBox2 << QGeoCoordinate(0.0, 20.0);
        inBox2 << QGeoCoordinate(5.0, 10.0);
        inBox2 << QGeoCoordinate(5.0, 15.0);
        inBox2 << QGeoCoordinate(5.0, 20.0);

        outBox << QGeoCoordinate(-5.0, 9.0);
        outBox << QGeoCoordinate(5.0,  9.0);
        outBox << QGeoCoordinate(-6.0, 10.0);
        outBox << QGeoCoordinate(6.0,  10.0);
        outBox << QGeoCoordinate(-6.0, 20.0);
        outBox << QGeoCoordinate(6.0, 20.0);
        outBox << QGeoCoordinate(-5.0, 21.0);
        outBox << QGeoCoordinate(5.0, 21.0);

        QList<QGeoCoordinate> inBox3;

        inBox3 << QGeoCoordinate(10.0, -5.0);
        inBox3 << QGeoCoordinate(10.0, 0.0);
        inBox3 << QGeoCoordinate(10.0, 5.0);
        inBox3 << QGeoCoordinate(15.0, -5.0);
        inBox3 << QGeoCoordinate(15.0, 0.0);
        inBox3 << QGeoCoordinate(15.0, 5.0);
        inBox3 << QGeoCoordinate(20.0, -5.0);
        inBox3 << QGeoCoordinate(20.0, 0.0);
        inBox3 << QGeoCoordinate(20.0, 5.0);

        outBox << QGeoCoordinate(10.0, -6.0);
        outBox << QGeoCoordinate(20.0, -6.0);
        outBox << QGeoCoordinate(9.0, -5.0);
        outBox << QGeoCoordinate(21.0, -5.0);
        outBox << QGeoCoordinate(9.0, 5.0);
        outBox << QGeoCoordinate(21.0, 5.0);
        outBox << QGeoCoordinate(10.0, 6.0);
        outBox << QGeoCoordinate(20.0, 6.0);

        QList<QGeoCoordinate> inBox4;

        inBox4 << QGeoCoordinate(10.0, 10.0);
        inBox4 << QGeoCoordinate(10.0, 15.0);
        inBox4 << QGeoCoordinate(10.0, 20.0);
        inBox4 << QGeoCoordinate(15.0, 10.0);
        inBox4 << QGeoCoordinate(15.0, 15.0);
        inBox4 << QGeoCoordinate(15.0, 20.0);
        inBox4 << QGeoCoordinate(20.0, 10.0);
        inBox4 << QGeoCoordinate(20.0, 15.0);
        inBox4 << QGeoCoordinate(20.0, 20.0);

        outBox << QGeoCoordinate(10.0, 9.0);
        outBox << QGeoCoordinate(20.0, 9.0);
        outBox << QGeoCoordinate(9.0, 10.0);
        outBox << QGeoCoordinate(21.0, 10.0);
        outBox << QGeoCoordinate(9.0, 20.0);
        outBox << QGeoCoordinate(21.0, 20.0);
        outBox << QGeoCoordinate(10.0, 21.0);
        outBox << QGeoCoordinate(20.0, 21.0);

        QList<QGeoCoordinate> inBox5;

        inBox5 << QGeoCoordinate(-5.0, 175.0);
        inBox5 << QGeoCoordinate(-5.0, 180.0);
        inBox5 << QGeoCoordinate(-5.0, -175.0);
        inBox5 << QGeoCoordinate(0.0, 175.0);
        inBox5 << QGeoCoordinate(0.0, 180.0);
        inBox5 << QGeoCoordinate(0.0, -175.0);
        inBox5 << QGeoCoordinate(5.0, 175.0);
        inBox5 << QGeoCoordinate(5.0, 180.0);
        inBox5 << QGeoCoordinate(5.0, -175.0);

        outBox << QGeoCoordinate(-6.0, 175.0);
        outBox << QGeoCoordinate(-6.0, -175.0);
        outBox << QGeoCoordinate(-5.0, 174.0);
        outBox << QGeoCoordinate(-5.0, -174.0);
        outBox << QGeoCoordinate(5.0, 174.0);
        outBox << QGeoCoordinate(5.0, -174.0);
        outBox << QGeoCoordinate(6.0, 175.0);
        outBox << QGeoCoordinate(6.0, -175.0);

        QList<QGeoCoordinate> coords = outBox;
        coords.append(inBox1);
        coords.append(inBox2);
        coords.append(inBox3);
        coords.append(inBox4);
        coords.append(inBox5);

        for (int i = 0; i < coords.size(); ++i) {
            QLandmark lm;
            QGeoLocation location;
            location.setCoordinate(coords.at(i));
            lm.setLocation(location);
            QVERIFY(m_manager->saveLandmark(&lm));
        }

        QLandmarkBoxFilter *filter1 = new QLandmarkBoxFilter(QGeoCoordinate(5.0, -5.0), QGeoCoordinate(-5.0, 5.0));
        QList<QLandmark> lms1 = m_manager->landmarks(filter1, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms1.size(), inBox1.size());

        QSet<QString> testSet1;
        for (int i = 0; i < lms1.size(); ++i)
            testSet1.insert(lms1.at(i).coordinate().toString());

        QSet<QString> inBoxSet1;
        for (int i = 0; i < inBox1.size(); ++i)
            inBoxSet1.insert(inBox1.at(i).toString());

        QCOMPARE(testSet1, inBoxSet1);

        delete filter1;

        QLandmarkBoxFilter *filter2 = new QLandmarkBoxFilter(QGeoCoordinate(5.0, 10.0), QGeoCoordinate(-5.0, 20.0));
        QList<QLandmark> lms2 = m_manager->landmarks(filter2, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms2.size(), inBox2.size());

        QSet<QString> testSet2;
        for (int i = 0; i < lms2.size(); ++i)
            testSet2.insert(lms2.at(i).coordinate().toString());

        QSet<QString> inBoxSet2;
        for (int i = 0; i < inBox2.size(); ++i)
            inBoxSet2.insert(inBox2.at(i).toString());

        QCOMPARE(testSet2, inBoxSet2);

        delete filter2;

        QLandmarkBoxFilter *filter3 = new QLandmarkBoxFilter(QGeoCoordinate(20.0, -5.0), QGeoCoordinate(10.0, 5.0));
        QList<QLandmark> lms3 = m_manager->landmarks(filter3, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms3.size(), inBox3.size());

        QSet<QString> testSet3;
        for (int i = 0; i < lms3.size(); ++i)
            testSet3.insert(lms3.at(i).coordinate().toString());

        QSet<QString> inBoxSet3;
        for (int i = 0; i < inBox3.size(); ++i)
            inBoxSet3.insert(inBox3.at(i).toString());

        QCOMPARE(testSet3, inBoxSet3);

        delete filter3;

        QLandmarkBoxFilter *filter4 = new QLandmarkBoxFilter(QGeoCoordinate(20.0, 10.0), QGeoCoordinate(10.0, 20.0));
        QList<QLandmark> lms4 = m_manager->landmarks(filter4, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms4.size(), inBox4.size());

        QSet<QString> testSet4;
        for (int i = 0; i < lms4.size(); ++i)
            testSet4.insert(lms4.at(i).coordinate().toString());

        QSet<QString> inBoxSet4;
        for (int i = 0; i < inBox4.size(); ++i)
            inBoxSet4.insert(inBox4.at(i).toString());

        QCOMPARE(testSet4, inBoxSet4);

        delete filter4;

        QLandmarkBoxFilter *filter5 = new QLandmarkBoxFilter(QGeoCoordinate(5.0, 175.0), QGeoCoordinate(-5.0, -175.0));
        QList<QLandmark> lms5 = m_manager->landmarks(filter5, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms5.size(), inBox5.size());

        QSet<QString> testSet5;
        for (int i = 0; i < lms5.size(); ++i)
            testSet5.insert(lms5.at(i).coordinate().toString());

        QSet<QString> inBoxSet5;
        for (int i = 0; i < inBox5.size(); ++i)
            inBoxSet5.insert(inBox5.at(i).toString());

        QCOMPARE(testSet5, inBoxSet5);

        delete filter5;
    }

    void filterLandmarksIntersection() {
        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QList<QLandmarkCategoryId> ids;
        ids << cat1.id();
        ids << cat2.id();
        ids << cat3.id();

        QList<QString> names;
        names << "LM1";
        names << "LM2";
        names << "LM3";

        QList<QGeoCoordinate> coords;
        coords << QGeoCoordinate(0.0, 0.0);
        coords << QGeoCoordinate(25.0, 25.0);
        coords << QGeoCoordinate(50.0, 50.0);

        QList<QLandmark> lmPool;

        for (int i = 0; i < ids.size(); ++i) {
            for (int j = 0; j < names.size(); ++j) {
                for (int k = 0; k < coords.size(); ++k) {
                    QLandmark lm;
                    lm.addCategory(ids.at(i));
                    lm.setName(names.at(j));
                    lm.setCoordinate(coords.at(k));
                    QVERIFY(m_manager->saveLandmark(&lm));
                    lmPool << lm;
                }
            }
        }

        QLandmarkCategoryFilter *f1 = new QLandmarkCategoryFilter(cat2.id());
        QLandmarkNameFilter *f2 = new QLandmarkNameFilter("LM2");
        QLandmarkProximityFilter *f3 = new QLandmarkProximityFilter(QGeoCoordinate(25.0, 25.0), 5.0);

        QLandmarkIntersectionFilter *filter = new QLandmarkIntersectionFilter();
        *filter << f1 << f2 << f3;

        QList<QLandmark> lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 1);

        QSet<QString> idSet;
        for (int i = 0; i < lms.size(); ++i)
            idSet.insert(lms.at(i).id().id());

        for (int i = 0; i < lmPool.size(); ++i) {
            QLandmark lm = lmPool.at(i);
            if ((lm.categories().at(0) == cat2.id())
                    && (lm.name() == "LM2")
                    && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
                QCOMPARE(idSet.contains(lm.id().id()), true);
            } else {
                QCOMPARE(idSet.contains(lm.id().id()), false);
            }
        }

        filter->remove(f2);

        lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 3);

        idSet.clear();
        for (int i = 0; i < lms.size(); ++i)
            idSet.insert(lms.at(i).id().id());

        for (int i = 0; i < lmPool.size(); ++i) {
            QLandmark lm = lmPool.at(i);
            if ((lm.categories().at(0) == cat2.id())
                    && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
                QCOMPARE(idSet.contains(lm.id().id()), true);
            } else {
                QCOMPARE(idSet.contains(lm.id().id()), false);
            }
        }

        filter->prepend(f2);

        lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 1);

        idSet.clear();
        for (int i = 0; i < lms.size(); ++i)
            idSet.insert(lms.at(i).id().id());

        for (int i = 0; i < lmPool.size(); ++i) {
            QLandmark lm = lmPool.at(i);
            if ((lm.categories().at(0) == cat2.id())
                    && (lm.name() == "LM2")
                    && (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
                QCOMPARE(idSet.contains(lm.id().id()), true);
            } else {
                QCOMPARE(idSet.contains(lm.id().id()), false);
            }
        }

        delete filter;
        delete f3;
        delete f2;
        delete f1;
    }

    void filterLandmarksUnion() {
        QLandmarkCategory cat1;
        cat1.setName("CAT1");
        QVERIFY(m_manager->saveCategory(&cat1));

        QLandmarkCategory cat2;
        cat2.setName("CAT2");
        QVERIFY(m_manager->saveCategory(&cat2));

        QLandmarkCategory cat3;
        cat3.setName("CAT3");
        QVERIFY(m_manager->saveCategory(&cat3));

        QList<QLandmarkCategoryId> ids;
        ids << cat1.id();
        ids << cat2.id();
        ids << cat3.id();

        QList<QString> names;
        names << "LM1";
        names << "LM2";
        names << "LM3";

        QList<QGeoCoordinate> coords;
        coords << QGeoCoordinate(0.0, 0.0);
        coords << QGeoCoordinate(25.0, 25.0);
        coords << QGeoCoordinate(50.0, 50.0);

        QList<QLandmark> lmPool;

        for (int i = 0; i < ids.size(); ++i) {
            for (int j = 0; j < names.size(); ++j) {
                for (int k = 0; k < coords.size(); ++k) {
                    QLandmark lm;
                    lm.addCategory(ids.at(i));
                    lm.setName(names.at(j));
                    lm.setCoordinate(coords.at(k));
                    QVERIFY(m_manager->saveLandmark(&lm));
                    lmPool << lm;
                }
            }
        }

        QLandmarkCategoryFilter *f1 = new QLandmarkCategoryFilter(cat2.id());
        QLandmarkNameFilter *f2 = new QLandmarkNameFilter("LM2");
        QLandmarkProximityFilter *f3 = new QLandmarkProximityFilter(QGeoCoordinate(25.0, 25.0), 5.0);

        QLandmarkUnionFilter *filter = new QLandmarkUnionFilter();
        *filter << f1 << f2 << f3;

        QList<QLandmark> lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 19);

        QSet<QString> idSet;
        for (int i = 0; i < lms.size(); ++i)
            idSet.insert(lms.at(i).id().id());

        for (int i = 0; i < lmPool.size(); ++i) {
            QLandmark lm = lmPool.at(i);
            if ((lm.categories().at(0) == cat2.id())
                    || (lm.name() == "LM2")
                    || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
                QCOMPARE(idSet.contains(lm.id().id()), true);
            } else {
                QCOMPARE(idSet.contains(lm.id().id()), false);
            }
        }

        filter->remove(f2);

        lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 15);

        idSet.clear();
        for (int i = 0; i < lms.size(); ++i)
            idSet.insert(lms.at(i).id().id());

        for (int i = 0; i < lmPool.size(); ++i) {
            QLandmark lm = lmPool.at(i);
            if ((lm.categories().at(0) == cat2.id())
                    || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
                QCOMPARE(idSet.contains(lm.id().id()), true);
            } else {
                QCOMPARE(idSet.contains(lm.id().id()), false);
            }
        }

        filter->prepend(f2);

        lms = m_manager->landmarks(filter, QList<const QLandmarkSortOrder*>());

        QCOMPARE(lms.size(), 19);

        idSet.clear();
        for (int i = 0; i < lms.size(); ++i)
            idSet.insert(lms.at(i).id().id());

        for (int i = 0; i < lmPool.size(); ++i) {
            QLandmark lm = lmPool.at(i);
            if ((lm.categories().at(0) == cat2.id())
                    || (lm.name() == "LM2")
                    || (lm.coordinate() == QGeoCoordinate(25.0, 25.0))) {
                QCOMPARE(idSet.contains(lm.id().id()), true);
            } else {
                QCOMPARE(idSet.contains(lm.id().id()), false);
            }
        }
        delete filter;
        delete f3;
        delete f2;
        delete f1;
    }

    void sortLandmarksNull() {
        QLandmark lm1;
        lm1.setName("b");
        QVERIFY(m_manager->saveLandmark(&lm1));

        QLandmark lm2;
        lm2.setName("a");
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmark lm3;
        lm3.setName("c");
        QVERIFY(m_manager->saveLandmark(&lm3));

        QList<QLandmark> expected;
        expected << lm1;
        expected << lm2;
        expected << lm3;

        QLandmarkFilter filter;
        QLandmarkSortOrder sortOrder;
        QList<const QLandmarkSortOrder*> sortOrders;

        QList<QLandmark> lms = m_manager->landmarks(&filter, 0);
        QCOMPARE(lms, expected);

        lms = m_manager->landmarks(&filter, &sortOrder);
        QCOMPARE(lms, expected);

        lms = m_manager->landmarks(&filter, sortOrders);
        QCOMPARE(lms, expected);

        sortOrders << &sortOrder;
        lms = m_manager->landmarks(&filter, sortOrders);
        QCOMPARE(lms, expected);

        sortOrders << 0;
        lms = m_manager->landmarks(&filter, sortOrders);
        QCOMPARE(lms, expected);

        sortOrders << &sortOrder;
        lms = m_manager->landmarks(&filter, sortOrders);
        QCOMPARE(lms, expected);
    }

    void sortLandmarksName() {
        QLandmark lm1;
        lm1.setName("b");
        QVERIFY(m_manager->saveLandmark(&lm1));

        QLandmark lm2;
        lm2.setName("a");
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmark lm3;
        lm3.setName("c");
        QVERIFY(m_manager->saveLandmark(&lm3));

        QList<QLandmark> expectedAscending;
        expectedAscending << lm2;
        expectedAscending << lm1;
        expectedAscending << lm3;

        QList<QLandmark> expectedDescending;
        expectedDescending << lm3;
        expectedDescending << lm1;
        expectedDescending << lm2;

        QLandmarkFilter filter;
        QLandmarkSortOrder *sortAscending = new QLandmarkNameSort(Qt::AscendingOrder);

        QList<QLandmark> lms = m_manager->landmarks(&filter, sortAscending);
        QCOMPARE(lms, expectedAscending);

        delete sortAscending;

        QLandmarkNameSort *sortDescending = new QLandmarkNameSort(Qt::DescendingOrder);

        lms = m_manager->landmarks(&filter, sortDescending);
        QCOMPARE(lms, expectedDescending);

        delete sortDescending;
    }

    void sortLandmarksDistance() {
        QLandmark lm1;
        lm1.setCoordinate(QGeoCoordinate(2.0, 2.0));
        QVERIFY(m_manager->saveLandmark(&lm1));

        QLandmark lm2;
        lm2.setCoordinate(QGeoCoordinate(1.0, 1.0));
        QVERIFY(m_manager->saveLandmark(&lm2));

        QLandmark lm3;
        QVERIFY(m_manager->saveLandmark(&lm3));

        QLandmark lm4;
        lm4.setCoordinate(QGeoCoordinate(3.0, 3.0));
        QVERIFY(m_manager->saveLandmark(&lm4));

        QList<QLandmark> expectedAscending;
        expectedAscending << lm2;
        expectedAscending << lm1;
        expectedAscending << lm4;
        expectedAscending << lm3;

        QList<QLandmark> expectedDescending;
        expectedDescending << lm3;
        expectedDescending << lm4;
        expectedDescending << lm1;
        expectedDescending << lm2;

        QLandmarkFilter filter;
        QLandmarkSortOrder *sortAscending = new QLandmarkDistanceSort(QGeoCoordinate(0.0, 0.0), Qt::AscendingOrder);

        QList<QLandmark> lms = m_manager->landmarks(&filter, sortAscending);

        QCOMPARE(lms, expectedAscending);

        delete sortAscending;

        QLandmarkDistanceSort *sortDescending = new QLandmarkDistanceSort(QGeoCoordinate(0.0, 0.0), Qt::DescendingOrder);

        lms = m_manager->landmarks(&filter, sortDescending);

        QCOMPARE(lms, expectedDescending);

        delete sortDescending;
    }

    /*
    void sortLandmarksNameDistance()
    {
    }

    void filterDistanceSortName()
    {
    }

    void filterNameSortDistance()
    {
    }

    void filterLandmarksCustom()
    {
    }

    void sortLandmarksCustom()
    {
    }
    */
};

QTEST_MAIN(tst_QLandmarkManagerEngineSqlite)
#include "tst_qlandmarkmanagerengine_sqlite.moc"

