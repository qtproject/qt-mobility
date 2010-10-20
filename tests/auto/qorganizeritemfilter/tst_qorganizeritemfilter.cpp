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
#include <QMetaType>

#include "qtorganizer.h"
#include "qorganizeritemengineid.h"

//TESTED_COMPONENT=src/organizer
//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QOrganizerItem)
Q_DECLARE_METATYPE(QOrganizerItemFilter)

class tst_QOrganizerItemFilter : public QObject
{
Q_OBJECT

public:
    tst_QOrganizerItemFilter();
    virtual ~tst_QOrganizerItemFilter();

public slots:
    void init();
    void cleanup();
private slots:
    void classHierarchy();
    void intersectionFilter();
    void unionFilter();
    void detailFilter();
    void detailRangeFilter();
    void changeLogFilter();
    void boringFilters();
    void idListFilter();
    void collectionFilter();
    void canonicalizedFilter();
    void canonicalizedFilter_data();
    void testFilter();
    void testFilter_data();

    void datastream();
    void datastream_data();
    void traits();

    void sortObject(); // should perhaps be in a different test :)
    void sortTraits();
};

class BasicItemLocalId : public QOrganizerItemEngineId
{
public:
    BasicItemLocalId(uint id) : m_id(id) {}
    bool isEqualTo(const QOrganizerItemEngineId* other) const {
        return m_id == static_cast<const BasicItemLocalId*>(other)->m_id;
    }
    bool isLessThan(const QOrganizerItemEngineId* other) const {
        return m_id < static_cast<const BasicItemLocalId*>(other)->m_id;
    }
    QOrganizerItemEngineId* clone() const {
        BasicItemLocalId* cloned = new BasicItemLocalId(m_id);
        return cloned;
    }
    QString managerUri() const {
        static const QString uri(QLatin1String("qtorganizer:basicItem:"));
        return uri;
    }
    QDebug& debugStreamOut(QDebug& dbg) const {
        return dbg << m_id;
    }
    QString toString() const {
        return QString::number(m_id);
    }
    uint hash() const {
        return m_id;
    }

private:
    uint m_id;
};

class BasicCollectionLocalId : public QOrganizerCollectionEngineId
{
public:
    BasicCollectionLocalId(uint id) : m_id(id) {}
    bool isEqualTo(const QOrganizerCollectionEngineId* other) const {
        return m_id == static_cast<const BasicCollectionLocalId*>(other)->m_id;
    }
    bool isLessThan(const QOrganizerCollectionEngineId* other) const {
        return m_id < static_cast<const BasicCollectionLocalId*>(other)->m_id;
    }
    QOrganizerCollectionEngineId* clone() const {
        BasicCollectionLocalId* cloned = new BasicCollectionLocalId(m_id);
        return cloned;
    }
    QString managerUri() const {
        static const QString uri(QLatin1String("qtorganizer:basicCollection:"));
        return uri;
    }
    QDebug& debugStreamOut(QDebug& dbg) const {
        return dbg << m_id;
    }
    QString toString() const {
        return QString::number(m_id);
    }
    uint hash() const {
        return m_id;
    }

private:
    uint m_id;
};

QOrganizerItemId makeId(uint id)
{
    return QOrganizerItemId(new BasicItemLocalId(id));
}

QOrganizerCollectionId makeCId(uint id)
{
    return QOrganizerCollectionId(new BasicCollectionLocalId(id));
}

tst_QOrganizerItemFilter::tst_QOrganizerItemFilter()
{
}

tst_QOrganizerItemFilter::~tst_QOrganizerItemFilter()
{
}

void tst_QOrganizerItemFilter::init()
{
}

void tst_QOrganizerItemFilter::cleanup()
{
}

