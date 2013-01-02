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

#ifndef BLUETOOTHLINKMANAGERDEVICEDISCOVERER_H
#define BLUETOOTHLINKMANAGERDEVICEDISCOVERER_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qmobilityglobal.h>
#include <QtCore/QObject>
#include "qbluetoothdevicediscoveryagent.h"

#include <es_sock.h>
#include <e32base.h>
#include <btdevice.h>
#include <bt_sock.h>
#include <btsdp.h>



QT_BEGIN_HEADER

QTM_BEGIN_NAMESPACE

class QBluetoothDeviceInfo;

class BluetoothLinkManagerDeviceDiscoverer : public QObject, public CActive
{
    Q_OBJECT
    
public:

    explicit BluetoothLinkManagerDeviceDiscoverer(RSocketServ& aSocketServ, QObject *parent = 0);
    ~BluetoothLinkManagerDeviceDiscoverer();

    void startDiscovery(const uint discoveryType);
    void stopDiscovery();
    bool isReallyActive() const;

protected: // From CActive
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);

private:

    void setError(int error);

private: // private helper functions

    QBluetoothDeviceInfo currentDeviceDataToQBluetoothDeviceInfo() const;

Q_SIGNALS: // SIGNALS
    void deviceDiscoveryComplete();
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void linkManagerError(QBluetoothDeviceDiscoveryAgent::Error error, QString errorString);
    void canceled();

private:

    //  socket server handle
    RSocketServ &m_socketServer;

    RHostResolver m_hostResolver;
    TInquirySockAddr m_addr;
    TNameEntry m_entry;

    TBool m_LIAC;
    bool m_pendingCancel;
    bool m_pendingStart;
    uint m_discoveryType;
    bool m_initialized;
};

QTM_END_NAMESPACE

QT_END_HEADER

#endif //BLUETOOTHLINKMANAGERDEVICEDISCOVERER_H
