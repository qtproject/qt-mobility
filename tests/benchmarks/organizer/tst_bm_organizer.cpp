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

//TESTED_COMPONENT=src/organizer

#include <QApplication>
#include <QtTest/QtTest>
#include "qtorganizer.h"

QTM_USE_NAMESPACE

class tst_QOrganizerBenchmark : public QObject
{
    Q_OBJECT

public:
    tst_QOrganizerBenchmark();
    virtual ~tst_QOrganizerBenchmark();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void benchmarking();
    void benchmarking_data() {addManagers();}

private:
    void addManagers();
    QString generateRandomString(int strlen, bool numeric = false) const;

};

tst_QOrganizerBenchmark::tst_QOrganizerBenchmark()
{
}

tst_QOrganizerBenchmark::~tst_QOrganizerBenchmark()
{
}

void tst_QOrganizerBenchmark::initTestCase()
{
}

void tst_QOrganizerBenchmark::cleanupTestCase()
{
}

void tst_QOrganizerBenchmark::addManagers()
{
    QTest::addColumn<QString>("uri");

    QStringList managers = QOrganizerManager::availableManagers();

    /* Known one that will not pass */
    managers.removeAll("invalid");
    managers.removeAll("testdummy");
    managers.removeAll("teststaticdummy");
    managers.removeAll("maliciousplugin");
    managers.removeAll("skeleton");

    foreach(QString mgr, managers) {
        QMap<QString, QString> params;
        if (mgr == "memory") {
            // parameters specific to the memory engine
            params.insert("id", "tst_QOrganizerManager");
            QTest::newRow(QString("mgr='%1', params").arg(mgr).toLatin1().constData()) << QOrganizerManager::buildUri(mgr, params);
        } else if (mgr == "symbian") {
            // parameters specific to the symbian engine
            QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QOrganizerManager::buildUri(mgr, params);
        } else {
            // all other engines, just generate the default manager.
            QTest::newRow(QString("mgr='%1'").arg(mgr).toLatin1().constData()) << QOrganizerManager::buildUri(mgr, params);
        }
    }
}

QString tst_QOrganizerBenchmark::generateRandomString(int strlen, bool numeric) const
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