void tst_QOrganizerItemFilter::classHierarchy()
{
    /* Test "casting" up and down the hierarchy */
    QOrganizerItemDetailFilter df;
    QVERIFY(df.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    df.setDetailDefinitionName("Frog", "Croak");
    df.setValue(42);

    QOrganizerItemFilter f = df;
    QVERIFY(f.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);

    QOrganizerItemDetailFilter df2 = f;
    QVERIFY(df2.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    QVERIFY(df2.detailDefinitionName() == "Frog");
    QVERIFY(df2.detailFieldName() == "Croak");
    QVERIFY(df2.value() == 42);

    /* Now try to check if we dangle pointers at all */
    {
        QOrganizerItemFilter f2 = df2;
    }
    QVERIFY(df2.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    QVERIFY(df2.detailDefinitionName() == "Frog");
    QVERIFY(df2.detailFieldName() == "Croak");
    QVERIFY(df2.value() == 42);

    {
        QOrganizerItemDetailFilter sdf2 = df2;
        sdf2.setDetailDefinitionName("Toad");
        QVERIFY(sdf2.detailDefinitionName() == "Toad");
        QVERIFY(df2.detailDefinitionName() == "Frog");
    }
    QVERIFY(df2.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    QVERIFY(df2.detailDefinitionName() == "Frog");
    QVERIFY(df2.detailFieldName() == "Croak");
    QVERIFY(df2.value() == 42);

    /* Try creating a default filter and making sure we don't break */
    QOrganizerItemFilter deff, deff2;

    QVERIFY(deff.type() == QOrganizerItemFilter::DefaultFilter);
    QVERIFY(deff == deff);
    QVERIFY(deff == deff2);
    QVERIFY(deff != df2);
    QVERIFY(df2 != deff);

    QOrganizerItemFilter fdeff = deff;
    QVERIFY(fdeff.type() == QOrganizerItemFilter::DefaultFilter);
    QVERIFY(fdeff == deff);
    QVERIFY(fdeff == deff2);

    /* Now some "invalid" filters */
    QOrganizerItemInvalidFilter iff, iff2;

    QVERIFY(iff.type() == QOrganizerItemFilter::InvalidFilter);
    QVERIFY(iff == iff);
    QVERIFY(iff == iff2);
    QVERIFY(iff != df2);
    QVERIFY(df2 != iff);

    QOrganizerItemFilter fiff = iff;
    QVERIFY(fiff.type() == QOrganizerItemFilter::InvalidFilter);
    QVERIFY(fiff == iff);
    QVERIFY(fiff == iff2);

    /* Now test some "cross casting" */

}

void tst_QOrganizerItemFilter::intersectionFilter()
{
    /* Test boolean ops */
    QOrganizerItemDetailFilter df;
    df.setDetailDefinitionName("Frog");

    QOrganizerItemDetailFilter df2;
    df2.setDetailDefinitionName("Toad");

    QOrganizerItemDetailFilter df3;
    df3.setDetailDefinitionName("Hippopotamus");

    QOrganizerItemIntersectionFilter bf;
    bf << df << df2;

    QOrganizerItemFilter f = df & df2;

    QVERIFY(bf == f);

    QOrganizerItemFilter f2 = bf & df3;
    QVERIFY(f2.type() == QOrganizerItemFilter::IntersectionFilter);
    QOrganizerItemIntersectionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == bf);
    QVERIFY(bf2.filters().at(1) == df3);

    f2 = df3 & bf;
    QVERIFY(f2.type() == QOrganizerItemFilter::IntersectionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == bf);

    /* Save this list */
    QList<QOrganizerItemFilter> filterList = bf2.filters();

    f2 = df & df2 & df3;
    QVERIFY(f2.type() == QOrganizerItemFilter::IntersectionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == (df & df2));
    QVERIFY(bf2.filters().at(1) == df3);

    /* Self assignment should do nothing */
    bf2 = bf2;
    QVERIFY(bf2 == f2);

    /* Test set filter */
    bf2.setFilters(filterList);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == bf);

    /* Test remove */
    bf2.remove(bf);
    QCOMPARE(bf2.filters().count(), 1);
    QVERIFY(bf2.filters().at(0) == df3);

    /* Double remove, should do nothing */
    bf2.remove(bf);
    QCOMPARE(bf2.filters().count(), 1);
    QVERIFY(bf2.filters().at(0) == df3);

    /* Append/prepend */
    QOrganizerItemIntersectionFilter bf3;
    bf3.append(df);
    QVERIFY(bf3.filters().count() == 1);
    bf3.prepend(df2);
    QVERIFY(bf3.filters().count() == 2);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    bf3.append(df3);
    QVERIFY(bf3.filters().count() == 3);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    QVERIFY(bf3.filters().at(2) == df3);
    bf3.prepend(df3);
    QVERIFY(bf3.filters().count() == 4);
    QVERIFY(bf3.filters().at(0) == df3);
    QVERIFY(bf3.filters().at(1) == df2);
    QVERIFY(bf3.filters().at(2) == df);
    QVERIFY(bf3.filters().at(3) == df3);

    /* Clear */
    bf3.clear();
    QVERIFY(bf3.filters().isEmpty());
}

void tst_QOrganizerItemFilter::unionFilter()
{
    /* Test boolean ops */
    QOrganizerItemDetailFilter df;
    df.setDetailDefinitionName("Frog");

    QOrganizerItemDetailFilter df2;
    df2.setDetailDefinitionName("Toad");

    QOrganizerItemDetailFilter df3;
    df3.setDetailDefinitionName("Hippopotamus");

    QOrganizerItemUnionFilter bf;
    bf << df << df2;

    QOrganizerItemFilter f = df | df2;

    QVERIFY(bf == f);

    QOrganizerItemFilter f2 = bf | df3;
    QVERIFY(f2.type() == QOrganizerItemFilter::UnionFilter);
    QOrganizerItemUnionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    f2 = df3 | bf;
    QVERIFY(f2.type() == QOrganizerItemFilter::UnionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    /* Save this list */
    QList<QOrganizerItemFilter> filterList = bf2.filters();

    f2 = df | df2 | df3;
    QVERIFY(f2.type() == QOrganizerItemFilter::UnionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    /* Self assignment should do nothing */
    bf2 = bf2;
    QVERIFY(bf2 == f2);

    /* Test set filter */
    bf2.setFilters(filterList);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    /* Test remove */
    bf2.remove(df);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df2);

    /* Double remove, should do nothing */
    bf2.remove(df);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df2);

    /* Append/prepend */
    QOrganizerItemUnionFilter bf3;
    bf3.append(df);
    QVERIFY(bf3.filters().count() == 1);
    bf3.prepend(df2);
    QVERIFY(bf3.filters().count() == 2);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    bf3.append(df3);
    QVERIFY(bf3.filters().count() == 3);
    QVERIFY(bf3.filters().at(0) == df2);
    QVERIFY(bf3.filters().at(1) == df);
    QVERIFY(bf3.filters().at(2) == df3);
    bf3.prepend(df3);
    QVERIFY(bf3.filters().count() == 4);
    QVERIFY(bf3.filters().at(0) == df3);
    QVERIFY(bf3.filters().at(1) == df2);
    QVERIFY(bf3.filters().at(2) == df);
    QVERIFY(bf3.filters().at(3) == df3);

    /* Clear */
    bf3.clear();
    QVERIFY(bf3.filters().isEmpty());
}


void tst_QOrganizerItemFilter::changeLogFilter()
{
    QOrganizerItemChangeLogFilter cf;
    QOrganizerItemChangeLogFilter cfadded(QOrganizerItemChangeLogFilter::EventAdded);
    QOrganizerItemChangeLogFilter cfchanged(QOrganizerItemChangeLogFilter::EventChanged);
    QOrganizerItemChangeLogFilter cfremoved(QOrganizerItemChangeLogFilter::EventRemoved);

    QVERIFY(cf.type() == QOrganizerItemFilter::ChangeLogFilter);
    QVERIFY(cf.eventType() == QOrganizerItemChangeLogFilter::EventAdded);

    QVERIFY(cfadded.type() == QOrganizerItemFilter::ChangeLogFilter);
    QVERIFY(cfadded.eventType() == QOrganizerItemChangeLogFilter::EventAdded);

    QVERIFY(cfchanged.type() == QOrganizerItemFilter::ChangeLogFilter);
    QVERIFY(cfchanged.eventType() == QOrganizerItemChangeLogFilter::EventChanged);

    QVERIFY(cfremoved.type() == QOrganizerItemFilter::ChangeLogFilter);
    QVERIFY(cfremoved.eventType() == QOrganizerItemChangeLogFilter::EventRemoved);


    /* Just to break the naming scheme */
    cfchanged.setEventType(QOrganizerItemChangeLogFilter::EventAdded);
    QVERIFY(cfchanged.eventType() == QOrganizerItemChangeLogFilter::EventAdded);

    QVERIFY(cf.since() == QDateTime());

    QDateTime now = QDateTime::currentDateTime();
    cf.setSince(now);

    QVERIFY(cf.since() == now);

    cf.setSince(QDateTime());
    QVERIFY(cf.since() == QDateTime());

    /* Test op= */
    QOrganizerItemFilter f = cf;
    QVERIFY(f == cf);

    QOrganizerItemChangeLogFilter cf2 = f;
    QVERIFY(cf2 == cf);

    /* Self assignment should do nothing */
    cf2 = cf2;
    QVERIFY(cf2 == cf);

    QOrganizerItemDetailFilter dfil;
    QOrganizerItemChangeLogFilter cf3(dfil);
    QVERIFY(cf3.type() == QOrganizerItemFilter::ChangeLogFilter);
    QOrganizerItemChangeLogFilter cf4(cf);
    QVERIFY(cf4 == cf);
    cf = dfil;
    QVERIFY(cf == cf3);
    cf = cf3;
    cf.setEventType(QOrganizerItemChangeLogFilter::EventRemoved); // force a detach
}

void tst_QOrganizerItemFilter::detailFilter()
{
    QOrganizerItemDetailFilter df;

    QVERIFY(df.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);

    QVERIFY(df.detailDefinitionName().isEmpty());
    QVERIFY(df.detailFieldName().isEmpty());
    QVERIFY(df.matchFlags() == 0);
    QVERIFY(df.value().isNull());

    df.setDetailDefinitionName("Definition");
    QVERIFY(df.detailDefinitionName() == "Definition");
    QVERIFY(df.detailFieldName().isEmpty());
    QVERIFY(df.matchFlags() == 0);
    QVERIFY(df.value().isNull());

    df.setDetailDefinitionName("Definition", "Field");
    QVERIFY(df.detailDefinitionName() == "Definition");
    QVERIFY(df.detailFieldName() == "Field");
    QVERIFY(df.matchFlags() == 0);
    QVERIFY(df.value().isNull());

    df.setMatchFlags(QOrganizerItemFilter::MatchExactly);
    QVERIFY(df.matchFlags() == QOrganizerItemFilter::MatchExactly);

    df.setValue(5);
    QVERIFY(df.value() == 5);

    df.setValue("String value");
    QVERIFY(df.value() == "String value");

    /* Test op= */
    QOrganizerItemFilter f = df;
    QVERIFY(f == df);

    QOrganizerItemDetailFilter df2 = f;
    QVERIFY(df2 == df);
    QVERIFY(df2.detailDefinitionName() == "Definition");
    QVERIFY(df2.detailFieldName() == "Field");

    /* Self assignment should do nothing */
    df2 = df2;
    QVERIFY(df2 == df);

    /* Some cross casting */
    QOrganizerItemDetailRangeFilter rf;

    /* Directly */
    df2 = rf;
    QVERIFY(df2.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    QVERIFY(df2.detailDefinitionName().isEmpty());
    QVERIFY(df2.detailFieldName().isEmpty());
    QVERIFY(df2.value().isNull());

    /* reset it */
    df2 = df;
    QVERIFY(df2.detailDefinitionName() == "Definition");
    QVERIFY(df2.detailFieldName() == "Field");

    /* Through base class */
    f = rf;
    df2 = f;
    QVERIFY(df2.detailDefinitionName().isEmpty());
    QVERIFY(df2.detailFieldName().isEmpty());
    QVERIFY(df2.value().isNull());

    /* Now test copy ctor */
    QOrganizerItemDetailFilter df3(rf);
    QVERIFY(df3.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    QVERIFY(df3.detailDefinitionName().isEmpty());
    QVERIFY(df3.detailFieldName().isEmpty());
    QVERIFY(df3.value().isNull());

    /* reset it */
    df3 = df;
    QVERIFY(df3.detailDefinitionName() == "Definition");
    QVERIFY(df3.detailFieldName() == "Field");

    /* Now test copy ctor through base class */
    QOrganizerItemDetailFilter df4(f);
    QVERIFY(df4.type() == QOrganizerItemFilter::OrganizerItemDetailFilter);
    QVERIFY(df4.detailDefinitionName().isEmpty());
    QVERIFY(df4.detailFieldName().isEmpty());
    QVERIFY(df4.value().isNull());

    /* reset it */
    df4 = df;
    QVERIFY(df4.detailDefinitionName() == "Definition");
    QVERIFY(df4.detailFieldName() == "Field");
}

void tst_QOrganizerItemFilter::detailRangeFilter()
{
    QOrganizerItemDetailRangeFilter rf;

    QVERIFY(rf.type() == QOrganizerItemFilter::OrganizerItemDetailRangeFilter);

    QVERIFY(rf.detailDefinitionName().isEmpty());
    QVERIFY(rf.detailFieldName().isEmpty());
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setDetailDefinitionName("Definition");
    QVERIFY(rf.detailDefinitionName() == "Definition");
    QVERIFY(rf.detailFieldName().isEmpty());
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setDetailDefinitionName("Definition", "Field");
    QVERIFY(rf.detailDefinitionName() == "Definition");
    QVERIFY(rf.detailFieldName() == "Field");
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setMatchFlags(QOrganizerItemFilter::MatchExactly);
    QVERIFY(rf.matchFlags() == QOrganizerItemFilter::MatchExactly);

    rf.setMatchFlags(QOrganizerItemFilter::MatchCaseSensitive);
    QVERIFY(rf.matchFlags() == QOrganizerItemFilter::MatchCaseSensitive);

    // Contains is not allowed
    rf.setMatchFlags(QOrganizerItemFilter::MatchCaseSensitive | QOrganizerItemFilter::MatchContains);
    QVERIFY(rf.matchFlags() == QOrganizerItemFilter::MatchCaseSensitive);

    rf.setMatchFlags(QOrganizerItemFilter::MatchEndsWith);
    QVERIFY(rf.matchFlags() == QOrganizerItemFilter::MatchExactly); // 0

    rf.setRange(5, 10);
    QVERIFY(rf.minValue() == 5);
    QVERIFY(rf.maxValue() == 10);
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setRange(QVariant(), 11);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue() == 11);
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setRange(6, QVariant());
    QVERIFY(rf.minValue() == 6);
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setRange(QVariant(), QVariant());
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setRange(5, 10, QOrganizerItemDetailRangeFilter::ExcludeLower);
    QVERIFY(rf.minValue() == 5);
    QVERIFY(rf.maxValue() == 10);
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::ExcludeLower));

    rf.setRange(QVariant(), 11, QOrganizerItemDetailRangeFilter::IncludeUpper);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue() == 11);
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::IncludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    rf.setRange(6, QVariant(), QOrganizerItemDetailRangeFilter::ExcludeLower | QOrganizerItemDetailRangeFilter::IncludeUpper);
    QVERIFY(rf.minValue() == 6);
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::IncludeUpper | QOrganizerItemDetailRangeFilter::ExcludeLower));

    rf.setRange(QVariant(), QVariant(), QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QOrganizerItemDetailRangeFilter::ExcludeUpper | QOrganizerItemDetailRangeFilter::IncludeLower));

    /* Test op= */
    QOrganizerItemFilter f = rf;
    QVERIFY(f == rf);

    QOrganizerItemDetailRangeFilter rf2 = f;
    QVERIFY(rf2 == rf);

    rf2 = rf;
    QVERIFY(rf2 == f);

    /* Self assignment should do nothing */
    rf2 = rf2;
    QVERIFY(rf2 == rf);
}


