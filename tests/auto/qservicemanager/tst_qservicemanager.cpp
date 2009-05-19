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
#include <qservicemanager.h>
#include <qservicecontext.h>
#include <qabstractsecuritysession.h>
#include <qserviceinterfacedescriptor_p.h>
#include "../../sampleserviceplugin/sampleserviceplugin.h"

typedef QList<QServiceInterfaceDescriptor> ServiceInterfaceDescriptorList;
Q_DECLARE_METATYPE(QServiceFilter)
Q_DECLARE_METATYPE(ServiceInterfaceDescriptorList)

uint qHash(const QServiceInterfaceDescriptor &desc)
{
    return qHash(desc.serviceName()) + qHash(desc.interfaceName()) + desc.majorVersion() * 7 + desc.minorVersion() * 7;
}


class MySecuritySession : public QAbstractSecuritySession
{
public:
    MySecuritySession() {}
    virtual ~MySecuritySession() {}

    virtual bool isAllowed(const QStringList&) { return true; }
};

class MyServiceContext : public QServiceContext
{
public:
    MyServiceContext() {}
    ~MyServiceContext() {}

    virtual void notify(ContextType, const QVariant&) {}
};


class tst_QServiceManager: public QObject
{
    Q_OBJECT

private:
    QString xmlTestDataPath(const QString &xmlFileName)
    {
        return QCoreApplication::applicationDirPath() + "/xmldata/" + xmlFileName;
    }

    QServiceInterfaceDescriptor validDescriptor( const QString& pluginPath ) const
    {
        QServiceInterfaceDescriptorPrivate *priv = new QServiceInterfaceDescriptorPrivate;
        priv->properties[QServiceInterfaceDescriptor::FilePath] = pluginPath;
        priv->interfaceName = "com.nokia.qt.TestInterfaceA";
        QServiceInterfaceDescriptor descriptor;
        QServiceInterfaceDescriptorPrivate::setPrivate(&descriptor, priv);
        return descriptor;
    }

    QByteArray sampleServiceXml() const
    {
        QString xml = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
        xml += "<service name=\"SampleService\" filepath=\"tst_sfw_sampleserviceplugin\" >";
        xml += "<description>tst_QServiceManager test service</description>";
        xml += "<interface name=\"com.nokia.qt.TestInterfaceA\" version=\"1.0\">";
        xml += "  <description>tst_QServiceManager test interface A 1.0</description>";
        xml += "</interface>";
        xml += "<interface name=\"com.nokia.qt.TestInterfaceB\" version=\"1.0\">";
        xml += "    <description>tst_QServiceManager test interface B 1.0</description>";
        xml += "</interface>";
        xml += "<interface name=\"com.nokia.qt.TestInterfaceB\" version=\"2.0\">";
        xml += "    <description>tst_QServiceManager test interface B 2.0</description>";
        xml += "</interface>";
        xml += "<interface name=\"com.nokia.qt.TestInterfaceB\" version=\"2.3\">";
        xml += "    <description>tst_QServiceManager test interface B 2.3</description>";
        xml += "</interface>";
        xml += "</service>";
        return QByteArray(xml.toLatin1());
    }

    QServiceInterfaceDescriptor sampleXmlInterface(const QString &s, int major, int minor) const
    {
        QString version = QString("%1.%2").arg(major).arg(minor);

        QServiceInterfaceDescriptorPrivate *priv = new QServiceInterfaceDescriptorPrivate;
        priv->serviceName = "SampleService";
        priv->interfaceName = "com.nokia.qt.TestInterface" + s;
        priv->major = major;
        priv->minor = minor;
        priv->properties[QServiceInterfaceDescriptor::Capabilities] = QStringList();
        priv->properties[QServiceInterfaceDescriptor::FilePath] = "tst_sfw_sampleserviceplugin";
        priv->properties[QServiceInterfaceDescriptor::ServiceDescription] = "tst_QServiceManager test service";
        priv->properties[QServiceInterfaceDescriptor::InterfaceDescription] = "tst_QServiceManager test interface " + s + " " + version;

        QServiceInterfaceDescriptor desc;
        QServiceInterfaceDescriptorPrivate::setPrivate(&desc, priv);
        return desc;
    }

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();

