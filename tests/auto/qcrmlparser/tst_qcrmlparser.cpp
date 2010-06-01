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

#include "../../../src/publishsubscribe/psmapperserver_symbian/qcrmlparser_p.h"

#include <qmobilityglobal.h>
#include <QtTest/QTest>
#include <QDebug>
#include <QDir>

#if defined(Q_OS_SYMBIAN)
# define TESTDATA_DIR "."
#endif

QTM_USE_NAMESPACE

class tst_QCrmlParser : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void empty();
    void simple();
    void keyRangeSequence();
    void bitmaskKey();
    void keyWithUnknownElements();
    void keyRangeNoSequence();
    void singleKey();
    void multipleKeyRanges();
    void multipleBitmaskKeys();

    void wrongXmlFile();
    void noRepositoryUidValue();
    void keyMissingEndTag();
    void outOfPlaceEndTag();
    void invalidTarget();

private:
    bool verifyKeyData(const QHash<QString,KeyData> &keyHash, const QString &key,
                    quint64 uid, KeyData::Target target = KeyData::CRepository, quint32 bitIndex = 0);
    QHash<QString,KeyData> makeHash(const QList<KeyData> &keyList);
    void printKeyList(const QList<KeyData> &list);
    QDir testData;
};

void tst_QCrmlParser::init()
{
    testData.setPath(TESTDATA_DIR "/testdata");
}

void tst_QCrmlParser::empty()
{
    QList<KeyData> keyData;
    KeyData *key = new KeyData();
    keyData.append(*key);
    KeyData::Target target = KeyData::RProperty;

    QHash<QString, KeyData> keyHash = makeHash(keyData);
    QVERIFY(verifyKeyData(keyHash, "", 0, target));
}

void tst_QCrmlParser::simple()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml("dontexist");
    QVERIFY(parser.error() == QCrmlParser::FileNotFound);
    QCOMPARE(parser.errorString(), QString("File does not exist: dontexist"));
    QVERIFY(keyData.count() == 0);

    keyData = parser.parseQCrml(testData.absoluteFilePath("test1.qcrml"));
    QVERIFY(parser.error() == QCrmlParser::NoError);
    QHash<QString, KeyData> keyHash = makeHash(keyData);

    KeyData::Target target = KeyData::CRepository;
    QVERIFY(verifyKeyData(keyHash, "/sensors/accelerometer/x",
                          Q_UINT64_C(0x4815162300000000),target));
    QVERIFY(verifyKeyData(keyHash, "/sensors/accelerometer/y",
                          Q_UINT64_C(0x4815162300000001),target));
    QVERIFY(verifyKeyData(keyHash, "/sensors/accelerometer/z",
                          Q_UINT64_C(0x4815162300000002),target));

    QVERIFY(parser.error() == QCrmlParser::NoError);

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == quint32(i));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::keyRangeSequence()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test2.qcrml"));
    QCOMPARE(keyData.count(), 763);
    QHash<QString,KeyData> keyHash = makeHash(keyData);

    quint64 repoUID = Q_UINT64_C(0x81531600000000);

    quint32 settingsIdentifier = 0x07000000;
    quint32 itemSettingIndex = 0x00010000;
    quint32 itemSettingIndexOffset = 0x00020000;
    quint64 uidPrefix = 0;

    QVERIFY(verifyKeyData(keyHash, QString("/PIM/Contact"), repoUID + settingsIdentifier));

    for(int i=0; i < 254; i++) {
        uidPrefix = repoUID + settingsIdentifier + itemSettingIndex * i + itemSettingIndexOffset;

        QVERIFY(verifyKeyData(keyHash,QString("/PIM/Contact/") + QString::number(i) + "/firstName",
                              uidPrefix + 0x1));
        QVERIFY(verifyKeyData(keyHash,QString("/PIM/Contact/") + QString::number(i) + "/lastName",
                              uidPrefix + 0x2));
        QVERIFY(verifyKeyData(keyHash,QString("/PIM/Contact/") + QString::number(i) + "/group",
                              uidPrefix + 0xf00f));
    }

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::bitmaskKey()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test3.qcrml"));
    QCOMPARE(keyData.count(), 4);
    QHash<QString, KeyData> keyHash = makeHash(keyData);

    quint64 uid = Q_UINT64_C(0x1234567800000001);

    KeyData::Target target = KeyData::CRepository;
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/Swan"), uid, target, 1));
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/Pearl"), uid, target, 2));
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/Hydra"), uid, target,32));
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/LookingGlass"), uid, target, 4));

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::keyWithUnknownElements()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test4.qcrml"));
    QCOMPARE(keyData.count(), 3);
    QHash<QString, KeyData> keyHash = makeHash(keyData);

    KeyData::Target target  = KeyData::RProperty;
    QVERIFY(verifyKeyData(keyHash, "/sensors/accelerometer/x",
                          Q_UINT64_C(0x4815162300000000),target));
    QVERIFY(verifyKeyData(keyHash, "/sensors/accelerometer/y",
                          Q_UINT64_C(0x4815162300000001),target));
    QVERIFY(verifyKeyData(keyHash, "/sensors/accelerometer/z",
                          Q_UINT64_C(0x4815162300000002),target));

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::keyRangeNoSequence()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test5.qcrml"));
    QCOMPARE(keyData.count(), 65535);
    QHash<QString,KeyData> keyHash = makeHash(keyData);

    quint64 repoUid = Q_UINT64_C(0x8765432100000000);
    quint32 firstAddr = 0x1;
    quint64 uidPrefix = repoUid + firstAddr;

    for (int i = 0; i < 65535; ++i) {
        QVERIFY(verifyKeyData(keyHash, QString("/MyFeature/MyNumbers/") + QString::number(i),
                        uidPrefix + i));
    }

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::singleKey()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test6.qcrml"));
    QCOMPARE(keyData.count(), 1);
    QHash<QString, KeyData> keyHash = makeHash(keyData);
    QVERIFY(verifyKeyData(keyHash, "/compass",
                          Q_UINT64_C(0x1111222298765432)));

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::multipleKeyRanges()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test7.qcrml"));
    QCOMPARE(keyData.count(),33 );
    QHash<QString, KeyData> keyHash = makeHash(keyData);

    for(int i = 0x8; i <=0xC; ++i) {
        QVERIFY(verifyKeyData(keyHash, QString("/section31/") + QString::number(i - 0x8,10),
                                    Q_UINT64_C(0x1212989800000000)+i));
    }

    for(int i = 0xE; i <= 0x13; ++i) {
     QVERIFY(verifyKeyData(keyHash, QString("/talshiar/") + QString::number(i - 0xE,10),
                                    Q_UINT64_C(0x1212989800000000)+i));
    }

    quint64 repoUID = Q_UINT64_C(0x1212989800000000);

    quint32 settingsIdentifier = 0x200;
    quint32 itemSettingIndex = 0x020;
    quint64 uidPrefix = 0;

    for(int i = 0; i < 11; i++) {
        uidPrefix = repoUID + settingsIdentifier + itemSettingIndex * i;

        QVERIFY(verifyKeyData(keyHash,QString("/obsidian/") + QString::number(i) + "/name",
                              uidPrefix + 0x1));
        QVERIFY(verifyKeyData(keyHash,QString("/obsidian/") + QString::number(i) + "/rank",
                              uidPrefix + 0x2));
    }

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::multipleBitmaskKeys()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("test8.qcrml"));
    QCOMPARE(keyData.count(), 7);
    QHash<QString, KeyData> keyHash = makeHash(keyData);
    
    KeyData::Target target = KeyData::CRepository;
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/Swan"),Q_UINT64_C(0x1234567800000001),target,1));
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/Pearl"),Q_UINT64_C(0x1234567800000001),target,2));
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/Hydra"),Q_UINT64_C(0x1234567800000001),target,32));
    QVERIFY(verifyKeyData(keyHash, QString("/Dharma/LookingGlass"),Q_UINT64_C(0x1234567800000001),target,4));

    QVERIFY(verifyKeyData(keyHash, QString("/Ben"),Q_UINT64_C(0x123456780000000A),target,2));
    QVERIFY(verifyKeyData(keyHash, QString("/Tom"),Q_UINT64_C(0x123456780000000A),target,4));
    QVERIFY(verifyKeyData(keyHash, QString("/Ethan"),Q_UINT64_C(0x123456780000000A),target,5));

    for (int i = 0; i < keyData.count(); ++i) {
        QVERIFY(keyData.at(i).keyId() == (keyData.at(i).uid() & 0xFFFFFFFF));
        QVERIFY(keyData.at(i).repoId() == (keyData.at(i).uid() >> 32));
    }
}

