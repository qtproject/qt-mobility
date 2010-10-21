/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in 
** accordance with the Qt Commercial License Agreement provided with
** the Software or, alternatively, in accordance with the terms
** contained in a written agreement between you and Nokia.
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/versit

#include "qversitdefs_p.h"
#include "tst_qversit.h"
#include "qversitwriter.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include "qversitcontactexporter.h"
#include "qversitcontactimporter.h"
#include "qversitcontacthandler.h"
#include "qversitorganizerexporter.h"
#include "qversitorganizerimporter.h"
#include "qtorganizer.h"
#include "qtcontacts.h"

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
Q_DECLARE_METATYPE(QList<QOrganizerItem>)
Q_DECLARE_METATYPE(QOrganizerItem)

class tst_QVersit : public QObject
{
    Q_OBJECT

private slots: // Tests
    void testImportVCardFiles();
    void testImportVCardFiles_data();
    void testExportImportVCard();
    void testExportImportVCard_data();
    void testImportICalFiles();
    void testImportICalFiles_data();

private:
};

void tst_QVersit::testImportVCardFiles()
{
    QFETCH(QString, filename);
    QFETCH(QByteArray, charset);
    QFETCH(QList<QContact>, expectedContacts);

    filename = QLatin1String(TESTDATA_DIR "testdata_vcf/") + filename;
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
    QVERIFY(!documents.isEmpty());
    QCOMPARE(reader.error(), QVersitReader::NoError);
    QVersitContactImporter importer;
    MyQVersitResourceHandler resourceHandler;
    importer.setResourceHandler(&resourceHandler);
    QVERIFY(importer.importDocuments(documents));
    QList<QContact> contacts = importer.contacts();
    QVERIFY(!contacts.isEmpty());

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

void tst_QVersit::testImportVCardFiles_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("charset");
    QTest::addColumn<QList<QContact> >("expectedContacts");

    QTest::newRow("AAB4-MultipleAll.vcf") << QString::fromAscii("AAB4-MultipleAll.vcf")
        << QByteArray("UTF-16BE") << QList<QContact>();
    QTest::newRow("AAB4-MultipleAscii.vcf") << QString::fromAscii("AAB4-MultipleAscii.vcf")
        << QByteArray("") << QList<QContact>();
    QTest::newRow("AAB4-SingleCompany.vcf") << QString::fromAscii("AAB4-SingleCompany.vcf")
        << QByteArray("") << QList<QContact>();
    QTest::newRow("AAB4-SingleExtensive.vcf") << QString::fromAscii("AAB4-SingleExtensive.vcf")
        << QByteArray("") << QList<QContact>();
    QTest::newRow("AAB4-SingleNonAscii.vcf") << QString::fromAscii("AAB4-SingleNonAscii.vcf")
        << QByteArray("UTF-16BE") << QList<QContact>();
    QTest::newRow("AAB4-SingleNonAsciiWithPhoto.vcf") << QString::fromAscii("AAB4-SingleNonAsciiWithPhoto.vcf")
        << QByteArray("UTF-16BE") << QList<QContact>();
    QTest::newRow("AAB5-SingleNonAscii.vcf") << QString::fromAscii("AAB5-SingleNonAscii.vcf")
        << QByteArray("UTF-8") << QList<QContact>();

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
        QTest::newRow("Entourage11-basic.vcf") << QString::fromAscii("Entourage11-basic.vcf")
            << QByteArray("UTF-16BE") << list;
    }

    QTest::newRow("Entourage11-image.vcf") << QString::fromAscii("Entourage11-image.vcf")
        << QByteArray("UTF-16BE") << QList<QContact>();

    QTest::newRow("Entourage11-nonascii.vcf") << QString::fromAscii("Entourage11-nonascii.vcf")
        << QByteArray("UTF-16BE") << QList<QContact>();

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
        QTest::newRow("Entourage12-basic.vcf") << QString::fromAscii("Entourage12-basic.vcf")
            << QByteArray("") << list;
    }

    QTest::newRow("Entourage12-kevin.vcf") << QString::fromAscii("Entourage12-kevin.vcf")
        << QByteArray("UTF-8") << QList<QContact>();
    QTest::newRow("Entourage12-nonascii.vcf") << QString::fromAscii("Entourage12-nonascii.vcf")
        << QByteArray("UTF-8") << QList<QContact>();
    QTest::newRow("gmail.vcf") << QString::fromAscii("gmail.vcf")
        << QByteArray("UTF-8") << QList<QContact>();

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
        QTest::newRow("test1.vcf") << QString::fromAscii("test1.vcf")
            << QByteArray("UTF-8") << (QList<QContact>() << contact);
    }

    // A file with bad wrapping (no preceding space on a line continuation)
    QTest::newRow("badwrap.vcf") << QString::fromAscii("badwrap.vcf")
        << QByteArray("UTF-8") << QList<QContact>();
}

