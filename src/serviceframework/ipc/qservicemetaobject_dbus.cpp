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

#include "qservicemetaobject_dbus_p.h"
#include "qmetaobjectbuilder_p.h"
#include <QDebug>

QTM_BEGIN_NAMESPACE

class QServiceMetaObjectDBusPrivate
{
public:
    QObject *service;
    const QMetaObject *serviceMeta;    //metaobject of the service 
    const QMetaObject *dbusMeta;       //constructed metaobject for dbus
};

QServiceMetaObjectDBus::QServiceMetaObjectDBus(QObject* service)
    : QDBusAbstractAdaptor(service)
{
    // Register our DBus custom type object
    qRegisterMetaType<QTM_PREPEND_NAMESPACE(QServiceUserTypeDBus)>();
    qDBusRegisterMetaType<QTM_PREPEND_NAMESPACE(QServiceUserTypeDBus)>();
 
    // Generate our DBus meta object
    d = new QServiceMetaObjectDBusPrivate();
    d->service = service;
    d->serviceMeta = service->metaObject();
    d->dbusMeta = dbusMetaObject();

    // Automatically relay signals from service object to adaptor
    setAutoRelaySignals(true);
}

QServiceMetaObjectDBus::~QServiceMetaObjectDBus()
{
    if (d->serviceMeta)
        delete d->serviceMeta;
    if (d->dbusMeta)
        delete d->dbusMeta;
    delete d;
}

/*!
    Build a metaobject that represents the service object as a valid service that 
    satisfies the QtDBus type system.
*/
const QMetaObject* QServiceMetaObjectDBus::dbusMetaObject() const
{
    // Create a meta-object to represent all the contents of our service on DBus
    QMetaObjectBuilder *builder = new QMetaObjectBuilder();
  
    builder->setClassName(d->serviceMeta->className());
    builder->setSuperClass(d->serviceMeta->superClass()); // needed?

    // Add our property accessor methods
    // NOTE: required because read/reset properties over DBus require adaptors
    //       otherwise a metacall won't be invoked as QMetaObject::ReadProperty
    //       or QMetaObject::ResetProperty
    QMetaMethodBuilder readProp;
    readProp = builder->addMethod(QByteArray("propertyRead(QString)"));
    readProp.setReturnType(QByteArray("QString"));
    QList<QByteArray> params;
    params << QByteArray("name");
    readProp.setParameterNames(params);
    
    QMetaMethodBuilder resetProp;
    resetProp = builder->addMethod(QByteArray("propertyReset(QString)"));
    resetProp.setReturnType(QByteArray("QString"));
    QList<QByteArray> paramsReset;
    paramsReset << QByteArray("name");
    resetProp.setParameterNames(paramsReset);

    // Add our methods, signals and slots
    int methodCount = d->serviceMeta->methodCount();
    for (int i=0; i<methodCount; i++) {
        QMetaMethod mm = d->serviceMeta->method(i);
       
        // Convert QVariant and custom return types to QDBusVariants
        QByteArray ret(mm.typeName());
        const QByteArray& type = mm.typeName();
        int variantType = QVariant::nameToType(type);
        if (variantType == QVariant::UserType)
            ret = QByteArray("QDBusVariant");

        // Convert QVariant and custom argument types to QDBusVariants
        QByteArray sig(mm.signature());
        const QList<QByteArray> pTypes = mm.parameterTypes();
        const int pTypesCount = pTypes.count();
        for (int i=0; i < pTypesCount; i++) {
            const QByteArray& t = pTypes[i];
            int variantType = QVariant::nameToType(t);
            if (variantType == QVariant::UserType) {
                sig.replace(QByteArray(t), QByteArray("QDBusVariant"));
            }
        }   

        // Add a MetaMethod with converted signature to our builder
        QMetaMethodBuilder method;
        switch (mm.methodType()) {
            case QMetaMethod::Method:
                method = builder->addMethod(sig);
                break;
            case QMetaMethod::Slot:
                method = builder->addSlot(sig);
                break;
            case QMetaMethod::Signal:
                method = builder->addSignal(sig);
                break;
            default:
                break;
        }

        // Make sure our built MetaMethod is identical, excluding conversion
        method.setReturnType(ret);
        method.setParameterNames(mm.parameterNames());
        method.setTag(mm.tag());
        method.setAccess(mm.access());
        method.setAttributes(mm.attributes());
    }

    // Add our properties/enums
    int propCount = d->serviceMeta->propertyCount();
    for (int i=0; i<propCount; i++) {
        QMetaProperty mp = d->serviceMeta->property(i);
        
        QMetaPropertyBuilder property = builder->addProperty(mp.name(), mp.typeName());
        property.setReadable(mp.isReadable());
        property.setWritable(mp.isWritable());
        property.setResettable(mp.isResettable());
        property.setDesignable(mp.isDesignable());
        property.setScriptable(mp.isScriptable());
        property.setStored(mp.isStored());
        property.setEditable(mp.isEditable());
        property.setUser(mp.isUser());
        property.setStdCppSet(mp.hasStdCppSet());
        property.setEnumOrFlag(mp.isEnumType());

        if (mp.hasNotifySignal()) {
            //TODO: signal notify for property
        }
    }

    // Add our CLASS_INFO
    // TODO: Introspect needed?
    /*
    QByteArray ifaceName("D-Bus Interface");
    QByteArray ifaceValue("com.qt.nokia.ipcunittest"); //TODO: fix based on interface
    builder->addClassInfo(ifaceName, ifaceValue);

    QString xml = ""
        "<interface name=\"com.qt.nokia.ipcunittest\" />\n"
            "<method name=\"ping\" >\n"
        "</interface>\n";

    QByteArray introspectName("D-Bus Introspection");
    QByteArray introspectValue = xml.toLatin1();
    builder->addClassInfo(introspectName, introspectValue);
    */
   
    //TODO: Add enumerator support

    // return our constructed dbus metaobject
    const QMetaObject *mo = builder->toMetaObject();

    return mo;
}

