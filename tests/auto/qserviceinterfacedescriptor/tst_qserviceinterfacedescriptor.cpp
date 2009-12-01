/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QtTest/QtTest>
#include <QtCore>
#ifndef QT_NO_DEBUG_STREAM
#include <QDebug>
#endif
#include <qserviceinterfacedescriptor.h>
#include <qserviceinterfacedescriptor_p.h>

QTM_USE_NAMESPACE
class tst_QServiceInterfaceDescriptor: public QObject
{
    Q_OBJECT
    
private slots:
    void initTestCase();
    void cleanupTestCase();
    void comparison();
#ifndef QT_NO_DATASTREAM
    void testStreamOperators();
#endif
#ifndef QT_NO_DEBUG_STREAM
    void testDebugStream();
#endif
    void destructor();
};

void tst_QServiceInterfaceDescriptor::initTestCase()
{
}

void tst_QServiceInterfaceDescriptor::comparison()
{
    QServiceInterfaceDescriptor desc;
    QVERIFY(desc.majorVersion() == -1);
    QVERIFY(desc.minorVersion() == -1);
    QVERIFY(desc.serviceName().isEmpty());
    QVERIFY(desc.interfaceName().isEmpty());
    QVERIFY(!desc.property(QServiceInterfaceDescriptor::Capabilities).isValid());
    QVERIFY(!desc.property(QServiceInterfaceDescriptor::Location).isValid());
    QVERIFY(!desc.property(QServiceInterfaceDescriptor::InterfaceDescription).isValid());
    QVERIFY(!desc.property(QServiceInterfaceDescriptor::ServiceDescription).isValid());
    QVERIFY(!desc.inSystemScope());
    QVERIFY(!desc.isValid());

    QServiceInterfaceDescriptor copy(desc);
    QVERIFY(copy.majorVersion() == -1);
    QVERIFY(copy.minorVersion() == -1);
    QVERIFY(copy.serviceName().isEmpty());
    QVERIFY(copy.interfaceName().isEmpty());
    QVERIFY(!copy.property(QServiceInterfaceDescriptor::Capabilities).isValid());
    QVERIFY(!copy.property(QServiceInterfaceDescriptor::Location).isValid());
    QVERIFY(!copy.property(QServiceInterfaceDescriptor::InterfaceDescription).isValid());
    QVERIFY(!copy.property(QServiceInterfaceDescriptor::ServiceDescription).isValid());
    QVERIFY(!desc.inSystemScope());
    QVERIFY(!copy.isValid());

    QVERIFY(desc == copy);

    QServiceInterfaceDescriptor valid;
    QServiceInterfaceDescriptorPrivate *d = new QServiceInterfaceDescriptorPrivate();
    QServiceInterfaceDescriptorPrivate::setPrivate(&valid, d);
    d->serviceName = "name";
    d->interfaceName = "interface";
    d->major = 3;
    d->minor = 1;
    d->properties.insert(QServiceInterfaceDescriptor::ServiceDescription, QString("mydescription"));
    d->customProperties.insert(QString("ckey"), QString("cvalue"));
    d->systemScope = true;

    QCOMPARE(valid.interfaceName(), QString("interface"));
    QCOMPARE(valid.serviceName(), QString("name"));
    QCOMPARE(valid.majorVersion(), 3);
    QCOMPARE(valid.minorVersion(), 1);
    QCOMPARE(valid.customProperty("ckey"), QString("cvalue"));
    QCOMPARE(valid.property(QServiceInterfaceDescriptor::ServiceDescription).toString(), QString("mydescription"));
    QCOMPARE(valid.property(QServiceInterfaceDescriptor::Location).toString(), QString(""));
    QCOMPARE(valid.inSystemScope(), true);
    QVERIFY(valid.isValid());

    QVERIFY(valid != desc);
    QVERIFY(desc != valid);

    //test copy constructor
    QServiceInterfaceDescriptor validCopy(valid);
    QVERIFY(valid==validCopy);
    QVERIFY(validCopy==valid);

    QServiceInterfaceDescriptorPrivate::getPrivate(&validCopy)->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue"));
    QVERIFY(valid!=validCopy);
    QVERIFY(validCopy!=valid);

    QCOMPARE(validCopy.interfaceName(), QString("interface"));
    QCOMPARE(validCopy.serviceName(), QString("name"));
    QCOMPARE(validCopy.majorVersion(), 3);
    QCOMPARE(validCopy.minorVersion(), 1);
    QCOMPARE(validCopy.property(QServiceInterfaceDescriptor::Location).toString(), QString("myValue"));
    QCOMPARE(validCopy.property(QServiceInterfaceDescriptor::ServiceDescription).toString(), QString("mydescription"));
    QCOMPARE(validCopy.customProperty("ckey"),QString("cvalue"));
    QCOMPARE(validCopy.inSystemScope(), true);
    QVERIFY(validCopy.isValid());

    //test assignment operator
    QServiceInterfaceDescriptor validCopy2 = valid;
    QVERIFY(valid==validCopy2);
    QVERIFY(validCopy2==valid);

    QServiceInterfaceDescriptorPrivate::getPrivate(&validCopy2)->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue2"));
    QVERIFY(valid!=validCopy2);
    QVERIFY(validCopy2!=valid);

    QCOMPARE(validCopy2.interfaceName(), QString("interface"));
    QCOMPARE(validCopy2.serviceName(), QString("name"));
    QCOMPARE(validCopy2.majorVersion(), 3);
    QCOMPARE(validCopy2.minorVersion(), 1);
    QCOMPARE(validCopy2.property(QServiceInterfaceDescriptor::Location).toString(), QString("myValue2"));
    QCOMPARE(validCopy2.customProperty("ckey"),QString("cvalue"));
    QCOMPARE(validCopy2.property(QServiceInterfaceDescriptor::ServiceDescription).toString(), QString("mydescription"));
    QCOMPARE(validCopy2.inSystemScope(), true);
    QVERIFY(validCopy2.isValid());

    //test customPropertyKeys
    d->customProperties.insert(QString("ckey"), QString("cvalue"));
    d->customProperties.insert(QString("ckey1"), QString("cvalue1"));
    d->customProperties.insert(QString("ckey2"), QString("cvalue2"));
    QStringList customPropertyKeys = valid.customPropertyKeys();
    QVERIFY(customPropertyKeys.contains("ckey"));
    QVERIFY(customPropertyKeys.contains("ckey1"));
    QVERIFY(customPropertyKeys.contains("ckey2"));
    QCOMPARE(customPropertyKeys.count(), 3);
}