void tst_QVersit::testExportImportVCard()
{
    // Test that using the backup profile, a contact, when exported and imported again, is unaltered.
    QFETCH(QContact, contact);

    QVersitContactExporter exporter(QVersitContactHandlerFactory::ProfileBackup);
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

    QVersitContactImporter importer(QVersitContactHandlerFactory::ProfileBackup);
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

void tst_QVersit::testExportImportVCard_data()
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
    QContact contact;
    QContactName name;
    name.setCustomLabel(QLatin1String("name"));
    contact.saveDetail(&name);
    QContactManagerEngine::setContactDisplayLabel(&contact, QLatin1String("name"));
    QContactDetail customDetail("CustomDetail");
    customDetail.setValue(QLatin1String("CustomField"), QByteArray("blob"));
    contact.saveDetail(&customDetail);
    QTest::newRow("binary field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), QDate(2010, 5, 18));
    contact.saveDetail(&customDetail);
    QTest::newRow("date field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), QTime(11, 25));
    contact.saveDetail(&customDetail);
    QTest::newRow("time field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), QDateTime(QDate(2010, 5, 18), QTime(11, 25)));
    contact.saveDetail(&customDetail);
    QTest::newRow("datetime field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), (int)42);
    contact.saveDetail(&customDetail);
    QTest::newRow("integer field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), UINT_MAX);
    contact.saveDetail(&customDetail);
    QTest::newRow("unsigned integer field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), QUrl(QLatin1String("http://www.nokia.com/")));
    contact.saveDetail(&customDetail);
    QTest::newRow("url field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), (bool)true);
    contact.saveDetail(&customDetail);
    QTest::newRow("bool field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), (bool)false);
    contact.saveDetail(&customDetail);
    QTest::newRow("false bool field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), (double)3.14159265);
    contact.saveDetail(&customDetail);
    QTest::newRow("double field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), (float)3.14159265);
    contact.saveDetail(&customDetail);
    QTest::newRow("float field") << contact;

    customDetail.setValue(QLatin1String("CustomField"), RED);
    contact.saveDetail(&customDetail);
    QTest::newRow("enum field") << contact;
}

void tst_QVersit::testImportICalFiles()
{
    QFETCH(QString, filename);
    QFETCH(QByteArray, charset);
    QFETCH(QList<QOrganizerItem>, expectedItems);

    filename = QLatin1String(TESTDATA_DIR "testdata_ics/") + filename;
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
    QCOMPARE(documents.size(), 1);
    QVersitOrganizerImporter importer;
    QVERIFY(importer.importDocument(documents.first()));
    QList<QOrganizerItem> items = importer.items();

    if (expectedItems.size() > 0) {
        QCOMPARE(items.size(), expectedItems.size());
        QListIterator<QOrganizerItem> i(expectedItems);
        foreach (QOrganizerItem parsed, items) {
            QOrganizerItem expected = i.next();
            QList<QOrganizerItemDetail> expectedDetails = expected.details();
            foreach(QOrganizerItemDetail expectedDetail, expectedDetails) {
                QString name = expectedDetail.definitionName();
                QOrganizerItemDetail parsedDetail = parsed.detail(name);
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

void tst_QVersit::testImportICalFiles_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("charset");
    QTest::addColumn<QList<QOrganizerItem> >("expectedItems");

    QTest::newRow("2010-FIFA-WorldCup.ics") << QString::fromAscii("2010-FIFA-WorldCup.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("2010-India-Holidays.ics") << QString::fromAscii("2010-India-Holidays.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("2010-US-Holidays.ics") << QString::fromAscii("2010-US-Holidays.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("Asiacup2010.ics") << QString::fromAscii("Asiacup2010.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("FinlandHolidays.ics") << QString::fromAscii("FinlandHolidays.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("india2010.ics") << QString::fromAscii("india2010.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("IndiaHolidays.ics") << QString::fromAscii("IndiaHolidays.ics")
        << QByteArray("") << QList<QOrganizerItem>();
    QTest::newRow("worldt2009.ics") << QString::fromAscii("worldt2009.ics")
        << QByteArray("") << QList<QOrganizerItem>();
}

QTEST_MAIN(tst_QVersit)

#include "tst_qversit.moc"
