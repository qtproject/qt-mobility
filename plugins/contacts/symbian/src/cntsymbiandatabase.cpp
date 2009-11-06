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
//system includes
#include <e32base.h>
#include <s32mem.h>

//user includes
#include "cntsymbiandatabase.h"
#include "cntsymbiantransformerror.h"

// Constants

CntSymbianDatabase::CntSymbianDatabase(QContactManager::Error& error) :
    m_contactDatabase(0)
{
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL())

    //Database not found, create it
    if(err == KErrNotFound)
    {
        TRAP(err, m_contactDatabase = CContactDatabase::CreateL())
    }

    //If database didn't open successfully return error
    if(err != KErrNone)
    {
        CntSymbianTransformError::transformError(err, error);
    }

    //Database opened successfully
    else
    {
    // In pre 10.1 platforms the AddObserverL & RemoveObserver functions are not
    // exported so we need to use CContactChangeNotifier.
    // TODO: Is it ok to use __SYMBIAN_CNTMODEL_USE_SQLITE__ flag for this?
#ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
        TRAP(err, m_contactChangeNotifier = CContactChangeNotifier::NewL(*m_contactDatabase, this));
#else
        TRAP(err, m_contactDatabase->AddObserverL(*this));
#endif
    }
}

CntSymbianDatabase::~CntSymbianDatabase()
{
    m_observer = NULL;
    #ifndef __SYMBIAN_CNTMODEL_USE_SQLITE__
        delete m_contactChangeNotifier;
    #else
        if (m_contactDatabase != 0) {
            m_contactDatabase->RemoveObserver(*this);
        }
    #endif
        delete m_contactDatabase;
}

CContactDatabase* CntSymbianDatabase::contactdatabase()
{
    return m_contactDatabase;
}

void CntSymbianDatabase::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
    // Temporary solution. To change once main classes are combined
    m_observer->HandleDatabaseEventL(aEvent);
}