void tst_QCrmlParser::wrongXmlFile()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("error1.qcrml"));
    QVERIFY(parser.error() == QCrmlParser::ParseError);
}

void tst_QCrmlParser::noRepositoryUidValue()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("error2.qcrml"));
    QVERIFY(parser.error() == QCrmlParser::ParseError);
}

void tst_QCrmlParser::keyMissingEndTag()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("error3.qcrml"));
    QVERIFY(parser.error() == QCrmlParser::ParseError);
}


void tst_QCrmlParser::outOfPlaceEndTag()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("error4.qcrml"));
    QVERIFY(parser.error() == QCrmlParser::ParseError);
    QVERIFY(keyData.count() == 0);
}

void tst_QCrmlParser::invalidTarget()
{
    QCrmlParser parser;
    QList<KeyData> keyData;
    keyData = parser.parseQCrml(testData.absoluteFilePath("error5.qcrml"));
    QVERIFY(parser.error() == QCrmlParser::ParseError);
    QVERIFY(keyData.count() == 0);
}

bool tst_QCrmlParser::verifyKeyData(const QHash<QString,KeyData> &keyHash, const QString &key,
                    quint64 uid, KeyData::Target target, quint32 bitIndex) {
    KeyData data = keyHash.value(key);

    if (data.target() != target) {
        qWarning() << "Target does not match, expected" << target
                << "\n actual:" << data.target();
        return false;
    }

    if ( data.uid()!= uid ) {
        qWarning() << "UID does not match expected:" << QString::number(uid,16) << "\n actual:"
                << QString::number(data.uid(),16);
        return false;
    }

    if (data.bitIndex() != bitIndex) {
        qWarning() << "BitIndex does not match, expected: " << QString::number(bitIndex,16)
                << "\n actual:" << QString::number(data.bitIndex(),16);
        return false;
    }

    return true;
}

QHash<QString,KeyData> tst_QCrmlParser::makeHash(const QList<KeyData> &keyList)
{
    QHash<QString,KeyData> rv;
    for (int i = 0; i < keyList.count(); ++i) {
        rv.insert(keyList.at(i).path(), keyList.at(i));
    }
    return rv;
}

void tst_QCrmlParser::printKeyList(const QList<KeyData> &keyList)
{
    for (int i=0; i < keyList.count(); ++i) {
        qDebug() << "KeyPath = " << keyList.at(i).path();
        qDebug() << "UID = " << QString::number(keyList.at(i).uid(), 16);
        qDebug() << "bitIndex"<< QString::number(keyList.at(i).bitIndex());
    }
}

QTEST_MAIN(tst_QCrmlParser)
#include "tst_qcrmlparser.moc"
