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

//TESTED_COMPONENT=src/contacts

#include <QApplication>
#include <QtTest/QtTest>
#include "qtcontacts.h"

QTM_USE_NAMESPACE

class tst_QContactBenchmark : public QObject
{
    Q_OBJECT

public:
    tst_QContactBenchmark();
    virtual ~tst_QContactBenchmark();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void benchmarking();
    void benchmarking_data() {addManagers();}

private:
    void addManagers();
    QString generateRandomString(int strlen, bool numeric = false) const;

};

tst_QContactBenchmark::tst_QContactBenchmark()
{
}

tst_QContactBenchmark::~tst_QContactBenchmark()
{
}

void tst_QContactBenchmark::initTestCase()
{
}

void tst_QContactBenchmark::cleanupTestCase()
{
}

void tst_QContactBenchmark::addManagers()
{
    QTest::addColumn<QString>("uri");

    QStringList managers = QContactManager::availableManagers();

    /* Known one that will not pass */
    managers.removeAll("invalid");
    managers.removeAll("testdummy");
    managers.removeAll("teststaticdummy");
    managers.removeAll("maliciousplugin");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        if (mgr == "memory") {
            // parameters specific to the memory engine
            params.insert("id", "tst_QContactManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        } else if (mgr == "symbian") {
            // parameters specific to the symbian engine
            QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        } else {
            // all other engines, just generate the default manager.
            QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QContactManager::buildUri(mgr, params);
        }
    }
}

QString tst_QContactBenchmark::generateRandomString(int strlen, bool numeric) const
{
    QString retn;

    for (int i = 0; i < strlen; ++i) {
        int random = qrand();
        if (numeric) {
            random = random % 10;
            retn += QString::number(random);
        } else {
            random = random % 26;
            unsigned char letter = ('a' + static_cast<unsigned char>(random));
            retn += QChar(letter);
        }
    }

    return retn;
}