void tst_QOrganizerItemFilter::sortObject()
{
    QOrganizerItemSortOrder sortorder;

    /* Defaults */
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QOrganizerItemSortOrder());

    /* Blank Policy */
    sortorder.setBlankPolicy(QOrganizerItemSortOrder::BlanksFirst);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QOrganizerItemSortOrder());

    sortorder.setBlankPolicy(QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QOrganizerItemSortOrder());

    /* Direction */
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QOrganizerItemSortOrder());

    sortorder.setDirection(Qt::AscendingOrder);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QOrganizerItemSortOrder());

    /* Case sensitivity */
    sortorder.setCaseSensitivity(Qt::CaseInsensitive);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseInsensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QOrganizerItemSortOrder());

    sortorder.setCaseSensitivity(Qt::CaseSensitive);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QOrganizerItemSortOrder());

    /* Definitions */
    sortorder.setDetailDefinitionName(QString(), QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("", QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName(QString(), "");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("", "");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("Definition", QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QOrganizerItemSortOrder());

    sortorder.setDetailDefinitionName("Definition", "Detail");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName() == "Definition");
    QVERIFY(sortorder.detailFieldName() == "Detail");
    QVERIFY(sortorder.isValid());

    sortorder.setDetailDefinitionName(QString(), "Detail");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    /* Copy ctor */
    sortorder.setDetailDefinitionName("Definition", "Detail");
    sortorder.setBlankPolicy(QOrganizerItemSortOrder::BlanksFirst);
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName() == "Definition");
    QVERIFY(sortorder.detailFieldName() == "Detail");
    QVERIFY(sortorder.isValid());

    QOrganizerItemSortOrder other(sortorder);
    QVERIFY(other.direction() == Qt::DescendingOrder);
    QVERIFY(other.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst);
    QVERIFY(other.detailDefinitionName() == "Definition");
    QVERIFY(other.detailFieldName() == "Detail");
    QVERIFY(other.isValid());
    QVERIFY(other == sortorder);
    QVERIFY(!(other != sortorder));

    other.setDetailDefinitionName("Another Definition", "Detail");
    QVERIFY(other != sortorder);

    other.setDetailDefinitionName("Definition", "Another Detail");
    QVERIFY(other != sortorder);

    /* Assignment operator */
    QOrganizerItemSortOrder another;
    another = other;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Another Detail");
    QVERIFY(another.isValid());
    QVERIFY(another == other);
    QVERIFY(!(other != another));

    /* Self assignment */
    another = another;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QOrganizerItemSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Another Detail");
    QVERIFY(another.isValid());
    QVERIFY(another == other);
    QVERIFY(!(other != another));
}

