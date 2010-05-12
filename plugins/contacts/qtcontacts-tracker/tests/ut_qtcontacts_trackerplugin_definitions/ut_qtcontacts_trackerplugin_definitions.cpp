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

#include "ut_qtcontacts_trackerplugin_definitions.h"

#include <QContactInvalidFilter>

typedef QSet<QString> QStringSet;

void ut_qtcontacts_trackerplugin_definitions::checkAllDefitionsTested()
{
    const QStringSet testSlots(testSlotNames());

    foreach(const QContactDetailDefinition d, mContactManager->detailDefinitions()) {
        const QString saveAndFetchOne(QLatin1String("saveAndFetchOne") + d.name());
        QVERIFY2(testSlots.contains(saveAndFetchOne), qPrintable(d.name()));
        const QString saveAndFetchMany(QLatin1String("saveAndFetchMany") + d.name());
        QVERIFY2(testSlots.contains(saveAndFetchMany), qPrintable(d.name()));
        const QString saveAndFetchAll(QLatin1String("saveAndFetchAll") + d.name());
        QVERIFY2(testSlots.contains(saveAndFetchAll), qPrintable(d.name()));
    }
}

class SampleGenerator
{
public:
    SampleGenerator(QVariant::Type type, const QVariantList &allowedValues) :
            mType(type), mComplete(false), mIndex(0)
    {
        if (allowedValues.isEmpty()) {
            QVariant sample;

            switch(type) {
            case QVariant::Date:
                sample.setValue(QDate::currentDate());
                break;

            case QVariant::DateTime:
                sample.setValue(QDateTime::currentDateTime());
                break;

            case QVariant::Double:
                sample.setValue(double(qrand()) / RAND_MAX);
                break;

            case QVariant::Pixmap:
                sample.setValue(QPixmap(2, 2));
                break;

            case QVariant::String:
                sample.setValue(QUuid::createUuid().toString());
                break;

            case QVariant::StringList:
                sample.setValue(QStringList(QUuid::createUuid().toString()) <<
                                QLatin1String("1") << QLatin1String("2") <<
                                QLatin1String("3"));
                break;

            default:
                QFAIL(qPrintable(QString("unsupported variant type: %1").
                                 arg(QVariant::typeToName(type))));
            }

            mValues.append(sample);
        } else {
            mValues.append(allowedValues);

            switch(type) {
            case QVariant::StringList:
                for(int i = 2; i <= allowedValues.count(); ++i) {
                    QStringList stringList;

                    for(int j = 0; j < i; ++j)
                        stringList.append(allowedValues[j].toString());

                    mValues.append(QVariant(stringList));
                }

                break;

            default:
                break;
            }
        }
    }

    void nextSample(QVariant &result)
    {
        currentSample(result);

        if (++mIndex == mValues.count()) {
            mComplete = true;
            mIndex = 0;
        }
    }

    void currentSample(QVariant &result)
    {
        result.setValue(mValues[mIndex]);

        if (result.isValid()) {
            QVERIFY2(result.convert(mType), QVariant::typeToName(mType));
        }
    }

    bool isComplete() const
    {
        return mComplete;
    }

private:
    QVariant::Type mType;
    QVariantList mValues;
    bool mComplete;
    int mIndex;
};

