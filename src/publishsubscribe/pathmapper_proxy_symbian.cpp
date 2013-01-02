/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
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
#include "pathmapper_proxy_symbian_p.h"
#include "clientservercommon.h"

#include <QDir>
#include <QMutex>
#include <QDebug>

// Requests to pathmapper server consist of two separate subrequests (first
// asking the length of the reply, and second actually receiving the reply).
// No other requests can be handled between those requests, so we need to
// protect the requests with a mutex.
static QMutex requestGuardMutex;

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
    QMutexLocker locker(&requestGuardMutex);
    return m_pathMapperServerSession.getChildren(path, children);
}

QStringList PathMapper::childPaths(const QString &path) const
{
    QMutexLocker locker(&requestGuardMutex);
    return m_pathMapperServerSession.childPaths(path);
}

bool PathMapper::resolvePath(const QString &path, Target &target, quint32 &category, quint32 &key) const
{
    QMutexLocker locker(&requestGuardMutex);
    return m_pathMapperServerSession.resolvePath(path, target, category, key);
}

PathMapper::RPathMapperServerSession::RPathMapperServerSession()
{
}

TInt PathMapper::RPathMapperServerSession::Connect()
{
    TInt retryCount = 2; // A maximum of two iterations of the loop required
    for (;;) {
        TInt err = CreateSession(KPSPathMapperServerName, Version(), 8, EIpcSession_Sharable);
        if ((err != KErrNotFound) && (err != KErrServerTerminated))
            return err;
        if (--retryCount == 0)
            return err;
        err = StartServer();
        if ((err != KErrNone) && (err != KErrAlreadyExists))
            return err;
    }
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
    TPtrC8 pathPtr((TUint8*) (pathByteArray.constData()), pathByteArray.size());
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

    QByteArray childrenByteArray((const char*) childrenPtr.Ptr(), childrenPtr.Length());
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
    TPtrC8 pathPtr((TUint8*) (pathByteArray.constData()), pathByteArray.size());
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

    QByteArray childPathsByteArray((const char*) childPathsPtr.Ptr(), childPathsPtr.Length());
    QDataStream in(childPathsByteArray);
    QStringList childPaths;
    in >> childPaths;

    delete childPathsBuf;
    return childPaths;
}

bool PathMapper::RPathMapperServerSession::resolvePath(const QString &path, Target &target,
    quint32 &category, quint32 &key) const
{
    QByteArray pathByteArray;
    QDataStream out(&pathByteArray, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_6);
    out << path;
    TPtrC8 pathPtr((TUint8*) (pathByteArray.constData()), pathByteArray.size());
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

    QByteArray keyDetailsByteArray((const char*) keyDetailsPtr.Ptr(), keyDetailsPtr.Length());
    QDataStream in(keyDetailsByteArray);

    int t;
    quint32 c;
    quint32 k;

    in >> t;
    in >> c;
    in >> k;
    target = (Target) t;
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
        if (ret != KErrNone) {
            return ret;
        }
        server.Rendezvous(status);
        if (status != KRequestPending) {
            server.Kill(KErrNone);
            server.Close();
            return KErrGeneral;
        }
        else {
            server.Resume();
        }

        User::WaitForRequest(status);
        if (status != KErrNone) {
            server.Close();
            return status.Int();
        }
        server.Close();
    }
    return ret;
}

#include "moc_pathmapper_proxy_symbian_p.cpp"

QTM_END_NAMESPACE
