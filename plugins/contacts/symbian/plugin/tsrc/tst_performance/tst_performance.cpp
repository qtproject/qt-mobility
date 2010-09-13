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
** accordance with the Qt Solutions Commercial License Agreement provided
** with the Software or, alternatively, in accordance with the terms
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
** Please note Third Party Software included with Qt Solutions may impose
** additional restrictions and it is the user's responsibility to ensure
** that they have met the licensing requirements of the GPL, LGPL, or Qt
** Solutions Commercial license and the relevant license of the Third
** Party Software they are using.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QObject>
#include <qmobilityglobal.h>
#include <qtcontacts.h>
#include <QtTest/QtTest>
#include <QDebug>
#include <QFile>
#include <qversitreader.h>
#include <qversitcontactimporter.h>

QTM_USE_NAMESPACE

const QString testDataPath("c:/data/others");
const QString resultFileName("c:/data/results/tst_performance_results.csv");

class tst_Performance : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void init();
    void cleanup();

private slots:  // Test cases
    void validate_data();
    void validate();
    void fetch_data();
    void fetch();

private:
    void addTestRows(QString fileNameFilter);
    bool importContacts(QString vcardFileName, int &count);
    bool detailsExist(QList<QContact> contacts, QStringList definitionNames);
    void startBenchmark(QString benchmarkName);
    bool endBenchmark(const int target);

    QContactManager *m_cm;
    QTime m_timer;
    QString m_benchmarkName;
};


void tst_Performance::init()
{
    m_cm = new QContactManager("symbian");
    m_cm->removeContacts(m_cm->contactIds(), 0);
}

void tst_Performance::cleanup()
{
    m_cm->removeContacts(m_cm->contactIds(), 0);
    delete m_cm;
}

void tst_Performance::validate_data()
{
    // Path to a file containing vcards, constructed dynamically
    QTest::addColumn<QString>("vcardFileName");

   // Add new test row for each vcf file
   addTestRows(QString("tst_performance_validate*.vcf"));
}

void tst_Performance::validate()
{
    QFETCH(QString, vcardFileName);

    // Import contacts from the file into contacts database
    int count(0);
    QVERIFY(importContacts(vcardFileName, count));

    // QContactName
    QContactFetchHint nameHint;
    nameHint.setDetailDefinitionsHint(QStringList() << QContactName::DefinitionName);
    QList<QContact> contacts = m_cm->contacts(QList<QContactSortOrder>(), nameHint);
    QCOMPARE(contacts.count(), count);
    QVERIFY(detailsExist(contacts, QStringList() << QContactName::DefinitionName));

    // QContactPhoneNumber
    QContactFetchHint numberHint;
    numberHint.setDetailDefinitionsHint(QStringList() << QContactPhoneNumber::DefinitionName);
    contacts = m_cm->contacts(QList<QContactSortOrder>(), numberHint);
    QCOMPARE(contacts.count(), count);
    QVERIFY(detailsExist(contacts, QStringList() << QContactPhoneNumber::DefinitionName));

    // QContactAddress
    QContactFetchHint addressHint;
    addressHint.setDetailDefinitionsHint(QStringList() << QContactAddress::DefinitionName);
    contacts = m_cm->contacts(QList<QContactSortOrder>(), addressHint);
    QCOMPARE(contacts.count(), count);
    QVERIFY(detailsExist(contacts, QStringList() << QContactAddress::DefinitionName));
}

void tst_Performance::fetch_data()
{
    // Path to a file containing vcards, constructed dynamically
    QTest::addColumn<QString>("vcardFileName");

    // Add new test row for each vcf file
    addTestRows(QString("tst_performance_fetch*.vcf"));
}

void tst_Performance::fetch()
{
    QFETCH(QString, vcardFileName);

    // Import contacts from the file into contacts database
    int count(0);
    QVERIFY(importContacts(vcardFileName, count));

    // The actual benchmark measurements begin... We cannot use QBENCHMARK
    // since it does not give separate results for each operation, but shows
    // only the latest result in the log. Also, it does not support writing the
    // result into an external CSV file.

    {
        // measure contact id fetch
        QList<QContactLocalId> ids;
        startBenchmark(vcardFileName + QString(" / QContactManager::contactIds"));
        ids = m_cm->contactIds();
        QVERIFY(endBenchmark(0));
        QCOMPARE(ids.count(), count);
    }

    {
        // measure contact fetch
        QList<QContact> contacts;
        startBenchmark(vcardFileName + QString(" / QContactManager::contacts"));
        contacts = m_cm->contacts();
        QVERIFY(endBenchmark(0));
        QCOMPARE(contacts.count(), count);
    }

    {
        // measure contact fetch with fetch hint (only name)
        QContactFetchHint hint;
        hint.setDetailDefinitionsHint(QStringList() << QContactName::DefinitionName);
        startBenchmark(vcardFileName + QString(" / QContactManager::contacts w/ detail definitions hint"));
        QList<QContact> contacts = m_cm->contacts(QList<QContactSortOrder>(), hint);
        QVERIFY(endBenchmark(0));
        QCOMPARE(contacts.count(), count);
    }

    {
        // measure contact fetch without relationships and binary blobs
        QContactFetchHint hint;
        hint.setOptimizationHints(QContactFetchHint::NoRelationships | QContactFetchHint::NoBinaryBlobs);
        startBenchmark(vcardFileName + QString(" / QContactManager::contacts w/ optimization hints"));
        QList<QContact> contacts = m_cm->contacts(QList<QContactSortOrder>(), hint);
        QVERIFY(endBenchmark(0));
        QCOMPARE(contacts.count(), count);
    }

    // TODO: measure sort
    // TODO: measure filter etc...
}

