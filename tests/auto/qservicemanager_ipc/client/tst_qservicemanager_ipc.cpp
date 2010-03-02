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

#include <QCoreApplication>
#include "qservicemanager.h"
#include "qservicefilter.h"
#include <QTimer>
#include <QMetaObject>
#include <QMetaMethod>
#include <QtTest/QtTest>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QServiceFilter)

class Test : public QObject 
{
    Q_OBJECT
public:
    Test() : QObject()
    {
        QServiceManager manager;
        service = manager.loadInterface("com.nokia.qt.ipcunittest");
        if (!service)  {
            qWarning() << "Cannot find service. Error:" << manager.error();
            QTimer::singleShot(1000, this, SLOT(killProcess()));
        } else {
            checkServiceObject();
        }
    }

Q_SIGNALS:
    void clientsignal();

public slots:
    void checkServiceObject()
    {
        const QMetaObject* mo = service->metaObject();
        qDebug() << "ServiceObject class: " << mo->className() << mo->superClass() << mo->superClass()->className();
        qDebug() << "------------------- Meta Methods -----------";
        qDebug() << "Methods:" << mo->methodCount()- mo->methodOffset() << "(" << mo->methodCount() << ")";
        for (int i=0; i< mo->methodCount(); i++) {
            QMetaMethod method = mo->method(i);
            QString type;
            switch(method.methodType()) {
            case QMetaMethod::Signal:
                type = "signal"; break;
            case QMetaMethod::Slot:
                type = "slot"; break;
            case QMetaMethod::Constructor:
                type = "constrcutor"; break;
            case QMetaMethod::Method:
                type = "method"; break;
            }
            qDebug() << "    " << i << "." << method.signature() << type;
        }
        qDebug() << "------------------- Meta Properties --------";
        qDebug() << "Properties:" << mo->propertyCount()- mo->propertyOffset() << "(" << mo->propertyCount() << ")";
        for(int i=0; i< mo->propertyCount(); i++) {
            QMetaProperty property = mo->property(i);
            QString info = "Readable: %1 Resettable: %2 Writeable: %3 Designable: %4 Scriptable: %5 User: %6 Stored: %7 Constant: %8 Final: %9 HasNotify: %10 EnumType: %11 FlagType: %12";
            info = info.arg(property.isReadable()).arg(property.isResettable()).arg(property.isWritable());
            info = info.arg(property.isDesignable()).arg(property.isScriptable()).arg(property.isUser());
            info = info.arg(property.isStored()).arg(property.isConstant()).arg(property.isFinal());
            info = info.arg(property.hasNotifySignal()).arg(property.isEnumType()).arg(property.isFlagType());

            qDebug() << "    " << i << "." << property.name() << "Type:" << property.typeName() << info;
        }

        qDebug() << "------------------- Meta Enumerators --------";
        qDebug() << "Enums:" << mo->enumeratorCount()- mo->enumeratorOffset() << "(" << mo->enumeratorCount() << ")";
        for(int i=0; i< mo->enumeratorCount(); i++) {
            QMetaEnum e = mo->enumerator(i);
            qDebug() << "    " << i << "." << e.name() << "Scope:" << e.scope() << "KeyCount: " << e.keyCount();
            for(int j = 0; j<e.keyCount(); j++)
                qDebug() << "         " << e.key(j) << " - " << e.value(j);
        }

        qDebug() << "------------------- Meta class info ---------";
        qDebug() << "ClassInfos:" << mo->classInfoCount()- mo->classInfoOffset() << "(" << mo->classInfoCount() << ")";
        for(int i=0; i< mo->classInfoCount(); i++) {
            QMetaClassInfo info = mo->classInfo(i);
            qDebug() << "    " << i << "." << info.name() << "Value:" << info.value();
        }

        qDebug() << "------------------- Testing remote calls ---------";


        QTimer::singleShot(1000, this, SLOT(useService()));
    }

