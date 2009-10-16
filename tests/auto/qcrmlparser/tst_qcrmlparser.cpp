/****************************************************************************
**
** Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in Technology Preview License Agreement accompanying
** this package.
**f
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** If you have questions regarding the use of this file, please
** contact Nokia at http://qt.nokia.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "../../../context/qcrmlparser_p.h"

#include <QtTest/QTest>
#include <QDebug>
#include <QDir>

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

class tst_QCRMLParser : public QObject
{
    Q_OBJECT

private slots:
    void test();
    void keyRangeSequence();
    void bitmaskKey();

private:
    bool verifyKeyData(const QList<KeyData> &keyData, const QString &key,
                    quint64 uid, quint32 bitIndex = 0);
};

void tst_QCRMLParser::test()
{
    QDir testData(TESTDATA_DIR "/testdata");

    QCRMLParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCRML("dontexist");
    QVERIFY(parser.error() == QCRMLParser::FileDoesNotExist);
    QVERIFY(keyData.count() == 0);

    keyData = parser.parseQCRML(testData.absoluteFilePath("test1.qcrml"));

    QVERIFY(verifyKeyData(keyData, "/sensors/accelerometer/x",
                          Q_UINT64_C(0x4815162300000000)));
    QVERIFY(verifyKeyData(keyData, "/sensors/accelerometer/y",
                          Q_UINT64_C(0x4815162300000001)));
    QVERIFY(verifyKeyData(keyData, "/sensors/accelerometer/z",
                          Q_UINT64_C(0x4815162300000002)));

    QVERIFY(parser.error() == QCRMLParser::NoError);
}

void tst_QCRMLParser::keyRangeSequence()
{
    QDir testData(TESTDATA_DIR "/testdata");
    QCRMLParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCRML(testData.absoluteFilePath("test2.qcrml"));
    QCOMPARE(keyData.count(), 763);

    quint64 repoUID = Q_UINT64_C(0x81531600000000);

    quint32 settingsIdentifier = 0x07000000;
    quint32 itemSettingIndex = 0x00010000;
    quint32 itemSettingIndexOffset = 0x00020000;
    quint64 uidPrefix = 0;

    for(int i=0; i < 254; i++) {
        uidPrefix = repoUID + settingsIdentifier + itemSettingIndex * i + itemSettingIndexOffset;

        QVERIFY(verifyKeyData(keyData,QString("/PIM/Contact/") + QString::number(i) + "/firstName",
                              uidPrefix + 0x1));
        QVERIFY(verifyKeyData(keyData,QString("/PIM/Contact/") + QString::number(i) + "/lastName",
                              uidPrefix + 0x2));
        QVERIFY(verifyKeyData(keyData,QString("/PIM/Contact/") + QString::number(i) + "/group",
                              uidPrefix + 0xf00f));
    }
}

void tst_QCRMLParser::bitmaskKey()
{
    QDir testData(TESTDATA_DIR "/testdata");
    QCRMLParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCRML(testData.absoluteFilePath("test3.qcrml"));
    QCOMPARE(keyData.count(), 4);

    quint64 uid = Q_UINT64_C(0x1234567800000001);

    QVERIFY(verifyKeyData(keyData, QString("/Dharma/Swan"), uid,1));
    QVERIFY(verifyKeyData(keyData, QString("/Dharma/Pearl"), uid,2));
    QVERIFY(verifyKeyData(keyData, QString("/Dharma/Hydra"), uid,32));
    QVERIFY(verifyKeyData(keyData, QString("/Dharma/LookingGlass"), uid,4));
}

bool tst_QCRMLParser::verifyKeyData(const QList<KeyData> &keyData, const QString &key,
                    quint64 uid, quint32 bitIndex) {
    KeyData data;
    for (int i=0; i < keyData.count(); i++) {
       if (keyData.at(i).path() == key)
           data = keyData.at(i);
    }

    if ( data.UID()!= uid ) {
        qWarning() << "UID does not match expected:" << QString::number(uid,16) << "\n actual: "
                << QString::number(data.UID(),16);
        return false;
    }

    if (data.bitIndex() != bitIndex) {
        qWarning() << "BitIndex does not match, expected: " << QString::number(bitIndex,16)
                << "\n actual" << QString::number(data.bitIndex(),16);
        return false;
    }
    return true;
}

QTEST_MAIN(tst_QCRMLParser)
#include "tst_qcrmlparser.moc"
