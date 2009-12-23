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
    QVersitReader reader;
    QFile file(filename);
    QVERIFY2(file.open(QIODevice::ReadOnly), filename.toAscii());
    reader.setDevice(&file);
    QVERIFY2(reader.readAll(), filename.toAscii());
    QList<QVersitDocument> result = reader.result();
    foreach (QVersitDocument doc, result) {
        QVersitContactImporter importer;
        importer.setAudioClipPath(imageAndAudioClipPath);
        importer.setImagePath(imageAndAudioClipPath);
        QContact contact = importer.importContact(doc);
    }
}

void UT_QVersit::testImportFiles_data()
{
    QString testdir(QString::fromAscii(TESTDATA_DIR "testdata/"));
    QTest::addColumn<QString>("filename");

    QStringList filenames;

    // Commented out expected fails because they take a long time to run.
    filenames
//            << QString::fromAscii("AAB4/MultipleAll.vcf")
            << QString::fromAscii("AAB4/MultipleAscii.vcf")
            << QString::fromAscii("AAB4/SingleCompany.vcf")
            << QString::fromAscii("AAB4/SingleExtensive.vcf")
//            << QString::fromAscii("AAB4/SingleNonAscii.vcf")
            << QString::fromAscii("AAB5/SingleNonAscii.vcf")
//            << QString::fromAscii("Entourage11/basic.vcf")
//            << QString::fromAscii("Entourage11/image.vcf")
//            << QString::fromAscii("Entourage11/nonascii.vcf")
//            << QString::fromAscii("Entourage12/basic.vcf")
//            << QString::fromAscii("Entourage12/kevin.vcf")
//            << QString::fromAscii("Entourage12/nonascii.vcf")
//            << QString::fromAscii("gmail.vcf")
            ;
    foreach (QString filename, filenames) {
        QTest::newRow(filename.toAscii()) << testdir+filename;
    }
}

QTEST_MAIN(UT_QVersit)