/*! 
    Provide custom Q_OBJECT implementation of the metaObject
*/
const QMetaObject* QServiceMetaObjectDBus::metaObject() const
{
    // Provide our construected DBus service metaobject
    return d->dbusMeta;
}

/*!
    Overrides metacall which relays the DBus service call to the actual service
    meta object. Positive return will indicate that the metacall was unsuccessful
*/
int QServiceMetaObjectDBus::qt_metacall(QMetaObject::Call c, int id, void **a)
{
    // Relay the meta-object call to the service object
    if (c == QMetaObject::InvokeMetaMethod) {
        // METHOD
        QMetaMethod method = d->dbusMeta->method(id);

        ///////////////////// CHECK SPECIAL PROPERTY ///////////////////////
        // This is required because property READ/RESET doesn't function
        // as desired over DBus without the use of adaptors. Temporary
        // methods propertyRead and propertyReset are added to the published
        // meta object and relay the correct property call
        QString methodName(method.signature());
        methodName.truncate(methodName.indexOf("("));
        
        if (methodName == "propertyRead") {
            int index = d->dbusMeta->indexOfProperty("value");
            id = qt_metacall(QMetaObject::ReadProperty, index, a);
            return id;

        } else if (methodName == "propertyReset") {
            int index = d->dbusMeta->indexOfProperty("value");
            id = qt_metacall(QMetaObject::ResetProperty, index, a);
            return id;
        }
        ////////////////////////////////////////////////////////////////////

        // Find the corresponding signature to our service object
        QByteArray sig(method.signature());
        int count = methodName.size() + 1;
        const QList<QByteArray> xTypes = method.parameterTypes();
        const int xTypesCount = xTypes.count();
        for (int i=0; i < xTypesCount; i++) {
            const QByteArray& t = xTypes[i];
            int variantType = QVariant::nameToType(t);
            if (variantType == QVariant::UserType) {
                variantType = QMetaType::type(t);
            }

            // Check for QVariants or custom types
            if (t == "QDBusVariant") {
                QVariant convert = QVariant(variantType, a[i+1]);
                QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(convert);
                QVariant theOne = dbusVariant.variant();
                
                bool isCustomType = theOne.canConvert<QDBusArgument>();
                QByteArray replacement("QVariant");
                
                // Custom types will have QDBusArgument streaming operators for
                // the QServiceUserTypeDBus object. Extract the real type name
                // and buffered QVariant from this
                if (isCustomType) {
                    QDBusArgument demarshall = theOne.value<QDBusArgument>();
                    QServiceUserTypeDBus userType = qdbus_cast<QServiceUserTypeDBus>(demarshall);
                    *reinterpret_cast<QVariant*>(a[i+1]) = userType.variantBuffer;

                    replacement = userType.typeName;
                }

                // Replace "QDBusVariant" with "QVariant" or custom type name
                sig.replace(count, 12, replacement);
                count += replacement.size();

            } else {
                // Supported type so skip this paramater
                count += t.size();
            }

            // Skips the comma if not last parameter
            if (i < xTypesCount)
                count += 1;
        }
        
        // Check if this is a signal emit
        const bool isSignal = (method.methodType() == QMetaMethod::Signal);
        if (isSignal) {
            // Activate the signal on this meta object
            QMetaObject::activate(this, id, a);
            return id;
        }
        
        // Find the corresponding method metaindex to our service object
        id = d->serviceMeta->indexOfMethod(sig);
        QMetaMethod mm = d->serviceMeta->method(id);

        const QList<QByteArray> pTypes = mm.parameterTypes();
        const int pTypesCount = pTypes.count();
        
        const char* typeNames[] = {0,0,0,0,0,0,0,0,0,0};
        const void* params[] = {0,0,0,0,0,0,0,0,0,0};
        bool isCustomType = false;

        // Process arguments
        for (int i=0; i < pTypesCount; i++) {
            const QByteArray& t = pTypes[i];
            int variantType = QVariant::nameToType(t);
            if (variantType == QVariant::UserType) {
                variantType = QMetaType::type(t);
            }
           
            if (variantType == QMetaType::User) {
                // Custom argument
                QVariant convert = QVariant(QVariant::ByteArray, a[i+1]);
                QByteArray buffer = convert.toByteArray();
                QDataStream stream(&buffer, QIODevice::ReadWrite);
               
                // Load our buffered variant-wrapped custom type
                QVariant *customType = new QVariant(variantType, (const void*)0);
                QMetaType::load(stream, QMetaType::type("QVariant"), customType); 
              
                typeNames[i] = customType->typeName();
                params[i] = customType->constData();
                isCustomType = true;
            }
        }
        
        // Check for custom return types and make the metacall
        const QByteArray& type = mm.typeName();
        int retType = QVariant::nameToType(type);
        retType = QMetaType::type(type);
        if (retType >= QMetaType::User) {
            // Invoke the object method directly for custom return types
            bool result = false;
            QVariant returnValue = QVariant(retType, (const void*)0);
            QGenericReturnArgument ret(type, returnValue.data());
            result = mm.invoke(d->service, ret,
                    QGenericArgument(typeNames[0], params[0]),
                    QGenericArgument(typeNames[1], params[1]),
                    QGenericArgument(typeNames[2], params[2]),
                    QGenericArgument(typeNames[3], params[3]),
                    QGenericArgument(typeNames[4], params[4]),
                    QGenericArgument(typeNames[5], params[5]),
                    QGenericArgument(typeNames[6], params[6]),
                    QGenericArgument(typeNames[7], params[7]),
                    QGenericArgument(typeNames[8], params[8]),
                    QGenericArgument(typeNames[9], params[9]));
            
            if (result) {
                // Wrap custom return type in a QDBusVariant of the type
                // and a buffer of its variant-wrapped data
                QByteArray buffer;
                QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);
                stream << returnValue;

                QServiceUserTypeDBus customType;
                customType.typeName = type;
                customType.variantBuffer = buffer;

                QDBusVariant replacement(QVariant::fromValue(customType));
                *reinterpret_cast<QDBusVariant*>(a[0]) = replacement;

                // Return negative id to say metacall was handled externally
                return -1;
            }

        } else {
            // Void or standard return types
            if (isCustomType == true) {
                // Invoke the object method directly for custom arguments
                bool result = false;
                result = mm.invoke(d->service,
                         QGenericArgument(typeNames[0], params[0]),
                         QGenericArgument(typeNames[1], params[1]),
                         QGenericArgument(typeNames[2], params[2]),
                         QGenericArgument(typeNames[3], params[3]),
                         QGenericArgument(typeNames[4], params[4]),
                         QGenericArgument(typeNames[5], params[5]),
                         QGenericArgument(typeNames[6], params[6]),
                         QGenericArgument(typeNames[7], params[7]),
                         QGenericArgument(typeNames[8], params[8]),
                         QGenericArgument(typeNames[9], params[9]));
                if (result) {
                    // Return negative id to say metacall was handled externally
                    return -1;
                }
            } else {
                // Relay standard metacall to service object
                id = d->service->qt_metacall(c, id, a);
            }
        }
    
    } else {
        // PROPERTY

        // Find the corresponding property metaindex of our service object
        QMetaProperty property = d->dbusMeta->property(id);
        QByteArray name(property.name());
        id = d->serviceMeta->indexOfProperty(name);
        
        // Metacall our service object property
        id = d->service->qt_metacall(c, id, a);
    }

    return id;
}

void *QServiceMetaObjectDBus::qt_metacast(const char* className)
{
    if (!className) return 0;
    //this object should not be castable to anything but QObject
    return QObject::qt_metacast(className);
}

/*
void QServiceMetaObjectDBus::connectNotify(const char* signal)
{
}

void QServiceMetaObjectDBus::disconnectNotify(const char* signal)
{
}
*/

QDBusArgument &operator<<(QDBusArgument &argument, const QServiceUserTypeDBus &myType)
{
    argument.beginStructure();
    argument << myType.typeName << myType.variantBuffer;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, QServiceUserTypeDBus &myType)
{
    argument.beginStructure();
    argument >> myType.typeName >> myType.variantBuffer;
    argument.endStructure();
    return argument;
}

QTM_END_NAMESPACE