void ut_qtcontacts_trackerplugin_definitions::createTestContacts(const QString &definitionName, QList<QContact> &result)
{
    const QContactDetailDefinition &definition
            (mContactManager->detailDefinition(definitionName));

    typedef QMap<QString, QContactDetailFieldDefinition> FieldMap;
    const FieldMap fields(definition.fields());

    typedef QHash<QString, SampleGenerator> GeneratorMap;
    GeneratorMap generators;

    for(FieldMap::const_iterator i(fields.begin()), e(fields.end()); i != e; ++i) {
        const QContactDetailFieldDefinition &field(i.value());
        generators.insert(i.key(), SampleGenerator(field.dataType(), field.allowableValues()));
        CHECK_CURRENT_TEST_FAILED;
    }

    QList<QContactDetail> detailList;

    for(GeneratorMap::iterator i(generators.begin()), e(generators.end()); i != e; ++i) {
        QVariant sample;
        SampleGenerator &generator(i.value());
        generator.currentSample(sample);
        CHECK_CURRENT_TEST_FAILED;

        QContactDetail detail(definition.name());
        detail.setValue(i.key(), sample);
        detailList.append(detail);
    }

    forever {
        QContactDetail detail(definition.name());
        bool samplesComplete = true;

        for(GeneratorMap::iterator i(generators.begin()), e(generators.end()); i != e; ++i) {
            QVariant sample;
            SampleGenerator &generator(i.value());
            generator.nextSample(sample);
            CHECK_CURRENT_TEST_FAILED;

            if (not generator.isComplete())
                samplesComplete = false;

            if (sample.isValid())
                detail.setValue(i.key(), sample);
        }

        detailList.append(detail);

        if (samplesComplete)
            break;
    }

    foreach(QContactDetail detail, detailList) {
        QContact contact;
        QVERIFY(contact.saveDetail(&detail));
        result.append(contact);
    }
}

static bool localIdOrder(const QContact &a, const QContact &b)
{
    return a.localId() < b.localId();
}

static bool isEmpty(const QVariant &value)
{
    const QVariant empty(value.type());
    return value == empty;
}

static const QStringSet newBrokenDetails()
{
    return (QStringSet() <<
            QLatin1String("Anniversary") <<         // FIXME: doesn't load at all
            QLatin1String("Avatar") <<              // FIXME: doesn't load at all
            QLatin1String("Birthday") <<            // FIXME: doesn't load at all
            QLatin1String("EmailAddress") <<        // FIXME: doesn't load for work context
            QLatin1String("Gender") <<              // FIXME: doesn't load at all
            QLatin1String("GeoLocation") <<         // FIXME: doesn't load at all
            QLatin1String("Guid") <<                // FIXME: doesn't load at all
            QLatin1String("Note") <<                // FIXME: doesn't load at all
            QLatin1String("OnlineAccount") <<       // FIXME: doesn't load at all
            QLatin1String("Organization") <<        // FIXME: doesn't load at all
            QLatin1String("StreetAddress") <<       // FIXME: doesn't load at all
            QLatin1String("SyncTarget") <<          // FIXME: doesn't load at all
            QLatin1String("Timestamp") <<           // FIXME: doesn't load at all
            QLatin1String("Url"));                  // FIXME: doesn't load at all
}

static const QHash<QString, QStringSet> newBrokenFields()
{
    QHash<QString, QStringSet> brokenFields;

    brokenFields.insert(QLatin1String("EmailAddress"),
                        QStringSet() << QLatin1String("Context"));

    brokenFields.insert(QLatin1String("Name"),
                        QStringSet() << QLatin1String("Context")        // FIXME: meaning of this field?
                                     << QLatin1String("CustomLabel")    // FIXME: meaning of this field?
                                     << QLatin1String("Suffix"));

    brokenFields.insert(QLatin1String("Nickname"),
                        QStringSet() << QLatin1String("Context"));      // FIXME: meaning of this field?

    brokenFields.insert(QLatin1String("PhoneNumber"),
                        QStringSet() << QLatin1String("Context")
                                     << QLatin1String("PhoneNumber")
                                     << QLatin1String("SubTypes"));

    return brokenFields;
}

static const QStringSet newSynthesizedDetails()
{
    return (QStringSet() <<
            QLatin1String("DisplayLabel") <<
            QLatin1String("Type"));
}

static const QStringSet brokenDetails(newBrokenDetails());
static const QHash<QString, QStringSet> brokenFields(newBrokenFields());
static const QStringSet synthesizedDetails(newSynthesizedDetails());

