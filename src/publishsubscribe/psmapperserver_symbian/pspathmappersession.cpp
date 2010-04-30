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

#include "clientservercommon.h"
#include "pspathmappersession.h"
#include "pspathmapperserver.pan"
#include "pathmapper_symbian_p.h"
#include <QDataStream>
#include <QSet>

#include <QDebug>

QTM_BEGIN_NAMESPACE

CPSPathMapperServerSession::CPSPathMapperServerSession(const PathMapper &aPathMapper) :
    iPathMapper(aPathMapper)
{
}

CPSPathMapperServerSession::~CPSPathMapperServerSession()
{
}

void CPSPathMapperServerSession::ServiceL(const RMessage2 &aMessage)
{
    TRAPD(err, DispatchMessageL(aMessage));
    aMessage.Complete(err);
}
	        
void CPSPathMapperServerSession::DispatchMessageL(const RMessage2 &aMessage)
{
    switch (aMessage.Function()) {
    case EGetChildrenLengthRequest:
        GetChildrenLengthL(aMessage);
        break;
    case EGetChildrenRequest:
        GetChildrenL(aMessage);
        break;
    case EChildPathsLengthRequest:
        ChildPathsLengthL(aMessage);
        break;
    case EChildPathsRequest:
        ChildPathsL(aMessage);
        break;
    case EResolvePathLengthRequest:
        ResolvePathLengthL(aMessage);
        break;
    case EResolvePathRequest:
        ResolvePathL(aMessage);
        break;
    default:
        aMessage.Panic(KUnknownOpCode, KErrNotSupported);
        break;
    }
}

void CPSPathMapperServerSession::GetChildrenLengthL(const RMessage2 &aMessage)
{
    HBufC8* pathBuf = HBufC8::NewLC(aMessage.GetDesLength(0));
    TPtr8 pathPtr(pathBuf->Des());
    aMessage.ReadL(0, pathPtr);

    QByteArray pathByteArray((const char*)pathPtr.Ptr(), pathPtr.Length());
    QDataStream in(pathByteArray);
    QString path;
    in >> path;

    CleanupStack::PopAndDestroy(pathBuf);

    QSet<QString> children;
    iPathMapper.getChildren(path, children);
    if (children.size() > 0) {
        QDataStream out(&iResultByteArray, QIODevice::WriteOnly);
        out << children;
    } else {
        iResultByteArray.clear();
    }
    TPckgBuf<TInt> lengthPckg(iResultByteArray.size());
    aMessage.Write(1, lengthPckg);
}

void CPSPathMapperServerSession::GetChildrenL(const RMessage2 &aMessage)
{
    TPtrC8 childrenPtr((TUint8*)(iResultByteArray.constData()), iResultByteArray.size());
    aMessage.Write(0, childrenPtr);
}

void CPSPathMapperServerSession::ChildPathsLengthL(const RMessage2 &aMessage)
{
    HBufC8* pathBuf = HBufC8::NewLC(aMessage.GetDesLength(0));
    TPtr8 pathPtr(pathBuf->Des());
    aMessage.ReadL(0, pathPtr);

    QByteArray pathByteArray((const char*)pathPtr.Ptr(), pathPtr.Length());
    QDataStream in(pathByteArray);
    QString path;
    in >> path;

    CleanupStack::PopAndDestroy(pathBuf);

    QStringList childPaths;
    childPaths = iPathMapper.childPaths(path);

    if (childPaths.size() > 0) {
        QDataStream out(&iResultByteArray, QIODevice::WriteOnly);
        out << childPaths;
    } else {
        iResultByteArray.clear();
    }
    TPckgBuf<TInt> lengthPckg(iResultByteArray.size());
    aMessage.Write(1, lengthPckg);
}

void CPSPathMapperServerSession::ChildPathsL(const RMessage2 &aMessage)
{
    TPtrC8 childPathsPtr((TUint8*)(iResultByteArray.constData()), iResultByteArray.size());
    aMessage.Write(0, childPathsPtr);
}

void CPSPathMapperServerSession::ResolvePathLengthL(const RMessage2 &aMessage)
{
    HBufC8* pathBuf = HBufC8::NewLC(aMessage.GetDesLength(0));
    TPtr8 pathPtr(pathBuf->Des());
    aMessage.ReadL(0, pathPtr);

    QByteArray pathByteArray((const char*)pathPtr.Ptr(), pathPtr.Length());
    QDataStream in(pathByteArray);
    QString path;
    in >> path;

    CleanupStack::PopAndDestroy(pathBuf);

    PathMapper::Target target;
    quint32 category;
    quint32 key;
    if (iPathMapper.resolvePath(path, target, category, key)) {
        QDataStream out(&iResultByteArray, QIODevice::WriteOnly);
        out << (int)target << category << key;
    } else {
        iResultByteArray.clear();
    }
    
    TPckgBuf<TInt> lengthPckg(iResultByteArray.size());
    aMessage.Write(1, lengthPckg);
}

void CPSPathMapperServerSession::ResolvePathL(const RMessage2 &aMessage)
{
    TPtrC8 keyDetailsPtr((TUint8*)(iResultByteArray.constData()), iResultByteArray.size());
    aMessage.Write(0, keyDetailsPtr);
}


QTM_END_NAMESPACE

// End of File