#ifndef QT_NO_DATASTREAM
void tst_QServiceInterfaceDescriptor::testStreamOperators()
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::ReadWrite);
    QDataStream stream(&buffer);


    QServiceInterfaceDescriptor empty;
    QVERIFY(!empty.isValid());

    //stream invalid into invalid
    QServiceInterfaceDescriptor invalid;
    QVERIFY(!invalid.isValid());
    QVERIFY(invalid == empty);
    buffer.seek(0);
    stream << empty;
    buffer.seek(0);
    stream >> invalid;
    QVERIFY(invalid == empty);

    //stream invalid into valid
    QServiceInterfaceDescriptor valid;
    QServiceInterfaceDescriptorPrivate *d = new QServiceInterfaceDescriptorPrivate();
    QServiceInterfaceDescriptorPrivate::setPrivate(&valid, d);
    d->serviceName = "name";
    d->interfaceName = "interface";
    d->major = 3;
    d->minor = 1;
    d->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue"));
    d->properties.insert(QServiceInterfaceDescriptor::Capabilities, QStringList() << "val1" << "val2");
    d->properties.insert(QServiceInterfaceDescriptor::ServiceDescription, QString("This is the service description"));
    d->properties.insert(QServiceInterfaceDescriptor::InterfaceDescription, QString("This is the interface description"));
    d->customProperties.insert(QString("key1"), QString("value1"));
    d->customProperties.insert(QString("abcd"), QString("efgh"));
    d->customProperties.insert(QString("empty"), QString(""));
    d->systemScope = true;
    QVERIFY(valid.isValid());
    QServiceInterfaceDescriptor validref = valid;
    QVERIFY(validref == valid);
    QVERIFY(!(validref!=valid));
    QVERIFY(empty!=validref);

    buffer.seek(0);
    stream << empty;
    buffer.seek(0);
    stream >> validref;
    QVERIFY(empty == validref);
    QVERIFY(!(empty!=validref));
    QVERIFY(validref != valid);
    
    //stream valid into invalid
    QServiceInterfaceDescriptor invalid2;
    QVERIFY(!invalid2.isValid());
    validref = valid;
    QVERIFY(validref == valid);
    QVERIFY(validref != invalid2);

    buffer.seek(0);
    stream << validref;
    buffer.seek(0);
    stream >> invalid2;
    QVERIFY(invalid2 == validref);
    QVERIFY(!(invalid2 != validref));
    QVERIFY(invalid2.isValid());
    QVERIFY(invalid2.interfaceName() == QString("interface"));
    QVERIFY(invalid2.serviceName() == QString("name"));
    QVERIFY(invalid2.majorVersion() == 3);
    QVERIFY(invalid2.minorVersion() == 1);
    QVERIFY(invalid2.property(QServiceInterfaceDescriptor::Location).toString() == QString("myValue"));
    QVERIFY(invalid2.property(QServiceInterfaceDescriptor::Capabilities).toStringList() == (QStringList() << "val1" << "val2"));
    QVERIFY(invalid2.property(QServiceInterfaceDescriptor::ServiceDescription).toString() == QString("This is the service description"));
    QVERIFY(invalid2.property(QServiceInterfaceDescriptor::InterfaceDescription).toString() == QString("This is the interface description"));
    QCOMPARE(invalid2.customProperty("key1"), QString("value1"));
    QCOMPARE(invalid2.customProperty("abcd"), QString("efgh"));
    QCOMPARE(invalid2.customProperty("notvalid"), QString());
    QVERIFY(invalid2.customProperty("notvalid").isEmpty());
    QVERIFY(invalid2.customProperty("notvalid").isNull());
    QCOMPARE(invalid2.customProperty("empty"), QString(""));
    QVERIFY(invalid2.customProperty("empty").isEmpty());
    QVERIFY(!invalid2.customProperty("empty").isNull());
    QCOMPARE(invalid2.inSystemScope(), true);

    //stream valid into valid
    QServiceInterfaceDescriptor valid2;
    QServiceInterfaceDescriptorPrivate *d2 = new QServiceInterfaceDescriptorPrivate();
    QServiceInterfaceDescriptorPrivate::setPrivate(&valid2, d2);
    d2->serviceName = "name2";
    d2->interfaceName = "interface2";
    d2->major = 5;
    d2->minor = 6;
    d2->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue1"));
    d2->properties.insert(QServiceInterfaceDescriptor::Capabilities, QStringList() << "val3" << "val4");
    d2->properties.insert(QServiceInterfaceDescriptor::ServiceDescription, QString("This is the second service description"));
    d2->properties.insert(QServiceInterfaceDescriptor::InterfaceDescription, QString("This is the second interface description"));
    d2->customProperties.insert(QString("key1"), QString("value2"));
    d2->customProperties.insert(QString("abcd1"), QString("efgh"));
    d2->customProperties.insert(QString("empty"), QString(""));
    d2->systemScope = false;
    QVERIFY(valid2.isValid());
    QCOMPARE(valid2.customProperty("key1"), QString("value2"));
    QCOMPARE(valid2.customProperty("abcd1"), QString("efgh"));
    QCOMPARE(valid2.customProperty("abcd"), QString());
    QVERIFY(valid2.customProperty("abcd").isEmpty());
    QVERIFY(valid2.customProperty("abcd").isNull());
    QCOMPARE(valid2.customProperty("empty"), QString(""));
    QVERIFY(valid2.customProperty("empty").isEmpty());
    QVERIFY(!valid2.customProperty("empty").isNull());


    QVERIFY(valid2 != valid);
    QVERIFY(!(valid2 == valid));

    buffer.seek(0);
    stream << valid;
    buffer.seek(0);
    stream >> valid2;
    QVERIFY(valid2 == valid);
    QVERIFY(!(valid2 != valid));
    QVERIFY(valid2.isValid());
    QVERIFY(valid2.interfaceName() == QString("interface"));
    QVERIFY(valid2.serviceName() == QString("name"));
    QVERIFY(valid2.majorVersion() == 3);
    QVERIFY(valid2.minorVersion() == 1);
    QVERIFY(valid2.property(QServiceInterfaceDescriptor::Location).toString() == QString("myValue"));
    QVERIFY(valid2.property(QServiceInterfaceDescriptor::Capabilities).toStringList() == (QStringList() << "val1" << "val2"));
    QVERIFY(valid2.property(QServiceInterfaceDescriptor::ServiceDescription).toString() == QString("This is the service description"));
    QVERIFY(valid2.property(QServiceInterfaceDescriptor::InterfaceDescription).toString() == QString("This is the interface description"));
    QCOMPARE(valid2.customProperty("key1"), QString("value1"));
    QCOMPARE(valid2.customProperty("abcd"), QString("efgh"));
    QCOMPARE(valid2.customProperty("notvalid"), QString());
    QVERIFY(valid2.customProperty("notvalid").isEmpty());
    QVERIFY(valid2.customProperty("notvalid").isNull());
    QCOMPARE(valid2.customProperty("empty"), QString(""));
    QVERIFY(valid2.customProperty("empty").isEmpty());
    QVERIFY(!valid2.customProperty("empty").isNull());
    QCOMPARE(valid2.customProperty("abcd1"), QString());
    QCOMPARE(valid2.inSystemScope(), true);
}
#endif //QT_NO_DATASTREAM