    void useService()
    {
        qDebug() << "Invoking testSlot()";
        QMetaObject::invokeMethod( service, "testSlot" );

        qDebug() << "Invoking testSlotWithArgs(QByteArray, int, QVariant) - default variant";
        QVariant test;
        QMetaObject::invokeMethod( service, "testSlotWithArgs",
              Q_ARG(QByteArray, "array"), Q_ARG(int, 5), Q_ARG(QVariant, test));

        qDebug() << "Invoking testSlotWithArgs(QByteArray, int, QVariant) ";
        test = QVariant(QString("teststring"));
        QMetaObject::invokeMethod( service, "testSlotWithArgs",
              Q_ARG(QByteArray, "array"), Q_ARG(int, 5), Q_ARG(QVariant, test));
        QServiceFilter filter("com.myInterface" , "4.5");
        filter.setServiceName("MyService");

        qDebug() << "Invoking testSlotWithCustomArg(QServiceFilter)";
        QMetaObject::invokeMethod( service, "testSlotWithCustomArg",
              Q_ARG(QServiceFilter, filter));

        //we expect this to fail
        QServiceInterfaceDescriptor desc;
        qDebug() << "Invoking testSlotWithUnknownArg(QServiceInterfaceDescriptor)";
        QMetaObject::invokeMethod( service, "testSlotWithUnknownArg",
              Q_ARG(QServiceInterfaceDescriptor, desc));

        //test known return type
        QString result;
        QString o("Invoking testFunctionWithReturnValue( int i = %1 ) returned '%2'");
        QMetaObject::invokeMethod(service, "testFunctionWithReturnValue",
              Q_RETURN_ARG(QString, result), Q_ARG(int, 4));
        o = o.arg(4).arg(result);
        qDebug() << o;

        //test QVariant return types
        QVariant varResult;
        o = "Invoking testFunctionWithVariantReturnValue() returned '%1'";
        QMetaObject::invokeMethod(service, "testFunctionWithVariantReturnValue",
              Q_RETURN_ARG(QVariant, varResult));
        o = o.arg(varResult.toInt()); //should return false
        qDebug() << o;

        //test custom return type
        QServiceFilter f;
        QMetaObject::invokeMethod(service, "testFunctionWithCustomReturnValue",
              Q_RETURN_ARG(QServiceFilter, f));
        o = "Invoking testFunctionWithCustomReturnValue return: %1: %2 - %3.%4";
        o = o.arg(f.serviceName()).arg(f.interfaceName()).arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << o;


        //signal support from service to client
        //we'll call a function which will trigger the signal on the server side
        connect(service, SIGNAL(signalWithIntParam(int)), this, SLOT(slotSignalWithIntParam(int)));
        QMetaObject::invokeMethod(service, "triggerSignalWithIntParam");

    }

    void slotSignalWithIntParam(int i)
    {
        qDebug() << "signalWithIntParam(int) received. value:" << i;
        
        //test signal with QVariant type
        connect(service, SIGNAL(signalWithVariousParam(QVariant,QString,QServiceFilter)), this, SLOT(slotSignalWithVariousParam(QVariant,QString,QServiceFilter)));
        QMetaObject::invokeMethod(service, "triggerSignalWithVariousParam");
    }

    void slotSignalWithVariousParam(const QVariant& v, const QString& s, const QServiceFilter& f)
    {
        QString o = "%1: %2 - %3.%4";
        o = o.arg(f.serviceName()).arg(f.interfaceName()).arg(f.majorVersion()).arg(f.minorVersion());
        qDebug() << "signalWithVariousParam(QVariant,QString,QServiceFilter) received. values:" << v << s << o;

        //signal support from client to service
        connect(this, SIGNAL(clientsignal()), service, SLOT(testSlot()));
        QTimer::singleShot(2000, this, SLOT(triggerClientSignal()));
    }

