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

#ifndef CNTSIMSTORE_H_
#define CNTSIMSTORE_H_

#include <e32base.h>
#ifdef SYMBIANSIM_BACKEND_USE_ETEL_TESTSERVER
#include <mpbutil_etel_test_server.h>
#else
#include <mpbutil.h>
#endif
#include <QObject>
#include <qcontactmanager.h>

QTM_USE_NAMESPACE

class CntSimStorePrivate;
class CntSymbianSimEngine;

class CntSimStore : public QObject
{
Q_OBJECT
public:
    CntSimStore(CntSymbianSimEngine* engine);
    ~CntSimStore();

    QContactManager::Error getInfo();
    QContactManager::Error read(int index, int numSlots);
    QContactManager::Error write(const QContact &contact);
    QContactManager::Error remove(int index);
    void cancel();
    bool isBusy();

signals:
    // NOTE: Use Qt::QueuedConnection as connection type to make signals asynchronous.
    // CntSimStorePrivate emitting these signals is an active object and emitting
    // signals synchronously will corrupt the AO state.
    void getInfoComplete(RMobilePhoneBookStore::TMobilePhoneBookInfoV5 info, QContactManager::Error error);
    void readComplete(QList<QContact> contacts, QContactManager::Error error);
    void writeComplete(QContact contacts, QContactManager::Error error);
    void removeComplete(QContactManager::Error error);
    
private:
    CntSimStorePrivate *d_ptr;
    friend class CntSimStorePrivate;    
};


#endif // CNTSIMSTORE_H_