void ut_qtcontacts_trackerplugin_definitions::verifyContacts(const QString &definitionName,
                                         QList<QContact> &savedContacts,
                                         QList<QContact> &fetchedContacts)
{
    qSort(savedContacts.begin(), savedContacts.end(), localIdOrder);
    qSort(fetchedContacts.begin(), fetchedContacts.end(), localIdOrder);

    QCOMPARE(fetchedContacts.count(), savedContacts.count());

    for(int i = 0, l = savedContacts.count(); i < l; ++i) {
        const QList<QContactDetail> &savedDetails(savedContacts[i].details(definitionName));
        const QList<QContactDetail> &fetchedDetails(fetchedContacts[i].details(definitionName));

        QCOMPARE(savedDetails.count(), 1);
        QCOMPARE(fetchedDetails.count(), 1);

        QVariantMap savedValues(savedDetails[0].variantValues());
        QVariantMap fetchedValues(fetchedDetails[0].variantValues());

        QStringSet fieldNames(savedValues.keys().toSet());
        fieldNames.unite(fetchedValues.keys().toSet());

        foreach(const QString &key, fieldNames) {
            if (brokenFields[definitionName].contains(key)) {
                const QString msg(QString::fromLatin1("ignoring broken %1 field").arg(key));
                QEXPECT_FAIL("", qPrintable(msg), Continue);
            }

            if (not savedValues.contains(key)) {
                QVERIFY(isEmpty(fetchedValues[key]));
            } else if (not fetchedValues.contains(key)) {
                QVERIFY(isEmpty(savedValues[key]));
            } else {
                QCOMPARE(savedValues[key], fetchedValues[key]);
            }
        }
    }
}

void ut_qtcontacts_trackerplugin_definitions::saveAndFetchOne(const QString &definitionName)
{
    qDebug() << "===============================================================================";
    qDebug() << qPrintable(definitionName);
    qDebug() << "===============================================================================";

    if (brokenDetails.contains(definitionName)) {
        QSKIP("support for this detail is broken right now", SkipAll);
    }

    if (synthesizedDetails.contains(definitionName)) {
        QSKIP("synthesized details are out of scope for now", SkipAll);
    }

    QList<QContact> savedContacts;
    QList<QContactLocalId> savedContactIds;

    createTestContacts(definitionName, savedContacts);
    CHECK_CURRENT_TEST_FAILED;

    saveContacts(savedContacts);
    CHECK_CURRENT_TEST_FAILED;

    foreach(const QContact &contact, savedContacts) {
        savedContactIds.append(contact.localId());
    }

    QCOMPARE(savedContactIds.toSet().count(), savedContacts.count());

    QList<QContact> fetchedContacts;

    foreach(const QContactLocalId &id, savedContactIds) {
        QContact contact;
        fetchContact(id, contact);
        CHECK_CURRENT_TEST_FAILED;
        fetchedContacts.append(contact);
    }

    verifyContacts(definitionName, savedContacts, fetchedContacts);
}

void ut_qtcontacts_trackerplugin_definitions::saveAndFetchMany(const QString &definitionName)
{
    qDebug() << "===============================================================================";
    qDebug() << qPrintable(definitionName);
    qDebug() << "===============================================================================";

    if (brokenDetails.contains(definitionName)) {
        QSKIP("support for this detail is broken right now", SkipAll);
    }

    if (synthesizedDetails.contains(definitionName)) {
        QSKIP("synthesized details are out of scope for now", SkipAll);
    }

    QList<QContact> savedContacts;
    QList<QContactLocalId> savedContactIds;

    createTestContacts(definitionName, savedContacts);
    CHECK_CURRENT_TEST_FAILED;

    saveContacts(savedContacts);
    CHECK_CURRENT_TEST_FAILED;

    foreach(const QContact &contact, savedContacts) {
        savedContactIds.append(contact.localId());
    }

    QCOMPARE(savedContactIds.toSet().count(), savedContacts.count());

    QList<QContact> fetchedContacts;
    fetchContacts(savedContactIds, fetchedContacts);
    CHECK_CURRENT_TEST_FAILED;

    verifyContacts(definitionName, savedContacts, fetchedContacts);
}

