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

#include "qtcontacts.h"
#include "qcontactmanagerdataholder.h" //QContactManagerDataHolder

//TESTED_CLASS=
//TESTED_FILES=

QTM_USE_NAMESPACE
class tst_QContactFilter : public QObject
{
Q_OBJECT

public:
    tst_QContactFilter();
    virtual ~tst_QContactFilter();

private:
    QContactManagerDataHolder managerDataHolder;

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
    void actionFilter();
    void relationshipFilter();
    void boringFilters();
    void idListFilter();

    void traits();

    void sortObject(); // should perhaps be in a different test :)
    void sortTraits();
};

tst_QContactFilter::tst_QContactFilter()
{
}

tst_QContactFilter::~tst_QContactFilter()
{
}

void tst_QContactFilter::init()
{
}

void tst_QContactFilter::cleanup()
{
}

void tst_QContactFilter::classHierarchy()
{
    /* Test "casting" up and down the hierarchy */
    QContactDetailRangeFilter drf;
    QVERIFY(drf.type() == QContactFilter::ContactDetailRangeFilter);
    drf.setDetailDefinitionName("Frog", "Croak");
    drf.setRange(1, 20);

    QContactFilter f = drf;
    QVERIFY(f.type() == QContactFilter::ContactDetailRangeFilter);

    QContactDetailRangeFilter drf2 = f;
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRangeFilter);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    /* Now try to check if we dangle pointers at all */
    {
        QContactFilter f2 = drf2;
    }
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRangeFilter);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    {
        QContactDetailRangeFilter rf2 = drf2;
        rf2.setDetailDefinitionName("Toad");
        QVERIFY(rf2.detailDefinitionName() == "Toad");
        QVERIFY(drf2.detailDefinitionName() == "Frog");
    }
    QVERIFY(drf2.type() == QContactFilter::ContactDetailRangeFilter);
    QVERIFY(drf2.detailDefinitionName() == "Frog");
    QVERIFY(drf2.detailFieldName() == "Croak");
    QVERIFY(drf2.maxValue() == 20);
    QVERIFY(drf2.minValue() == 1);

    /* Try creating a default filter and making sure we don't break */
    QContactFilter deff, deff2;

    QVERIFY(deff.type() == QContactFilter::DefaultFilter);
    QVERIFY(deff == deff);
    QVERIFY(deff == deff2);
    QVERIFY(deff != drf2);
    QVERIFY(drf2 != deff);

    QContactFilter fdeff = deff;
    QVERIFY(fdeff.type() == QContactFilter::DefaultFilter);
    QVERIFY(fdeff == deff);
    QVERIFY(fdeff == deff2);

    /* Now some "invalid" filters */
    QContactInvalidFilter iff, iff2;

    QVERIFY(iff.type() == QContactFilter::InvalidFilter);
    QVERIFY(iff == iff);
    QVERIFY(iff == iff2);
    QVERIFY(iff != drf2);
    QVERIFY(drf2 != iff);

    QContactFilter fiff = iff;
    QVERIFY(fiff.type() == QContactFilter::InvalidFilter);
    QVERIFY(fiff == iff);
    QVERIFY(fiff == iff2);

    /* Now test some "cross casting" */

}

void tst_QContactFilter::intersectionFilter()
{
    /* Test boolean ops */
    QContactDetailFilter df;
    df.setDetailDefinitionName("Frog");

    QContactDetailFilter df2;
    df2.setDetailDefinitionName("Toad");

    QContactDetailFilter df3;
    df3.setDetailDefinitionName("Hippopotamus");

    QContactIntersectionFilter bf;
    bf << df << df2;

    QContactFilter f = df & df2;

    QVERIFY(bf == f);

    QContactFilter f2 = bf & df3;
    QVERIFY(f2.type() == QContactFilter::IntersectionFilter);
    QContactIntersectionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == bf);
    QVERIFY(bf2.filters().at(1) == df3);

    f2 = df3 & bf;
    QVERIFY(f2.type() == QContactFilter::IntersectionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 2);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == bf);

    /* Save this list */
    QList<QContactFilter> filterList = bf2.filters();

    f2 = df & df2 & df3;
    QVERIFY(f2.type() == QContactFilter::IntersectionFilter);
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
    QContactIntersectionFilter bf3;
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
}

