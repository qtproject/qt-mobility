/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the either Technology Preview License Agreement or the
** Beta Release License Agreement.
**
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
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QtTest/QtTest>
#include <QtCore>
#include <qserviceinterfacedescriptor.h>
#include <qserviceinterfacedescriptor_p.h>

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
    QVERIFY(!copy.isValid());

    QVERIFY(desc == copy);

    QServiceInterfaceDescriptor valid;
    QServiceInterfaceDescriptorPrivate *d = new QServiceInterfaceDescriptorPrivate();
    QServiceInterfaceDescriptorPrivate::setPrivate(&valid, d);
    d->serviceName = "name";
    d->interfaceName = "interface";
    d->major = 3;
    d->minor = 1;

    QCOMPARE(valid.interfaceName(), QString("interface"));
    QCOMPARE(valid.serviceName(), QString("name"));
    QCOMPARE(valid.majorVersion(), 3);
    QCOMPARE(valid.minorVersion(), 1);
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
    QVERIFY(validCopy.isValid());

    //test assignment operator
    QServiceInterfaceDescriptor validCopy2 = valid;
    QVERIFY(valid==validCopy2);
    QVERIFY(validCopy2==valid);

    QServiceInterfaceDescriptorPrivate::getPrivate(&validCopy2)->properties.insert(QServiceInterfaceDescriptor::Location, QString("myValue"));
    QVERIFY(valid!=validCopy2);
    QVERIFY(validCopy2!=valid);

    QCOMPARE(validCopy2.interfaceName(), QString("interface"));
    QCOMPARE(validCopy2.serviceName(), QString("name"));
    QCOMPARE(validCopy2.majorVersion(), 3);
    QCOMPARE(validCopy2.minorVersion(), 1);
    QCOMPARE(validCopy2.property(QServiceInterfaceDescriptor::Location).toString(), QString("myValue"));
    QVERIFY(validCopy2.isValid());

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
    QVERIFY(valid2.isValid());
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
}
#endif //QT_NO_DATASTREAM

void tst_QServiceInterfaceDescriptor::cleanupTestCase()
{
}

QTEST_MAIN(tst_QServiceInterfaceDescriptor)
#include "tst_qserviceinterfacedescriptor.moc"
