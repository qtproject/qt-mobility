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
    const QMetaObject* serviceMeta;    //metaobject of the service 
    const QMetaObject* dbusMeta;       //constructed metaobject for dbus
};

QServiceMetaObjectDBus::QServiceMetaObjectDBus(QObject* service)
{
    d = new QServiceMetaObjectDBusPrivate();
    d->service = service;
    d->serviceMeta = service->metaObject();
    d->dbusMeta = serviceMetaObject();
    // Create a meta-object to represent all the contents of our service on DBus

    /*
    QDataStream stream(d->metadata);
    QMetaObjectBuilder builder;
    QMap<QByteArray, const QMetaObject*> refs;

    builder.deserialize(stream, refs);
    if (stream.status() != QDataStream::Ok) {
        qWarning() << "Invalid metaObject for service received";
    } else {
        QMetaMethodBuilder b = builder.addSignal("errorUnrecoverableIPCFault(QService::UnrecoverableIPCError)");
        
        // After all methods are filled in, otherwise qvector won't be big enough
        localSignals.fill(false, builder.methodCount());        
        localSignals.replace(b.index(), true); // Call activate locally
        
        d->meta = builder.toMetaObject();
        qWarning() << "Proxy object for" << d->meta->className() << "created.";
    }
    */
}

QServiceMetaObjectDBus::~QServiceMetaObjectDBus()
{
    if (d->serviceMeta)
        delete d->serviceMeta;
    if (d->dbusMeta)
        delete d->dbusMeta;
    delete d;
}

