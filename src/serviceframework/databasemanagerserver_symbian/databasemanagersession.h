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
#ifndef CDATABASEMANAGERSESSION_H_
#define CDATABASEMANAGERSESSION_H_

#include <qmobilityglobal.h>
#include <e32base.h>
#include <QObject>
#include <QStringList>

class QFileSystemWatcher;

QTM_BEGIN_NAMESPACE

class CDatabaseManagerServer;
class DatabaseManagerSignalHandler;
class ServiceDatabase;

typedef TPckgBuf<TInt> TError;

class CDatabaseManagerServerSession : public CSession2
    {
    public:
        static CDatabaseManagerServerSession* NewL(CDatabaseManagerServer& aServer);
        static CDatabaseManagerServerSession* NewLC(CDatabaseManagerServer& aServer);
        virtual ~CDatabaseManagerServerSession();
    
        void ServiceL(const RMessage2& aMessage);
        void DispatchMessageL(const RMessage2& aMessage);
        
        TInt RegisterServiceL(const RMessage2& aMessage);
        TInt UnregisterServiceL(const RMessage2& aMessage);
        TInt ServiceInitializedL(const RMessage2& aMessage);
        TInt InterfacesL(const RMessage2& aMessage);
        TInt ServiceNamesL(const RMessage2& aMessage);
        TInt InterfaceDefaultL(const RMessage2& aMessage);
        TInt SetInterfaceDefaultL(const RMessage2& aMessage);
        TInt SetInterfaceDefault2L(const RMessage2& aMessage);
        void NotifyServiceSignal(const RMessage2& aMessage);
        TInt CancelNotifyServiceSignal(const RMessage2& aMessage);
        void SetChangeNotificationsEnabled(const RMessage2& aMessage);
        TInt InterfaceDefaultSize(const RMessage2& aMessage);
        TInt InterfacesSizeL(const RMessage2& aMessage);
        TInt ServiceNamesSizeL(const RMessage2& aMessage);
        
        void ServiceRemoved(const QString& aServiceName);
        void ServiceAdded(const QString& aServiceName);
        void databaseChanged(const QString &path);
        
    private:
        CDatabaseManagerServerSession(CDatabaseManagerServer& aServer);
        void ConstructL();
        TError LastErrorCode();
        void initDbPath();
        bool openDb();
            
    protected:
        void PanicClient(const RMessage2& aMessage, TInt aPanic) const;
        
    private:
        CDatabaseManagerServer& iServer;
        QByteArray* iByteArray;
        TBool iWaitingAsyncRequest;
        RMessage2 iMsg;
        DatabaseManagerSignalHandler* iDatabaseManagerSignalHandler;
        ServiceDatabase *iDb;
        QStringList m_knownServices;
        QFileSystemWatcher *m_watcher;
    };

QTM_END_NAMESPACE

#endif

// End of File
