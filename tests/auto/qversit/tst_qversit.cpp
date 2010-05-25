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
#include "qversitwriter.h"
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
    // Test that using the backup handler, a contact, when exported and imported again, is unaltered.
    QFETCH(QContact, contact);

    QVersitContactExporter exporter;
    exporter.setDetailHandler(QVersitContactExporterDetailHandlerV2::createBackupHandler());
    QVERIFY(exporter.exportContacts(QList<QContact>() << contact, QVersitDocument::VCard30Type));
    QList<QVersitDocument> documents = exporter.documents();
    QCOMPARE(documents.size(), 1);

    QByteArray documentBytes;
    QVersitWriter writer(&documentBytes);
    writer.startWriting(documents);
    writer.waitForFinished();

    QVersitReader reader(documentBytes);
    reader.startReading();
    reader.waitForFinished();
    QList<QVersitDocument> parsedDocuments = reader.results();
    QCOMPARE(parsedDocuments.size(), 1);

    QVersitContactImporter importer;
    importer.setPropertyHandler(QVersitContactImporterPropertyHandlerV2::createBackupHandler());
    QVERIFY(importer.importDocuments(parsedDocuments));
    QList<QContact> contacts = importer.contacts();
    QCOMPARE(contacts.size(), 1);
    if (contacts.first().details() != contact.details()) {
        qDebug() << "Versit documents:" << documentBytes;
        qDebug() << "Actual:" << contacts.first();
        qDebug() << "Expected:" << contact;
        QCOMPARE(contacts.first().details(), contact.details());
    }
}

enum Color { RED, GREEN, BLUE };

void tst_QVersit::testExportImport_data()
{
    QTest::addColumn<QContact>("contact");

    // The test contacts need at least a name (so the resulting vCard is valid and a display label (because
    // otherwise, the imported display label will be set from the name or something)
    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("just a name") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setFirstName(QLatin1String("first"));
        name.setLastName(QLatin1String("last"));
        name.setCustomLabel(QLatin1String("custom"));
        name.setValue(QLatin1String("RandomField1"), QLatin1String("RandomValue1"));
        name.setValue(QLatin1String("RandomField2"), QLatin1String("RandomValue1"));
        contact.saveDetail(&name);
        QContactDetail customDetail1("CustomDetail");
        customDetail1.setValue(QLatin1String("CustomField11"), QLatin1String("Value11"));
        customDetail1.setValue(QLatin1String("CustomField12"), QLatin1String("Value12"));
        contact.saveDetail(&customDetail1);
        QContactDetail customDetail2("CustomDetail");
        customDetail2.setValue(QLatin1String("CustomField21"), QLatin1String("Value21"));
        contact.saveDetail(&customDetail2);
        contact.setType(QContactType::TypeContact);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("custom"));
        QTest::newRow("custom detail grouping") << contact;
    }

    // Test of some non-string fields
    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), QByteArray("blob"));
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("binary field") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), (int)42);
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("integer field") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), (bool)true);
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("bool field") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), (double)3.14159265);
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("double field") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), (float)3.14159265);
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("float field") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), QDateTime(QDate(2010, 5, 18), QTime(11, 25)));
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("datetime field") << contact;
    }

    {
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("name"));
        contact.saveDetail(&name);
        QContactDetail customDetail("CustomDetail");
        customDetail.setValue(QLatin1String("CustomField"), RED);
        contact.saveDetail(&customDetail);
        QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
        QTest::newRow("enum field") << contact;
    }
}

QTEST_MAIN(tst_QVersit)