    void triggerClientSignal()
    {
        qDebug() << "Triggering signal on client side";
        emit clientsignal();

        int pIndex = service->metaObject()->indexOfProperty("value");
        QMetaProperty prop = service->metaObject()->property(pIndex);
        if (prop.isValid()) {
            if ( prop.hasNotifySignal())
                QMetaObject::connect(service, prop.notifySignalIndex(), this, metaObject()->indexOfMethod("propertyChanged()"));
            qDebug() << "--------------------------------------------";
            qDebug() << "Property value:" << service->property("value");
            qDebug() << "Changing property to: " << "QWERTY";
            service->setProperty("value", "QWERTY");
            qDebug() << "New property value:" << service->property("value");
            qDebug() << "Resetting property";
            prop.reset(service);
            qDebug() << "New property value:" << service->property("value");
        }

        pIndex = service->metaObject()->indexOfProperty("priority");
        prop = service->metaObject()->property(pIndex);
        if (prop.isValid()) {
            if ( prop.hasNotifySignal())
                QMetaObject::connect(service, prop.notifySignalIndex(), this, metaObject()->indexOfMethod("propertyChanged()"));
            qDebug() << "--------------------------------------------";
            qDebug() << "Property value:" << service->property("priority");
            qDebug() << "Changing property to: " << "Low";
            service->setProperty("priority", "Low");
            qDebug() << "New property value:" << service->property("priority");
        }

        pIndex = service->metaObject()->indexOfProperty("serviceFlags");
        prop = service->metaObject()->property(pIndex);
        if (prop.isValid()) {
            if ( prop.hasNotifySignal())
                QMetaObject::connect(service, prop.notifySignalIndex(), this, metaObject()->indexOfMethod("propertyChanged()"));
            qDebug() << "--------------------------------------------";
            qDebug() << "Property value:" << service->property("serviceFlags");
            qDebug() << "Changing property to: " << "101";
            service->setProperty("serviceFlags", "FirstBit|ThirdBit");
            qDebug() << "New property value:" << service->property("serviceFlags");
        }



        QTimer::singleShot(1000, this, SLOT(killService()));
    }

    void propertyChanged()
    {
        qDebug() << "Property change detected";
    }

    void killService()
    {
        delete service;
        QTimer::singleShot(1000, this, SLOT(killProcess()));
    }

    void killProcess()
    {
        QTimer::singleShot(1000, qApp, SLOT(quit()));
    }

private:
    QObject* service;
};

class tst_QServiceManager_IPC: public QObject
{
    Q_OBJECT
public:
    bool requiresLackey();
private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void verifyTransmittedServiceObject(); //rough count
    void verifyMethods();
    void verifyMethods_data();
    void verifyProperties();
    void verifyProperties_data();
private:
    QObject* service;
    QServiceManager* manager;
    bool verbose;
    QProcess* lackey;
};

/*
    TODO:
    -Test service with multiple inheritance hierarchy
*/

bool tst_QServiceManager_IPC::requiresLackey()
{
#ifdef Q_OS_SYMBIAN
    return false; //service is started when requested
#else
    return true;
#endif
}

void tst_QServiceManager_IPC::initTestCase()
{
    verbose = true;
    lackey = 0;
    service = 0;
    qRegisterMetaType<QServiceFilter>();
    qRegisterMetaTypeStreamOperators<QServiceFilter>("QServiceFilter");
    QServiceManager* manager = new QServiceManager(this);

    //start lackey that represents the service
    if (requiresLackey()) {
        lackey = new QProcess(this);
        lackey->start("./qservicemanager_ipc_service");
        qDebug() << lackey->error() << lackey->errorString();
        QVERIFY(lackey->waitForStarted());
        //Give the lackey some time to come up;
        QTest::qWait(500);
    }

    //test that the service is installed
    service = manager->loadInterface("com.nokia.qt.ipcunittest");
    QString errorCode = "Cannot find service. Error: %1";
    errorCode = errorCode.arg(manager->error());
    QVERIFY2(service,errorCode.toLatin1());
}

void tst_QServiceManager_IPC::cleanupTestCase()
{
    if (service) {
        delete service;
    }
    
    if (requiresLackey()) {
        lackey->terminate();
        lackey->waitForFinished();
        switch(lackey->exitCode()) {
        case 0:
            qDebug("Lackey returned exit success(0)");
            break; 
        default:
            qDebug("Lackey failed.");
            break;
        }
    }
}

void tst_QServiceManager_IPC::init()
{
}

void tst_QServiceManager_IPC::cleanup()
{
}