void tst_QOrganizerItemFilter::boringFilters()
{
    QOrganizerItemFilter all;
    QVERIFY(all.type() == QOrganizerItemFilter::DefaultFilter);

    QOrganizerItemInvalidFilter invalid;
    QVERIFY(invalid.type() == QOrganizerItemFilter::InvalidFilter);

    QVERIFY(all != invalid);
    QVERIFY(!(all == invalid));

    /* Test op= */
    QOrganizerItemFilter f = all;
    QVERIFY(f == all);

    QOrganizerItemFilter f2;
    f2 = f;
    QVERIFY(f2 == all);

    /* Self assignment should do nothing */
    f2 = f2;
    QVERIFY(f2 == all);

    /* InvalidFilter, op= */
    QOrganizerItemInvalidFilter inv2 = invalid;
    QVERIFY(inv2 == invalid);

    QOrganizerItemInvalidFilter inv3;
    inv3 = inv2;
    QVERIFY(inv3 == invalid);

    inv3 = inv3;
    QVERIFY(inv3 == invalid);

    inv3 = all;
    QVERIFY(inv3 == invalid); // won't be all
}

void tst_QOrganizerItemFilter::idListFilter()
{
    QOrganizerItemIdFilter idf;

    QVERIFY(idf.type() == QOrganizerItemFilter::IdFilter);

    QVERIFY(idf.ids().count() == 0);

    QList<QOrganizerItemId> ids;
    ids << makeId(5) << makeId(6) << makeId(17);

    idf.setIds(ids);
    QVERIFY(idf.ids() == ids);

    idf.setIds(QList<QOrganizerItemId>());
    QVERIFY(idf.ids().count() == 0);

    QOrganizerItemId singleId = makeId(12);
    idf.insert(singleId);
    QVERIFY(idf.ids().contains(singleId));
    idf.remove(singleId);
    QVERIFY(!idf.ids().contains(singleId));
    QList<QOrganizerItemId> allIds = idf.ids();
    idf.remove(singleId); // remove again
    QVERIFY(idf.ids() == allIds);
    idf.clear();
    QVERIFY(idf.ids().isEmpty());
    idf.setIds(allIds);

    /* Test op= */
    idf.setIds(ids);
    QOrganizerItemFilter f = idf;
    QVERIFY(f == idf);

    QOrganizerItemIdFilter idf2 = f;
    QVERIFY(idf2 == idf);
    QVERIFY(idf2.ids() == ids);

    idf2 = idf;
    QVERIFY(idf2 == f);

    /* Self assignment should do nothing */
    idf2 = idf2;
    QVERIFY(idf2 == idf);

    QOrganizerItemDetailFilter dfil;
    QOrganizerItemIdFilter idf3(dfil);
    QVERIFY(idf3.type() == QOrganizerItemFilter::IdFilter); // should be a blank id list filter
    QOrganizerItemIdFilter idf4(idf);
    QVERIFY(idf4 == idf); // should be a copy of idf.
    idf = dfil; // now assign.
    QVERIFY(idf == idf3); // again, should be a blank id list filter.
    idf = idf3;
    idf.setIds(ids); // force a detach
}