void tst_QContactBenchmark::benchmarking()
{
    QFETCH(QString, uri);
    QContactManager* cm = QContactManager::fromUri(uri);

    qDebug() << "Performing Benchmark Test: this may take some time, please be patient...";

    // we aggregate results from multiple runs of each benchmark
    int AGGREGATION_LIMIT = 3;

    // we perform each benchmark synchronously, and asynchronously.
    // the average time for each benchmark is reported, however
    // this time does not include the time taken for erroneous operations,
    // as this is reported separately.

    // successful synchronous benchmarks
    quint64 ssb[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // erroneous synchronous benchmarks
    quint64 esb[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // successful asynchronous benchmarks
    quint64 sab[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // erroneous asynchronous benchmarks
    quint64 eab[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // count of erroneous synchronous benchmark operations
    quint64 esbc[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // count of erroneous asynchronous benchmark operations
    quint64 eabc[10] = {
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0),
        Q_UINT64_C(0)
    };

    // define some details which we'll use.
    QContactName name;
    QContactPhoneNumber phoneNumberOne;
    QContactPhoneNumber phoneNumberTwo;
    QContactPhoneNumber phoneNumberThree;
    QContactEmailAddress emailAddressOne;
    QContactEmailAddress emailAddressTwo;
    QContactEmailAddress emailAddressThree;
    QContactAddress streetAddress;
    QContactThumbnail thumbnail;
    QByteArray thumbnailArray =
        "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"\
        "ngAAAABJRU5ErkJggg==";
    QImage thumbnailImage = QImage::fromData(QByteArray::fromBase64(thumbnailArray));
    thumbnail.setThumbnail(thumbnailImage);

    // the filter and sort order which we'll use.
    QContactDetailFilter df;
    df.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldPrefix);
    df.setValue("Dr.");
    df.setMatchFlags(QContactFilter::MatchExactly);
    QContactSortOrder singleSort;
    singleSort.setDetailDefinitionName(QContactName::DefinitionName, QContactName::FieldLastName);
    QList<QContactSortOrder> sorting;
    sorting.append(singleSort);

    // intialise the random number generator
    qsrand(QDateTime::currentDateTime().toTime_t());

    // We run the benchmarks with different numbers of contacts each time.
    QString outputMessage = QString("\nBenchmark results for manager: %1\n========================================").arg(cm->managerUri());
    QList<int> testSetSizes;
    testSetSizes << 10 << 100 << 1000 << 10000;
    for (int n = 0; n < testSetSizes.size(); ++n) {
        int numberOfContacts = testSetSizes.at(n);
        int prefixCutoff = numberOfContacts / 5; // the first 20 % of contacts get a prefix we can filter on.

        // benchmark 1: save numberOfContacts new contacts.
        // benchmark 2: update numberOfContacts existing contacts.
        // benchmark 3: retrieve all contact ids.
        // benchmark 4: retrieve contact ids matching filter.
        // benchmark 5: retrieve contact ids matching filter, sorted.
        // benchmark 6: retrieve all contacts.
        // benchmark 7: retrieve contacts matching filter.
        // benchmark 8: retrieve contacts matching filter, sorted.
        // benchmark 9: remove some selected contacts.
        // benchmark 10: remove all contacts.
        QStringList benchmarkDescriptions;
        benchmarkDescriptions << QString("Save %1 contacts").arg(QString::number(numberOfContacts))
                              << QString("Update %1 contacts").arg(QString::number(numberOfContacts))
                              << QString("Retrieve %1 ids (all)").arg(QString::number(numberOfContacts))
                              << QString("Retrieve %1 ids (filter)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve %1 ids (filter, sorted)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve %1 contacts (all)").arg(QString::number(numberOfContacts))
                              << QString("Retrieve %1 contacts (filter)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve %1 contacts (filter, sorted)").arg(QString::number(prefixCutoff))
                              << QString("Remove %1 contacts (filter)").arg(QString::number(prefixCutoff))
                              << QString("Remove %1 contacts (all)").arg(QString::number((numberOfContacts - prefixCutoff)));
        int largestDescriptionLength = 0;
        for (int i = 0; i < benchmarkDescriptions.size(); ++i) {
            if (benchmarkDescriptions.at(i).length() > largestDescriptionLength) {
                largestDescriptionLength = benchmarkDescriptions.at(i).length();
            }
        }

        // now generate the contacts.
        QList<QContact> contacts;
        for (int i = 0; i < numberOfContacts; ++i) {
            // generated contact.
            QContact temp;
            if (i % 4 == 0) {
                temp.saveDetail(&thumbnail);
            }

            // all contacts have just one name detail, the first 20% have a filterable prefix.
            if (i < prefixCutoff) {
                name.setPrefix("Dr.");
            } else {
                name.setPrefix("");
            }
            name.setFirstName(generateRandomString(((qrand()%8) + 1)));
            name.setLastName(generateRandomString(((qrand()%10) + 1)));
            temp.saveDetail(&name);

            // contacts have different amounts of phone numbers.
            phoneNumberOne.setNumber(generateRandomString(((qrand()%10) + 1), true));
            temp.saveDetail(&phoneNumberOne);
            if (i % 2 == 0) {
                phoneNumberTwo.setNumber(generateRandomString(((qrand()%10) + 1), true));
                phoneNumberTwo.setContexts(QContactPhoneNumber::ContextHome);
                phoneNumberTwo.setSubTypes(QContactPhoneNumber::SubTypeMobile);
                temp.saveDetail(&phoneNumberTwo);
            }
            if (i % 3 == 0) {
                phoneNumberThree.setNumber(generateRandomString(((qrand()%10) + 1), true));
                phoneNumberThree.setContexts(QStringList() << QContactPhoneNumber::ContextHome << QContactPhoneNumber::ContextWork);
                phoneNumberThree.setSubTypes(QContactPhoneNumber::SubTypeVoice);
                temp.saveDetail(&phoneNumberThree);
            }

            QString email = generateRandomString(((qrand()%7) + 1));
            email += "@test.com";
            emailAddressOne.setEmailAddress(email);
            temp.saveDetail(&emailAddressOne);
            if (i % 5 == 0) {
                email = generateRandomString(((qrand()%7) + 1));
                email += "@test.com";
                emailAddressTwo.setEmailAddress(email);
                temp.saveDetail(&emailAddressTwo);
            }
            if (i % 7 == 0) {
                email = generateRandomString(((qrand()%7) + 1));
                email += "@test.com";
                emailAddressThree.setEmailAddress(email);
                temp.saveDetail(&emailAddressThree);
            }

            if (i % 9 == 0) {
                streetAddress.setStreet(generateRandomString(((qrand()%7) + 5)));
                streetAddress.setLocality(generateRandomString(((qrand()%7) + 5)));
                streetAddress.setRegion(generateRandomString(((qrand()%7) + 5)));
                streetAddress.setPostcode(generateRandomString(5, true));
                streetAddress.setCountry(generateRandomString(((qrand()%7) + 5)));
                temp.saveDetail(&streetAddress);
            }

            contacts.append(temp);
        }

        // now we can perform the benchmarks.
        QTime timer;
        bool success = false;
        QMap<int, QContactManager::Error> errorMap;
        for (int i = 0; i < AGGREGATION_LIMIT; ++i) {
            // synchronous benchmark 1 - saving new contacts.
            QList<QContact> tempList = contacts;
            tempList.swap(0,1); // force detach, we don't want that to contaminate benchmark.
            timer.start();
            success = cm->saveContacts(&tempList, &errorMap);
            int tempElapsed = timer.elapsed();
            if (!success) {
                esb[0] += tempElapsed;
                esbc[0] += 1;
            } else {
                ssb[0] += tempElapsed;
            }

            // synchronous benchmark 2 - update all existing contacts.
            for (int j = 0; j < tempList.size(); ++j) {
                QContact tempContact = tempList.at(j);
                QContactName tempName = tempContact.detail<QContactName>();
                tempName.setMiddleName("updated");
                tempContact.saveDetail(&tempName);
                tempList.replace(j, tempContact);
            }
            timer.start();
            success = cm->saveContacts(&tempList, &errorMap);
            tempElapsed = timer.elapsed();
            if (!success) {
                esb[1] += tempElapsed;
                esbc[1] += 1;
            } else {
                ssb[1] += tempElapsed;
            }

            // synchronous benchmark 3 - retrieve all contact ids.
            timer.start();
            QList<QContactLocalId> allIds = cm->contactIds();
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[2] += tempElapsed;
                esbc[2] += 1;
            } else {
                ssb[2] += tempElapsed;
            }

            // synchronous benchmark 4 - fetching contactids with filter.
            timer.start();
            QList<QContactLocalId> ids = cm->contactIds(df);
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[3] += tempElapsed;
                esbc[3] += 1;
            } else {
                ssb[3] += tempElapsed;
            }

            // synchronous benchmark 5 - fetching contactids with filter, sorted.
            timer.start();
            ids = cm->contactIds(df, sorting);
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[4] += tempElapsed;
                esbc[4] += 1;
            } else {
                ssb[4] += tempElapsed;
            }

            // synchronous benchmark 6 - fetching all contacts.
            tempList.clear();
            timer.start();
            tempList = cm->contacts();
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[5] += tempElapsed;
                esbc[5] += 1;
            } else {
                ssb[5] += tempElapsed;
            }

            // synchronous benchmark 7 - fetching contacts with filter
            tempList.clear();
            timer.start();
            tempList = cm->contacts(df);
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[6] += tempElapsed;
                esbc[6] += 1;
            } else {
                ssb[6] += tempElapsed;
            }

            // synchronous benchmark 8 - fetching contacts with filter, sorted
            tempList.clear();
            timer.start();
            tempList = cm->contacts(df, sorting);
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[7] += tempElapsed;
                esbc[7] += 1;
            } else {
                ssb[7] += tempElapsed;
            }

            // synchronous benchmark 9 - removing some contacts
            ids.clear();
            ids = cm->contactIds(df);
            timer.start();
            success = cm->removeContacts(ids, &errorMap);
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[8] += tempElapsed;
                esbc[8] += 1;
            } else {
                ssb[8] += tempElapsed;
            }

            // synchronous benchmark 10 - removing all contacts
            ids.clear();
            ids = cm->contactIds();
            timer.start();
            success = cm->removeContacts(ids, &errorMap);
            tempElapsed = timer.elapsed();
            if (cm->error() != QContactManager::NoError) {
                esb[9] += tempElapsed;
                esbc[9] += 1;
            } else {
                ssb[9] += tempElapsed;
            }

            // asynchronous benchmark 1 - save new contacts.
            QContactSaveRequest csr;
            csr.setManager(cm);
            tempList = contacts;
            tempList.swap(0,1); // force detach, we don't want that to contaminate benchmark.
            csr.setContacts(tempList);
            timer.start();
            csr.start();
            success = csr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (csr.error() != QContactManager::NoError || !success) {
                eab[0] += tempElapsed;
                eabc[0] += 1;
            } else {
                sab[0] += tempElapsed;
            }

            // asynchronous benchmark 2 - update existing contacts.
            for (int j = 0; j < tempList.size(); ++j) {
                QContact tempContact = tempList.at(j);
                QContactName tempName = tempContact.detail<QContactName>();
                tempName.setMiddleName("updated");
                tempContact.saveDetail(&tempName);
                tempList.replace(j, tempContact);
            }
            csr.setContacts(tempList);
            timer.start();
            csr.start();
            success = csr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (csr.error() != QContactManager::NoError || !success) {
                eab[1] += tempElapsed;
                eabc[1] += 1;
            } else {
                sab[1] += tempElapsed;
            }

            // asynchronous benchmark 3 - retrieve all contact ids.
            QContactLocalIdFetchRequest cifr;
            cifr.setManager(cm);
            timer.start();
            cifr.start();
            success = cifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (cifr.error() != QContactManager::NoError || !success) {
                eab[2] += tempElapsed;
                eabc[2] += 1;
            } else {
                sab[2] += tempElapsed;
            }

            // asynchronous benchmark 4 - retrieve contact ids matching filter.
            cifr.setFilter(df);
            timer.start();
            cifr.start();
            success = cifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (cifr.error() != QContactManager::NoError || !success) {
                eab[3] += tempElapsed;
                eabc[3] += 1;
            } else {
                sab[3] += tempElapsed;
            }

            // asynchronous benchmark 5 - retrieve contact ids matching filter, sorted.
            cifr.setFilter(df);
            cifr.setSorting(sorting);
            timer.start();
            cifr.start();
            success = cifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (cifr.error() != QContactManager::NoError || !success) {
                eab[4] += tempElapsed;
                eabc[4] += 1;
            } else {
                sab[4] += tempElapsed;
            }

            // asynchronous benchmark 6 - retrieve all contacts.
            QContactFetchRequest cfr;
            cfr.setManager(cm);
            cfr.setFilter(QContactFilter()); // matches all.
            timer.start();
            cfr.start();
            success = cfr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (cfr.error() != QContactManager::NoError || !success) {
                eab[5] += tempElapsed;
                eabc[5] += 1;
            } else {
                sab[5] += tempElapsed;
            }

            // asynchronous benchmark 7 - retrieve contacts matching filter.
            cfr.setFilter(df);
            timer.start();
            cfr.start();
            success = cfr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (cfr.error() != QContactManager::NoError || !success) {
                eab[6] += tempElapsed;
                eabc[6] += 1;
            } else {
                sab[6] += tempElapsed;
            }

            // asynchronous benchmark 8 - retrieve contacts matching filter, sorted.
            cfr.setFilter(df);
            cfr.setSorting(sorting);
            timer.start();
            cfr.start();
            success = cfr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (cfr.error() != QContactManager::NoError || !success) {
                eab[7] += tempElapsed;
                eabc[7] += 1;
            } else {
                sab[7] += tempElapsed;
            }

            // asynchronous benchmark 9 - remove selected contacts.
            ids = cm->contactIds(df);
            QContactRemoveRequest crr;
            crr.setManager(cm);
            crr.setContactIds(ids);
            timer.start();
            crr.start();
            success = crr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (crr.error() != QContactManager::NoError || !success) {
                eab[8] += tempElapsed;
                eabc[8] += 1;
            } else {
                sab[8] += tempElapsed;
            }

            // asynchronous benchmark 10 - remove all contacts.
            ids = cm->contactIds();
            crr.setContactIds(ids);
            timer.start();
            crr.start();
            success = crr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (crr.error() != QContactManager::NoError || !success) {
                eab[9] += tempElapsed;
                eabc[9] += 1;
            } else {
                sab[9] += tempElapsed;
            }
        }

        qint64 succeededCount, failedCount, averageSuccessTime, averageFailTime;
        outputMessage += QString("\nwith numberOfContacts = %1").arg(numberOfContacts);
        outputMessage += QLatin1String("\n  Synchronous:");

        for (int i = 0; i < 10; ++i) {
            succeededCount = AGGREGATION_LIMIT - esbc[i];
            failedCount = esbc[i];
            if (succeededCount)
                averageSuccessTime = (ssb[i] / succeededCount);
            else
                averageSuccessTime = -1;
            if (failedCount)
                averageFailTime = (esb[i] / failedCount);
            else
                averageFailTime = -1;
            QString tempStr = QString("%1:").arg(benchmarkDescriptions.at(i));
            int whitespaceAdjust = largestDescriptionLength - benchmarkDescriptions.at(i).length();
            for (int j = 0; j < whitespaceAdjust; ++j) {
                tempStr += " ";
            }
            tempStr += QString("  succeeded: %1  average_time(success): %2\t  failed: %3  average_time(failed): %4").arg(succeededCount).arg(averageSuccessTime).arg(failedCount).arg(averageFailTime);
            if (i == 9) {
                outputMessage += QString(QLatin1String("\n    (")) + QString::number(i+1) + QString(QLatin1String(") "));
            } else {
                outputMessage += QString(QLatin1String("\n     (")) + QString::number(i+1) + QString(QLatin1String(") "));
            }
            outputMessage += tempStr;
        }

        outputMessage += QLatin1String("\n  Asynchronous:");
        for (int i = 0; i < 10; ++i) {
            succeededCount = AGGREGATION_LIMIT - eabc[i];
            failedCount = eabc[i];
            if (succeededCount)
                averageSuccessTime = (sab[i] / succeededCount);
            else
                averageSuccessTime = -1;
            if (failedCount)
                averageFailTime = (sab[i] / failedCount);
            else
                averageFailTime = -1;
            QString tempStr = QString("%1:").arg(benchmarkDescriptions.at(i));
            int whitespaceAdjust = largestDescriptionLength - benchmarkDescriptions.at(i).length();
            for (int j = 0; j < whitespaceAdjust; ++j) {
                tempStr += " ";
            }
            tempStr += QString("  succeeded: %1  average_time(success): %2\t  failed: %3 average_time(failed): %4").arg(succeededCount).arg(averageSuccessTime).arg(failedCount).arg(averageFailTime);
            if (i == 9) {
                outputMessage += QString(QLatin1String("\n    (")) + QString::number(i+1) + QString(QLatin1String(") "));
            } else {
                outputMessage += QString(QLatin1String("\n     (")) + QString::number(i+1) + QString(QLatin1String(") "));
            }
            outputMessage += tempStr;
        }
        outputMessage += QLatin1String("\n----------------------------------------");

        // clear the variables.
        memset(ssb, 0, sizeof(ssb));
        memset(esb, 0, sizeof(esb));
        memset(sab, 0, sizeof(sab));
        memset(eab, 0, sizeof(eab));
        memset(esbc, 0, sizeof(esbc));
        memset(eabc, 0, sizeof(eabc));
    }

    outputMessage += QLatin1String("\n(All times measured in milliseconds).");
    qDebug() << outputMessage;
    delete cm;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    tst_QContactBenchmark test1;
    QTest::qExec(&test1, argc, argv);
}

#include "tst_bm_contacts.moc"

