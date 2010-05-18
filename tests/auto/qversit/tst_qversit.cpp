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

#include "qversitdefs_p.h"
#include "tst_qversit.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include "qversitcontactexporter.h"
#include "qversitcontactimporter.h"
#include "qcontact.h"
#include "qcontactmanager.h"
#include "qcontactmanagerengine.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDir>
#include <QList>

QTM_BEGIN_NAMESPACE
class MyQVersitResourceHandler : public QVersitResourceHandler
{
public:
    MyQVersitResourceHandler() : mIndex(0)
    {
    }

    bool saveResource(const QByteArray& contents, const QVersitProperty& property,
                      QString* location)
    {
        Q_UNUSED(property)
        *location = QString::number(mIndex++);
        mObjects.insert(*location, contents);
        return true;
    }

    bool loadResource(const QString &location, QByteArray *contents, QString *mimeType)
    {
        Q_UNUSED(location)
        Q_UNUSED(contents)
        Q_UNUSED(mimeType)
        return false;
    }

    int mIndex;
    QMap<QString, QByteArray> mObjects;
};


class MyDetailHandler : public QVersitContactExporterDetailHandler {
public:
    MyDetailHandler() : detailNumber(0) {}
    bool preProcessDetail(const QContact& contact, const QContactDetail& detail,
                          QVersitDocument* document) {
        Q_UNUSED(contact) Q_UNUSED(detail) Q_UNUSED(document)
        return false;
    }
    /* eg. a detail with definition name "Detail" and fields "Field1"="Value1" and
     * "Field2"="Value2" will be exported to the vCard properties:
     * G0.DETAIL-FIELD1:Value1
     * G0.DETAIL-FIELD2:Value2
     * And the next detail (say, "Detail" with a field "Field1"="Value3" will generate:
     * G1.DETAIL-FIELD1:Value3
     * ie. Different details will have different vCard groups.
     */
    bool postProcessDetail(const QContact& contact, const QContactDetail& detail,
                           bool alreadyProcessed, QVersitDocument* document) {
        Q_UNUSED(contact)
        // beware: if the base implementation exports some but not all fields, alreadyProcessed
        // will be true and the unprocessed fields won't be exported
        if (alreadyProcessed)
            return false;
        if (detail.definitionName() == QContactType::DefinitionName)
            return false; // special case of an unhandled detail that we don't export
        QVersitProperty property;
        QVariantMap fields = detail.variantValues();
        // fields from the same detail have the same group so the importer can collate them
        QString detailGroup = QLatin1String("G") + QString::number(detailNumber++);
        for (QVariantMap::const_iterator it = fields.constBegin();
                it != fields.constEnd();
                it++) {
            property.setGroups(QStringList(detailGroup));
            // beware: detail.definitionName and the field name will be made uppercase on export
            property.setName(QLatin1String("X-QCONTACTDETAIL-")
                             + detail.definitionName()
                             + QLatin1String("-")
                             + it.key());
            // beware: this might not handle nonstring values properly:
            property.setValue(it.value());
            document->addProperty(property);
        }
        return true;
    }
private:
    int detailNumber;
};

class MyPropertyHandler : public QVersitContactImporterPropertyHandler {
public:
    bool preProcessProperty(const QVersitDocument& document, const QVersitProperty& property,
                            int contactIndex, QContact* contact) {
        Q_UNUSED(document) Q_UNUSED(property) Q_UNUSED(contactIndex) Q_UNUSED(contact)
        return false;
    }
    /* eg. if the document has the properties:
     * G0.DETAIL-FIELD1:Value1
     * G0.DETAIL-FIELD2:Value2
     * G1.DETAIL-FIELD1:Value3
     * This will generate two details - the first with fields "FIELD1"="Value1" and
     * "FIELD2"="Value2" and the second with "FIELD1"="Value3"
     * ie. the vCard groups determine which properties form a single detail.
     */
    bool postProcessProperty(const QVersitDocument& document, const QVersitProperty& property,
                             bool alreadyProcessed, int contactIndex, QContact* contact) {
        Q_UNUSED(document) Q_UNUSED(contactIndex)
        const QString prefix = QLatin1String("X-QCONTACTDETAIL-");
        if (alreadyProcessed)
            return false;
        if (!property.name().startsWith(prefix))
            return false;
        QString detailAndField = property.name().mid(prefix.size());
        QStringList detailAndFieldParts = detailAndField.split(QLatin1Char('-'),
                                                               QString::SkipEmptyParts);
        if (detailAndFieldParts.size() != 2)
            return false;
        QString definitionName = detailAndFieldParts.at(0);
        QString fieldName = detailAndFieldParts.at(1);
        if (property.groups().size() != 1)
            return false;
        QString group = property.groups().first();
        // find a detail generated from the a property with the same group
        QContactDetail detail = handledDetails.value(group);
        // make sure the the existing detail has the same definition name
        if (detail.definitionName() != definitionName)
            detail = QContactDetail(definitionName);
        detail.setValue(fieldName, property.value());
        contact->saveDetail(&detail);
        handledDetails.insert(group, detail);
        return false;
    }
    QMap<QString, QContactDetail> handledDetails; // map from group name to detail
};

