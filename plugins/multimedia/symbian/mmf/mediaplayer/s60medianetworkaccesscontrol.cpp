/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "s60medianetworkaccesscontrol.h"

#define KBuffersize 512

S60MediaNetworkAccessControl::S60MediaNetworkAccessControl(QObject *parent)
    : QMediaNetworkAccessControl(parent)
    , m_iapId(KUseDefaultIap)
    , m_currentIndex(0)
{

}

void S60MediaNetworkAccessControl::accessPointChanged(int id)
{
    if (!m_iapIdList.isEmpty())
        m_networkObject = m_networkObjectList.at(m_iapIdList.indexOf(id));
    emit configurationChanged(m_networkObject);
}

S60MediaNetworkAccessControl::~S60MediaNetworkAccessControl()
{
    m_networkObjectList.clear();
    m_iapIdList.clear();
}

void S60MediaNetworkAccessControl::resetIndex()
{
    m_currentIndex = 0;
}

void S60MediaNetworkAccessControl::setConfigurations(const QList<QNetworkConfiguration> &configurations)
{
    if (!configurations.isEmpty()) {
        m_currentIndex =0;
        TRAPD(error, retriveAccesspointIDL(configurations));
        if (error != KErrNone) {
            m_networkObjectList.clear();
            m_iapIdList.clear();
        }
     }
}

bool S60MediaNetworkAccessControl::isLastAccessPoint()
{
    if (m_currentIndex == m_networkObjectList.size())
        return true;
    else
        return false;
}

int S60MediaNetworkAccessControl::accessPointId()
{
    if (m_iapIdList.isEmpty())
        return m_iapId;
    m_iapId = m_iapIdList.at(m_currentIndex);
    if (isLastAccessPoint())
        m_currentIndex = 0;
    else
        m_currentIndex ++;
    return m_iapId;
}

QNetworkConfiguration S60MediaNetworkAccessControl::currentConfiguration() const
{
    return m_networkObject;
}

void S60MediaNetworkAccessControl::retriveAccesspointIDL(const QList<QNetworkConfiguration> &configurationList)
{
     m_networkObjectList.clear();
     m_iapIdList.clear();
     TBuf<KBuffersize> iapName;
     TUint32 iapId;
     TInt err;

     // open the IAP communications database
     CCommsDatabase* commDB = CCommsDatabase::NewL();
     CleanupStack::PushL(commDB);

     // Open the IAP table
     CCommsDbTableView* view = commDB->OpenTableLC(TPtrC(IAP));

     for (int i=0; i<=configurationList.size()- 1; ++i) {
         QString accesspointname = configurationList.at(i).name();
         TBuf<KBuffersize> accesspointbuffer(accesspointname.utf16());
         // Point to the first entry
         if (view->GotoFirstRecord() == KErrNone) {
             do {
                 view->ReadTextL(TPtrC(COMMDB_NAME), iapName);
                 view->ReadUintL(TPtrC(COMMDB_ID), iapId);
                 if (accesspointbuffer == iapName) {
                     m_networkObjectList << configurationList.at(i);
                     m_iapIdList << iapId;
                 }
                 // Store name and ID to where you want to
             } while (err = view->GotoNextRecord(), err == KErrNone);
         }
    }
    CleanupStack::PopAndDestroy(); // view
    CleanupStack::PopAndDestroy(); // commDB
}
