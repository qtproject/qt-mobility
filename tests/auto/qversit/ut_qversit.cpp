#include "qversitdefs.h"
#include "ut_qversit.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include "qversitcontactimporter.h"
#include "qcontact.h"
#include "qcontactmanager.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDir>
#include <QList>

QTM_BEGIN_NAMESPACE
class MyQVersitFileSaver : public QVersitFileSaver
{
public:
    MyQVersitFileSaver() : mIndex(0)
    {
    }

    bool saveFile(const QByteArray& contents, const QVersitProperty& property, QString* filename)
    {
        Q_UNUSED(property);
        *filename = QString::number(mIndex++);
        mObjects.insert(*filename, contents);
        return true;
    }

    int mIndex;
    QMap<QString, QByteArray> mObjects;
};
QTM_END_NAMESPACE

QTM_USE_NAMESPACE

#ifndef TESTDATA_DIR
#define TESTDATA_DIR "./"
#endif

Q_DECLARE_METATYPE(QList<QContact>);

void UT_QVersit::testImportFiles()
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
    QVERIFY(reader.readAll());
    QList<QVersitDocument> documents = reader.results();
    QVersitContactImporter importer;
    MyQVersitFileSaver fileSaver;
    importer.setFileSaver(&fileSaver);
    QList<QContact> contacts = importer.importContacts(documents);

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
                    qDebug() << "Actual:" << parsedDetail.values();
                    qDebug() << "Expected:" << expectedDetail.values();
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
        << (contact);

void UT_QVersit::testImportFiles_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("charset");
    QTest::addColumn<QList<QContact> >("expectedContacts");

    QTEST_NEW_ROW("AAB4/MultipleAll.vcf", "UTF-16BE", QList<QContact>());
    QTEST_NEW_ROW("AAB4/MultipleAscii.vcf", "", QList<QContact>());
    QTEST_NEW_ROW("AAB4/SingleCompany.vcf", "", QList<QContact>());
    QTEST_NEW_ROW("AAB4/SingleExtensive.vcf", "", QList<QContact>());
    QTEST_NEW_ROW("AAB4/SingleNonAscii.vcf", "UTF-16BE", QList<QContact>());
    QTEST_NEW_ROW("AAB4/SingleNonAsciiWithPhoto.vcf", "UTF-16BE", QList<QContact>());
    QTEST_NEW_ROW("AAB5/SingleNonAscii.vcf", "UTF-8", QList<QContact>());

    {
        QList<QContact> list;
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("Firstname Lastname"));
        name.setFirst(QLatin1String("Firstname"));
        name.setLast(QLatin1String("Lastname"));
        name.setMiddle(QString());
        name.setPrefix(QLatin1String("Title"));
        name.setSuffix(QLatin1String("Suffix"));
        contact.saveDetail(&name);
        QContactOrganization org;
        org.setName(QLatin1String("Company Name"));
        org.setDepartment(QStringList(QLatin1String("Department Name")));
        org.setTitle(QLatin1String("Job title"));
        contact.saveDetail(&org);
        list.append(contact);
        QContactNote note;
        note.setNote(QLatin1String("This is a note field.  Pretty boring."));
        contact.saveDetail(&note);
        QContactUrl homeUrl;
        homeUrl.setUrl(QLatin1String("http://mywebpage.com"));
        homeUrl.setContexts(QContactDetail::ContextHome);
        contact.saveDetail(&homeUrl);
        QContactUrl workUrl;
        workUrl.setUrl(QLatin1String("http://workwebpage"));
        workUrl.setContexts(QContactDetail::ContextWork);
        contact.saveDetail(&workUrl);
        QTEST_NEW_ROW("Entourage11/basic.vcf", "UTF-16BE", list);
    }

    QTEST_NEW_ROW("Entourage11/image.vcf", "UTF-16BE", QList<QContact>());

    QTEST_NEW_ROW("Entourage11/nonascii.vcf", "UTF-16BE", QList<QContact>());

    {
        QList<QContact> list;
        QContact contact;
        QContactName name;
        name.setCustomLabel(QLatin1String("first last"));
        name.setFirst(QLatin1String("first"));
        name.setLast(QLatin1String("last"));
        name.setMiddle(QString());
        name.setPrefix(QString());
        name.setSuffix(QString());
        contact.saveDetail(&name);
        QContactOrganization org;
        org.setName(QLatin1String("Nokia"));
        org.setDepartment(QStringList(QLatin1String("Qt DF")));
        contact.saveDetail(&org);
        list.append(contact);
        QTEST_NEW_ROW("Entourage12/basic.vcf", "", list);
    }

    QTEST_NEW_ROW("Entourage12/kevin.vcf", "UTF-8", QList<QContact>());
    QTEST_NEW_ROW("Entourage12/nonascii.vcf", "UTF-8", QList<QContact>());
    QTEST_NEW_ROW("gmail.vcf", "UTF-8", QList<QContact>());
}

QTEST_MAIN(UT_QVersit)