    void findServices();

    void findInterfaces_filter();
    void findInterfaces_filter_data();

    void loadInterface_string();

    void loadInterface_descriptor();
    void loadInterface_descriptor_data();

    void loadInterface_testLoadedObjectAttributes();

    void getInterface();

    void addService_iodevice();
    void addService_qstring();

    void removeService();

    void setDefaultServiceForInterface_strings();
    void setDefaultServiceForInterface_descriptor();

    void defaultServiceInterface();

    void serviceAdded();
    void serviceAdded_data();

    void serviceRemoved();
    void serviceRemoved_data();
};

void tst_QServiceManager::initTestCase()
{
    /*QSettings s(QSettings::SystemScope, "Trolltech", "App");
    qDebug() << s.fileName();
    for (int i = 0; i<=10; i++)
        qDebug() << QLibraryInfo::location((QLibraryInfo::LibraryLocation)i);*/
}

void tst_QServiceManager::init()
{
    QFile f(QCoreApplication::applicationDirPath() + "/services.db");
    f.remove();

    QSettings settings("com.nokia.qt.serviceframework.tests", "SampleServicePlugin");
    settings.setValue("installed", false);
}

void tst_QServiceManager::cleanupTestCase()
{
    QFile f(QCoreApplication::applicationDirPath() + "/services.db");
    f.remove();
}