void ut_qtcontacts_trackerplugin_definitions::saveAndFetchAll(const QString &definitionName)
{
    qDebug() << "===============================================================================";
    qDebug() << qPrintable(definitionName);
    qDebug() << "===============================================================================";

    if (brokenDetails.contains(definitionName)) {
        QSKIP("support for this detail is broken right now", SkipAll);
    }

    if (synthesizedDetails.contains(definitionName)) {
        QSKIP("synthesized details are out of scope for now", SkipAll);
    }

    QList<QContact> savedContacts;
    QSet<QContactLocalId> savedContactIds;

    createTestContacts(definitionName, savedContacts);
    CHECK_CURRENT_TEST_FAILED;

    saveContacts(savedContacts);
    CHECK_CURRENT_TEST_FAILED;

    foreach(const QContact &contact, savedContacts) {
        savedContactIds.insert(contact.localId());
    }

    QCOMPARE(savedContactIds.count(), savedContacts.count());

    QList<QContact> fetchedContacts;
    fetchContacts(QContactInvalidFilter(), fetchedContacts);
    CHECK_CURRENT_TEST_FAILED;

    QList<QContact>::iterator i = fetchedContacts.begin();

    while (i != fetchedContacts.end()) {
        if (not savedContactIds.contains(i->localId())) {
            i = fetchedContacts.erase(i);
        } else {
            ++i;
        }
    }

    verifyContacts(definitionName, savedContacts, fetchedContacts);
}

#define IMPLEMENT_SAVE_AND_FETCH(DefinitionName)                \
                                                                \
void ut_qtcontacts_trackerplugin_definitions::saveAndFetchOne##DefinitionName()     \
{                                                               \
    saveAndFetchOne(QString::fromLatin1(#DefinitionName));      \
    CHECK_CURRENT_TEST_FAILED;                                  \
}                                                               \
                                                                \
void ut_qtcontacts_trackerplugin_definitions::saveAndFetchMany##DefinitionName()    \
{                                                               \
    saveAndFetchMany(QString::fromLatin1(#DefinitionName));     \
    CHECK_CURRENT_TEST_FAILED;                                  \
}                                                               \
                                                                \
void ut_qtcontacts_trackerplugin_definitions::saveAndFetchAll##DefinitionName()     \
{                                                               \
    saveAndFetchAll(QString::fromLatin1(#DefinitionName));      \
    CHECK_CURRENT_TEST_FAILED;                                  \
}

IMPLEMENT_SAVE_AND_FETCH(Anniversary)
IMPLEMENT_SAVE_AND_FETCH(Avatar)
IMPLEMENT_SAVE_AND_FETCH(Birthday)
IMPLEMENT_SAVE_AND_FETCH(DisplayLabel)
IMPLEMENT_SAVE_AND_FETCH(EmailAddress)
IMPLEMENT_SAVE_AND_FETCH(Gender)
IMPLEMENT_SAVE_AND_FETCH(GeoLocation)
IMPLEMENT_SAVE_AND_FETCH(Guid)
IMPLEMENT_SAVE_AND_FETCH(Name)
IMPLEMENT_SAVE_AND_FETCH(Nickname)
IMPLEMENT_SAVE_AND_FETCH(Note)
IMPLEMENT_SAVE_AND_FETCH(OnlineAccount)
IMPLEMENT_SAVE_AND_FETCH(Organization)
IMPLEMENT_SAVE_AND_FETCH(PhoneNumber)
IMPLEMENT_SAVE_AND_FETCH(StreetAddress)
IMPLEMENT_SAVE_AND_FETCH(SyncTarget)
IMPLEMENT_SAVE_AND_FETCH(Timestamp)
IMPLEMENT_SAVE_AND_FETCH(Type)
IMPLEMENT_SAVE_AND_FETCH(Url)

QTEST_MAIN(ut_qtcontacts_trackerplugin_definitions)