void tst_Performance::addTestRows(QString fileNameFilter)
{
    // add new test row for each vcard file
    QDir vcfDir(testDataPath);
    QStringList tst_dataFiles = vcfDir.entryList(QStringList() << fileNameFilter);
    foreach (QString tst_dataFile, tst_dataFiles) {
        qDebug() << "Found new vcard file: " << tst_dataFile;
        QTest::newRow(tst_dataFile.toAscii()) << tst_dataFile;
    }
}

bool tst_Performance::importContacts(QString vcardFileName, int &contactCount)
{
    contactCount = 0;

    QFile vcardFile(testDataPath + "/" + vcardFileName);
    if (!vcardFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    // Read contacts from vcard file
    while (!vcardFile.atEnd()) {
        QBuffer vcard;
        QByteArray line = vcardFile.readLine();
        if (line.startsWith(QByteArray("BEGIN:VCARD"))) {
            vcard.open(QBuffer::ReadWrite);
            vcard.write(line);
            bool complete(false);
            while (!complete && !vcardFile.atEnd()) {
                line = vcardFile.readLine();
                vcard.write(line);
                if (line.contains(QByteArray("END:VCARD"))) {
                    complete = true;
                }
            }

            // Parse the input into QVersitDocuments
            // Note: we could also use the more convenient QVersitReader(QByteArray) constructor.
            QVersitReader reader(vcard.buffer());
            if (!reader.startReading()) {return false;}
            if (!reader.waitForFinished()) {return false;}

            // Convert the QVersitDocuments to QContacts
            QList<QVersitDocument> inputDocuments = reader.results();
            QVersitContactImporter importer;
            if (!importer.importDocuments(inputDocuments)) {
                return false;
            }

            QList<QContact> contacts = importer.contacts();
            if (!m_cm->saveContacts(&contacts, 0)) {
                return false;
            }
            contactCount++;
        }
    }
    //qDebug() << contactCount << "contacts added in total";
    return true;
}

bool tst_Performance::detailsExist(QList<QContact> contacts, QStringList definitionNames)
{
    foreach(QContact contact, contacts) {
        foreach (QString definitionName, definitionNames) {
            QList<QContactDetail> details = contact.details(definitionName);
            if (!details.count()) {
                qDebug() << "Detail not found:" << definitionName;
                return false;
            }
            foreach (QContactDetail detail, details) {
                contact.removeDetail(&detail);
            }
        }

        // Verify that there were no extra details (this is important from
        // performance point of view; we don't want to waste time by giving
        // details that the client does not need.
        foreach (QContactDetail detail, contact.details()) {
            // Skip "always on" details
            if (detail.definitionName() != QContactDisplayLabel::DefinitionName
                && detail.definitionName() != QContactType::DefinitionName
                && detail.definitionName() != QContactGuid::DefinitionName
                && detail.definitionName() != QContactTimestamp::DefinitionName) {
                qDebug() << "Extra detail: " << detail.definitionName();
                // displaylabel, type, guid, timestamp
                return false;
            }
        }
    }
    return true;
}

void tst_Performance::startBenchmark(QString benchmarkName)
{
    qDebug() << "Starting benchmark" << benchmarkName;
    m_benchmarkName = benchmarkName;
    m_timer.start();
}

bool tst_Performance::endBenchmark(const int target)
{
    Q_ASSERT(!m_benchmarkName.isEmpty());
    int result = m_timer.elapsed();
    qDebug() << "The result for benchmark" << m_benchmarkName << "is [" << result << "]ms";
    QFile resultFile(resultFileName);
    if (resultFile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QTextStream out(&resultFile);
        out << m_benchmarkName + QString(";") + QString::number(result) + "\r\n";
        resultFile.close();
    }

    if (target == 0 || result <= target) {
        return true;
    }
    qDebug() << "Target not reached!";
    return false;
}

QTEST_MAIN(tst_Performance);
#include "tst_performance.moc"
