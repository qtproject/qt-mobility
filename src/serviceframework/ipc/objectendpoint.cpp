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

#include "objectendpoint_p.h"
#include "instancemanager_p.h"
#include "qmetaobjectbuilder_p.h"
#include "proxyobject_p.h"
#include <QTimer>
#include <QEventLoop>

QTM_BEGIN_NAMESPACE

class Response
{
public:
    Response() : isFinished(false), result(0) 
    { }

    bool isFinished;
    void* result;
};

typedef QHash<QUuid, Response*> Replies;
Q_GLOBAL_STATIC(Replies, openRequests);


ObjectEndPoint::ObjectEndPoint(Type type, QServiceIpcEndPoint* comm, QObject* parent)
    : QObject(parent), endPointType(type), dispatch(comm), service(0), 
      serviceInstanceId(QUuid()), typeIdent(QServiceTypeIdent())
{
    Q_ASSERT(dispatch);
    dispatch->setParent(this);
    connect(dispatch, SIGNAL(readyRead()), this, SLOT(newPackageReady()));
    connect(dispatch, SIGNAL(disconnected()), this, SLOT(disconnected()));
    if (type == Client) {
        return; //we are waiting for conctructProxy() call
    } else {
        if (dispatch->packageAvailable())
            QTimer::singleShot(0, this, SLOT(newPackageReady()));
    }
}

ObjectEndPoint::~ObjectEndPoint()
{
    if (endPointType == Service) {
        InstanceManager::instance()->removeObjectInstance(typeIdent, serviceInstanceId);
    }
}

void ObjectEndPoint::disconnected()
{
    deleteLater();
}

/*!
    Client requests proxy object. The proxy is owned by calling
    code and this object must clean itself up upon destruction of
    proxy.
*/
QObject* ObjectEndPoint::constructProxy(const QServiceTypeIdent& ident)
{
    
    //ask for serialized meta object
    //get proxy based on meta object
    //return meta object
    QServicePackage p;
    p.d = new QServicePackagePrivate();
    p.d->type = QServicePackage::ObjectCreation;
    p.d->id = QUuid::createUuid();
    p.d->typeId = ident;

    Response* response = new Response();
    openRequests()->insert(p.d->id, response);

    dispatch->writePackage(p);

    waitForResponse(p.d->id);

    if (response->isFinished) {
        if (response->result == 0)
            qWarning() << "Request for remote service failed";
        else
            service = reinterpret_cast<QServiceProxy* >(response->result);
    } else {
        qDebug() << "response passed but not finished";
    }

    openRequests()->take(p.d->id);
    delete response;

    return service;
}

void ObjectEndPoint::newPackageReady()
{
    while(dispatch->packageAvailable())
    {
        QServicePackage p = dispatch->nextPackage();
        if (!p.isValid())
            continue;

        switch(p.d->type) {
            case QServicePackage::ObjectCreation:
                objectRequest(p);
                break;
            case QServicePackage::SignalEmission:
            case QServicePackage::MethodCall:
                methodCall(p);
                break;
            default:
                qWarning() << "Unknown package type received.";
        }
    }
}

void ObjectEndPoint::objectRequest(const QServicePackage& p)
{
    if (p.d->responseType != QServicePackage::NotAResponse ) {
        qDebug() << p;
        Response* response = openRequests()->value(p.d->id);
        if (p.d->responseType == QServicePackage::Failed) {
            response->result = 0;
            response->isFinished = true;
            qWarning() << "Service instanciation failed";
            return;
        }
        //deserialize meta object
        QByteArray payload = p.d->payload.toByteArray();
        QMetaObject mo;
        QMetaObjectBuilder::fromRelocatableData(&mo, 0, payload);
        qDebug() << mo.className() << payload.size() << sizeof(mo);

        //create proxy object
        QServiceProxy* proxy = new QServiceProxy(payload);
        response->result = reinterpret_cast<void *>(proxy);
        response->isFinished = true;

        //wake up waiting code
        QTimer::singleShot(0, this, SIGNAL(pendingRequestFinished()));

    } else {
        qDebug() << p;
        QServicePackage response = p.createResponse();
        InstanceManager* m = InstanceManager::instance();

        //get meta object from type register
        const QMetaObject* meta = m->metaObject(p.d->typeId);
        if (!meta) {
            qDebug() << "Unknown type" << p.d->typeId;
            dispatch->writePackage(response);
            return;
        }

        //serialize meta object
        bool ok = false;
        QMetaObjectBuilder builder(meta);

        const QByteArray serializedMetaObject = builder.toRelocatableData(&ok);
        if (!ok) {
            qWarning() << "Cannot serialize QMetaObject";
            dispatch->writePackage(response);
            return;
        }
        
        //instanciate service object from type register
        service = m->createObjectInstance(p.d->typeId, serviceInstanceId);
        if (!service) {
            qWarning() << "Cannot instanciate service object";
            dispatch->writePackage(response);
            return;
        }

        //send meta object 
        typeIdent = p.d->typeId;
        response.d->typeId = p.d->typeId;
        response.d->responseType = QServicePackage::Success;
        response.d->payload = QVariant(serializedMetaObject);
        dispatch->writePackage(response);
    }
}
    
void ObjectEndPoint::methodCall(const QServicePackage& p)
{

    if (p.d->responseType != QServicePackage::NotAResponse ) {
        qDebug() << p;
        //TODO
    } else {
        qDebug() << p;
        //TODO
    }
}

void ObjectEndPoint::waitForResponse(const QUuid& requestId)
{
    if (openRequests()->contains(requestId) ) {
        Response* response = openRequests()->value(requestId);

        QEventLoop* loop = new QEventLoop( this );
        connect(this, SIGNAL(pendingRequestFinished()), loop, SLOT(quit())); 

        while(!response->isFinished) {
            loop->exec();
        }
 
        delete loop;
    }
}

#include "moc_objectendpoint_p.cpp"

QTM_END_NAMESPACE