void tst_QOrganizerItemFilter::collectionFilter()
{
    QOrganizerItemCollectionFilter icf;

    QVERIFY(icf.collectionIds().isEmpty());

    QOrganizerCollectionId id1 = makeCId(5);
    QOrganizerCollectionId id2 = makeCId(6);
    QOrganizerCollectionId id3 = makeCId(7);
    QOrganizerCollectionId id4 = makeCId(12);
    QSet<QOrganizerCollectionId> ids;
    ids << id1 << id2 << id3;

    icf.setCollectionIds(ids);
    QVERIFY(icf.collectionIds() == ids);

    icf.setCollectionId(id4);
    ids.clear();
    ids << id4;
    QVERIFY(icf.collectionIds() == ids);

    QOrganizerItemCollectionFilter icf2;
    icf2 = icf;
    QVERIFY(icf2.collectionIds() == ids);

    QOrganizerItemFilter fil;
    fil = icf;
    QVERIFY(fil.type() == QOrganizerItemFilter::CollectionFilter);

    QOrganizerItemCollectionFilter icf3(fil);
    QVERIFY(fil.type() == QOrganizerItemFilter::CollectionFilter);
    QVERIFY(icf3.collectionIds() == ids);
}

void tst_QOrganizerItemFilter::canonicalizedFilter()
{
    QFETCH(QOrganizerItemFilter, in);
    QFETCH(QOrganizerItemFilter, expected);

    QOrganizerItemFilter out = QOrganizerManagerEngine::canonicalizedFilter(in);
    QCOMPARE(out, expected);
}