#ifndef QT_NO_DEBUG_STREAM
static QByteArray msg;
static QtMsgType type;

static void customMsgHandler(QtMsgType t, const char* m)
{
    msg = m;
    type = t;

}

void tst_QServiceInterfaceDescriptor::testDebugStream()
{
    QServiceInterfaceDescriptor valid2;
    QServiceInterfaceDescriptorPrivate *d2 = new QServiceInterfaceDescriptorPrivate();
    QServiceInterfaceDescriptorPrivate::setPrivate(&valid2, d2);
    d2->serviceName = "name2";
    d2->interfaceName = "interface2";
    d2->major = 5;
    d2->minor = 6;
    d2->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue1"));
    d2->properties.insert(QServiceInterfaceDescriptor::Capabilities, QStringList() << "val3" << "val4");
    d2->properties.insert(QServiceInterfaceDescriptor::ServiceDescription, QString("This is the second service description"));
    d2->properties.insert(QServiceInterfaceDescriptor::InterfaceDescription, QString("This is the second interface description"));
    QVERIFY(valid2.isValid());

    QServiceInterfaceDescriptor invalid;

    qInstallMsgHandler(customMsgHandler);
    qDebug() << valid2 << invalid;
    QCOMPARE(type, QtDebugMsg);
    QCOMPARE(QString::fromLatin1(msg.data()),QString::fromLatin1("QServiceInterfaceDescriptor(service=\"name2\", interface=\"interface2 5.6\") QServiceInterfaceDescriptor(invalid) "));
    qInstallMsgHandler(0);
}
#endif

