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

#ifndef QSERVICE_PACKAGE_H
#define QSERVICE_PACKAGE_H

#include "qmobilityglobal.h"
#include <QExplicitlySharedDataPointer>
#include <QSharedData>
#include <QUuid>
#include <QVariant>
#include "qremoteserviceclassregister.h"

QT_BEGIN_NAMESPACE
class QDataStream;
class QDebug;
QT_END_NAMESPACE

QTM_BEGIN_NAMESPACE

class QServicePackagePrivate;
class Q_AUTOTEST_EXPORT QServicePackage
{
public:
    QServicePackage();
    QServicePackage(const QServicePackage& other);
    QServicePackage& operator=(const QServicePackage& other);
    ~QServicePackage();

    enum Type {
        ObjectCreation = 0,
        MethodCall,
        PropertyCall
    };

    enum ResponseType {
        NotAResponse = 0,
        Success,
        Failed
    };

    QServicePackage createResponse() const;

    bool isValid() const;
    
    QExplicitlySharedDataPointer<QServicePackagePrivate> d;

#ifndef QT_NO_DATASTREAM
    friend QDataStream &operator<<(QDataStream &, const QServicePackage&);
    friend QDataStream &operator>>(QDataStream &, QServicePackage&);
#endif
};

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &, const QServicePackage&);
QDataStream &operator>>(QDataStream &, QServicePackage&);
#endif

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, const QServicePackage&);
#endif

class QServicePackagePrivate : public QSharedData
{
public:
    QServicePackagePrivate() 
        :   packageType(QServicePackage::ObjectCreation),
            typeId(QRemoteServiceIdentifier()), payload(QVariant()),
            messageId(QUuid()), instanceId(QUuid()), responseType(QServicePackage::NotAResponse)
    {
    }

    QServicePackage::Type packageType;
    QRemoteServiceIdentifier typeId;
    QVariant payload;
    QUuid messageId;
    QUuid instanceId;
    QServicePackage::ResponseType responseType;

    virtual void clean()
    {
        packageType = QServicePackage::ObjectCreation;
        messageId = QUuid();
        instanceId = QUuid();
        payload = QVariant();
        typeId = QRemoteServiceIdentifier();
        responseType = QServicePackage::NotAResponse;
    }
};

QTM_END_NAMESPACE

#endif
