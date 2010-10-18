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

//TESTED_COMPONENT=src/versit

#include "qversitwriter.h"
#include "qversitreader.h"
#include "qversitcontactexporter.h"
#include "qversitcontactimporter.h"
#include "qtcontacts.h"

#include <QtTest/QtTest>
#include <QDir>
#include <QList>

QTM_USE_NAMESPACE

#ifndef TESTDATA_DIR
#define TESTDATA_DIR "./"
#endif

Q_DECLARE_METATYPE(QList<QContact>)
Q_DECLARE_METATYPE(QContact)

class tst_QVersit : public QObject
{
    Q_OBJECT

private slots: // Tests
    void addFiles();
    void testRead();
    void testRead_data();
    void testWrite();
    void testWrite_data();

private:
};

void tst_QVersit::addFiles()
{
    QTest::addColumn<QString>("filename");

    QDir testDir(TESTDATA_DIR "testdata_vcf/");
    QStringList files(testDir.entryList());
    foreach (QString filename, files) {
        if (filename != "." && filename != "..") {
            QTest::newRow(filename.toAscii()) << testDir.filePath(filename);
        }
    }
}

void tst_QVersit::testRead()
{
    QFETCH(QString, filename);

    QFile file(filename);
    QVERIFY2(file.open(QIODevice::ReadOnly), filename.toAscii());
    QVersitReader reader(&file);
    QBENCHMARK {
        QVERIFY(reader.startReading());
        QVERIFY(reader.waitForFinished());
    }
}

void tst_QVersit::testRead_data()
{
    addFiles();
}

void tst_QVersit::testWrite()
{
    QFETCH(QString, filename);

    QFile file(filename);
    QVERIFY2(file.open(QIODevice::ReadOnly), filename.toAscii());
    QVersitReader reader(&file);
    QVERIFY(reader.startReading());
    QVERIFY(reader.waitForFinished());
    QList<QVersitDocument> documents = reader.results();
    QVERIFY(!documents.isEmpty());
    QCOMPARE(reader.error(), QVersitReader::NoError);

    QByteArray buffer;
    QVersitWriter writer(&buffer);
    QBENCHMARK {
        QVERIFY(writer.startWriting(documents));
        QVERIFY(writer.waitForFinished());
    }
}

void tst_QVersit::testWrite_data()
{
    addFiles();
}

QTEST_MAIN(tst_QVersit)

#include "tst_versit.moc"