void tst_QOrganizerItemFilter::canonicalizedFilter_data()
{
    QTest::addColumn<QOrganizerItemFilter>("in");
    QTest::addColumn<QOrganizerItemFilter>("expected");

    QOrganizerItemFilter detailFilter1 = QOrganizerItemLocation::match("1");
    QOrganizerItemFilter detailFilter2 = QOrganizerItemLocation::match("2");
    QOrganizerItemInvalidFilter invalidFilter;
    QOrganizerItemFilter defaultFilter;

    {
        QTest::newRow("Normal detail filter")
                << static_cast<QOrganizerItemFilter>(detailFilter1)
                << static_cast<QOrganizerItemFilter>(detailFilter1);
    }

    {
        QOrganizerItemIntersectionFilter qcif;
        qcif << detailFilter1;
        qcif << detailFilter2;
        QTest::newRow("Normal intersection filter")
                << static_cast<QOrganizerItemFilter>(qcif)
                << static_cast<QOrganizerItemFilter>(qcif);
    }

    {
        QOrganizerItemUnionFilter qcuf;
        qcuf << detailFilter1;
        qcuf << detailFilter2;
        QTest::newRow("Normal intersection filter")
                << static_cast<QOrganizerItemFilter>(qcuf)
                << static_cast<QOrganizerItemFilter>(qcuf);
    }

    {
        QOrganizerItemIntersectionFilter qcif;
        QTest::newRow("Empty intersection")
                << static_cast<QOrganizerItemFilter>(qcif)
                << static_cast<QOrganizerItemFilter>(defaultFilter);
    }

    {
        QOrganizerItemUnionFilter qcuf;
        QTest::newRow("Empty union")
                << static_cast<QOrganizerItemFilter>(qcuf)
                << static_cast<QOrganizerItemFilter>(invalidFilter);
    }

    {
        QOrganizerItemIntersectionFilter qcif;
        qcif << detailFilter1;
        QTest::newRow("Single entry intersection filter")
                << static_cast<QOrganizerItemFilter>(qcif)
                << static_cast<QOrganizerItemFilter>(detailFilter1);
    }

    {
        QOrganizerItemUnionFilter qcuf;
        qcuf << detailFilter1;
        QTest::newRow("Single entry union filter")
                << static_cast<QOrganizerItemFilter>(qcuf)
                << static_cast<QOrganizerItemFilter>(detailFilter1);
    }

    {
        QOrganizerItemIntersectionFilter qcif;
        qcif << invalidFilter;
        qcif << detailFilter1;
        qcif << detailFilter2;
        QTest::newRow("Intersection with invalid")
                << static_cast<QOrganizerItemFilter>(qcif)
                << static_cast<QOrganizerItemFilter>(invalidFilter);
    }

    {
        QOrganizerItemIntersectionFilter qcif;
        qcif << defaultFilter;
        qcif << detailFilter1;
        qcif << detailFilter2;
        QOrganizerItemIntersectionFilter expected;
        expected << detailFilter1;
        expected << detailFilter2;
        QTest::newRow("Intersection with default")
                << static_cast<QOrganizerItemFilter>(qcif)
                << static_cast<QOrganizerItemFilter>(expected);
    }

    {
        QOrganizerItemUnionFilter qcuf;
        qcuf << invalidFilter;
        qcuf << detailFilter1;
        qcuf << detailFilter2;
        QOrganizerItemUnionFilter expected;
        expected << detailFilter1;
        expected << detailFilter2;
        QTest::newRow("Union with invalid")
                << static_cast<QOrganizerItemFilter>(qcuf)
                << static_cast<QOrganizerItemFilter>(expected);
    }

    {
        QOrganizerItemUnionFilter qcuf;
        qcuf << defaultFilter;
        qcuf << detailFilter1;
        qcuf << detailFilter2;
        QTest::newRow("Union with default")
                << static_cast<QOrganizerItemFilter>(qcuf)
                << static_cast<QOrganizerItemFilter>(defaultFilter);
    }

    {
        QOrganizerItemIdFilter qclif;
        QTest::newRow("Empty local id filter")
                << static_cast<QOrganizerItemFilter>(qclif)
                << static_cast<QOrganizerItemFilter>(invalidFilter);
    }

    {
        QOrganizerItemIdFilter qclif;
        qclif.setIds(QList<QOrganizerItemId>() << makeId(1) << makeId(2));
        QTest::newRow("Normal local id filter")
                << static_cast<QOrganizerItemFilter>(qclif)
                << static_cast<QOrganizerItemFilter>(qclif);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        QOrganizerItemDetailFilter expected;
        expected.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        QTest::newRow("Null valued range filter")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(expected);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        qcdrf.setRange(QLatin1String("a"), QLatin1String("a"));
        qcdrf.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        QOrganizerItemDetailFilter expected;
        expected.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        expected.setValue(QLatin1String("a"));
        expected.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        QTest::newRow("Equal valued range filter")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(expected);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        qcdrf.setRange(QLatin1String("a"), QLatin1String("a"),
               QOrganizerItemDetailRangeFilter::ExcludeLower | QOrganizerItemDetailRangeFilter::ExcludeUpper);
        qcdrf.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        QTest::newRow("Equal valued range filter with excluded bounds")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(invalidFilter);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        qcdrf.setRange(QLatin1String("a"), QLatin1String("b"));
        qcdrf.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        QTest::newRow("Normal range filter")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(qcdrf);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        qcdrf.setRange(QVariant(QVariant::String), QVariant(QVariant::String)); // null bounds
        qcdrf.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        QOrganizerItemDetailFilter qcdf;
        qcdf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        qcdf.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        qcdf.setValue(QVariant(QVariant::String));
        QTest::newRow("Null valued range filter")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(qcdf);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        qcdrf.setDetailDefinitionName(QOrganizerItemLocation::DefinitionName, QOrganizerItemLocation::FieldLabel);
        qcdrf.setRange(QVariant(QVariant::String), QLatin1String("a")); // min is null
        qcdrf.setMatchFlags(QOrganizerItemFilter::MatchFixedString);
        QTest::newRow("One sided range filter")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(qcdrf);
    }

    {
        QOrganizerItemDetailRangeFilter qcdrf;
        QTest::newRow("Empty range filter")
                << static_cast<QOrganizerItemFilter>(qcdrf)
                << static_cast<QOrganizerItemFilter>(invalidFilter);
    }

    {
        QOrganizerItemDetailFilter qcdf;
        QTest::newRow("Empty detail filter")
                << static_cast<QOrganizerItemFilter>(qcdf)
                << static_cast<QOrganizerItemFilter>(invalidFilter);
    }
}