void tst_QServiceManager::findServices()
{
    QServiceManager mgr;
    QServiceFilter wildcardFilter;

    QVERIFY(mgr.findServices().isEmpty());
    QVERIFY(mgr.findServices("com.nokia.qt.TestInterfaceA").isEmpty());
    QVERIFY(mgr.findInterfaces(wildcardFilter).count() == 0);

    QByteArray bytes = sampleServiceXml();
    QBuffer buffer(&bytes);
    QVERIFY(mgr.addService(&buffer));

    QCOMPARE(mgr.findServices(), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.invalid.interface") , QStringList());
}

void tst_QServiceManager::findInterfaces_filter_data()
{
    QTest::addColumn<QServiceFilter>("filter");
    QTest::addColumn<ServiceInterfaceDescriptorList>("expectedInterfaces");

    QServiceFilter filter;

    QTest::newRow("empty/wildcard filter") << QServiceFilter() 
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceA");
    QTest::newRow("by interface name (A)") << filter
            << (ServiceInterfaceDescriptorList() << sampleXmlInterface("A",1,0));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB");
    QTest::newRow("by interface name (B)") << filter
            << (ServiceInterfaceDescriptorList() << sampleXmlInterface("B",1,0)
                                                 << sampleXmlInterface("B",2,0)
                                                 << sampleXmlInterface("B",2,3) );

    filter = QServiceFilter();
    filter.setServiceName("SampleService");
    QTest::newRow("by service name") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));
    
    filter = QServiceFilter();
    filter.setInterface("com.invalid.interface");
    QTest::newRow("by non-existing interface name") << filter << ServiceInterfaceDescriptorList();

    filter = QServiceFilter();
    filter.setServiceName("InvalidServiceName");
    QTest::newRow("by non-existing service name") << filter << ServiceInterfaceDescriptorList();


    //version lookup testing for existing interface
    //valid from first version onwards
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.0");
    QTest::newRow("by version name 1.0 DefaultMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.0", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 1.0 MinimumMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.0", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 1.0 ExactMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",1,0));

    //valid with exact version match
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "2.0");
    QTest::newRow("by version name 2.0 DefaultMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "2.0", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 2.0 MinimumMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));
    
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "2.0", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 2.0 ExactMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",2,0));

    //valid but not exact version match
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.9");
    QTest::newRow("by version name 1.9 DefaultMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.9", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 1.9 MinimumMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "1.9", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 1.9 ExactMatch") << filter
            << ServiceInterfaceDescriptorList();

    //version doesn't exist yet
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "3.9");
    QTest::newRow("by version name 3.9 DefaultMatch") << filter
            << ServiceInterfaceDescriptorList();

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "3.9", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name 3.9 MinimumMatch") << filter
            << ServiceInterfaceDescriptorList();

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "3.9", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name 3.9 ExactMatch") << filter
            << ServiceInterfaceDescriptorList();
   
    //invalid version tag 1 -> match anything
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "x3.9");
    QTest::newRow("by version name x3.9 DefaultMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "x3.9", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name x3.9 MinimumMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "x3.9", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name x3.9 ExactMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    //envalid/empty version tag
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "");
    QTest::newRow("by empty version string DefaultMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by empty version string MinimumMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "", QServiceFilter::ExactVersionMatch); //what's the result of this?
    QTest::newRow("by empty version string ExactMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    //invalid version tag 2
    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "abc");
    QTest::newRow("by version name abc DefaultMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "abc", QServiceFilter::MinimumVersionMatch);
    QTest::newRow("by version name abc MinimumMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));

    filter = QServiceFilter();
    filter.setInterface("com.nokia.qt.TestInterfaceB", "abc", QServiceFilter::ExactVersionMatch);
    QTest::newRow("by version name abc ExactMatch") << filter
            << (ServiceInterfaceDescriptorList()    << sampleXmlInterface("A",1,0)
                                                    << sampleXmlInterface("B",1,0)
                                                    << sampleXmlInterface("B",2,0)
                                                    << sampleXmlInterface("B",2,3));
}


void tst_QServiceManager::findInterfaces_filter()
{
    QFETCH(QServiceFilter, filter);
    QFETCH(QList<QServiceInterfaceDescriptor>, expectedInterfaces);

    QServiceManager mgr;

    QByteArray bytes = sampleServiceXml();
    QBuffer buffer(&bytes);
    QVERIFY(mgr.addService(&buffer));

    QList<QServiceInterfaceDescriptor> result = mgr.findInterfaces(filter);
    QCOMPARE(result.toSet(), expectedInterfaces.toSet());
}

void tst_QServiceManager::loadInterface_string()
{
    QString serviceA = "SampleService";
    QString serviceALib = "tst_sfw_sampleserviceplugin";
    QString serviceAClassName = "SampleServicePluginClass";

    QString serviceB = "SampleService2";
    QString serviceBLib = "tst_sfw_sampleserviceplugin2";
    QString serviceBClassName = "SampleServicePluginClass2";

    QString commonInterface = "com.nokia.qt.TestInterfaceA";
    QObject *obj = 0;

    QServiceManager mgr;
    QVERIFY(mgr.addService(xmlTestDataPath("sampleservice.xml")));
    QVERIFY(mgr.addService(xmlTestDataPath("sampleservice2.xml")));

    QVERIFY(mgr.setDefaultServiceForInterface(serviceA, commonInterface));
    obj = mgr.loadInterface(commonInterface, 0, 0);
    QVERIFY(obj != 0);
    QCOMPARE(QString(obj->metaObject()->className()), serviceAClassName);

    QVERIFY(mgr.setDefaultServiceForInterface(serviceB, commonInterface));
    obj = mgr.loadInterface(commonInterface, 0, 0);
    QVERIFY(obj != 0);
    QCOMPARE(QString(obj->metaObject()->className()), serviceBClassName);
}

void tst_QServiceManager::loadInterface_descriptor()
{
    QFETCH(QString, library);
    QFETCH(QString, classname);

    //ensure the plugin exists 
    QLibrary lib(QCoreApplication::applicationDirPath() + '/' + library);
    QCOMPARE(lib.load(), true);

    QObject* obj;
    {
        QServiceManager mgr;
        MySecuritySession session;
        MyServiceContext context;
        obj = mgr.loadInterface(validDescriptor(lib.fileName()), &context, &session);
        QVERIFY(obj != 0);
        QCOMPARE(classname, QString(obj->metaObject()->className()));
    }

    QVERIFY(obj != 0);

    delete obj;
}

void tst_QServiceManager::loadInterface_descriptor_data()
{
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("classname");

    QTest::newRow("tst_sfw_sampleserviceplugin") << "tst_sfw_sampleserviceplugin" << "SampleServicePluginClass";
    QTest::newRow("tst_sfw2_sampleserviceplugin") << "tst_sfw2_sampleserviceplugin" << "TestService";
}

void tst_QServiceManager::loadInterface_testLoadedObjectAttributes()
{
    QLibrary lib(QCoreApplication::applicationDirPath() + "/tst_sfw2_sampleserviceplugin");
    QCOMPARE(lib.load(), true);

    QServiceManager mgr;
    MySecuritySession session;
    MyServiceContext context;
    QObject *obj = mgr.loadInterface(validDescriptor(lib.fileName()), &context, &session);
    QVERIFY(obj != 0);

    bool invokeOk = false;
    QString name;

    // check properties
    QMetaProperty nameProperty = obj->metaObject()->property(obj->metaObject()->indexOfProperty("name"));
    QVERIFY(nameProperty.isValid());
    QVERIFY(nameProperty.write(obj, "A service name"));
    QCOMPARE(nameProperty.read(obj), QVariant("A service name"));

    // check signals
    QVERIFY(obj->metaObject()->indexOfSignal("someSignal()") >= 0);
    QSignalSpy spy(obj, SIGNAL(someSignal()));
    invokeOk = QMetaObject::invokeMethod(obj, "someSignal");
    QVERIFY(invokeOk);
    QVERIFY(spy.count() == 1);

    // check slots
    invokeOk = QMetaObject::invokeMethod(obj, "callSlot");
    QVERIFY(invokeOk);
    invokeOk = QMetaObject::invokeMethod(obj, "callSlotAndSetName", Q_ARG(QString, "test string"));
    QVERIFY(invokeOk);
    invokeOk = QMetaObject::invokeMethod(obj, "callSlotAndReturnName", Q_RETURN_ARG(QString, name));
    QVERIFY(invokeOk);
    QCOMPARE(name, QString("test string"));

    // check invokables
    invokeOk = QMetaObject::invokeMethod(obj, "callInvokable");
    QVERIFY(invokeOk);

    // call method on a returned object
    QObject *embeddedObj = 0;
    int value = 0;
    invokeOk = QMetaObject::invokeMethod(obj, "embeddedTestService", Q_RETURN_ARG(QObject*, embeddedObj));
    QVERIFY(invokeOk);
    invokeOk = QMetaObject::invokeMethod(embeddedObj, "callWithInt", Q_RETURN_ARG(int, value), Q_ARG(int, 10));
    QVERIFY(invokeOk);
    QCOMPARE(value, 10);

    // call a method that is not invokable via meta system
    invokeOk = QMetaObject::invokeMethod(embeddedObj, "callNormalMethod");
    QVERIFY(!invokeOk);

    delete obj;
}

void tst_QServiceManager::getInterface()
{
    //ensure the plugin exists 
    QLibrary lib(QCoreApplication::applicationDirPath() + "/tst_sfw_sampleserviceplugin");
    QCOMPARE(lib.load(), true);
    lib.unload();

    QServiceManager mgr;
    MySecuritySession session;
    MyServiceContext context;

    //use manual descriptor -> avoid database involvement
    SampleServicePluginClass *plugin = 0;
    plugin = mgr.getInterface<SampleServicePluginClass>(validDescriptor(lib.fileName()), &context, &session);

    QVERIFY(plugin != 0);
    QCOMPARE(plugin->context(), &context);
    QCOMPARE(plugin->securitySession(), &session);

    delete plugin;
    plugin = 0;

    //use database descriptor
    QFile file1(xmlTestDataPath("sampleservice.xml"));
    QVERIFY(file1.exists());
    QVERIFY(mgr.addService(&file1));

    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceB"), QStringList("SampleService"));
    QList<QServiceInterfaceDescriptor> ifaces = mgr.findInterfaces("SampleService");
    QVERIFY(ifaces.count() == 2);
    for (int i = 0; i<ifaces.count(); i++) {
        plugin = mgr.getInterface<SampleServicePluginClass>(ifaces.at(i), &context, &session);

        QVERIFY(plugin != 0);
        QCOMPARE(plugin->context(), &context);
        QCOMPARE(plugin->securitySession(), &session);

        delete plugin;
        plugin = 0;
    }

    //use default lookup
    plugin = mgr.getInterface<SampleServicePluginClass>("com.nokia.qt.TestInterfaceA", &context, &session);
    QVERIFY(plugin != 0);
    QCOMPARE(plugin->context(), &context);
    QCOMPARE(plugin->securitySession(), &session);

    delete plugin;
    plugin = 0;

    //use totally wrong but QObject based template class type
    QFile *w = mgr.getInterface<QFile>("com.nokia.qt.TestInterfaceA", &context, &session);
    QVERIFY(!w);

    //use non QObject based template class type
    //doesn't compile and should never compile
    /*QString* s = mgr.getInterface<QString>("com.nokia.qt.TestInterfaceA", &context, &session);
    QVERIFY(!s);*/
}

void tst_QServiceManager::addService_iodevice()
{
    QByteArray bytes = sampleServiceXml();
    QBuffer buffer(&bytes);

    QSettings settings("com.nokia.qt.serviceframework.tests", "SampleServicePlugin");
    QCOMPARE(settings.value("installed").toBool(), false);

    QServiceManager mgr;
    QVERIFY(mgr.addService(&buffer));

    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceB"), QStringList("SampleService"));

    // the service should be automatically set as the default for its
    // implemented interfaces since it was the first service added for them
    QCOMPARE(mgr.defaultServiceInterface("com.nokia.qt.TestInterfaceA").serviceName(), QString("SampleService"));
    QCOMPARE(mgr.defaultServiceInterface("com.nokia.qt.TestInterfaceB").serviceName(), QString("SampleService"));

    // test installService() was called on the plugin
    QCOMPARE(settings.value("installed").toBool(), true);
}

void tst_QServiceManager::addService_qstring()
{
    QServiceManager mgr;
    QServiceFilter wildcardFilter;

    QStringList allServices = mgr.findServices();
    QVERIFY(allServices.count() == 0);
    QList<QServiceInterfaceDescriptor> allDescriptors = mgr.findInterfaces(wildcardFilter);
    QVERIFY(allDescriptors.count() == 0);

    QFile file1(xmlTestDataPath("sampleservice.xml"));
    QVERIFY(file1.exists());
    QFile file2(xmlTestDataPath("testserviceplugin.xml"));
    QVERIFY(file2.exists());

    QVERIFY(mgr.addService(file1.fileName()));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceB"), QStringList("SampleService"));

    QVERIFY(mgr.addService(file2.fileName()));
    QCOMPARE(mgr.findServices("com.nokia.qt.ISimpleTypeTest"), QStringList("TestService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.IComplexTypeTest"), QStringList("TestService"));

    allServices = mgr.findServices();
    QVERIFY(allServices.count() == 2);
    QVERIFY(allServices.contains("SampleService"));
    QVERIFY(allServices.contains("TestService"));

    allDescriptors = mgr.findInterfaces(wildcardFilter);
    QVERIFY(allDescriptors.count()==4);
}

void tst_QServiceManager::removeService()
{
    QByteArray bytes = sampleServiceXml();
    QBuffer buffer(&bytes);

    QServiceManager mgr;

    QSettings settings("com.nokia.qt.serviceframework.tests", "SampleServicePlugin");
    QCOMPARE(settings.value("installed").toBool(), false);

    QVERIFY(mgr.addService(&buffer));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList("SampleService"));
    QCOMPARE(settings.value("installed").toBool(), true);

    QVERIFY(mgr.removeService("SampleService"));
    QCOMPARE(mgr.findServices("com.nokia.qt.TestInterfaceA"), QStringList());
    QCOMPARE(settings.value("installed").toBool(), false);
}

void tst_QServiceManager::setDefaultServiceForInterface_strings()
{
    QServiceManager mgr;
    QServiceInterfaceDescriptor desc;

    QByteArray xml = sampleServiceXml();
    QTemporaryFile *file = new QTemporaryFile(this);
    QVERIFY(file->open());
    QVERIFY(file->write(xml) > 0);
    QVERIFY(file->seek(0));

    // fails if the specified interface hasn't been registered
    QCOMPARE(mgr.setDefaultServiceForInterface("SampleService", "com.nokia.qt.TestInterfaceA"), false);

    // now it works
    QVERIFY(mgr.addService(file));
    QCOMPARE(mgr.setDefaultServiceForInterface("SampleService", "com.nokia.qt.TestInterfaceA"), true);

    desc = mgr.defaultServiceInterface("com.nokia.qt.TestInterfaceA");
    QCOMPARE(desc.serviceName(), QString("SampleService"));
    QCOMPARE(desc.majorVersion(), 1);
    QCOMPARE(desc.minorVersion(), 0);

    // replace the default with another service
    QVERIFY(mgr.addService(xmlTestDataPath("sampleservice2.xml")));
    QCOMPARE(mgr.setDefaultServiceForInterface("SampleService2", "com.nokia.qt.TestInterfaceA"), true);

    // if there are multiple interfaces, the default should be the latest version
    QCOMPARE(mgr.setDefaultServiceForInterface("SampleService", "com.nokia.qt.TestInterfaceB"), true);
    desc = mgr.defaultServiceInterface("com.nokia.qt.TestInterfaceB");
    file->close();
    QCOMPARE(desc.serviceName(), QString("SampleService"));
    QCOMPARE(desc.majorVersion(), 2);
    QCOMPARE(desc.minorVersion(), 3);

    // bad arguments
    QCOMPARE(mgr.setDefaultServiceForInterface("", ""), false);
    QCOMPARE(mgr.setDefaultServiceForInterface("blah", "blah"), false);
    QCOMPARE(mgr.setDefaultServiceForInterface("SampleService", ""), false);

    file->close();
    delete file;
}

void tst_QServiceManager::setDefaultServiceForInterface_descriptor()
{
    QServiceManager mgr;
    QServiceInterfaceDescriptor desc;

    QCOMPARE(mgr.setDefaultServiceForInterface(desc), false);

    QByteArray xml = sampleServiceXml();
    QTemporaryFile *file = new QTemporaryFile(this);
    QVERIFY(file->open());
    QVERIFY(file->write(xml) > 0);
    QVERIFY(file->seek(0));

    desc = sampleXmlInterface("B", 1, 0);

    // fails if the specified interface hasn't been registered
    QCOMPARE(mgr.setDefaultServiceForInterface(desc), false);

    // now it works
    QVERIFY(mgr.addService(file));
    QCOMPARE(mgr.setDefaultServiceForInterface(desc), true);

    QServiceInterfaceDescriptor r = mgr.defaultServiceInterface("com.nokia.qt.TestInterfaceB");
    QCOMPARE(mgr.defaultServiceInterface("com.nokia.qt.TestInterfaceB"), desc);

    file->close();
    delete file;
}

void tst_QServiceManager::defaultServiceInterface()
{
    QServiceManager mgr;
    QVERIFY(!mgr.defaultServiceInterface("").isValid());
}

void tst_QServiceManager::serviceAdded()
{
    QFETCH(QByteArray, xml);
    QFETCH(QString, serviceName);

    QBuffer buffer;
    buffer.setData(xml);
    QServiceManager mgr;

    QSignalSpy spy(&mgr, SIGNAL(serviceAdded(QString)));
    QVERIFY(mgr.addService(&buffer));

    QEXPECT_FAIL("", "Not yet implemented", Abort);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), serviceName);
}