void tst_QServiceManager_IPC::verifyTransmittedServiceObject()
{
    const QMetaObject* mo = service->metaObject();
    QCOMPARE(mo->className(), "UniqueTestService");
    QVERIFY(mo->superClass());
    QCOMPARE(mo->superClass()->className(), "QObject");
    QCOMPARE(mo->methodCount()-mo-> methodOffset(), 15);
    QCOMPARE(mo->methodCount(), 19); //19 meta functions available
    //actual function presence will be tested later

    //test properties
    QCOMPARE(mo->propertyCount()-mo->propertyOffset(), 3);
    QCOMPARE(mo->propertyCount(), 4);

    QCOMPARE(mo->enumeratorCount()-mo->enumeratorOffset(), 3);
    QCOMPARE(mo->enumeratorCount(), 3);
    
    QCOMPARE(mo->classInfoCount()-mo->classInfoOffset(), 2);
    QCOMPARE(mo->classInfoCount(), 2);
    
    

    if (verbose) {
        qDebug() << "ServiceObject class: " << mo->className() << mo->superClass() << mo->superClass()->className();
        qDebug() << "------------------- Meta Methods -----------";
        qDebug() << "Methods:" << mo->methodCount()- mo->methodOffset() << "(" << mo->methodCount() << ")";
        for (int i=0; i< mo->methodCount(); i++) {
            QMetaMethod method = mo->method(i);
            QString type;
            switch(method.methodType()) {
            case QMetaMethod::Signal:
                type = "signal"; break;
            case QMetaMethod::Slot:
                type = "slot"; break;
            case QMetaMethod::Constructor:
                type = "constrcutor"; break;
            case QMetaMethod::Method:
                type = "method"; break;
            }
            qDebug() << "    " << i << "." << method.signature() << type;
        }

        qDebug() << "------------------- Meta Properties --------";
        qDebug() << "Properties:" << mo->propertyCount()- mo->propertyOffset() << "(" << mo->propertyCount() << ")";
        for(int i=0; i< mo->propertyCount(); i++) {
            QMetaProperty property = mo->property(i);
            QString info = "Readable: %1 Resettable: %2 Writeable: %3 Designable: %4 Scriptable: %5 User: %6 Stored: %7 Constant: %8 Final: %9 HasNotify: %10 EnumType: %11 FlagType: %12";
            info = info.arg(property.isReadable()).arg(property.isResettable()).arg(property.isWritable());
            info = info.arg(property.isDesignable()).arg(property.isScriptable()).arg(property.isUser());
            info = info.arg(property.isStored()).arg(property.isConstant()).arg(property.isFinal());
            info = info.arg(property.hasNotifySignal()).arg(property.isEnumType()).arg(property.isFlagType());

            qDebug() << "    " << i << "." << property.name() << "Type:" << property.typeName() << info;
        }

        qDebug() << "------------------- Meta Enumerators --------";
        qDebug() << "Enums:" << mo->enumeratorCount()- mo->enumeratorOffset() << "(" << mo->enumeratorCount() << ")";
        for(int i=0; i< mo->enumeratorCount(); i++) {
            QMetaEnum e = mo->enumerator(i);
            qDebug() << "    " << i << "." << e.name() << "Scope:" << e.scope() << "KeyCount: " << e.keyCount();
            for(int j = 0; j<e.keyCount(); j++)
                qDebug() << "         " << e.key(j) << " - " << e.value(j);
        }

        qDebug() << "------------------- Meta class info ---------";
        qDebug() << "ClassInfos:" << mo->classInfoCount()- mo->classInfoOffset() << "(" << mo->classInfoCount() << ")";
        for(int i=0; i< mo->classInfoCount(); i++) {
            QMetaClassInfo info = mo->classInfo(i);
            qDebug() << "    " << i << "." << info.name() << "Value:" << info.value();
        }
    }
}