void tst_QContactFilter::unionFilter()
{
    /* Test boolean ops */
    QContactDetailFilter df;
    df.setDetailDefinitionName("Frog");

    QContactDetailFilter df2;
    df2.setDetailDefinitionName("Toad");

    QContactDetailFilter df3;
    df3.setDetailDefinitionName("Hippopotamus");

    QContactUnionFilter bf;
    bf << df << df2;

    QContactFilter f = df | df2;

    QVERIFY(bf == f);

    QContactFilter f2 = bf | df3;
    QVERIFY(f2.type() == QContactFilter::UnionFilter);
    QContactUnionFilter bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df);
    QVERIFY(bf2.filters().at(1) == df2);
    QVERIFY(bf2.filters().at(2) == df3);

    f2 = df3 | bf;
    QVERIFY(f2.type() == QContactFilter::UnionFilter);
    bf2 = f2;
    QVERIFY(bf2 == f2);
    QCOMPARE(bf2.filters().count(), 3);
    QVERIFY(bf2.filters().at(0) == df3);
    QVERIFY(bf2.filters().at(1) == df);
    QVERIFY(bf2.filters().at(2) == df2);

    /* Save this list */
    QList<QContactFilter> filterList = bf2.filters();

    f2 = df | df2 | df3;
    QVERIFY(f2.type() == QContactFilter::UnionFilter);
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
    QContactUnionFilter bf3;
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
}

void tst_QContactFilter::actionFilter()
{
    QContactActionFilter af;

    /* Test initial conditions */
    QVERIFY(af.type() == QContactFilter::ActionFilter);
    QVERIFY(af.actionName().isEmpty());
    QVERIFY(af.value().isNull());
    QVERIFY(af.vendorName().isEmpty());
    QVERIFY(af.implementationVersion() == -1);

    af.setActionName("Action Name");
    QVERIFY(af.actionName() == "Action Name");

    af.setActionName(QString());
    QVERIFY(af.actionName().isEmpty());

    af.setValue(5);
    QVERIFY(af.value() == 5);

    af.setValue("This is a string");
    QVERIFY(af.value() == "This is a string");

    af.setVendor("Vendor");
    QVERIFY(af.vendorName() == "Vendor");
    QVERIFY(af.implementationVersion() == -1);

    af.setVendor(QString());
    QVERIFY(af.vendorName().isEmpty());
    QVERIFY(af.implementationVersion() == -1);

    af.setVendor(QString(), 10);
    QVERIFY(af.vendorName().isEmpty());
    QVERIFY(af.implementationVersion() == -1);

    af.setVendor("Vendor", 10);
    QVERIFY(af.vendorName() == "Vendor");
    QVERIFY(af.implementationVersion() == 10);

    af.setVendor("Vendor", -1);
    QVERIFY(af.vendorName() == "Vendor");
    QVERIFY(af.implementationVersion() == -1);

    af.setVendor("Vendor", 10);
    QVERIFY(af.vendorName() == "Vendor");
    QVERIFY(af.implementationVersion() == 10);

    af.setVendor("Vendor");
    QVERIFY(af.vendorName() == "Vendor");
    QVERIFY(af.implementationVersion() == -1);

    // Make sure there isn't a shadow copy
    af.setVendor("Vendor", 10);
    af.setVendor(QString());
    QVERIFY(af.implementationVersion() == -1);
    af.setVendor("Vendor");
    QVERIFY(af.implementationVersion() == -1);

    /* Test op= */
    QContactFilter f = af;
    QVERIFY(f == af);

    QContactActionFilter af2 = f;
    QVERIFY(af2 == af);

    /* Self assignment should do nothing */
    af2 = af2;
    QVERIFY(af2 == af);

    QContactDetailFilter dfil;
    QContactActionFilter af3(dfil);
    QVERIFY(af3.type() == QContactFilter::ActionFilter);
    QContactActionFilter af4(af);
    QVERIFY(af4 == af);
    af = dfil;
    QVERIFY(af == af3);
    af = af3;
    af.setActionName("test"); // should force a detach
}