void tst_QServiceManager::serviceAdded_data()
{
    QTest::addColumn<QByteArray>("xml");
    QTest::addColumn<QString>("serviceName");

    QFile file1(xmlTestDataPath("sampleservice.xml"));
    QVERIFY(file1.open(QIODevice::ReadOnly));
    QFile file2(xmlTestDataPath("testserviceplugin.xml"));
    QVERIFY(file2.open(QIODevice::ReadOnly));

    QTest::newRow("SampleService") << file1.readAll() << "SampleService";
    QTest::newRow("TestService") << file2.readAll() << "TestService";
}

void tst_QServiceManager::serviceRemoved()
{
    QFETCH(QByteArray, xml);
    QFETCH(QString, serviceName);

    QBuffer buffer;
    buffer.setData(xml);
    QServiceManager mgr;

    QVERIFY(mgr.addService(&buffer));

    QSignalSpy spy(&mgr, SIGNAL(serviceRemoved(QString)));
    QVERIFY(mgr.removeService(serviceName));

    QEXPECT_FAIL("", "Not yet implemented", Abort);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), serviceName);
}

void tst_QServiceManager::serviceRemoved_data()
{
    serviceAdded_data();
}

QTEST_MAIN(tst_QServiceManager)

#include "tst_qservicemanager.moc"
