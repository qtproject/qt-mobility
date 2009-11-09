/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
#ifndef CNTSRVCONNECTION_H
#define CNTSRVCONNECTION_H

// System includes
#include <QObject>
#include <e32std.h>
#include <cntdb.h>
#include <cntdbobs.h>
#include "qcontactmanager.h"

// User includes

// Forward declarations

// External data types

// Constants

class CntSymbianDatabase : public QObject,
                           public MContactDbObserver
{
    Q_OBJECT

public:
    CntSymbianDatabase(QContactManager::Error& error);
    ~CntSymbianDatabase();
    
public:
    CContactDatabase* contactDatabase();
    
public:
    // From MContactDbObserver
    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);
    
/*  Signals to be moved here after CntSymbianEnginePrivate & CntSymbianEngine
 *  are combined
signals:
    // Database change notifications
    void contactAdded(const QContactLocalId &contactId);
    void contactRemoved(const QContactLocalId &contactId);
    void contactChanged(const QContactLocalId &contactId);
    void relationshipAdded(const QContactLocalId &contactId);
    void relationshipRemoved(const QContactLocalId &contactId);
*/
    // Temporary helper functions. To be removed after signals are implemented
    void observer(MContactDbObserver* observer){m_observer = observer;};
private:
    MContactDbObserver* m_observer;
    
private:
    CContactDatabase* m_contactDatabase;
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
    CContactChangeNotifier* m_contactChangeNotifier;
#endif
    
};





#endif CNTSRVCONNECTION_H