void tst_QContactFilter::changeLogFilter()
{
    QContactChangeLogFilter cf;
    QContactChangeLogFilter cfadded(QContactChangeLogFilter::EventAdded);
    QContactChangeLogFilter cfchanged(QContactChangeLogFilter::EventChanged);
    QContactChangeLogFilter cfremoved(QContactChangeLogFilter::EventRemoved);

    QVERIFY(cf.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cf.eventType() == QContactChangeLogFilter::EventAdded);

    QVERIFY(cfadded.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cfadded.eventType() == QContactChangeLogFilter::EventAdded);

    QVERIFY(cfchanged.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cfchanged.eventType() == QContactChangeLogFilter::EventChanged);

    QVERIFY(cfremoved.type() == QContactFilter::ChangeLogFilter);
    QVERIFY(cfremoved.eventType() == QContactChangeLogFilter::EventRemoved);


    /* Just to break the naming scheme */
    cfchanged.setEventType(QContactChangeLogFilter::EventAdded);
    QVERIFY(cfchanged.eventType() == QContactChangeLogFilter::EventAdded);

    QVERIFY(cf.since() == QDateTime());

    QDateTime now = QDateTime::currentDateTime();
    cf.setSince(now);

    QVERIFY(cf.since() == now);

    cf.setSince(QDateTime());
    QVERIFY(cf.since() == QDateTime());

    /* Test op= */
    QContactFilter f = cf;
    QVERIFY(f == cf);

    QContactChangeLogFilter cf2 = f;
    QVERIFY(cf2 == cf);

    /* Self assignment should do nothing */
    cf2 = cf2;
    QVERIFY(cf2 == cf);

    QContactDetailFilter dfil;
    QContactChangeLogFilter cf3(dfil);
    QVERIFY(cf3.type() == QContactFilter::ChangeLogFilter);
    QContactChangeLogFilter cf4(cf);
    QVERIFY(cf4 == cf);
    cf = dfil;
    QVERIFY(cf == cf3);
    cf = cf3;
    cf.setEventType(QContactChangeLogFilter::EventRemoved); // force a detach
}

void tst_QContactFilter::detailFilter()
{
    QContactDetailFilter df;

    QVERIFY(df.type() == QContactFilter::ContactDetailFilter);

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

    df.setMatchFlags(QContactFilter::MatchExactly);
    QVERIFY(df.matchFlags() == QContactFilter::MatchExactly);

    df.setValue(5);
    QVERIFY(df.value() == 5);

    df.setValue("String value");
    QVERIFY(df.value() == "String value");

    /* Test op= */
    QContactFilter f = df;
    QVERIFY(f == df);

    QContactDetailFilter df2 = f;
    QVERIFY(df2 == df);

    /* Self assignment should do nothing */
    df2 = df2;
    QVERIFY(df2 == df);
}

void tst_QContactFilter::detailRangeFilter()
{
    QContactDetailRangeFilter rf;

    QVERIFY(rf.type() == QContactFilter::ContactDetailRangeFilter);

    QVERIFY(rf.detailDefinitionName().isEmpty());
    QVERIFY(rf.detailFieldName().isEmpty());
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setDetailDefinitionName("Definition");
    QVERIFY(rf.detailDefinitionName() == "Definition");
    QVERIFY(rf.detailFieldName().isEmpty());
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setDetailDefinitionName("Definition", "Field");
    QVERIFY(rf.detailDefinitionName() == "Definition");
    QVERIFY(rf.detailFieldName() == "Field");
    QVERIFY(rf.matchFlags() == 0);

    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setMatchFlags(QContactFilter::MatchExactly);
    QVERIFY(rf.matchFlags() == QContactFilter::MatchExactly);

    rf.setRange(5, 10);
    QVERIFY(rf.minValue() == 5);
    QVERIFY(rf.maxValue() == 10);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(QVariant(), 11);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue() == 11);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(6, QVariant());
    QVERIFY(rf.minValue() == 6);
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(QVariant(), QVariant());
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(5, 10, QContactDetailRangeFilter::ExcludeLower);
    QVERIFY(rf.minValue() == 5);
    QVERIFY(rf.maxValue() == 10);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::ExcludeLower));

    rf.setRange(QVariant(), 11, QContactDetailRangeFilter::IncludeUpper);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue() == 11);
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::IncludeUpper | QContactDetailRangeFilter::IncludeLower));

    rf.setRange(6, QVariant(), QContactDetailRangeFilter::ExcludeLower | QContactDetailRangeFilter::IncludeUpper);
    QVERIFY(rf.minValue() == 6);
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::IncludeUpper | QContactDetailRangeFilter::ExcludeLower));

    rf.setRange(QVariant(), QVariant(), QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower);
    QVERIFY(rf.minValue().isNull());
    QVERIFY(rf.maxValue().isNull());
    QVERIFY(rf.rangeFlags() == (QContactDetailRangeFilter::ExcludeUpper | QContactDetailRangeFilter::IncludeLower));

    /* Test op= */
    QContactFilter f = rf;
    QVERIFY(f == rf);

    QContactDetailRangeFilter rf2 = f;
    QVERIFY(rf2 == rf);

    rf2 = rf;
    QVERIFY(rf2 == f);

    /* Self assignment should do nothing */
    rf2 = rf2;
    QVERIFY(rf2 == rf);
}