void tst_QServiceInterfaceDescriptor::destructor()
{
    //test destructor if descriptor is invalid
    QServiceInterfaceDescriptor* invalid = new QServiceInterfaceDescriptor();
    delete invalid;

    //test destructor if descriptor is valid
    QServiceInterfaceDescriptor* valid = new QServiceInterfaceDescriptor();
    QServiceInterfaceDescriptorPrivate *d = new QServiceInterfaceDescriptorPrivate();
    QServiceInterfaceDescriptorPrivate::setPrivate(valid, d);
    d->serviceName = "name";
    d->interfaceName = "interface";
    d->major = 3;
    d->minor = 1;
    d->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue"));
    d->properties.insert(QServiceInterfaceDescriptor::Capabilities, QStringList() << "val1" << "val2");
    d->properties.insert(QServiceInterfaceDescriptor::ServiceDescription, QString("This is the service description"));
    d->properties.insert(QServiceInterfaceDescriptor::InterfaceDescription, QString("This is the interface description"));
    d->customProperties.insert("ckey", "cvalue");
    QVERIFY(valid->isValid());
    delete valid;
}

void tst_QServiceInterfaceDescriptor::cleanupTestCase()
{
}

QTEST_MAIN(tst_QServiceInterfaceDescriptor)
#include "tst_qserviceinterfacedescriptor.moc"