void tst_QOrganizerItemFilter::testFilter()
{
    QFETCH(QOrganizerItem, item);
    QFETCH(QOrganizerItemFilter, filter);
    QFETCH(bool, expected);

    QCOMPARE(QOrganizerManagerEngine::testFilter(filter, item), expected);
}

void tst_QOrganizerItemFilter::testFilter_data()
{
    QTest::addColumn<QOrganizerItem>("item");
    QTest::addColumn<QOrganizerItemFilter>("filter");
    QTest::addColumn<bool>("expected");

    // XXX TODO: other detail types (comment, description, ...)

    {
        QOrganizerItem item;
        QOrganizerItemLocation name;
        name.setLabel("test location");
        item.saveDetail(&name);

        QTest::newRow("QOrganizerItemLocation::match location")
                << item
                << QOrganizerItemLocation::match("test location")
                << true;
        QTest::newRow("QOrganizerItemLocation::match substring")
                << item
                << QOrganizerItemLocation::match(QLatin1String("ocati"))
                << true;
        QTest::newRow("QOrganizerItemLocation::match negative")
                << item
                << QOrganizerItemLocation::match("foo")
                << false;
    }

    {
        QOrganizerItem item;
        item.setDisplayLabel(QLatin1String("foo"));
        QTest::newRow("QOrganizerItemDisplayLabel::match positive")
                << item
                << QOrganizerItemDisplayLabel::match("foo")
                << true;
        QTest::newRow("QOrganizerItemDisplayLabel::match positive substring")
                << item
                << QOrganizerItemDisplayLabel::match("o")
                << true;
        QTest::newRow("QOrganizerItemDisplayLabel::match negative")
                << item
                << QOrganizerItemDisplayLabel::match("bar")
                << false;
    }

    {
        QOrganizerItem item;
        QOrganizerItemPriority priority;
        priority.setPriority(QOrganizerItemPriority::VeryHighPriority);
        item.saveDetail(&priority);
        QTest::newRow("QOrganizerItemPriority::match positive")
                << item
                << QOrganizerItemPriority::match(QOrganizerItemPriority::VeryHighPriority)
                << true;
        QTest::newRow("QOrganizerItemPhoneNumber::match negative")
                << item
                << QOrganizerItemPriority::match(QOrganizerItemPriority::VeryLowPriority)
                << false;
    }
}