void tst_QContactFilter::relationshipFilter()
{
    QContactRelationshipFilter crf;

    QVERIFY(crf.type() == QContactFilter::RelationshipFilter);


    QVERIFY(crf.relationshipType() == QString());
    QVERIFY(crf.relatedContactId() == QContactId());

    QContactId newId;
    newId.setManagerUri("test");
    newId.setLocalId(QContactLocalId(5));
    crf.setRelatedContactId(newId);

    QVERIFY(crf.relationshipType() == QString());
    QVERIFY(crf.relatedContactId() == newId);

    crf.setRelatedContactRole(QContactRelationship::First);
    
    QVERIFY(crf.relationshipType() == QString());
    QVERIFY(crf.relatedContactId() == newId);

    crf.setRelationshipType(QContactRelationship::HasManager);

    QVERIFY(crf.relationshipType() == QContactRelationship::HasManager);
    QVERIFY(crf.relatedContactId() == newId);

    /* Test op= */
    QContactFilter f = crf;
    QVERIFY(f == crf);

    QContactRelationshipFilter crf2 = f;
    QVERIFY(crf2 == crf);

    /* Self assignment should do nothing */
    crf2 = crf2;
    QVERIFY(crf2 == crf);

    QContactDetailFilter dfil;
    QContactRelationshipFilter crf3(dfil);
    QVERIFY(crf3.type() == QContactFilter::RelationshipFilter); // should be a blank rel fil
    QContactRelationshipFilter crf4(crf);
    QVERIFY(crf4 == crf);
    crf = dfil;
    QVERIFY(crf == crf3);
    crf = crf3;
    crf.setRelationshipType("test"); // force a detach
}

void tst_QContactFilter::sortObject()
{
    QContactSortOrder sortorder;

    /* Defaults */
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Blank Policy */
    sortorder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QContactSortOrder());

    sortorder.setBlankPolicy(QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Direction */
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QContactSortOrder());

    sortorder.setDirection(Qt::AscendingOrder);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Case sensitivity */
    sortorder.setCaseSensitivity(Qt::CaseInsensitive);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseInsensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder != QContactSortOrder());

    sortorder.setCaseSensitivity(Qt::CaseSensitive);
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(sortorder.caseSensitivity() == Qt::CaseSensitive);
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    /* Definitions */
    sortorder.setDetailDefinitionName(QString(), QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("", QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName(QString(), "");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("", "");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    sortorder.setDetailDefinitionName("Definition", QString());
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());
    QVERIFY(sortorder == QContactSortOrder());

    sortorder.setDetailDefinitionName("Definition", "Detail");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName() == "Definition");
    QVERIFY(sortorder.detailFieldName() == "Detail");
    QVERIFY(sortorder.isValid());

    sortorder.setDetailDefinitionName(QString(), "Detail");
    QVERIFY(sortorder.direction() == Qt::AscendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksLast);
    QVERIFY(sortorder.detailDefinitionName().isEmpty());
    QVERIFY(sortorder.detailFieldName().isEmpty());
    QVERIFY(!sortorder.isValid());

    /* Copy ctor */
    sortorder.setDetailDefinitionName("Definition", "Detail");
    sortorder.setBlankPolicy(QContactSortOrder::BlanksFirst);
    sortorder.setDirection(Qt::DescendingOrder);
    QVERIFY(sortorder.direction() == Qt::DescendingOrder);
    QVERIFY(sortorder.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(sortorder.detailDefinitionName() == "Definition");
    QVERIFY(sortorder.detailFieldName() == "Detail");
    QVERIFY(sortorder.isValid());

    QContactSortOrder other(sortorder);
    QVERIFY(other.direction() == Qt::DescendingOrder);
    QVERIFY(other.blankPolicy() == QContactSortOrder::BlanksFirst);
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
    QContactSortOrder another;
    another = other;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Another Detail");
    QVERIFY(another.isValid());
    QVERIFY(another == other);
    QVERIFY(!(other != another));

    /* Self assignment */
    another = another;
    QVERIFY(another.direction() == Qt::DescendingOrder);
    QVERIFY(another.blankPolicy() == QContactSortOrder::BlanksFirst);
    QVERIFY(another.detailDefinitionName() == "Definition");
    QVERIFY(another.detailFieldName() == "Another Detail");
    QVERIFY(another.isValid());
    QVERIFY(another == other);
    QVERIFY(!(other != another));
}