void tst_QServiceManager_IPC::verifyMethods_data()
{
    QTest::addColumn<QByteArray>("signature");
    QTest::addColumn<int>("metaMethodType");
    QTest::addColumn<QByteArray>("returnType");

    //list of all slots, signals and invokable functions
    QTest::newRow("signalWithIntParam(int)") 
        << QByteArray("signalWithIntParam(int)") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("signalWithVariousParam(QVariant,QString,QServiceFilter)") 
        << QByteArray("signalWithVariousParam(QVariant,QString,QServiceFilter)") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("valueChanged()") 
        << QByteArray("valueChanged()") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("priorityChanged()") 
        << QByteArray("priorityChanged()") <<  (int)( QMetaMethod::Signal) << QByteArray("");
    QTest::newRow("triggerSignalWithIntParam()") 
        << QByteArray("triggerSignalWithIntParam()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithVariousParam()") 
        << QByteArray("triggerSignalWithVariousParam()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithIntParamExecute()") 
        << QByteArray("triggerSignalWithIntParamExecute()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("triggerSignalWithVariousParamExecute()") 
        << QByteArray("triggerSignalWithVariousParamExecute()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlot()") 
        << QByteArray("testSlot()") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWithArgs(QByteArray,int,QVariant)") 
        << QByteArray("testSlotWithArgs(QByteArray,int,QVariant)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testSlotWithCustomArg(QServiceFilter)") 
        << QByteArray("testSlotWithCustomArg(QServiceFilter)") <<  (int)( QMetaMethod::Slot) << QByteArray("");

    //QServiceInterfaceDescriptor has not been declared as meta type
    QTest::newRow("testSlotWithUnknownArg(QServiceInterfaceDescriptor)") 
        << QByteArray("testSlotWithUnknownArg(QServiceInterfaceDescriptor)") <<  (int)( QMetaMethod::Slot) << QByteArray("");
    QTest::newRow("testFunctionWithReturnValue(int)") 
        << QByteArray("testFunctionWithReturnValue(int)") <<  (int)( QMetaMethod::Method) << QByteArray("QString");
    QTest::newRow("testFunctionWithVariantReturnValue()") 
        << QByteArray("testFunctionWithVariantReturnValue()") <<  (int)( QMetaMethod::Method) << QByteArray("QVariant");
    QTest::newRow("testFunctionWithCustomReturnValue()") 
        << QByteArray("testFunctionWithCustomReturnValue()") <<  (int)( QMetaMethod::Method) << QByteArray("QServiceFilter");
}

void tst_QServiceManager_IPC::verifyMethods()
{
    QVERIFY(service);
    QFETCH(QByteArray, signature);
    QFETCH(int, metaMethodType);
    QFETCH(QByteArray, returnType);

    const QMetaObject* meta = service->metaObject();
    const int index = meta->indexOfMethod(signature.constData());
    QVERIFY(index>=0);
    QMetaMethod method = meta->method(index);
    QCOMPARE(metaMethodType, (int)method.methodType());
    QCOMPARE(returnType, QByteArray(method.typeName()));

}

void tst_QServiceManager_IPC::verifyProperties_data()
{
    QTest::addColumn<QByteArray>("signature");
    QTest::addColumn<QString>("typeName");
    QTest::addColumn<qint16>("flags");

    /*
        bit order:
            0 isWritable - 1 isUser - 2 isStored - 3 isScriptable 
            4 isResettable - 5 isReadable - 6 isFlagType - 7 isFinal
            8 isEnumType - 9 isDesignable - 10 isConstant - 11 hasNotifySgnal

            for more details see verifyProperties()
    */

    QTest::newRow("value property") << QByteArray("value")
            << QString("QString") << qint16(0x0A3D);

    
}

void tst_QServiceManager_IPC::verifyProperties()
{
    QVERIFY(service);
    QFETCH(QByteArray, signature);
    QFETCH(QString, typeName);
    QFETCH(qint16, flags);

    const QMetaObject* meta = service->metaObject();
    const int index = meta->indexOfProperty(signature.constData());
    QVERIFY(index>=0);
    QMetaProperty property = meta->property(index);
    QVERIFY(property.isValid());
    QCOMPARE(QString(property.typeName()), typeName);

    QVERIFY( property.isWritable() == (bool) (flags & 0x0001) );
    QVERIFY( property.isUser() == (bool) (flags & 0x0002) );
    QVERIFY( property.isStored() == (bool) (flags & 0x0004) );
    QVERIFY( property.isScriptable() == (bool) (flags & 0x0008) );
    QVERIFY( property.isResettable() == (bool) (flags & 0x0010) );
    QVERIFY( property.isReadable() == (bool) (flags & 0x0020) );
    QVERIFY( property.isFlagType() == (bool) (flags & 0x0040) );
    QVERIFY( property.isFinal() == (bool) (flags & 0x0080) );
    QVERIFY( property.isEnumType() == (bool) (flags & 0x0100) );
    QVERIFY( property.isDesignable() == (bool) (flags & 0x0200) );
    QVERIFY( property.isConstant() == (bool) (flags & 0x0400) );
    QVERIFY( property.hasNotifySignal() == (bool) (flags & 0x0800) );

    

}
QTEST_MAIN(tst_QServiceManager_IPC);
#include "tst_qservicemanager_ipc.moc"