void tst_QOrganizerItemFilter::datastream()
{
    QFETCH(QOrganizerItemFilter, filterIn);

    QByteArray buffer;
    QDataStream stream1(&buffer, QIODevice::WriteOnly);
    stream1 << filterIn;

    QVERIFY(buffer.size() > 0);

    QDataStream stream2(buffer);
    QOrganizerItemFilter filterOut;
    stream2 >> filterOut;
    QCOMPARE(filterOut, filterIn);
}

void tst_QOrganizerItemFilter::datastream_data()
{
    QTest::addColumn<QOrganizerItemFilter>("filterIn");

    {
        QOrganizerItemFilter filter;
        QTest::newRow("default") << filter;
    }

    {
        QOrganizerItemChangeLogFilter filter;
        filter.setEventType(QOrganizerItemChangeLogFilter::EventAdded);
        filter.setSince(QDateTime(QDate(2010, 6, 1), QTime(1, 2, 3)));
        QTest::newRow("changelog") << (QOrganizerItemFilter)filter;
    }

    {
        QOrganizerItemDetailFilter filter;
        filter.setDetailDefinitionName("detail", "field");
        filter.setMatchFlags(QOrganizerItemFilter::MatchEndsWith);
        filter.setValue("ski");
        QTest::newRow("detail") << (QOrganizerItemFilter)filter;
    }

    {
        QOrganizerItemIntersectionFilter filter;
        QTest::newRow("intersection") << (QOrganizerItemFilter)filter;
    }

    {
        QOrganizerItemInvalidFilter filter;
        QTest::newRow("invalid") << (QOrganizerItemFilter)filter;
    }

    {
        QOrganizerItemUnionFilter filter;
        QTest::newRow("union") << (QOrganizerItemFilter)filter;
    }

    // NOTE: LocalIdFilter streaming is not supported
}

void tst_QOrganizerItemFilter::traits()
{
    QCOMPARE(sizeof(QOrganizerItemFilter), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemFilter)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QOrganizerItemFilter::sortTraits()
{
    QCOMPARE(sizeof(QOrganizerItemSortOrder), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QOrganizerItemSortOrder)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}


QTEST_MAIN(tst_QOrganizerItemFilter)
#include "tst_qorganizeritemfilter.moc"