void tst_QContactFilter::boringFilters()
{
    QContactFilter all;
    QVERIFY(all.type() == QContactFilter::DefaultFilter);

    QContactInvalidFilter invalid;
    QVERIFY(invalid.type() == QContactFilter::InvalidFilter);

    QVERIFY(all != invalid);
    QVERIFY(!(all == invalid));

    /* Test op= */
    QContactFilter f = all;
    QVERIFY(f == all);

    QContactFilter f2;
    f2 = f;
    QVERIFY(f2 == all);

    /* Self assignment should do nothing */
    f2 = f2;
    QVERIFY(f2 == all);

    /* InvalidFilter, op= */
    QContactInvalidFilter inv2 = invalid;
    QVERIFY(inv2 == invalid);

    QContactInvalidFilter inv3;
    inv3 = inv2;
    QVERIFY(inv3 == invalid);

    inv3 = inv3;
    QVERIFY(inv3 == invalid);

    inv3 = all;
    QVERIFY(inv3 == invalid); // won't be all
}

void tst_QContactFilter::idListFilter()
{
    QContactLocalIdFilter idf;

    QVERIFY(idf.type() == QContactFilter::LocalIdFilter);

    QVERIFY(idf.ids().count() == 0);

    QList<QContactLocalId> ids;
    ids << 5 << 6 << 17;

    idf.setIds(ids);
    QVERIFY(idf.ids() == ids);

    idf.setIds(QList<QContactLocalId>());
    QVERIFY(idf.ids().count() == 0);

    /* Test op= */
    idf.setIds(ids);
    QContactFilter f = idf;
    QVERIFY(f == idf);

    QContactLocalIdFilter idf2 = f;
    QVERIFY(idf2 == idf);
    QVERIFY(idf2.ids() == ids);

    idf2 = idf;
    QVERIFY(idf2 == f);

    /* Self assignment should do nothing */
    idf2 = idf2;
    QVERIFY(idf2 == idf);

    QContactDetailFilter dfil;
    QContactLocalIdFilter idf3(dfil);
    QVERIFY(idf3.type() == QContactFilter::LocalIdFilter); // should be a blank id list filter
    QContactLocalIdFilter idf4(idf);
    QVERIFY(idf4 == idf); // should be a copy of idf.
    idf = dfil; // now assign.
    QVERIFY(idf == idf3); // again, should be a blank id list filter.
    idf = idf3;
    idf.setIds(ids); // force a detach
}

void tst_QContactFilter::traits()
{
    // QCOMPARE(sizeof(QContactFilter), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QContactFilter)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(ti.isLarge); // virtual table + d pointer
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}

void tst_QContactFilter::sortTraits()
{
    QCOMPARE(sizeof(QContactSortOrder), sizeof(void *));
    QTypeInfo<QTM_PREPEND_NAMESPACE(QContactSortOrder)> ti;
    QVERIFY(ti.isComplex);
    QVERIFY(!ti.isStatic);
    QVERIFY(!ti.isLarge);
    QVERIFY(!ti.isPointer);
    QVERIFY(!ti.isDummy);
}


QTEST_MAIN(tst_QContactFilter)
#include "tst_qcontactfilter.moc"