QTM_END_NAMESPACE

QTM_USE_NAMESPACE

#ifndef TESTDATA_DIR
#define TESTDATA_DIR "./"
#endif

Q_DECLARE_METATYPE(QList<QContact>)
Q_DECLARE_METATYPE(QContact)

void tst_QVersit::testImportFiles()
{
    QFETCH(QString, filename);
    QFETCH(QByteArray, charset);
    QFETCH(QList<QContact>, expectedContacts);

    QVersitReader reader;
    QFile file(filename);
    QVERIFY2(file.open(QIODevice::ReadOnly), filename.toAscii());
    reader.setDevice(&file);
    if (charset != "") {
        reader.setDefaultCodec(QTextCodec::codecForName(charset));
    }
    QVERIFY(reader.startReading());
    QVERIFY(reader.waitForFinished());
    QList<QVersitDocument> documents = reader.results();
    QCOMPARE(reader.error(), QVersitReader::NoError);
    QVersitContactImporter importer;
    MyQVersitResourceHandler resourceHandler;
    importer.setResourceHandler(&resourceHandler);
    MyPropertyHandler propertyHandler;
    importer.setPropertyHandler(&propertyHandler);
    QVERIFY(importer.importDocuments(documents));
    QList<QContact> contacts = importer.contacts();

    if (expectedContacts.size() > 0) {
        QCOMPARE(contacts.size(), expectedContacts.size());
        QListIterator<QContact> i(expectedContacts);
        foreach (QContact parsed, contacts) {
            QContact expected = i.next();
            QList<QContactDetail> expectedDetails = expected.details();
            foreach(QContactDetail expectedDetail, expectedDetails) {
                QString name = expectedDetail.definitionName();
                QContactDetail parsedDetail = parsed.detail(name);
                if (parsedDetail != expectedDetail) {
                    qDebug() << "Detail: " << name.toAscii();
                    qDebug() << "Actual:" << parsedDetail.variantValues();
                    qDebug() << "Expected:" << expectedDetail.variantValues();
                    QCOMPARE(parsedDetail, expectedDetail);
                }
            }
        }
    }
}

#define QTEST_NEW_ROW(filename,charset,contact) \
        QTest::newRow(filename) \
        << QString::fromAscii(TESTDATA_DIR "testdata/") + QString::fromAscii(filename) \
        << QByteArray(charset) \
        << (contact)

