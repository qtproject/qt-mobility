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

#ifndef STORAGEDISKNOTIFIER_S60_H
#define STORAGEDISKNOTIFIER_S60_H

#include <QList>
#include <QString>

#include <e32base.h>
#include <e32std.h>
#include <f32file.h>
#include <disknotifyhandler.h>

class MStorageSpaceNotifyObserver
{
public:
    virtual void DiskSpaceChanged(const QString &) = 0;
};

class CStorageDiskNotifier : public CBase, public MDiskNotifyHandlerCallback
{
public:
    static CStorageDiskNotifier* NewL();
    void AddObserver(MStorageSpaceNotifyObserver *observer);
    void RemoveObserver(MStorageSpaceNotifyObserver *observer);
    ~CStorageDiskNotifier();

private:
    CStorageDiskNotifier();
    void ConstructL();
    void SubscribeStorageDiskNotificationL();
    void RegisterDiskSpaceEvents(TInt aDrive);
    // From observer MDiskNotifyHandlerCallback
    void HandleNotifyDisk(TInt aError, const TDiskEvent& aEvent);
    void HandleNotifyDiskSpace(TInt aError, const TDiskSpaceEvent& aEvent);

private:
    RFs                     iFs;
    CDiskNotifyHandler*     iStorageDiskNotifyHandler;
    QList<MStorageSpaceNotifyObserver *> m_observers;
};

#endif //STORAGEDISKNOTIFIER_S60_H