const QMetaObject* QServiceMetaObjectDBus::serviceMetaObject() const
{
    QMetaObjectBuilder *builder = new QMetaObjectBuilder();
  
    builder->setClassName(d->serviceMeta->className());
    
    builder->setSuperClass(d->serviceMeta->superClass()); // needed?

    // Add our property accessor methods
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
       
        const int returnType = QMetaType::type(mm.typeName());

        // Convert QVariants to QDBusVariants
        QByteArray sig(mm.signature());
        sig.replace(QByteArray("QVariant"), QByteArray("QDBusVariant"));
        QByteArray ret(mm.typeName());
        ret.replace(QByteArray("QVariant"), QByteArray("QDBusVariant"));

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
                // TODO: signal conversion to DBUS
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
            //TODO: signal notify for method
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

//provide custom Q_OBJECT implementation
const QMetaObject* QServiceMetaObjectDBus::metaObject() const
{
    // provide our construected dbus metaobject
    return d->dbusMeta;
}

int QServiceMetaObjectDBus::qt_metacall(QMetaObject::Call c, int id, void **a)
{
    // given QDBusVariants, recast to original QVariants or custom args

    // relay the meta-object cal to the service object
    if (c == QMetaObject::InvokeMetaMethod) {
        //method
       
        // find the corresponding metaindex to our service object
        QMetaMethod method = d->dbusMeta->method(id);

        ////////// CHECK SPECIAL PROPERTY ////////////
        QString methodName(method.signature());
        int index = methodName.indexOf("(");
        methodName.chop(methodName.size()-index);
        
        if (methodName == "propertyRead") {
            int index = d->dbusMeta->indexOfProperty("value");
            id = qt_metacall(QMetaObject::ReadProperty, index, a);
            return id;

        } else if (methodName == "propertyReset") {
            int index = d->dbusMeta->indexOfProperty("value");
            id = qt_metacall(QMetaObject::ResetProperty, index, a);
            return id;
        }
        //////////////////////////////////////////////

        QByteArray sig(method.signature());
        sig.replace(QByteArray("QDBusVariant"), QByteArray("QVariant"));
        id = d->serviceMeta->indexOfMethod(sig);
        //qDebug() << "METHOD TO INVOKE IS" << id << method.signature();

        //process arguments
        const QList<QByteArray> pTypes = method.parameterTypes();
        const int pTypesCount = pTypes.count();
        QVariantList args;
       
        for (int i=0; i < pTypesCount; i++) {
            const QByteArray& t = pTypes[i];
            int variantType = QVariant::nameToType(t);
            if (variantType == QVariant::UserType)
                variantType = QMetaType::type(t);
            
            if (pTypes[i] == "QDBusVariant") {
                QVariant convert = QVariant(variantType, a[i+1]);
                QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(convert);
                *reinterpret_cast< QVariant*>(a[i+1]) = dbusVariant.variant();
            }
            
            //const QByteArray& t = pTypes[i];

            /*
            int variantType = QVariant::nameToType(t);
            if (variantType == QVariant::UserType)
                variantType = QMetaType::type(t);

            if (t == "QVariant") {  //ignore whether QVariant is declared as metatype
                args << *reinterpret_cast<const QVariant(*)>(a[i+1]);
            } else if ( variantType == 0 ){
                qWarning("%s: argument %s has unknown type. Use qRegisterMetaType to register it.",
                        method.signature(), t.data());
                return id;
            } else {
                args << QVariant(variantType, a[i+1]);
            }
            */
        }
        
        // make the metacall to our service object
        id = d->service->qt_metacall(c, id, a);

        //const QByteArray& retType = QByteArray(method.typeName());
        //if (retType == "QDBusVariant")
       // {
            //QVariant convert = QVariant(QVariant::String, a[0]);
            //QDBusVariant replacement(convert);
            //*reinterpret_cast< QVariant*>(a[0]) = QVariant::fromValue(replacement);
            //qDebug() << "RETURN CASTED VALUE" << QVariant(QVariant::String, a[0]);
        //}
    } else {
        // property

        // find the corresponding metaindex to our service object
        QMetaProperty property = d->dbusMeta->property(id);
        QByteArray name(property.name());
        id = d->serviceMeta->indexOfProperty(name);
        
        // metacall our service object property
        id = d->service->qt_metacall(c, id, a);
    }

    return id;
    //QVariant result(-7);
    //if (a[0]) *reinterpret_cast< QVariant*>(a[0]) = result;

    // cast any QVariant or custom return values to QDBusVariants

    /*
    id = QObject::qt_metacall(c, id, a);
    if (id < 0 || !d->meta) 
        return id;
    
    if(localSignals.at(id)){
      QMetaObject::activate(this, d->meta, id, a);
      return id;      
    }

    if (c == QMetaObject::InvokeMetaMethod) {
        const int mcount = d->meta->methodCount() - d->meta->methodOffset();
        const int metaIndex = id + d->meta->methodOffset();

        QMetaMethod method = d->meta->method(metaIndex);

        const int returnType = QMetaType::type(method.typeName());

        //process arguments
        const QList<QByteArray> pTypes = method.parameterTypes();
        const int pTypesCount = pTypes.count();
        QVariantList args ;
        if (pTypesCount > 10) {
            qWarning() << "Cannot call" << method.signature() << ". More than 10 parameter.";
            return id;
        }
        for (int i=0; i < pTypesCount; i++) {
            const QByteArray& t = pTypes[i];

            int variantType = QVariant::nameToType(t);
            if (variantType == QVariant::UserType)
                variantType = QMetaType::type(t);

            if (t == "QVariant") {  //ignore whether QVariant is declared as metatype
                args << *reinterpret_cast<const QVariant(*)>(a[i+1]);
            } else if ( variantType == 0 ){
                qWarning("%s: argument %s has unknown type. Use qRegisterMetaType to register it.",
                        method.signature(), t.data());
                return id;
            } else {
                args << QVariant(variantType, a[i+1]);
            }
        }

        //QVariant looks the same as Void type. we need to distinguish them
        if (returnType == QMetaType::Void && strcmp(method.typeName(),"QVariant") ) {
            d->endPoint->invokeRemote(metaIndex, args, returnType);
        } else {
            //TODO: ugly but works
            //add +1 if we have a variant return type to avoid triggering of void
            //code path
            //invokeRemote() parameter list needs review
            QVariant result = d->endPoint->invokeRemote(metaIndex, args, 
                    returnType==0 ? returnType+1: returnType);
            if (returnType != 0 && strcmp(method.typeName(),"QVariant")) {
                QByteArray buffer;
                QDataStream stream(&buffer, QIODevice::ReadWrite);
                QMetaType::save(stream, returnType, result.constData());
                stream.device()->seek(0);
                QMetaType::load(stream, returnType, a[0]);
            } else {
                if (a[0]) *reinterpret_cast< QVariant*>(a[0]) = result;
            }
        }
        id-=mcount;
    } else if ( c == QMetaObject::ReadProperty 
            || c == QMetaObject::WriteProperty
            || c == QMetaObject::ResetProperty ) {
        const int pCount = d->meta->propertyCount() - d->meta->propertyOffset();
        const int metaIndex = id + d->meta->propertyOffset();
        QMetaProperty property = d->meta->property(metaIndex);
        if (property.isValid()) {
            int pType = property.type();
            if (pType == QVariant::UserType)
                pType = QMetaType::type(property.typeName());

            QVariant arg;
            if ( c == QMetaObject::WriteProperty ) {
                if (pType == QVariant::Invalid && QByteArray(property.typeName()) == "QVariant")
                    arg =  *reinterpret_cast<const QVariant(*)>(a[0]);
                else if (pType == 0) {
                    qWarning("%s: property %s has unkown type", property.name(), property.typeName());
                    return id;
                } else {
                    arg = QVariant(pType, a[0]);
                }
            }
            QVariant result;
            if (c == QMetaObject::ReadProperty) {
                result = d->endPoint->invokeRemoteProperty(metaIndex, arg, pType, c);
                //wrap result for client
                if (pType != 0) {
                    QByteArray buffer;
                    QDataStream stream(&buffer, QIODevice::ReadWrite);
                    QMetaType::save(stream, pType, result.constData());
                    stream.device()->seek(0);
                    QMetaType::load(stream, pType, a[0]);
                } else {
                    if (a[0]) *reinterpret_cast< QVariant*>(a[0]) = result;
                }
            } else {
                d->endPoint->invokeRemoteProperty(metaIndex, arg, pType, c);
            }
        } 
        id-=pCount;
    } else if ( c == QMetaObject::QueryPropertyDesignable
            || c == QMetaObject::QueryPropertyScriptable
            || c == QMetaObject::QueryPropertyStored
            || c == QMetaObject::QueryPropertyEditable
            || c == QMetaObject::QueryPropertyUser ) 
    {
        //Nothing to do?
        //These values are part of the transferred meta object already
    } else {
        //TODO
        qWarning() << "MetaCall type" << c << "not yet handled";
    }
    return id;
    */
}

void *QServiceMetaObjectDBus::qt_metacast(const char* className)
{
    if (!className) return 0;
    //this object should not be castable to anything but QObject
    return QObject::qt_metacast(className);
}
QTM_END_NAMESPACE