void tst_QVersit::testImportFiles_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("charset");
    QTest::addColumn<QList<QContact> >("expectedContacts");

    QTEST_NEW_ROW("AAB4-MultipleAll.vcf", "UTF-16BE", QList<QContact>());
    QTEST_NEW_ROW("AAB4-MultipleAscii.vcf", "", QList<QContact>());
    QTEST_NEW_ROW("AAB4-SingleCompany.vcf", "", QList<QContact>());
    QTEST_NEW_ROW("AAB4-SingleExtensive.vcf", "", QList<QContact>());
    QTEST_NEW_ROW("AAB4-SingleNonAscii.vcf", "UTF-16BE", QList<QContact>());
    QTEST_NEW_ROW("AAB4-SingleNonAsciiWithPhoto.vcf", "UTF-16BE", QList<QContact>());
    QTEST_NEW_ROW("AAB5-SingleNonAscii.vcf", "UTF-8", QList<QContact>());

    {
        QList<QContact> list;
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("Firstname Lastname"));
        name.setFirstName(QLatin1String("Firstname"));
        name.setLastName(QLatin1String("Lastname"));
        name.setPrefix(QLatin1String("Title"));
        name.setSuffix(QLatin1String("Suffix"));
        contact.saveDetail(&name);
        QContactOrganization org;
        org.setName(QLatin1String("Company Name"));
        org.setDepartment(QStringList(QLatin1String("Department Name")));
        org.setTitle(QLatin1String("Job title"));
        contact.saveDetail(&org);
        QContactNote note;
        note.setNote(QLatin1String("This is a note field.  Pretty boring."));
        contact.saveDetail(&note);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("Firstname Lastname"));
        list.append(contact);
        QContactUrl homeUrl;
        homeUrl.setUrl(QLatin1String("http://mywebpage.com"));
        homeUrl.setContexts(QContactDetail::ContextHome);
        contact.saveDetail(&homeUrl);
        QContactUrl workUrl;
        workUrl.setUrl(QLatin1String("http://workwebpage"));
        workUrl.setContexts(QContactDetail::ContextWork);
        contact.saveDetail(&workUrl);
        QTEST_NEW_ROW("Entourage11-basic.vcf", "UTF-16BE", list);
    }

    QTEST_NEW_ROW("Entourage11-image.vcf", "UTF-16BE", QList<QContact>());

    QTEST_NEW_ROW("Entourage11-nonascii.vcf", "UTF-16BE", QList<QContact>());

    {
        QList<QContact> list;
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("first last"));
        name.setFirstName(QLatin1String("first"));
        name.setLastName(QLatin1String("last"));
        contact.saveDetail(&name);
        QContactOrganization org;
        org.setName(QLatin1String("Nokia"));
        org.setDepartment(QStringList(QLatin1String("Qt DF")));
        contact.saveDetail(&org);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("first last"));
        list.append(contact);
        QTEST_NEW_ROW("Entourage12-basic.vcf", "", list);
    }

    QTEST_NEW_ROW("Entourage12-kevin.vcf", "UTF-8", QList<QContact>());
    QTEST_NEW_ROW("Entourage12-nonascii.vcf", "UTF-8", QList<QContact>());
    QTEST_NEW_ROW("gmail.vcf", "UTF-8", QList<QContact>());

    {
        QContact contact;
        QContactName name;
        name.setFirstName(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactFamily family;
        family.setChildren(QStringList(QLatin1String("Child1")));
        contact.saveDetail(&family);
        family.setChildren(QStringList(QLatin1String("Child2")) << QLatin1String("Child3"));
        contact.saveDetail(&family);
        QContactNickname nickname;
        nickname.setNickname(QLatin1String("Nick6"));
        contact.saveDetail(&nickname);
        nickname.setNickname(QLatin1String("Nick5"));
        contact.saveDetail(&nickname);
        nickname.setNickname(QLatin1String("Nick4"));
        contact.saveDetail(&nickname);
        nickname.setNickname(QLatin1String("Nick3"));
        contact.saveDetail(&nickname);
        nickname.setNickname(QLatin1String("Nick2"));
        contact.saveDetail(&nickname);
        nickname.setNickname(QLatin1String("Nick1"));
        contact.saveDetail(&nickname);
        QContactPhoneNumber assistantphone;
        assistantphone.setNumber(QLatin1String("1234"));
        assistantphone.setSubTypes(QContactPhoneNumber::SubTypeAssistant);
        contact.saveDetail(&assistantphone);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTEST_NEW_ROW("test1.vcf", "UTF-8", QList<QContact>() << contact);
    }
}

void tst_QVersit::testExportImport()
{
    // Test that a contact, when exported, then imported again, is unaltered
    QFETCH(QContact, contact);

    QVersitContactExporter exporter;
    MyDetailHandler detailHandler;
    exporter.setDetailHandler(&detailHandler);
    QVERIFY(exporter.exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QList<QVersitDocument> documents = exporter.documents();
    QCOMPARE(documents.size(), 1);

    QVersitContactImporter importer;
    MyPropertyHandler propertyHandler;
    importer.setPropertyHandler(&propertyHandler);
    QVERIFY(importer.importDocuments(documents));
    QList<QContact> contacts = importer.contacts();
    QCOMPARE(contacts.size(), 1);
    // We can't do a deep compare because detail ids are different
    QCOMPARE(contacts.first().details().count(), contact.details().count());
}

void tst_QVersit::testExportImport_data()
{
    QTest::addColumn<QContact>("contact");

    QContact contact;
    QContactName name;
    name.setFirstName(QLatin1String("first"));
    name.setLastName(QLatin1String("last"));
    name.setCustomLabel(QLatin1String("custom"));
    contact.saveDetail(&name);
    // detail definition/field names are encoded as vCard property names, which must be uppercase,
    // so only uppercase definition/field names work.
    QContactDetail customDetail1("CUSTOMDETAIL");
    customDetail1.setValue(QLatin1String("CUSTOMFIELD11"), QLatin1String("Value11"));
    customDetail1.setValue(QLatin1String("CUSTOMFIELD12"), QLatin1String("Value12"));
    contact.saveDetail(&customDetail1);
    QContactDetail customDetail2("CUSTOMDETAIL");
    customDetail2.setValue(QLatin1String("CUSTOMFIELD21"), QLatin1String("Value21"));
    customDetail2.setValue(QLatin1String("CUSTOMFIELD22"), QLatin1String("Value22"));
    contact.saveDetail(&customDetail2);
    contact.setType(QContactType::TypeContact);

    QTest::newRow("custom detail") << contact;
}

QTEST_MAIN(tst_QVersit)