void tst_QOrganizerBenchmark::benchmarking()
{
    QFETCH(QString, uri);
    QOrganizerManager* oim = QOrganizerManager::fromUri(uri);

    qDebug() << "Performing Benchmark Test: this may take some time, please be patient...";

    // we aggregate results from multiple runs of each benchmark
    int AGGREGATION_LIMIT = 3;

    // we perform each benchmark synchronously, and asynchronously.
    // the average time for each benchmark is reported, however
    // this time does not include the time taken for erroneous operations,
    // as this is reported separately.

    // successful synchronous benchmarks
    quint64 ssb[11] = {
        Q_UINT64_C(0),
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
    quint64 esb[11] = {
        Q_UINT64_C(0),
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
    quint64 sab[11] = {
        Q_UINT64_C(0),
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
    quint64 eab[11] = {
        Q_UINT64_C(0),
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
    quint64 esbc[11] = {
        Q_UINT64_C(0),
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
    quint64 eabc[11] = {
        Q_UINT64_C(0),
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
    QOrganizerItemDescription description;
    QOrganizerItemDisplayLabel displayLabel;
    QOrganizerItemLocation locationOne;
    QOrganizerItemTag tagOne;
    QOrganizerItemTag tagTwo;
    QOrganizerItemTag tagThree;

    // the filter and sort order which we'll use.
    QOrganizerItemDetailFilter df;
    df.setDetailDefinitionName(QOrganizerItemDescription::DefinitionName, QOrganizerItemDescription::FieldDescription);
    df.setValue("Description matching the filter");
    df.setMatchFlags(QOrganizerItemFilter::MatchExactly);
    QOrganizerItemSortOrder singleSort;
    singleSort.setDetailDefinitionName(QOrganizerItemDisplayLabel::DefinitionName, QOrganizerItemDisplayLabel::FieldLabel);
    QList<QOrganizerItemSortOrder> sorting;
    sorting.append(singleSort);

    // intialise the random number generator
    qsrand(QDateTime::currentDateTime().toTime_t());

    // We run the benchmarks with different numbers of contacts each time.
    QString outputMessage = QString("\nBenchmark results for manager: %1\n========================================").arg(oim->managerUri());
    QList<int> testSetSizes;
    testSetSizes << 10 << 100 << 1000;// << 10000;
    for (int n = 0; n < testSetSizes.size(); ++n) {
        int numberOfItems = testSetSizes.at(n);
        int prefixCutoff = numberOfItems / 5; // the first 20 % of items get a description we can filter on.

        // benchmark 1: save numberOfItems new items.
        // benchmark 2: update numberOfItems existing items.
        // benchmark 3: retrieve all item ids.
        // benchmark 4: retrieve item ids matching filter.
        // benchmark 5: retrieve item ids matching filter, sorted.
        // benchmark 6: retrieve all items.
        // benchmark 7: retrieve items matching filter.
        // benchmark 8: retrieve items matching filter, sorted.
        // benchmark 9: retrieve all items and their occurrences, sorted.
        // benchmark 10: remove some selected items.
        // benchmark 11: remove all items.
        QStringList benchmarkDescriptions;
        benchmarkDescriptions << QString("Save %1 items").arg(QString::number(numberOfItems))
                              << QString("Update %1 items").arg(QString::number(numberOfItems))
                              << QString("Retrieve %1 ids (all)").arg(QString::number(numberOfItems))
                              << QString("Retrieve %1 ids (filter)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve %1 ids (filter, sorted)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve %1 items (all)").arg(QString::number(numberOfItems))
                              << QString("Retrieve %1 items (filter)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve %1 items (filter, sorted)").arg(QString::number(prefixCutoff))
                              << QString("Retrieve all items and their occurrences (sorted)")
                              << QString("Remove %1 items (filter)").arg(QString::number(prefixCutoff))
                              << QString("Remove %1 items (all)").arg(QString::number((numberOfItems - prefixCutoff)));
        int largestDescriptionLength = 0;
        for (int i = 0; i < benchmarkDescriptions.size(); ++i) {
            if (benchmarkDescriptions.at(i).length() > largestDescriptionLength) {
                largestDescriptionLength = benchmarkDescriptions.at(i).length();
            }
        }

        // now generate the items.
        QList<QOrganizerItem> items;
        for (int i = 0; i < numberOfItems; ++i) {
            // generate an item.
            QOrganizerItem temp;
            if (i % 7 == 0) {
                QOrganizerTodo tempTodo;
                tempTodo.setStartDateTime(QDateTime::currentDateTime());
                tempTodo.setDueDateTime(QDateTime::currentDateTime().addDays(3));
                tempTodo.setPriority(QOrganizerItemPriority::LowPriority);
                tempTodo.setAllDay(true);
                temp = tempTodo;
            } else if (i % 3 == 0) {
                temp = QOrganizerJournal();
            } else {
                QOrganizerEvent tempEvent;
                tempEvent.setStartDateTime(QDateTime::currentDateTime());
                tempEvent.setEndDateTime(QDateTime::currentDateTime().addSecs(3600));
                QOrganizerRecurrenceRule rrule;
                rrule.setLimit(6);
                if (i % 5 == 0) {
                    rrule.setFrequency(QOrganizerRecurrenceRule::Daily);
                } else {
                    rrule.setFrequency(QOrganizerRecurrenceRule::Weekly);
                }
                if (i % 2 == 0) {
                    // only half of the events should reoccur.
                    tempEvent.setRecurrenceRule(rrule);
                }

                // all events have a single location
                QString location = generateRandomString(((qrand()%7) + 1));
                locationOne.setLabel(location);
                temp.saveDetail(&locationOne);

                temp = tempEvent;
            }

            // all items have just one description detail, the first 20% have a filterable description.
            if (i < prefixCutoff) {
                description.setDescription("Description matching the filter");
            } else {
                description.setDescription("Description not matching the filter");
            }
            temp.saveDetail(&description);

            // items have different amounts of tags
            tagOne.setTag(generateRandomString(((qrand()%10) + 1), true));
            temp.saveDetail(&tagOne);
            if (i % 2 == 0) {
                tagTwo.setTag(generateRandomString(((qrand()%10) + 1), true));
                temp.saveDetail(&tagTwo);
            }
            if (i % 3 == 0) {
                tagThree.setTag(generateRandomString(((qrand()%10) + 1), true));
                temp.saveDetail(&tagThree);
            }

            items.append(temp);
        }

        // now we can perform the benchmarks.
        QTime timer;
        bool success = false;
        for (int i = 0; i < AGGREGATION_LIMIT; ++i) {
            // synchronous benchmark 1 - saving new items.
            QList<QOrganizerItem> tempList = items;
            tempList.swap(0,1); // force detach, we don't want that to contaminate benchmark.
            timer.start();
            success = oim->saveItems(&tempList);
            int tempElapsed = timer.elapsed();
            if (!success) {
                esb[0] += tempElapsed;
                esbc[0] += 1;
            } else {
                ssb[0] += tempElapsed;
            }

            // synchronous benchmark 2 - update all existing items.
            for (int j = 0; j < tempList.size(); ++j) {
                QOrganizerItem tempItem = tempList.at(j);
                QOrganizerItemDisplayLabel tempLabel = tempItem.detail<QOrganizerItemDisplayLabel>();
                tempLabel.setLabel(QString(tempLabel.label() + QLatin1String("updated")));
                tempItem.saveDetail(&tempLabel);
                tempList.replace(j, tempItem);
            }
            timer.start();
            success = oim->saveItems(&tempList);
            tempElapsed = timer.elapsed();
            if (!success) {
                esb[1] += tempElapsed;
                esbc[1] += 1;
            } else {
                ssb[1] += tempElapsed;
            }

            // synchronous benchmark 3 - retrieve all item ids.
            timer.start();
            QList<QOrganizerItemId> allIds = oim->itemIds();
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[2] += tempElapsed;
                esbc[2] += 1;
            } else {
                ssb[2] += tempElapsed;
            }

            // synchronous benchmark 4 - fetching ids with filter.
            timer.start();
            QList<QOrganizerItemId> ids = oim->itemIds(df);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[3] += tempElapsed;
                esbc[3] += 1;
            } else {
                ssb[3] += tempElapsed;
            }

            // synchronous benchmark 5 - fetching item ids with filter, sorted.
            timer.start();
            ids = oim->itemIds(df, sorting);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[4] += tempElapsed;
                esbc[4] += 1;
            } else {
                ssb[4] += tempElapsed;
            }

            // synchronous benchmark 6 - fetching all items.
            tempList.clear();
            timer.start();
            tempList = oim->itemsForExport();
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[5] += tempElapsed;
                esbc[5] += 1;
            } else {
                ssb[5] += tempElapsed;
            }

            // synchronous benchmark 7 - fetching items with filter
            tempList.clear();
            timer.start();
            tempList = oim->itemsForExport(QDateTime(), QDateTime(), df);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[6] += tempElapsed;
                esbc[6] += 1;
            } else {
                ssb[6] += tempElapsed;
            }

            // synchronous benchmark 8 - fetching items with filter, sorted
            tempList.clear();
            timer.start();
            tempList = oim->itemsForExport(QDateTime(), QDateTime(), df, sorting);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[7] += tempElapsed;
                esbc[7] += 1;
            } else {
                ssb[7] += tempElapsed;
            }

            // synchronous benchmark 9 - fetching all items and their occurrences, sorted
            tempList.clear();
            timer.start();
            tempList = oim->items(QDateTime(), QDateTime(), QOrganizerItemFilter(), sorting);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[8] += tempElapsed;
                esbc[8] += 1;
            } else {
                ssb[8] += tempElapsed;
            }

            // synchronous benchmark 10 - removing some contacts
            ids.clear();
            ids = oim->itemIds(df);
            timer.start();
            success = oim->removeItems(ids);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[9] += tempElapsed;
                esbc[9] += 1;
            } else {
                ssb[9] += tempElapsed;
            }

            // synchronous benchmark 11 - removing all contacts
            ids.clear();
            ids = oim->itemIds();
            timer.start();
            success = oim->removeItems(ids);
            tempElapsed = timer.elapsed();
            if (oim->error() != QOrganizerManager::NoError) {
                esb[10] += tempElapsed;
                esbc[10] += 1;
            } else {
                ssb[10] += tempElapsed;
            }

            // asynchronous benchmark 1 - save new items.
            QOrganizerItemSaveRequest isr;
            isr.setManager(oim);
            tempList = items;
            tempList.swap(0,1); // force detach, we don't want that to contaminate benchmark.
            isr.setItems(tempList);
            timer.start();
            isr.start();
            success = isr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (isr.error() != QOrganizerManager::NoError || !success) {
                eab[0] += tempElapsed;
                eabc[0] += 1;
            } else {
                sab[0] += tempElapsed;
            }

            // asynchronous benchmark 2 - update existing items.
            for (int j = 0; j < tempList.size(); ++j) {
                QOrganizerItem tempItem = tempList.at(j);
                QOrganizerItemDisplayLabel tempLabel = tempItem.detail<QOrganizerItemDisplayLabel>();
                tempLabel.setLabel(QString(tempLabel.label() + QLatin1String("updated")));
                tempItem.saveDetail(&tempLabel);
                tempList.replace(j, tempItem);
            }
            isr.setItems(tempList);
            timer.start();
            isr.start();
            success = isr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (isr.error() != QOrganizerManager::NoError || !success) {
                eab[1] += tempElapsed;
                eabc[1] += 1;
            } else {
                sab[1] += tempElapsed;
            }

            // asynchronous benchmark 3 - retrieve all item ids.
            QOrganizerItemIdFetchRequest iifr;
            iifr.setManager(oim);
            timer.start();
            iifr.start();
            success = iifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (iifr.error() != QOrganizerManager::NoError || !success) {
                eab[2] += tempElapsed;
                eabc[2] += 1;
            } else {
                sab[2] += tempElapsed;
            }

            // asynchronous benchmark 4 - retrieve item ids matching filter.
            iifr.setFilter(df);
            timer.start();
            iifr.start();
            success = iifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (iifr.error() != QOrganizerManager::NoError || !success) {
                eab[3] += tempElapsed;
                eabc[3] += 1;
            } else {
                sab[3] += tempElapsed;
            }

            // asynchronous benchmark 5 - retrieve item ids matching filter, sorted.
            iifr.setFilter(df);
            iifr.setSorting(sorting);
            timer.start();
            iifr.start();
            success = iifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (iifr.error() != QOrganizerManager::NoError || !success) {
                eab[4] += tempElapsed;
                eabc[4] += 1;
            } else {
                sab[4] += tempElapsed;
            }

            // asynchronous benchmark 6 - retrieve all items.
            QOrganizerItemFetchForExportRequest ifer;
            ifer.setManager(oim);
            timer.start();
            ifer.start();
            success = ifer.waitForFinished();
            tempElapsed = timer.elapsed();
            if (ifer.error() != QOrganizerManager::NoError || !success) {
                eab[5] += tempElapsed;
                eabc[5] += 1;
            } else {
                sab[5] += tempElapsed;
            }

            // asynchronous benchmark 7 - retrieve items matching filter.
            ifer.setFilter(df);
            timer.start();
            ifer.start();
            success = ifer.waitForFinished();
            tempElapsed = timer.elapsed();
            if (ifer.error() != QOrganizerManager::NoError || !success) {
                eab[6] += tempElapsed;
                eabc[6] += 1;
            } else {
                sab[6] += tempElapsed;
            }

            // asynchronous benchmark 8 - retrieve items matching filter, sorted.
            ifer.setFilter(df);
            ifer.setSorting(sorting);
            timer.start();
            ifer.start();
            success = ifer.waitForFinished();
            tempElapsed = timer.elapsed();
            if (ifer.error() != QOrganizerManager::NoError || !success) {
                eab[7] += tempElapsed;
                eabc[7] += 1;
            } else {
                sab[7] += tempElapsed;
            }

            // asynchronous benchmark 9 - retrieve all items and their occurrences, sorted.
            QOrganizerItemFetchRequest ifr;
            ifr.setManager(oim);
            ifr.setSorting(sorting);
            timer.start();
            ifr.start();
            success = ifr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (ifr.error() != QOrganizerManager::NoError || !success) {
                eab[8] += tempElapsed;
                eabc[8] += 1;
            } else {
                sab[8] += tempElapsed;
            }

            // asynchronous benchmark 10 - remove selected items.
            ids = oim->itemIds(df);
            QOrganizerItemRemoveRequest irr;
            irr.setManager(oim);
            irr.setItemIds(ids);
            timer.start();
            irr.start();
            success = irr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (irr.error() != QOrganizerManager::NoError || !success) {
                eab[9] += tempElapsed;
                eabc[9] += 1;
            } else {
                sab[9] += tempElapsed;
            }

            // asynchronous benchmark 11 - remove all items.
            ids = oim->itemIds();
            irr.setItemIds(ids);
            timer.start();
            irr.start();
            success = irr.waitForFinished();
            tempElapsed = timer.elapsed();
            if (irr.error() != QOrganizerManager::NoError || !success) {
                eab[10] += tempElapsed;
                eabc[10] += 1;
            } else {
                sab[10] += tempElapsed;
            }
        }

        qint64 succeededCount, failedCount, averageSuccessTime, averageFailTime;
        outputMessage += QString("\nwith numberOfItems = %1").arg(numberOfItems);
        outputMessage += QLatin1String("\n  Synchronous:");

        for (int i = 0; i < 11; ++i) {
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
            if (i >= 9) {
                outputMessage += QString(QLatin1String("\n    (")) + QString::number(i+1) + QString(QLatin1String(") "));
            } else {
                outputMessage += QString(QLatin1String("\n     (")) + QString::number(i+1) + QString(QLatin1String(") "));
            }
            outputMessage += tempStr;
        }

        outputMessage += QLatin1String("\n  Asynchronous:");
        for (int i = 0; i < 11; ++i) {
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
            tempStr += QString("  succeeded: %1  average_time(success): %2\t  failed: %3  average_time(failed): %4").arg(succeededCount).arg(averageSuccessTime).arg(failedCount).arg(averageFailTime);
            if (i >= 9) {
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
    delete oim;
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    tst_QOrganizerBenchmark test1;
    QTest::qExec(&test1, argc, argv);
}

#include "tst_bm_organizer.moc"

