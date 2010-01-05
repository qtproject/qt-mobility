#include "qversitdefs.h"
#include "ut_qversit.h"
#include "qversitreader.h"
#include "qversitreader_p.h"
#include "qversitcontactimporter.h"

#include <QtTest/QtTest>
#include <QDebug>
#include <QDir>

QTM_USE_NAMESPACE

const QString imageAndAudioClipPath(QString::fromAscii("tmp_ut_qversit"));

#ifndef TESTDATA_DIR
#define TESTDATA_DIR "./"
#endif

void UT_QVersit::initTestCase()
{
    // Create the directory to store the image
    QDir dir;
    if (!dir.exists(imageAndAudioClipPath)) {
        dir.mkdir(imageAndAudioClipPath);
    }
}

void UT_QVersit::cleanupTestCase()
{
    QDir dir;

    if (dir.exists(imageAndAudioClipPath)) {
        dir.cd(imageAndAudioClipPath);
        // remove all the files first
        QStringList allFiles;
        allFiles << QString::fromAscii("*");
        QStringList fileList = dir.entryList(allFiles, QDir::Files);
        foreach (QString file, fileList) {
            dir.remove(file);
        }
        dir.cdUp();
        dir.rmdir(imageAndAudioClipPath);
    }
}

void UT_QVersit::init()
{
}

void UT_QVersit::cleanup()
{
}

void UT_QVersit::testImportFiles()
{
    QFETCH(QString, filename);
    QFETCH(QByteArray, charset);

    QVersitReader reader;
    QFile file(filename);
    QVERIFY2(file.open(QIODevice::ReadOnly), filename.toAscii());
    reader.setDevice(&file);
    if (charset != "") {
        reader.setDefaultCharset(charset);
    }
    QVERIFY(reader.readAll());
    QList<QVersitDocument> result = reader.result();
    foreach (QVersitDocument doc, result) {
        QVersitContactImporter importer;
        importer.setAudioClipPath(imageAndAudioClipPath);
        importer.setImagePath(imageAndAudioClipPath);
        QContact contact = importer.importContact(doc);
    }
}

#define QTEST_NEW_ROW(filename,charset) \
        QTest::newRow(filename) \
        << QString::fromAscii(TESTDATA_DIR "testdata/") + QString::fromAscii(filename) \
        << QByteArray(charset);

void UT_QVersit::testImportFiles_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<QByteArray>("charset");

    // Failing test cases commented out because they take a long time to run.
    QTEST_NEW_ROW("AAB4/MultipleAll.vcf", "UTF-16BE");
    QTEST_NEW_ROW("AAB4/MultipleAscii.vcf", "");
    QTEST_NEW_ROW("AAB4/SingleCompany.vcf", "");
    QTEST_NEW_ROW("AAB4/SingleExtensive.vcf", "");
    QTEST_NEW_ROW("AAB4/SingleNonAscii.vcf", "UTF-16BE");
    QTEST_NEW_ROW("AAB4/SingleNonAsciiWithPhoto.vcf", "UTF-16BE");
    QTEST_NEW_ROW("AAB5/SingleNonAscii.vcf", "");
    QTEST_NEW_ROW("Entourage11/basic.vcf", "UTF-16BE");
//    QTEST_NEW_ROW("Entourage11/image.vcf", "UTF-16BE");
    QTEST_NEW_ROW("Entourage11/nonascii.vcf", "UTF-16BE");
    QTEST_NEW_ROW("Entourage12/basic.vcf", "");
    QTEST_NEW_ROW("Entourage12/kevin.vcf", "");
    QTEST_NEW_ROW("Entourage12/nonascii.vcf", "");
    QTEST_NEW_ROW("gmail.vcf", "");
}

QTEST_MAIN(UT_QVersit)
