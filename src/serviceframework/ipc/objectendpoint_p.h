/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef OBJECT_ENDPOINT_H
#define OBJECT_ENDPOINT_H

#include "qmobilityglobal.h"
#include "ipcendpoint_p.h"
#include "qremoteserviceregister.h"
#include "qservice.h"
#include <QPointer>
#include <QHash>
#ifdef Q_OS_SYMBIAN
#include <QAtomicInt>
#endif 
QTM_BEGIN_NAMESPACE

class ObjectEndPointPrivate;
class ObjectEndPoint : public QObject
{
    Q_OBJECT
public:
    enum Type {
        Service = 0,
        Client
    };
#ifdef Q_OS_SYMBIAN
    enum DeleteStat {
        NotMarkedForDelete = 0,
        MarkedForDelete    = 0x8000
    };
	
	int getRefCount();
    bool updateRefCount(int expectedVal, int newVal);
#endif //End Q_OS_SYMBIAN
    ObjectEndPoint(Type type, QServiceIpcEndPoint* comm, QObject* parent = 0);
    ~ObjectEndPoint();
    QObject* constructProxy(const QRemoteServiceRegister::Entry& entry);

    void objectRequest(const QServicePackage& p);
    void methodCall(const QServicePackage& p);
    void propertyCall(const QServicePackage& p);

    QVariant invokeRemote(int metaIndex, const QVariantList& args, int returnType);
    QVariant invokeRemoteProperty(int metaIndex, const QVariant& arg, int returnType, QMetaObject::Call c);
    
Q_SIGNALS:
    void pendingRequestFinished();

public Q_SLOTS:
    void newPackageReady();
    void disconnected();

private:
    void waitForResponse(const QUuid& requestId);

    QServiceIpcEndPoint* dispatch;
    QPointer<QObject> service;
    ObjectEndPointPrivate* d;
	#ifdef Q_OS_SYMBIAN
    QAtomicInt referenceCnt;
	#endif //End Q_OS_SYMBIAN
};
#ifdef Q_OS_SYMBIAN
class ObjectEndPointMonitor 
{
public:
		ObjectEndPointMonitor(ObjectEndPoint *aOe);
		~ObjectEndPointMonitor();
		
		static void MarkForDelete(ObjectEndPoint *aOe);
		static bool IncRefCountIfNotMarkedDelete(ObjectEndPoint *aOe);
		static void DecRefCount(ObjectEndPoint *aOe);

private:
	ObjectEndPoint*	iOe;
	bool incrementDone;
};
#endif //End Q_OS_SYMBIAN
QTM_END_NAMESPACE

#endif //OBJECT_ENDPOINT_H
