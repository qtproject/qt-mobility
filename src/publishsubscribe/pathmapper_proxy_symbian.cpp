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
#include "pathmapper_proxy_symbian_p.h"
#include "clientservercommon.h"

#include <QDir>
#include <QDebug>

QTM_BEGIN_NAMESPACE

PathMapper::PathMapper()
{
    m_pathMapperServerSession.Connect();
}

PathMapper::~PathMapper()
{
    m_pathMapperServerSession.Close();
}

bool PathMapper::getChildren(const QString &path, QSet<QString> &children) const
{
    return m_pathMapperServerSession.getChildren(path, children);
}

QStringList PathMapper::childPaths(const QString &path) const
{
    return m_pathMapperServerSession.childPaths(path);
}

bool PathMapper::resolvePath(const QString &path, Target &target, quint32 &category, quint32 &key) const
{
    return m_pathMapperServerSession.resolvePath(path, target, category, key);
}

PathMapper::RPathMapperServerSession::RPathMapperServerSession()
{
}

TInt PathMapper::RPathMapperServerSession::Connect()
{
    TInt err = StartServer();
    if (err == KErrNone)    {
        err = CreateSession(KPSPathMapperServerName, Version(), 8, EIpcSession_Sharable);
    }
    return err;
}


TVersion PathMapper::RPathMapperServerSession::Version() const
{
    return TVersion(KServerMajorVersionNumber, KServerMinorVersionNumber, KServerBuildVersionNumber);
}

bool PathMapper::RPathMapperServerSession::getChildren(const QString &path, QSet<QString> &children) const
{
    QByteArray pathByteArray;
    QDataStream out(&pathByteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << path;
    TPtrC8 pathPtr((TUint8*)(pathByteArray.constData()), pathByteArray.size());
    TPckgBuf<TInt> lengthPckg(0);
    SendReceive(EGetChildrenLengthRequest, TIpcArgs(&pathPtr, &lengthPckg));
    int length = lengthPckg();
    if (length == 0)
        return false;

    HBufC8 *childrenBuf = HBufC8::New(length);
    if (!childrenBuf)
        return false;

    TPtr8 childrenPtr(childrenBuf->Des());
    SendReceive(EGetChildrenRequest, TIpcArgs(&childrenPtr));

    QByteArray childrenByteArray((const char*)childrenPtr.Ptr(), childrenPtr.Length());
    QDataStream in(childrenByteArray);
    in >> children;

    delete childrenBuf;

    return true;
}

QStringList PathMapper::RPathMapperServerSession::childPaths(const QString &path) const
{
    QByteArray pathByteArray;
    QDataStream out(&pathByteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << path;
    TPtrC8 pathPtr((TUint8*)(pathByteArray.constData()), pathByteArray.size());
    TPckgBuf<TInt> lengthPckg(0);
    SendReceive(EChildPathsLengthRequest, TIpcArgs(&pathPtr, &lengthPckg));
    int length = lengthPckg();
    if (length == 0)
        return QStringList();

    HBufC8 *childPathsBuf = HBufC8::New(length);
    if (!childPathsBuf)
        return QStringList();

    TPtr8 childPathsPtr(childPathsBuf->Des());
    SendReceive(EChildPathsRequest, TIpcArgs(&childPathsPtr));

    QByteArray childPathsByteArray((const char*)childPathsPtr.Ptr(), childPathsPtr.Length());
    QDataStream in(childPathsByteArray);
    QStringList childPaths;
    in >> childPaths;

    delete childPathsBuf;
    return childPaths;
}

bool PathMapper::RPathMapperServerSession::resolvePath(const QString &path, Target &target, quint32 &category, quint32 &key) const
{
    QByteArray pathByteArray;
    QDataStream out(&pathByteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << path;
    TPtrC8 pathPtr((TUint8*)(pathByteArray.constData()), pathByteArray.size());
    TPckgBuf<TInt> lengthPckg(0);
    SendReceive(EResolvePathLengthRequest, TIpcArgs(&pathPtr, &lengthPckg));
    int length = lengthPckg();
    if (length == 0)
        return false;

    HBufC8 *keyDetailsBuf = HBufC8::New(length);
    if (!keyDetailsBuf)
        return false;

    TPtr8 keyDetailsPtr(keyDetailsBuf->Des());
    SendReceive(EResolvePathRequest, TIpcArgs(&keyDetailsPtr));

    QByteArray keyDetailsByteArray((const char*)keyDetailsPtr.Ptr(), keyDetailsPtr.Length());
    QDataStream in(keyDetailsByteArray);

    int t;
    quint32 c;
    quint32 k;

    in >> t;
    in >> c;
    in >> k;
    target = (Target)t;
    category = c;
    key = k;
    delete keyDetailsBuf;
    return true;
}

TInt PathMapper::RPathMapperServerSession::StartServer()
{
    TInt ret = KErrNone;
    TFindServer findServer(KPSPathMapperServerName);
    TFullName name;
    if (findServer.Next(name) != KErrNone) {
        TRequestStatus status;
        RProcess server;
        ret = server.Create(KPSPathMapperServerProcess, KNullDesC);
        if(ret != KErrNone) {
            return ret;
        }
        server.Rendezvous(status);
        if(status != KRequestPending) {
            server.Kill(KErrNone);
            server.Close();
            return KErrGeneral;
        } else {
            server.Resume();
        }

        User::WaitForRequest(status);
        if(status != KErrNone) {
            server.Close();
            return status.Int();
        }
        server.Close();
    }
    return ret;
}

#include "moc_pathmapper_proxy_symbian_p.cpp"

QTM_END_NAMESPACE
